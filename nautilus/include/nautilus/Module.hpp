
#pragma once

#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/JITCompiler.hpp"
#include "nautilus/config.hpp"
#include "nautilus/core.hpp"
#include "nautilus/options.hpp"
#include <any>
#include <atomic>
#include <functional>
#include <list>
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

namespace nautilus::engine {

namespace details {

/// The IR types of one compiled function: its result and its parameters.
struct FunctionTypeSignature {
	Type result = Type::v;
	std::vector<Type> params;
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
	std::shared_ptr<compiler::Executable> executable;
	std::unordered_map<std::string, std::any> interpretedFunctions;
	std::atomic<uint64_t> version {0};
	mutable std::shared_mutex mutex;
	/// The graph the module was compiled from, kept only when the module was
	/// compiled with `engine.retainIR` (or built from IR). Backends treat a
	/// graph as read-only, so it stays valid after compilation.
	std::shared_ptr<compiler::ir::IRGraph> ir;
	/// The options the retained graph was compiled with.
	ModuleOptions options;
	/// Per-function IR signatures. Set for a module built from IR, where no C++
	/// signature ties a caller to the function's real types; getFunction()
	/// checks the requested signature against it. Empty means unchecked.
	std::unordered_map<std::string, FunctionTypeSignature> signatures;
};

/// The IR type of the raw C++ type @p T, or nullopt for a type IR signatures
/// do not describe (such as an enum, whose IR type depends on its underlying type).
template <typename T>
constexpr std::optional<Type> rawIRType() {
	if constexpr (std::is_void_v<T>) {
		return Type::v;
	} else if constexpr (requires { tracing::TypeResolver<T>::to_type(); }) {
		return tracing::TypeResolver<T>::to_type();
	} else {
		return std::nullopt;
	}
}

/// Unpacks a raw function signature for the check below.
template <typename Signature>
struct SignatureChecker;

template <typename R, typename... Args>
struct SignatureChecker<R(Args...)> {
	/// Throws if a function requested as `R(Args...)` cannot be the function
	/// @p name of @p state. A module without known signatures (anything traced
	/// in-process) is not checked.
	static void check(const ModuleState& state, const std::string& name) {
		if (state.signatures.empty()) {
			return;
		}
		const auto it = state.signatures.find(name);
		if (it == state.signatures.end()) {
			throw std::invalid_argument("module has no function named '" + name + "'");
		}
		const auto& expected = it->second;
		const std::optional<Type> requestedParams[] = {rawIRType<Args>()..., std::nullopt};
		bool matches =
		    rawIRType<R>().value_or(expected.result) == expected.result && sizeof...(Args) == expected.params.size();
		for (size_t i = 0; matches && i < sizeof...(Args); ++i) {
			matches = requestedParams[i].value_or(expected.params[i]) == expected.params[i];
		}
		if (!matches) {
			std::string actual = std::string(toString(expected.result)) + "(";
			for (size_t i = 0; i < expected.params.size(); ++i) {
				actual += (i > 0 ? ", " : "") + std::string(toString(expected.params[i]));
			}
			actual += ")";
			throw std::invalid_argument("function '" + name + "' has IR signature " + actual +
			                            ", which the requested signature does not match");
		}
	}
};

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
		details::SignatureChecker<Signature>::check(*state_, name);
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
	 * @brief The IR graph this module was compiled from, or nullptr.
	 *
	 * Only kept when the module was compiled with the `engine.retainIR` option
	 * (or built from IR via NautilusEngine::compileIR). It is the graph the
	 * backends compiled, i.e. after the IR pass pipeline, and must not be
	 * modified: a background tier-1 compile may still be reading it.
	 */
	std::shared_ptr<const compiler::ir::IRGraph> getIR() const {
		std::shared_lock<std::shared_mutex> lock(state_->mutex);
		return state_->ir;
	}

	/// The options the retained IR was compiled with (empty without retained IR).
	ModuleOptions getCompileOptions() const {
		std::shared_lock<std::shared_mutex> lock(state_->mutex);
		return state_->options;
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

	/// Get the shared module state (used by NautilusModule for tiered compilation).
	std::shared_ptr<details::ModuleState> getState() const {
		return state_;
	}

private:
	std::shared_ptr<details::ModuleState> state_;
};

// Forward-declare for inline definition after NautilusEngine
class NautilusModule;

} // namespace nautilus::engine
