#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/MultiTierJitCompiler.hpp"
#include "nautilus/options.hpp"
#include "nautilus/tracing/Types.hpp"
#include <atomic>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <string>

namespace nautilus::compiler {

class CompilationBackendRegistry;

/**
 * @brief Executable that manages tier transitions.
 *
 * Thread safety:
 * - Invocation counting is atomic
 * - Tier transitions and type metadata are protected by mutex
 * - Multiple threads can safely invoke the same executable
 */
class MultiTierExecutable : public Executable {
public:
	MultiTierExecutable(std::unique_ptr<Executable> tier1Executable,
	                    MultiTierJitCompiler::wrapper_function wrapperFunction, engine::Options options,
	                    const CompilationBackendRegistry* backends, uint64_t tier2Threshold,
	                    std::string tier1BackendName, std::string tier2BackendName, Type returnType);

	~MultiTierExecutable() override;

	void* getInvocableFunctionPtr(const std::string& member) override;
	bool hasInvocableFunctionPtr() override;
	std::unique_ptr<GenericInvocable> getGenericInvocable(const std::string& member) override;

	uint8_t getCurrentTier() const {
		return current_tier_.load(std::memory_order_acquire);
	}

	uint64_t getInvocationCount() const {
		return invocation_count_.load(std::memory_order_relaxed);
	}

	bool isTier2Compiling() const {
		return tier2_compiling_.load(std::memory_order_acquire);
	}

	bool isTier2Failed() const {
		return tier2_failed_.load(std::memory_order_acquire);
	}

	std::string getTier1BackendName() const {
		return tier1_backend_name_;
	}

	std::string getTier2BackendName() const {
		return tier2_backend_name_;
	}

	std::string getCurrentBackendName() const {
		return getCurrentTier() == 1 ? tier1_backend_name_ : tier2_backend_name_;
	}

	/// Blocks until any in-progress tier 2 compilation completes.
	void waitForTier2Compilation() {
		if (tier2_compilation_future_.valid()) {
			tier2_compilation_future_.wait();
		}
	}

private:
	void onInvocation();
	void compileTier2();
	void switchToTier2(std::unique_ptr<Executable> newExecutable, Type return_type);
	Executable* getActiveExecutable();
	GenericInvocable* getFunctionPointerInvocable(const std::string& member, void* fptr);

	std::unique_ptr<Executable> tier1_executable_;
	std::unique_ptr<Executable> tier2_executable_;
	MultiTierJitCompiler::wrapper_function wrapper_function_;
	engine::Options options_;
	const CompilationBackendRegistry* backends_;
	uint64_t tier2_threshold_;
	std::string tier1_backend_name_;
	std::string tier2_backend_name_;

	std::atomic<uint8_t> current_tier_;
	std::atomic<uint64_t> invocation_count_;
	std::atomic<bool> tier2_compiling_;
	std::atomic<bool> tier2_failed_;
	std::future<void> tier2_compilation_future_;
	std::mutex tier_transition_mutex_;

	// Return type for dyncall dispatch, protected by tier_transition_mutex_
	Type return_type_ = Type::v;

	// Cache for function pointer invocables (member -> (fptr, invocable))
	std::map<std::string, std::pair<void*, std::unique_ptr<GenericInvocable>>> fptr_invocable_cache_;

	class MultiTierInvocable;
	friend class MultiTierInvocable;
};

} // namespace nautilus::compiler
