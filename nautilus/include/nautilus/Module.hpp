
#pragma once

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
#include <shared_mutex>
#include <stdexcept>
#include <unordered_map>

#ifdef ENABLE_TRACING
#include "nautilus/CompilableFunction.hpp"
#endif

namespace nautilus::engine {

namespace details {

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
	std::unique_ptr<compiler::Executable> executable;
	std::unordered_map<std::string, std::any> interpretedFunctions;
	std::atomic<uint64_t> version {0};
	mutable std::shared_mutex mutex;
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
 * Thread-safety: operator() is safe to call concurrently with setExecutable().
 * The hot path (cached version matches) is lock-free — just an atomic load.
 * Re-resolution takes a shared lock, setExecutable takes an exclusive lock.
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

	std::shared_ptr<details::ModuleState> state_;
	std::string name_;
	mutable ImplType cachedImpl_;
	mutable uint64_t cachedVersion_ = ~0ULL; // force first resolve

	void resolve() const {
		std::shared_lock<std::shared_mutex> lock(state_->mutex);
		if (state_->executable) {
			if (state_->executable->hasInvocableFunctionPtr()) {
				auto* fptr = reinterpret_cast<R (*)(Args...)>(state_->executable->getInvocableFunctionPtr(name_));
				cachedImpl_ = fptr;
			} else {
				auto invocable = std::make_shared<compiler::Executable::Invocable<R, Args...>>(
				    state_->executable->getInvocableMember<R, Args...>(name_));
				cachedImpl_ = [invocable](Args... args) -> R {
					return (*invocable)(std::forward<Args>(args)...);
				};
			}
		} else {
			try {
				auto typedFunc = std::any_cast<ValFuncType>(state_->interpretedFunctions.at(name_));
				cachedImpl_ = [typedFunc = std::move(typedFunc)](Args... args) -> R {
					if constexpr (std::is_void_v<R>) {
						typedFunc(make_value(args)...);
					} else {
						auto result = typedFunc(make_value(args)...);
						return nautilus::details::RawValueResolver<R>::getRawValue(result);
					}
				};
			} catch (const std::bad_any_cast&) {
				throw std::runtime_error("ModuleFunction type mismatch for '" + name_ + "'");
			}
		}
		cachedVersion_ = state_->version.load(std::memory_order_relaxed);
	}

public:
	ModuleFunction(std::shared_ptr<details::ModuleState> state, std::string name)
	    : state_(std::move(state)), name_(std::move(name)) {
	}

	R operator()(Args... args) const {
		if (cachedVersion_ != state_->version.load(std::memory_order_acquire)) {
			resolve();
		}
		return cachedImpl_(std::forward<Args>(args)...);
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
	 * @param executable The new executable (or nullptr for interpreted mode)
	 */
	void setExecutable(std::unique_ptr<compiler::Executable> executable) {
		std::unique_lock<std::shared_mutex> lock(state_->mutex);
		state_->executable = std::move(executable);
		state_->version.fetch_add(1, std::memory_order_release);
	}

	const compiler::Executable* getExecutable() const {
		return state_->executable.get();
	}

	/**
	 * @brief Release ownership of the underlying executable.
	 * Reverts this module to interpreted mode and returns the executable.
	 */
	std::unique_ptr<compiler::Executable> releaseExecutable() {
		std::unique_lock<std::shared_mutex> lock(state_->mutex);
		auto exe = std::move(state_->executable);
		state_->version.fetch_add(1, std::memory_order_release);
		return exe;
	}

private:
	std::shared_ptr<details::ModuleState> state_;
};

// Forward-declare for inline definition after NautilusEngine
class NautilusModule;

} // namespace nautilus::engine
