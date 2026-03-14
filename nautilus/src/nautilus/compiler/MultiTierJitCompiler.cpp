#include "nautilus/MultiTierJitCompiler.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/MultiTierExecutable.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/config.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <fmt/core.h>
#include <string>

#ifdef ENABLE_COMPILER

#include "nautilus/compiler/ir/util/GraphVizUtil.hpp"
#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#endif

namespace nautilus::compiler {

MultiTierJitCompiler::MultiTierJitCompiler() : options(), backends_(CompilationBackendRegistry::getInstance()) {
}

MultiTierJitCompiler::MultiTierJitCompiler(engine::Options options)
    : options(std::move(options)), backends_(CompilationBackendRegistry::getInstance()) {
}

MultiTierJitCompiler::~MultiTierJitCompiler() = default;

std::string MultiTierJitCompiler::getName() const {
	return fmt::format("multi-tier({}->{})", getTier1BackendName(), getTier2BackendName());
}

#if defined(ENABLE_COMPILER) && defined(ENABLE_TRACING)

extern std::string createCompilationUnitID();

std::unique_ptr<Executable> MultiTierJitCompiler::compile(MultiTierJitCompiler::wrapper_function function) const {
	const auto tier1_backend_name = getTier1BackendName();

	const CompilationUnitID compilationId = createCompilationUnitID();
	auto dumpHandler = DumpHandler(options, compilationId + "_tier1");

	auto traceMode = options.getOptionOrDefault("engine.traceMode", std::string("lazyTracing"));
	auto executionTrace = (traceMode == "lazyTracing")
	                          ? tracing::LazyTraceContext::trace(function, options)
	                          : tracing::ExceptionBasedTraceContext::trace(function, options);
	dumpHandler.dump("after_tracing", "trace", [&]() { return executionTrace->toString(); });

	auto ssaCreationPhase = tracing::SSACreationPhase();
	auto afterSSA = ssaCreationPhase.apply(std::move(executionTrace));
	dumpHandler.dump("after_ssa", "trace", [&]() { return afterSSA->toString(); });

	auto irGenerationPhase = tracing::TraceToIRConversionPhase();
	const auto ir = irGenerationPhase.apply(std::move(afterSSA), compilationId);
	dumpHandler.dump("after_ir_creation", "ir", [&]() { return ir->toString(); });

	if (options.getOptionOrDefault("dump.graph", false)) {
		ir::createGraphVizFromIr(ir, options, dumpHandler);
	}

	const auto tier1Backend = backends_->getBackend(tier1_backend_name);
	auto tier1_executable = tier1Backend->compile(ir, dumpHandler, options);
	tier1_executable->setGeneratedFiles(dumpHandler.getGeneratedFiles());

	return std::make_unique<MultiTierExecutable>(std::move(tier1_executable), function, options, backends_,
	                                             getTier2Threshold(), tier1_backend_name, getTier2BackendName());
}

#else

std::unique_ptr<Executable> MultiTierJitCompiler::compile(MultiTierJitCompiler::wrapper_function) const {
	throw RuntimeException("Multi-tier JIT not initialized");
}

#endif

} // namespace nautilus::compiler
