#include "nautilus/MultiTierExecutable.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/logging.hpp"
#include <chrono>
#include <fmt/core.h>

#ifdef ENABLE_COMPILER

#include "nautilus/compiler/ir/util/GraphVizUtil.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"

#endif

namespace nautilus::compiler {

#ifdef ENABLE_COMPILER
// Forward declaration of createCompilationUnitID from JITCompiler.cpp
extern std::string createCompilationUnitID();
#endif

/**
 * @brief Generic invocable wrapper for multi-tier executable.
 *
 * This class wraps the multi-tier executable and dispatches invocations
 * to the currently active tier while tracking invocation counts.
 */
class MultiTierExecutable::MultiTierInvocable : public Executable::GenericInvocable {
public:
	explicit MultiTierInvocable(MultiTierExecutable& multiTierExecutable, const std::string& member)
	    : multiTierExecutable(multiTierExecutable), member(member) {
	}

	std::any invokeGeneric(const std::vector<std::any>& args) override {
		// Track invocation and potentially trigger tier 2 compilation
		multiTierExecutable.onInvocation();

		// Get the currently active executable and invoke it
		auto* activeExecutable = multiTierExecutable.getActiveExecutable();
		auto genericInvocable = activeExecutable->getGenericInvocable(member);
		return genericInvocable->invokeGeneric(args);
	}

private:
	MultiTierExecutable& multiTierExecutable;
	std::string member;
};

MultiTierExecutable::MultiTierExecutable(std::unique_ptr<Executable> tier1Executable,
                                         MultiTierJitCompiler::wrapper_function wrapperFunction,
                                         engine::Options options, const CompilationBackendRegistry* backends,
                                         uint64_t tier2Threshold, std::string tier2BackendName)
    : tier1Executable(std::move(tier1Executable)),
      tier2Executable(nullptr),
      wrapperFunction(std::move(wrapperFunction)),
      options(std::move(options)),
      backends(backends),
      tier2Threshold(tier2Threshold),
      tier2BackendName(std::move(tier2BackendName)),
      currentTier(1),
      invocationCount(0),
      tier2Compiling(false) {
}

MultiTierExecutable::~MultiTierExecutable() {
	// Wait for tier 2 compilation to complete if in progress
	if (tier2CompilationFuture.valid()) {
		tier2CompilationFuture.wait();
	}
}

void* MultiTierExecutable::getInvocableFunctionPtr(const std::string& member) {
	// Check cache first
	{
		std::lock_guard<std::mutex> lock(functionPointerCacheMutex);
		auto it = functionPointerCache.find(member);
		if (it != functionPointerCache.end()) {
			return it->second;
		}
	}

	// For multi-tier executable, we need to create a stable trampoline function
	// Since we can't generate dynamic trampolines easily in C++, we'll use
	// the GenericInvocable approach for now
	//
	// A production implementation would generate a small assembly trampoline
	// that calls into the MultiTierExecutable::onInvocation() and then
	// dispatches to the active tier's function pointer
	//
	// For now, we return nullptr to force use of GenericInvocable
	return nullptr;
}

bool MultiTierExecutable::hasInvocableFunctionPtr() {
	// We currently don't support direct function pointers for multi-tier
	// because we need to intercept calls to track invocations and switch tiers
	// A production implementation would use a stable trampoline
	return false;
}

std::unique_ptr<Executable::GenericInvocable> MultiTierExecutable::getGenericInvocable(const std::string& member) {
	return std::make_unique<MultiTierInvocable>(*this, member);
}

void MultiTierExecutable::onInvocation() {
	// Increment invocation count
	uint64_t count = invocationCount.fetch_add(1, std::memory_order_acq_rel);

	// Check if we should trigger tier 2 compilation
	if (count == tier2Threshold && !tier2Compiling.load(std::memory_order_acquire)) {
		// Try to start tier 2 compilation
		bool expected = false;
		if (tier2Compiling.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
			// Launch async compilation
			tier2CompilationFuture = std::async(std::launch::async, [this]() { this->compileTier2(); });
		}
	}
}

Executable* MultiTierExecutable::getActiveExecutable() {
	uint8_t tier = currentTier.load(std::memory_order_acquire);
	if (tier == 2 && tier2Executable) {
		return tier2Executable.get();
	}
	return tier1Executable.get();
}

void MultiTierExecutable::compileTier2() {
#if defined(ENABLE_COMPILER) && defined(ENABLE_TRACING)
	try {
		const CompilationUnitID compilationId = createCompilationUnitID();
		auto dumpHandler = DumpHandler(options, compilationId + "_tier2");

		// Derive trace from function
		auto executionTrace = tracing::TraceContext::trace(wrapperFunction, options);
		dumpHandler.dump("after_tracing", "trace", [&]() { return executionTrace->toString(); });

		// Create SSA
		auto ssaCreationPhase = tracing::SSACreationPhase();
		auto afterSSA = ssaCreationPhase.apply(std::move(executionTrace));
		dumpHandler.dump("after_ssa", "trace", [&]() { return afterSSA->toString(); });

		// Get nautilus IR from trace
		auto irGenerationPhase = tracing::TraceToIRConversionPhase();
		const auto ir = irGenerationPhase.apply(std::move(afterSSA), compilationId);
		dumpHandler.dump("after_ir_creation", "ir", [&]() { return ir->toString(); });

		if (options.getOptionOrDefault("dump.graph", false)) {
			ir::createGraphVizFromIr(ir, options, dumpHandler);
		}

		// Compile with tier 2 backend
		const auto tier2Backend = backends->getBackend(tier2BackendName);
		auto newExecutable = tier2Backend->compile(ir, dumpHandler, options);
		newExecutable->setGeneratedFiles(dumpHandler.getGeneratedFiles());

		// Switch to tier 2
		switchToTier2(std::move(newExecutable));

	} catch (const std::exception& e) {
		// Log error but don't crash - continue using tier 1
		tier2Compiling.store(false, std::memory_order_release);
	}
#else
	tier2Compiling.store(false, std::memory_order_release);
#endif
}

void MultiTierExecutable::switchToTier2(std::unique_ptr<Executable> newExecutable) {
	std::lock_guard<std::mutex> lock(tierTransitionMutex);

	// Store tier 2 executable
	tier2Executable = std::move(newExecutable);

	// Atomically switch to tier 2
	currentTier.store(2, std::memory_order_release);

	// Compilation complete
	tier2Compiling.store(false, std::memory_order_release);
}

} // namespace nautilus::compiler
