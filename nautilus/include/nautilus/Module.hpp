
#pragma once

#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/JITCompiler.hpp"
#include "nautilus/config.hpp"
#include "nautilus/core.hpp"
#include "nautilus/options.hpp"
#include <any>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef ENABLE_TRACING
#include "nautilus/CompilableFunction.hpp"
#endif

namespace nautilus::link {
class ModuleRegistry;
struct SessionAccess;
} // namespace nautilus::link

namespace nautilus::engine {

/// The executable a module runs (11-tiered-compilation.md §4.3).
enum class Tier : uint8_t {
	/// No executable: the original callables are invoked directly, until a promotion installs one.
	interpreted,
	/// The fast executable that is compiled before `compile()` returns.
	tier0,
	/// The optimized executable: from a promotion, or the only one of a single-tier compile.
	tier1,
};

/// Where the promotion of a module to tier 1 stands.
enum class PromotionState : uint8_t {
	/// No promotion is under way: the module has none (single-tier), or the policy is `manual` and `promote()` was not
	/// called yet.
	none,
	/// Waiting for a worker of the engine's promotion service.
	queued,
	/// A worker compiles the tier-1 executable.
	running,
	/// The tier-1 executable runs the module (or, if it was ready before tier 0, tier 0 was dropped).
	done,
	/// The tier-1 compile threw; the module stays on tier 0. `PromotionStatus::error` has the message. Not retried.
	failed,
	/// The module was dropped, or the engine shut down, before the promotion finished; nothing was installed.
	cancelled,
	/// The admission cap (`engine.tiered.maxQueuedPromotions`) was reached: the module stays on tier 0.
	skipped,
};

/// The result of `CompiledModule::promotionStatus()`.
struct PromotionStatus {
	PromotionState state = PromotionState::none;
	/// The message of the exception of a `failed` promotion; empty otherwise.
	std::string error;
};

namespace details {

/// The part of a module's promotion that the compiler implements: the module asks it to queue the job (`promote()`),
/// and destroying it, with the module's state, cancels the job. Set once by the compiler before `compileModule`
/// returns.
class PromotionControl {
public:
	virtual ~PromotionControl() = default;
	/// Queues the promotion if nobody has yet. Returns true if this call queued it, false if the promotion was
	/// requested before, was skipped, or the engine is gone.
	virtual bool promote() = 0;
};

struct ModuleState;

/**
 * @brief The version counter of a ModuleState, with a notification when the executable changes.
 *
 * Every path that installs an executable (CompiledModule::setExecutable, the tier-1 promotion) bumps the version after
 * it has stored the executable, while holding the state's mutex exclusively. That makes the bump the single point where
 * the module registry (docs/plans/nautilus-0.4/13-nir-spec.md §4.4.2) learns that importers must be pointed at new
 * code: it registers an observer on the state, and fetch_add calls it.
 */
class ModuleVersion {
public:
	explicit ModuleVersion(ModuleState& owner) noexcept : owner_(owner) {
	}

	uint64_t load(std::memory_order order = std::memory_order_seq_cst) const noexcept {
		return value_.load(order);
	}

	/// Increments the version, then calls the owner's install observers. The caller holds the state's mutex
	/// exclusively: an observer reads `ModuleState::executable` directly and must not lock the mutex.
	inline uint64_t fetch_add(uint64_t delta, std::memory_order order = std::memory_order_seq_cst);

private:
	std::atomic<uint64_t> value_ {0};
	ModuleState& owner_;
};

/**
 * @brief Shared mutable state backing a CompiledModule and all its ModuleFunction handles.
 *
 * Holds the current executable (may be null for interpreted mode) and the interpreted
 * function map. A monotonically increasing version counter allows ModuleFunction handles
 * to detect when they need to re-resolve their cached implementation.
 *
 * Thread-safety: a shared_mutex protects the executable pointer. The version counter
 * is atomic so the fast path (version check in operator()) requires no lock.
 */
struct ModuleState {
	/// The module's membership in its engine's module registry and the exporters it is linked to (a
	/// link::ModuleHandle, type-erased). Declared first so that it is destroyed last: the module's code goes before the
	/// modules it calls.
	std::shared_ptr<void> link;
	std::shared_ptr<compiler::Executable> executable;
	std::unordered_map<std::string, std::any> interpretedFunctions;
	ModuleVersion version {*this};
	mutable std::shared_mutex mutex;
	/// Called by ModuleVersion::fetch_add; added and read with the mutex held exclusively.
	std::vector<std::function<void(ModuleState&)>> installObservers;

	/// The tier of `executable`; written under `mutex` together with it. A module with an executable and no
	/// promotion runs its only tier, hence the default.
	std::atomic<Tier> tier {Tier::tier1};

	/// The promotion status and the signal for the waiters of `CompiledModule::waitForPromotion`. Separate from
	/// `mutex` so that a waiter never holds up a call that re-resolves.
	mutable std::mutex promotionMutex;
	mutable std::condition_variable promotionChanged;
	PromotionStatus promotion; // guarded by promotionMutex

	/// Declared last: destroyed first, so a cancelled job never sees a half-destroyed state.
	std::unique_ptr<PromotionControl> promotionControl;

	/// Publishes a new promotion status and wakes the waiters.
	void setPromotion(PromotionState state, std::string error = {}) {
		{
			const std::lock_guard<std::mutex> lock(promotionMutex);
			promotion.state = state;
			promotion.error = std::move(error);
		}
		promotionChanged.notify_all();
	}
};

uint64_t ModuleVersion::fetch_add(uint64_t delta, std::memory_order order) {
	const auto previous = value_.fetch_add(delta, order);
	for (auto& observer : owner_.installObservers) {
		observer(owner_);
	}
	return previous;
}

} // namespace details

/**
 * @brief A stable, lightweight function handle returned by CompiledModule::getFunction().
 *
 * ModuleFunction is callable via operator() and caches its underlying implementation
 * (compiled or interpreted). When the owning CompiledModule's executable is swapped
 * via setExecutable(), all outstanding ModuleFunction handles automatically pick up
 * the new implementation on their next call — no re-fetching required.
 *
 * Thread-safety: operator() is safe to call concurrently with setExecutable(), and a
 * single handle may be shared by many threads. The hot path (cached version matches)
 * is lock-free: two atomic loads. Re-resolution takes a shared lock, setExecutable
 * takes an exclusive lock.
 *
 * Lifetime: a handle keeps every executable it has resolved against alive until the
 * handle (and, for a copy, that copy) is destroyed, not just while a call is running.
 * Swapping in a new executable therefore does not free the old one's JIT'd code
 * while any handle that has called into it still exists. Retention is bounded by
 * the number of distinct executables the handle has seen: swapping the same
 * executables back and forth adds nothing.
 *
 * @tparam Signature Raw function signature, e.g. int32_t(int32_t, int32_t)
 */
template <typename Signature>
class ModuleFunction;

template <typename R, typename... Args>
class ModuleFunction<R(Args...)> {
	using ValReturnType = std::conditional_t<std::is_void_v<R>, void, val<R>>;
	using ValFuncType = std::function<ValReturnType(val<Args>...)>;
	using ImplType = std::function<R(Args...)>;

	/// One resolved implementation, keyed by the executable it dispatches into
	/// (null for interpreted mode). The functor captures a shared_ptr to that
	/// executable, so the key's address cannot be reused by another executable
	/// while the entry exists.
	struct Entry {
		const compiler::Executable* executable;
		std::unique_ptr<const ImplType> impl;
	};

	/// Every resolved implementation is immutable once published, and superseded
	/// implementations are retired rather than destroyed: they stay alive, together
	/// with the executable they capture, until the cache itself is destroyed. A
	/// thread that shares this handle and is still running inside an older
	/// implementation when another thread re-resolves after a swap therefore never
	/// has that functor or its JIT'd code freed under it (see issue #506). This keeps
	/// the hot path free of locks and reference counting: a version check plus one
	/// acquire load of the current implementation.
	///
	/// Re-resolving against an executable this cache has already seen reuses its
	/// entry, so the cache holds at most one entry per distinct executable (plus one
	/// for interpreted mode), however often the module swaps back and forth. The
	/// cost is that every executable this handle has resolved against lives as long
	/// as the handle does, even after the module has moved on to a newer one.
	struct Cache {
		std::atomic<const ImplType*> current {nullptr};
		std::atomic<uint64_t> version {~0ULL}; // force first resolve
		std::mutex mutex;
		std::vector<Entry> entries; // guarded by mutex
	};

	std::shared_ptr<details::ModuleState> state_;
	std::string name_;
	std::shared_ptr<Cache> cache_;

	void resolve() const {
		std::unique_lock<std::mutex> resolveLock(cache_->mutex);
		// Double-check after acquiring the local lock.
		if (cache_->version.load(std::memory_order_relaxed) == state_->version.load(std::memory_order_relaxed)) {
			return;
		}
		std::shared_lock<std::shared_mutex> lock(state_->mutex);
		const compiler::Executable* key = state_->executable.get();
		const ImplType* reused = nullptr;
		for (const auto& entry : cache_->entries) {
			if (entry.executable == key) {
				reused = entry.impl.get();
				break;
			}
		}
		if (reused != nullptr) {
			cache_->current.store(reused, std::memory_order_release);
		} else {
			cache_->entries.push_back({key, makeImpl()});
			cache_->current.store(cache_->entries.back().impl.get(), std::memory_order_release);
		}
		cache_->version.store(state_->version.load(std::memory_order_relaxed), std::memory_order_release);
	}

	/// Builds the implementation for the module's current executable (or its
	/// interpreted function). Caller holds cache_->mutex and a shared lock on
	/// state_->mutex.
	std::unique_ptr<const ImplType> makeImpl() const {
		std::unique_ptr<const ImplType> impl;
		if (state_->executable) {
			// Take our own reference to the executable so it outlives a concurrent
			// swap: the impl published below captures it, keeping the executable (and its
			// JIT'd code) alive for as long as this cached impl is reachable, even
			// after ModuleState::executable itself has moved on to a newer tier.
			// The version check in operator() is unlocked and happens *before* the
			// call, so ownership has to be held across the call itself, not just
			// checked ahead of it (see issue #449).
			std::shared_ptr<compiler::Executable> executable = state_->executable;
			// A NativeUnwind backend (MLIR), or a captured-host-rethrow function
			// with no exceptional call sites of its own, can be called through
			// the raw function pointer: neither ever touches the ExceptionFrame
			// machinery. Everything else must go through the Invocable wrapper
			// so the frame is pushed/rethrown around the call.
			if (executable->hasInvocableFunctionPtr() &&
			    executable->getExceptionPropagationMode(name_) == compiler::ExceptionPropagationMode::NativeUnwind) {
				auto* fptr = reinterpret_cast<R (*)(Args...)>(executable->getInvocableFunctionPtr(name_));
				// See NAUTILUS_NO_SANITIZE_FUNCTION in Executable.hpp: fptr is a JIT
				// entry point with no UBSan type-hash prologue, so the indirect call
				// through it must be exempted from -fsanitize=function.
				impl = std::make_unique<const ImplType>(
				    [executable, fptr](Args... args)
				        NAUTILUS_NO_SANITIZE_FUNCTION -> R { return fptr(std::forward<Args>(args)...); });
			} else {
				auto invocable = std::make_shared<compiler::Executable::Invocable<R, Args...>>(
				    executable->getInvocableMember<R, Args...>(name_));
				impl = std::make_unique<const ImplType>(
				    [executable, invocable](Args... args) -> R { return (*invocable)(std::forward<Args>(args)...); });
			}
		} else {
			try {
				auto typedFunc = std::any_cast<ValFuncType>(state_->interpretedFunctions.at(name_));
				impl = std::make_unique<const ImplType>([typedFunc = std::move(typedFunc)](Args... args) -> R {
					if constexpr (std::is_void_v<R>) {
						typedFunc(make_value(args)...);
					} else {
						auto result = typedFunc(make_value(args)...);
						return nautilus::details::RawValueResolver<R>::getRawValue(result);
					}
				});
			} catch (const std::bad_any_cast&) {
				throw std::runtime_error("ModuleFunction type mismatch for '" + name_ + "'");
			}
		}
		return impl;
	}

public:
	ModuleFunction(std::shared_ptr<details::ModuleState> state, std::string name)
	    : state_(std::move(state)), name_(std::move(name)), cache_(std::make_shared<Cache>()) {
	}

	/// Each copy gets its own cache, so copies resolve independently of one another.
	ModuleFunction(const ModuleFunction& other)
	    : state_(other.state_), name_(other.name_), cache_(std::make_shared<Cache>()) {
	}

	ModuleFunction(ModuleFunction&&) noexcept = default;
	ModuleFunction& operator=(const ModuleFunction& other) {
		if (this != &other) {
			state_ = other.state_;
			name_ = other.name_;
			cache_ = std::make_shared<Cache>();
		}
		return *this;
	}
	ModuleFunction& operator=(ModuleFunction&&) noexcept = default;

	R operator()(Args... args) const {
		if (cache_->version.load(std::memory_order_acquire) != state_->version.load(std::memory_order_acquire)) {
			resolve();
		}
		const ImplType* impl = cache_->current.load(std::memory_order_acquire);
		return (*impl)(std::forward<Args>(args)...);
	}
};

/**
 * @brief Result of compiling a NautilusModule. Owns the compiled executable and provides
 * typed access to individual functions by name.
 *
 * Supports two modes:
 * - Compiled: backed by a compiler::Executable (when compilation is enabled)
 * - Interpreted: directly invokes the original val<T>-based functions (when compilation is disabled)
 *
 * The implementation can be swapped at runtime via setExecutable(). All ModuleFunction
 * handles previously returned by getFunction() will automatically use the new implementation.
 *
 * Thread-safety: setExecutable() is safe to call while ModuleFunction handles are being invoked
 * from other threads. The swap is protected by a shared_mutex internally.
 */
class CompiledModule {
public:
	/// Compiled mode: owns the executable produced by a backend.
	explicit CompiledModule(std::unique_ptr<compiler::Executable> executable,
	                        std::unordered_map<std::string, std::any> interpretedFunctions)
	    : state_(std::make_shared<details::ModuleState>()) {
		state_->executable = std::move(executable);
		state_->interpretedFunctions = std::move(interpretedFunctions);
	}

	/// Interpreted mode: no compilation, functions are invoked directly.
	explicit CompiledModule(std::unordered_map<std::string, std::any> interpretedFunctions)
	    : state_(std::make_shared<details::ModuleState>()) {
		state_->interpretedFunctions = std::move(interpretedFunctions);
	}

	/// Construct from pre-built module state (used by NautilusModule for tiered compilation).
	explicit CompiledModule(std::shared_ptr<details::ModuleState> state) : state_(std::move(state)) {
	}

	CompiledModule(const CompiledModule&) = delete;
	CompiledModule(CompiledModule&&) noexcept = default;
	CompiledModule& operator=(const CompiledModule&) = delete;
	CompiledModule& operator=(CompiledModule&&) noexcept = default;

	/**
	 * @brief Retrieve a stable function handle by name.
	 *
	 * The returned ModuleFunction is a lightweight, copyable handle. It caches
	 * its dispatch target and automatically refreshes when the module's executable
	 * is swapped via setExecutable().
	 *
	 * @tparam Signature Raw function signature, e.g. int32_t(int32_t, int32_t)
	 * @param name The function name as registered in the module
	 * @return ModuleFunction handle callable via operator()
	 */
	template <typename Signature>
	ModuleFunction<Signature> getFunction(const std::string& name) {
		return ModuleFunction<Signature>(state_, name);
	}

	/**
	 * @brief Swap the underlying executable at runtime.
	 *
	 * After this call, all ModuleFunction handles will use the new executable
	 * on their next invocation. Pass nullptr to revert to interpreted mode.
	 *
	 * Thread-safe: can be called while ModuleFunction handles are invoked from other threads.
	 *
	 * The previous executable is not necessarily freed by this call: every
	 * ModuleFunction handle that has resolved against it keeps it alive until that
	 * handle is destroyed (see the ModuleFunction lifetime notes). Destroy or
	 * replace such handles to reclaim its memory.
	 *
	 * @param executable The new executable (or nullptr for interpreted mode). Accepts a
	 * unique_ptr (implicitly converted, e.g. from a fresh compile) or a shared_ptr
	 * (e.g. one previously obtained from releaseExecutable()).
	 */
	void setExecutable(std::shared_ptr<compiler::Executable> executable) {
		std::unique_lock<std::shared_mutex> lock(state_->mutex);
		state_->executable = std::move(executable);
		state_->version.fetch_add(1, std::memory_order_release);
	}

	const compiler::Executable* getExecutable() const {
		return state_->executable.get();
	}

	/**
	 * @brief Statistics recorded while compiling this module's executable.
	 *
	 * Returns a null shared_ptr in interpreted mode (no executable) or if
	 * the active executable was produced by a path that did not attach
	 * statistics. When tiered compilation swaps in a tier‑1 executable,
	 * the pointer automatically reflects the new executable's stats.
	 */
	std::shared_ptr<const compiler::CompilationStatistics> getStatistics() const {
		std::shared_lock<std::shared_mutex> lock(state_->mutex);
		if (!state_->executable) {
			return nullptr;
		}
		return state_->executable->getCompilationStatistics();
	}

	/**
	 * @brief Release this module's reference to the underlying executable.
	 * Reverts this module to interpreted mode and returns the executable.
	 *
	 * Returns a shared_ptr rather than a unique_ptr: every ModuleFunction handle that
	 * resolved against this executable while it was active holds its own reference
	 * until the handle is destroyed (see the ModuleFunction lifetime notes), so this
	 * call cannot promise exclusive ownership of the result.
	 */
	std::shared_ptr<compiler::Executable> releaseExecutable() {
		std::unique_lock<std::shared_mutex> lock(state_->mutex);
		auto exe = std::move(state_->executable);
		state_->version.fetch_add(1, std::memory_order_release);
		return exe;
	}

	/**
	 * @brief The tier that runs the module now (11-tiered-compilation.md §4.3).
	 *
	 * `interpreted` while the module has no executable, `tier0` after the fast compile, `tier1` once a promotion
	 * installed the optimized executable (or from the start, for a single-tier compile).
	 */
	[[nodiscard]] Tier tier() const {
		std::shared_lock<std::shared_mutex> lock(state_->mutex);
		return state_->executable ? state_->tier.load(std::memory_order_relaxed) : Tier::interpreted;
	}

	/// Where the promotion to tier 1 stands, with the error message of a failed one.
	[[nodiscard]] PromotionStatus promotionStatus() const {
		const std::lock_guard<std::mutex> lock(state_->promotionMutex);
		return state_->promotion;
	}

	/**
	 * @brief Waits until this module's promotion is no longer queued or running.
	 *
	 * Waits for this module only. Returns at once if no promotion is pending (single-tier compile, policy `manual`
	 * before `promote()`, or a promotion that ended). Returns false if @p timeout passed first.
	 */
	bool waitForPromotion(std::chrono::milliseconds timeout = std::chrono::milliseconds::max()) const {
		std::unique_lock<std::mutex> lock(state_->promotionMutex);
		const auto settled = [this] {
			return state_->promotion.state != PromotionState::queued &&
			       state_->promotion.state != PromotionState::running;
		};
		if (timeout == std::chrono::milliseconds::max()) {
			state_->promotionChanged.wait(lock, settled);
			return true;
		}
		return state_->promotionChanged.wait_for(lock, timeout, settled);
	}

	/**
	 * @brief Requests the promotion to tier 1 now (policy `manual`, `engine.tiered.promotion`).
	 *
	 * Queues one promotion job however many threads call it at once. Returns true if this call queued it, false if
	 * it was requested already, the policy is `eager` (the job was queued by `compile()`), the module has nothing to
	 * promote to, or the engine was destroyed. A no-op after that; the outcome shows in `promotionStatus()`.
	 */
	bool promote() {
		return state_->promotionControl != nullptr && state_->promotionControl->promote();
	}

	/// Get the shared module state (used by NautilusModule for tiered compilation).
	std::shared_ptr<details::ModuleState> getState() const {
		return state_;
	}

private:
	std::shared_ptr<details::ModuleState> state_;
};

// ---- modules and linking (docs/plans/nautilus-0.4/13-nir-spec.md §4.4) ---------------------------------------------

namespace details {

/// A function a named module exports: its name and its signature as the type words of the NIR (the result, then the
/// parameters; 13 §4.5.1), and the guarantees it declares for its callers (13 §4.6.2).
struct ExportDeclaration {
	std::string name;
	std::vector<uint32_t> signature;
	FunctionAttributes attributes;
};

/// A function of another module that this module calls, as `NautilusModule::importFunction` declares it.
struct ImportDeclaration {
	std::string module;
	std::string function;
	/// The signature the importer is built against: the type words of the result, then of the parameters.
	std::vector<uint32_t> signature;
	/// What the importer assumes the exporter guarantees; the link fails if the export does not guarantee it.
	FunctionAttributes attributes;
	/// `compiler::captureThrowingCall<R, Args...>` and `compiler::callInExceptionFrame<R, Args...>` for the C++
	/// signature of the import. They bridge the exception propagation of the exporter (whatever tier it is in) to
	/// that of the importer's backend.
	void* captureThunk = nullptr;
	void* frameThunk = nullptr;
};

/// The type word of a traced type at a native boundary (13 §4.5.1).
uint32_t signatureWord(Type type);

template <typename T>
uint32_t signatureWordOf() {
	if constexpr (std::is_void_v<T>) {
		return signatureWord(Type::v);
	} else {
		return signatureWord(tracing::TypeResolver<T>::to_type());
	}
}

/// The declaration of an exported function whose val-typed result is @p R (void or a `val<T>`) and whose parameters
/// have the raw types @p Args.
template <typename R, typename = void>
struct ExportedResult {
	using type = void;
};

template <typename R>
struct ExportedResult<R, std::void_t<typename R::raw_type>> {
	using type = typename R::raw_type;
};

template <typename R, typename... Args>
ExportDeclaration makeExportDeclaration(std::string name) {
	return ExportDeclaration {
	    std::move(name),
	    {signatureWordOf<typename ExportedResult<R>::type>(), signatureWordOf<typename val<Args>::raw_type>()...},
	    {}};
}

/// The declaration of an import of the raw signature @p Signature with its typed exception thunks.
template <typename Signature>
struct ImportBuilder;

template <typename R, typename... Args>
struct ImportBuilder<R(Args...)> {
	static ImportDeclaration declaration(std::string module, std::string function, FunctionAttributes attributes) {
		ImportDeclaration declaration;
		declaration.module = std::move(module);
		declaration.function = std::move(function);
		declaration.signature = {signatureWordOf<R>(), signatureWordOf<Args>()...};
		declaration.attributes = attributes;
		declaration.captureThunk = reinterpret_cast<void*>(&compiler::captureThrowingCall<R, Args...>);
		declaration.frameThunk = reinterpret_cast<void*>(&compiler::callInExceptionFrame<R, Args...>);
		return declaration;
	}
};

/// The NIR guarantee bits of `attributes` and back (13 §4.6.2).
uint32_t attributeBits(const FunctionAttributes& attributes);
FunctionAttributes attributesFromBits(uint32_t bits);

/// Checks `declaration` against the exporter if it is already compiled in the registry (the early check of
/// `importFunction`, 13 §4.4.4) and returns the guarantees of the export, or nothing if the exporter is not compiled
/// yet. Throws link::LinkError on a signature mismatch.
std::optional<FunctionAttributes> checkImport(const std::shared_ptr<link::ModuleRegistry>& registry,
                                              const ImportDeclaration& declaration);

/// The state of the compiled module `module` in the registry (to call it from interpreted code). Throws
/// link::LinkError if there is none.
std::shared_ptr<ModuleState> importedState(const std::shared_ptr<link::ModuleRegistry>& registry,
                                           const std::string& module);

/**
 * @brief The linking of one `NautilusModule::compile()` (13 §4.4.1-§4.4.2).
 *
 * Built -> linked -> compiled: the constructor reserves the module's name in the registry of its engine (a name refers
 * to one module, so a name that is taken fails here, before any work is done); the compilation pipeline calls back into
 * the session after tracing to resolve the imports against the registry and to publish the import slots to the
 * backends; `commit` registers the module's exports once its executable is installed. A session that is destroyed
 * without a commit (the compile failed) gives the name back. The session is current on the thread that compiles.
 */
class LinkSession {
public:
	LinkSession(std::shared_ptr<link::ModuleRegistry> registry, std::string moduleName,
	            std::vector<ExportDeclaration> exports, std::vector<ImportDeclaration> imports);
	~LinkSession();
	LinkSession(const LinkSession&) = delete;
	LinkSession& operator=(const LinkSession&) = delete;

	/// Registers the module as compiled, with `state` as the code importers call. Later promotions of the state point
	/// the import slots of its importers at the new code.
	void commit(const std::shared_ptr<ModuleState>& state);

	/// The session of the compile running on this thread, or null.
	static LinkSession* current() noexcept;

	struct Impl;

private:
	friend struct ::nautilus::link::SessionAccess;
	std::unique_ptr<Impl> impl_;
	LinkSession* previous_ = nullptr;
};

} // namespace details

/**
 * @brief A function of another, compiled module, callable from a traced function: `NautilusModule::importFunction`.
 *
 * Traced, a call is recorded as a call to an imported callee that goes through an import slot (the exporter's current
 * entry point), so the exporter can be promoted to another tier without recompiling the importer. Outside tracing (an
 * engine that interprets) the call runs the exporter's function.
 *
 * @tparam Signature Raw function signature, e.g. uint64_t(uint64_t)
 */
template <typename Signature>
class ImportedFunction;

template <typename R, typename... Args>
class ImportedFunction<R(Args...)> {
	using ValReturnType = std::conditional_t<std::is_void_v<R>, void, val<R>>;

public:
	ImportedFunction(std::shared_ptr<link::ModuleRegistry> registry, std::string module, std::string function,
	                 FunctionAttributes attributes)
	    : registry_(std::move(registry)), module_(std::move(module)), function_(std::move(function)),
	      attributes_(attributes) {
	}

	[[nodiscard]] const std::string& module() const {
		return module_;
	}
	[[nodiscard]] const std::string& function() const {
		return function_;
	}

	ValReturnType operator()(val<Args>... args) const {
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			auto references = getArgumentReferences(args...);
			if constexpr (std::is_void_v<R>) {
				tracing::traceImportedCall(module_, function_, Type::v, references, attributes_);
				return;
			} else {
				auto& result = tracing::traceImportedCall(module_, function_, tracing::TypeResolver<R>::to_type(),
				                                          references, attributes_);
				return val<R>(result);
			}
		}
#endif
		// not tracing: the exporter runs in the interpreter or as compiled code, as any caller would call it
		ModuleFunction<R(Args...)> target(details::importedState(registry_, module_), function_);
		if constexpr (std::is_void_v<R>) {
			target(::nautilus::details::RawValueResolver<Args>::getRawValue(args)...);
		} else {
			return val<R>(target(::nautilus::details::RawValueResolver<Args>::getRawValue(args)...));
		}
	}

private:
	std::shared_ptr<link::ModuleRegistry> registry_;
	std::string module_;
	std::string function_;
	FunctionAttributes attributes_;
};

// Forward-declare for inline definition after NautilusEngine
class NautilusModule;

} // namespace nautilus::engine
