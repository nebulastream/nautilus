#include "nautilus/JITCompiler.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
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

#include "nautilus/compiler/CompilableFunction.hpp"
#include "nautilus/compiler/ir/phases/ConstantFoldingPhase.hpp"
#include "nautilus/compiler/ir/util/GraphVizUtil.hpp"
#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#endif

namespace nautilus::compiler {

JITCompiler::JITCompiler() : options(), backends(CompilationBackendRegistry::getInstance()) {
}

JITCompiler::JITCompiler(engine::Options options)
    : options(std::move(options)), backends(CompilationBackendRegistry::getInstance()) {
}

JITCompiler::~JITCompiler() = default;

#if defined(ENABLE_COMPILER) && defined(ENABLE_TRACING)

std::string createCompilationUnitID() {
	// Get the current time point
	// Create a timestamp string from the current time

	auto now = std::chrono::system_clock::now();
	std::string timestamp = fmt::format(fmt::runtime("{:%Y-%m-%d_%H-%M-%S}"), now);

	// Create a random device and generator
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> distribution(0, 15);

	// Generate a 7-character UUID
	std::string uuid;
	for (int i = 0; i < 7; ++i) {
		int random_number = distribution(generator);
		if (random_number < 10)
			uuid += std::to_string(random_number);
		else
			uuid += char('A' + random_number - 10);
	}

	// Concatenate timestamp and UUID
	return timestamp + "_#" + uuid;
}

static constexpr auto ROOT_FUNCTION_NAME = "execute";
static constexpr auto TRACE_MODE_OPTION = "engine.traceMode";
static constexpr auto TRACE_MODE_LAZY = "lazyTracing";

std::unique_ptr<Executable> JITCompiler::compile(JITCompiler::wrapper_function function) const {
	const CompilationUnitID compilationId = createCompilationUnitID();
	auto dumpHandler = DumpHandler(options, compilationId);
	auto rootFunction = CompilableFunction(ROOT_FUNCTION_NAME, function);
	std::list<compiler::CompilableFunction> functionsToTrace;
	functionsToTrace.push_back(rootFunction);

	auto traceMode = options.getOptionOrDefault(TRACE_MODE_OPTION, std::string(TRACE_MODE_LAZY));
	std::shared_ptr<tracing::TraceModule> traceModule =
	    (traceMode == TRACE_MODE_LAZY) ? tracing::LazyTraceContext::Trace(functionsToTrace, options)
	                                   : tracing::ExceptionBasedTraceContext::Trace(functionsToTrace, options);
	dumpHandler.dump("after_tracing", "trace", [&]() { return traceModule->toString(); });

	// Create SSA for all functions in the module
	auto ssaCreationPhase = tracing::SSACreationPhase();
	auto afterSSAModule = ssaCreationPhase.apply(std::move(traceModule));
	dumpHandler.dump("after_ssa", "trace", [&]() { return afterSSAModule->toString(); });

	// Convert the entire trace module to nautilus IR
	auto irGenerationPhase = tracing::TraceToIRConversionPhase();
	auto ir = irGenerationPhase.apply(afterSSAModule, compilationId);
	dumpHandler.dump("after_ir_creation", "ir", [&]() { return ir->toString(); });
	if (options.getOptionOrDefault("dump.graph", false)) {
		ir::createGraphVizFromIr(ir, options, dumpHandler);
	}

	// Apply constant folding optimization
	auto constantFoldingPhase = ir::ConstantFoldingPhase();
	constantFoldingPhase.apply(ir, options);
	dumpHandler.dump("after_constant_folding", "ir", [&]() { return ir->toString(); });

	// lower to backend
	const auto backendName = getName();
	const auto backend = backends->getBackend(backendName);
	auto executable = backend->compile(ir, dumpHandler, options);
	executable->setGeneratedFiles(dumpHandler.getGeneratedFiles());
	return executable;
}

#else

std::unique_ptr<Executable> JITCompiler::compile(JITCompiler::wrapper_function) const {
	throw RuntimeException("Jit not initialised");
}

#endif
} // namespace nautilus::compiler
