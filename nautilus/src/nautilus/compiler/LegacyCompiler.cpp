#include "nautilus/compiler/LegacyCompiler.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
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

#include "nautilus/CompilableFunction.hpp"
#include "nautilus/compiler/ir/util/GraphVizUtil.hpp"
#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#endif

namespace nautilus::compiler {

LegacyCompiler::LegacyCompiler() : options(), backends(CompilationBackendRegistry::getInstance()) {
}

LegacyCompiler::LegacyCompiler(engine::Options options)
    : options(std::move(options)), backends(CompilationBackendRegistry::getInstance()) {
}

LegacyCompiler::~LegacyCompiler() = default;

std::string LegacyCompiler::getName() const {
	auto name = options.getOptionOrDefault<std::string>("engine.backend", "");
	if (name.empty()) {
		return backends->getDefaultBackendName();
	}
	return name;
}

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

std::unique_ptr<Executable> LegacyCompiler::compile(JITCompiler::wrapper_function function) const {
	auto rootFunction = CompilableFunction(ROOT_FUNCTION_NAME, function);
	std::list<compiler::CompilableFunction> functionsToTrace;
	functionsToTrace.push_back(rootFunction);
	return compile(functionsToTrace);
}

std::shared_ptr<ir::IRGraph> LegacyCompiler::compileToIR(std::list<CompilableFunction>& functions) const {
	const CompilationUnitID compilationId = createCompilationUnitID();
	auto dumpHandler = DumpHandler(options, compilationId);
	auto statsLogger =
	    log::CompilationStatsLogger(options.getOptionOrDefault("engine.compilationStats", false), compilationId);
	auto tTotal = log::now();

	auto t0 = log::now();
	auto traceMode = options.getOptionOrDefault(TRACE_MODE_OPTION, std::string(TRACE_MODE_LAZY));
	std::shared_ptr<tracing::TraceModule> traceModule =
	    (traceMode == TRACE_MODE_LAZY) ? tracing::LazyTraceContext::Trace(functions, options)
	                                   : tracing::ExceptionBasedTraceContext::Trace(functions, options);
	statsLogger.logTiming(t0, "Tracing completed");
	dumpHandler.dump("after_tracing", "trace", [&]() { return traceModule->toString(); });

	auto t1 = log::now();
	auto ssaCreationPhase = tracing::SSACreationPhase();
	auto afterSSAModule = ssaCreationPhase.apply(std::move(traceModule));
	statsLogger.logTiming(t1, "SSA creation completed");
	dumpHandler.dump("after_ssa", "trace", [&]() { return afterSSAModule->toString(); });

	auto t2 = log::now();
	auto irGenerationPhase = tracing::TraceToIRConversionPhase();
	auto ir = irGenerationPhase.apply(afterSSAModule, compilationId);
	statsLogger.logTiming(t2, "IR generation completed");
	dumpHandler.dump("after_ir_creation", "ir", [&]() { return ir->toString(); });
	if (options.getOptionOrDefault("dump.graph", false)) {
		ir::createGraphVizFromIr(ir, options, dumpHandler);
	}

	statsLogger.logTiming(tTotal, "Frontend (trace + SSA + IR) completed");
	return ir;
}

std::unique_ptr<Executable> LegacyCompiler::compileIR(const std::shared_ptr<ir::IRGraph>& ir,
                                                      const std::string& backendName) const {
	const CompilationUnitID compilationId = createCompilationUnitID();
	auto dumpHandler = DumpHandler(options, compilationId);
	auto statsLogger =
	    log::CompilationStatsLogger(options.getOptionOrDefault("engine.compilationStats", false), compilationId);

	auto t0 = log::now();
	const auto backend = backends->getBackend(backendName);
	auto executable = backend->compile(ir, dumpHandler, options);
	statsLogger.logTiming(t0, "Backend compilation ({}) completed", backendName);
	executable->setGeneratedFiles(dumpHandler.getGeneratedFiles());
	return executable;
}

std::unique_ptr<Executable> LegacyCompiler::compile(std::list<CompilableFunction>& functions) const {
	auto ir = compileToIR(functions);
	return compileIR(ir, getName());
}

#else

std::unique_ptr<Executable> LegacyCompiler::compile(JITCompiler::wrapper_function) const {
	throw RuntimeException("Jit not initialised");
}

std::unique_ptr<Executable> LegacyCompiler::compile(std::list<CompilableFunction>&) const {
	throw RuntimeException("Jit not initialised");
}

std::shared_ptr<ir::IRGraph> LegacyCompiler::compileToIR(std::list<CompilableFunction>&) const {
	throw RuntimeException("Jit not initialised");
}

std::unique_ptr<Executable> LegacyCompiler::compileIR(const std::shared_ptr<ir::IRGraph>&, const std::string&) const {
	throw RuntimeException("Jit not initialised");
}

#endif
} // namespace nautilus::compiler
