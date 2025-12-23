#include "nautilus/MultiTierJitCompiler.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/MultiTierExecutable.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/config.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"
#include <chrono>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>

#ifdef ENABLE_COMPILER

#include "nautilus/compiler/ir/util/GraphVizUtil.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#endif

namespace nautilus::compiler {

MultiTierJitCompiler::MultiTierJitCompiler() : options(), backends(CompilationBackendRegistry::getInstance()) {
}

MultiTierJitCompiler::MultiTierJitCompiler(engine::Options options)
    : options(std::move(options)), backends(CompilationBackendRegistry::getInstance()) {
}

MultiTierJitCompiler::~MultiTierJitCompiler() = default;

std::string MultiTierJitCompiler::getName() const {
	return fmt::format("multi-tier({}->{})", getTier1BackendName(), getTier2BackendName());
}

#if defined(ENABLE_COMPILER) && defined(ENABLE_TRACING)

extern std::string createCompilationUnitID();

std::unique_ptr<Executable> MultiTierJitCompiler::compile(MultiTierJitCompiler::wrapper_function function) const {
	const CompilationUnitID compilationId = createCompilationUnitID();
	auto dumpHandler = DumpHandler(options, compilationId + "_tier1");

	// Derive trace from function
	auto executionTrace = tracing::TraceContext::trace(function, options);
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

	// Compile with tier 1 backend
	const auto tier1BackendName = getTier1BackendName();
	const auto tier1Backend = backends->getBackend(tier1BackendName);
	auto tier1Executable = tier1Backend->compile(ir, dumpHandler, options);
	tier1Executable->setGeneratedFiles(dumpHandler.getGeneratedFiles());

	// Create multi-tier executable that will handle tier 2 compilation lazily
	auto multiTierExecutable = std::make_unique<MultiTierExecutable>(
	    std::move(tier1Executable), function, options, backends, getTier2Threshold(), getTier2BackendName());

	return multiTierExecutable;
}

#else

std::unique_ptr<Executable> MultiTierJitCompiler::compile(MultiTierJitCompiler::wrapper_function) const {
	throw RuntimeException("Multi-tier JIT not initialized");
}

#endif

} // namespace nautilus::compiler
