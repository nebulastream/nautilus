#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/MultiTierJitCompiler.hpp"
#include "nautilus/options.hpp"
#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <string>

namespace nautilus::compiler {

class CompilationBackendRegistry;

/**
 * @brief Executable that manages tier transitions with a stable function pointer.
 *
 * This class provides a stable function pointer that dispatches to different
 * tier executables based on invocation count and compilation status.
 *
 * Thread safety:
 * - Invocation counting is atomic and thread-safe
 * - Tier transitions are protected by mutex
 * - Multiple threads can safely invoke the same executable
 */
class MultiTierExecutable : public Executable {
public:
	/**
	 * @brief Construct a multi-tier executable.
	 *
	 * @param tier1Executable The tier 1 (low latency) executable
	 * @param wrapperFunction Original wrapper function for tier 2 compilation
	 * @param options Configuration options
	 * @param backends Backend registry for tier 2 compilation
	 * @param tier2Threshold Number of invocations before tier 2 compilation
	 * @param tier2BackendName Name of the tier 2 backend
	 */
	MultiTierExecutable(std::unique_ptr<Executable> tier1Executable,
	                    MultiTierJitCompiler::wrapper_function wrapperFunction, engine::Options options,
	                    const CompilationBackendRegistry* backends, uint64_t tier2Threshold,
	                    std::string tier2BackendName);

	~MultiTierExecutable() override;

	/**
	 * @brief Get a stable function pointer that dispatches to the current tier.
	 *
	 * This function pointer remains valid throughout the lifetime of this object
	 * and across all tier transitions. It internally dispatches to the appropriate
	 * tier executable based on current state.
	 *
	 * @param member Function name (typically "execute")
	 * @return Stable function pointer
	 */
	void* getInvocableFunctionPtr(const std::string& member) override;

	/**
	 * @brief Check if this executable supports function pointers.
	 * @return Always true for multi-tier executables
	 */
	bool hasInvocableFunctionPtr() override;

	/**
	 * @brief Get a generic invocable for this executable.
	 * @param member Function name
	 * @return Generic invocable wrapper
	 */
	std::unique_ptr<GenericInvocable> getGenericInvocable(const std::string& member) override;

	/**
	 * @brief Get the current tier (1 or 2).
	 * @return Current tier number
	 */
	uint8_t getCurrentTier() const {
		return currentTier.load(std::memory_order_acquire);
	}

	/**
	 * @brief Get the current invocation count.
	 * @return Number of times this executable has been invoked
	 */
	uint64_t getInvocationCount() const {
		return invocationCount.load(std::memory_order_relaxed);
	}

	/**
	 * @brief Check if tier 2 compilation is in progress.
	 * @return true if tier 2 is being compiled
	 */
	bool isTier2Compiling() const {
		return tier2Compiling.load(std::memory_order_acquire);
	}

private:
	/**
	 * @brief Increment invocation counter and trigger tier 2 compilation if needed.
	 *
	 * This method is called before each invocation to track usage and potentially
	 * start tier 2 compilation.
	 */
	void onInvocation();

	/**
	 * @brief Compile tier 2 executable asynchronously.
	 *
	 * This method runs in a background thread and compiles the function using
	 * the tier 2 backend. When complete, it switches the active tier.
	 */
	void compileTier2();

	/**
	 * @brief Switch from tier 1 to tier 2.
	 *
	 * This method is called when tier 2 compilation completes. It atomically
	 * switches the active executable to tier 2.
	 *
	 * @param newExecutable The compiled tier 2 executable
	 */
	void switchToTier2(std::unique_ptr<Executable> newExecutable);

	/**
	 * @brief Get the currently active executable.
	 * @return Pointer to the active tier executable
	 */
	Executable* getActiveExecutable();

	// Tier 1 executable (low latency)
	std::unique_ptr<Executable> tier1Executable;

	// Tier 2 executable (high performance, nullptr until compiled)
	std::unique_ptr<Executable> tier2Executable;

	// Original wrapper function for tier 2 compilation
	MultiTierJitCompiler::wrapper_function wrapperFunction;

	// Configuration options
	engine::Options options;

	// Backend registry for tier 2 compilation
	const CompilationBackendRegistry* backends;

	// Threshold for tier 2 compilation
	uint64_t tier2Threshold;

	// Name of tier 2 backend
	std::string tier2BackendName;

	// Current tier (1 or 2)
	std::atomic<uint8_t> currentTier;

	// Invocation counter
	std::atomic<uint64_t> invocationCount;

	// Flag indicating tier 2 compilation in progress
	std::atomic<bool> tier2Compiling;

	// Future for async tier 2 compilation
	std::future<void> tier2CompilationFuture;

	// Mutex for tier transition
	std::mutex tierTransitionMutex;

	// Cache for function pointers by member name
	std::map<std::string, void*> functionPointerCache;
	std::mutex functionPointerCacheMutex;

	// GenericInvocable implementation for multi-tier
	class MultiTierInvocable;
	friend class MultiTierInvocable;
};

} // namespace nautilus::compiler
