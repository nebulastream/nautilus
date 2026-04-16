#include "nautilus/compiler/LegacyCompiler.hpp"
#include "nautilus/CompilationStatistics.hpp"
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
#include "nautilus/compiler/ir/passes/ConstantFoldingAndCopyPropagationPass.hpp"
#include "nautilus/compiler/ir/passes/EmptyBlockEliminationPass.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/compiler/ir/passes/IRStatistics.hpp"
#include "nautilus/compiler/ir/util/GraphVizUtil.hpp"
#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#endif

namespace nautilus::compiler {

LegacyCompiler::LegacyCompiler(engine::Options options, common::Arena& arena, common::ArenaPool& irArenaPool)
    : options(std::move(options)), backends(CompilationBackendRegistry::getInstance()), arena_(&arena),
      irArenaPool_(&irArenaPool) {
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

namespace {

/// Emit the final report to the user's log sink. Called from the top-level
/// compile entry points only, so a full pipeline produces exactly one
/// report even when it internally calls compileToIR + compileIR.
void logStatisticsIfEnabled(const engine::Options& options, const CompilationStatistics& stats,
                            std::string_view compilationId, std::string_view backendName) {
	if (!options.getOptionOrDefault("engine.logStatistics", false)) {
		return;
	}
	log::info("\n{}", stats.formatReport(compilationId, backendName));
}

} // namespace

std::unique_ptr<Executable> LegacyCompiler::compile(JITCompiler::wrapper_function function) const {
	auto rootFunction = CompilableFunction(ROOT_FUNCTION_NAME, function);
	std::list<compiler::CompilableFunction> functionsToTrace;
	functionsToTrace.push_back(rootFunction);
	return compile(functionsToTrace);
}

std::shared_ptr<ir::IRGraph> LegacyCompiler::compileToIR(std::list<CompilableFunction>& functions,
                                                         CompilationStatistics* statistics) const {
	const CompilationUnitID compilationId = createCompilationUnitID();
	auto dumpHandler = DumpHandler(options, compilationId);

	if (statistics != nullptr && !statistics->contains("compilation.unitId")) {
		statistics->set("compilation.unitId", compilationId);
	}

	const auto frontendStart = std::chrono::steady_clock::now();

	const auto tracingStart = std::chrono::steady_clock::now();
	auto traceMode = options.getOptionOrDefault(TRACE_MODE_OPTION, std::string(TRACE_MODE_LAZY));
	// Recycle the chunks from the previous compile before this one starts.
	// Any TraceModule from a previous compilation has already been
	// destroyed by the time we get here, so no live pointers remain.
	arena_->softReset();
	std::shared_ptr<tracing::TraceModule> traceModule =
	    (traceMode == TRACE_MODE_LAZY) ? tracing::LazyTraceContext::Trace(functions, options, *arena_)
	                                   : tracing::ExceptionBasedTraceContext::Trace(functions, options, *arena_);
	if (statistics != nullptr) {
		statistics->recordTimingMs("tracing.ms", tracingStart);
	}
	dumpHandler.dump("after_tracing", "trace", [&]() { return traceModule->toString(); });

	const auto ssaStart = std::chrono::steady_clock::now();
	auto ssaCreationPhase = tracing::SSACreationPhase();
	auto afterSSAModule = ssaCreationPhase.apply(std::move(traceModule));
	if (statistics != nullptr) {
		statistics->recordTimingMs("ssaCreation.ms", ssaStart);
	}
	dumpHandler.dump("after_ssa", "trace", [&]() { return afterSSAModule->toString(); });

	const auto irGenStart = std::chrono::steady_clock::now();
	auto irGenerationPhase = tracing::TraceToIRConversionPhase();
	auto ir = irGenerationPhase.apply(afterSSAModule, *irArenaPool_, compilationId);
	if (statistics != nullptr) {
		statistics->recordTimingMs("irGeneration.ms", irGenStart);
		const auto snapshot = ir::computeStatistics(*ir);
		statistics->set("ir.functions", static_cast<int64_t>(snapshot.numFunctions));
		statistics->set("ir.blocks", static_cast<int64_t>(snapshot.numBlocks));
		statistics->set("ir.operations", static_cast<int64_t>(snapshot.numOperations));
	}
	dumpHandler.dump("after_ir_creation", "ir", [&]() { return ir->toString(); });
	if (options.getOptionOrDefault("dump.graph", false)) {
		ir::createGraphVizFromIr(ir, options, dumpHandler);
	}

	if (options.getOptionOrDefault("ir.runPasses", true)) {
		ir::IRPassManager passManager(options, &dumpHandler, statistics);
		if (!options.getOptionOrDefault("ir.disableConstantFolding", false)) {
			passManager.addPass(std::make_unique<ir::ConstantFoldingAndCopyPropagationPass>());
		}
		if (!options.getOptionOrDefault("ir.disableEmptyBlockElimination", false)) {
			passManager.addPass(std::make_unique<ir::EmptyBlockEliminationPass>());
		}
		passManager.run(*ir);
		dumpHandler.dump("after_ir_passes", "ir", [&]() { return ir->toString(); });
	}

	if (statistics != nullptr) {
		statistics->recordTimingMs("frontend.totalMs", frontendStart);
	}
	return ir;
}

std::unique_ptr<Executable> LegacyCompiler::compileIR(const std::shared_ptr<ir::IRGraph>& ir,
                                                      const std::string& backendName,
                                                      CompilationStatistics* statistics) const {
	const CompilationUnitID compilationId = createCompilationUnitID();
	auto dumpHandler = DumpHandler(options, compilationId);

	if (statistics != nullptr) {
		if (!statistics->contains("compilation.unitId")) {
			statistics->set("compilation.unitId", compilationId);
		}
		statistics->set("backend.name", backendName);
	}

	const auto backend = backends->getBackend(backendName);
	auto executable = backend->compile(ir, dumpHandler, options, statistics);
	executable->setGeneratedFiles(dumpHandler.getGeneratedFiles());
	return executable;
}

std::unique_ptr<Executable> LegacyCompiler::compile(std::list<CompilableFunction>& functions) const {
	auto statistics = std::make_shared<CompilationStatistics>();
	const auto compilationStart = std::chrono::steady_clock::now();

	auto ir = compileToIR(functions, statistics.get());
	const auto backendName = getName();
	auto executable = compileIR(ir, backendName, statistics.get());

	statistics->recordTimingMs("compilation.totalMs", compilationStart);

	const auto compilationId = statistics->find("compilation.unitId") != nullptr
	                               ? std::get<std::string>(*statistics->find("compilation.unitId"))
	                               : std::string {};
	logStatisticsIfEnabled(options, *statistics, compilationId, backendName);

	executable->setCompilationStatistics(std::static_pointer_cast<const CompilationStatistics>(std::move(statistics)));
	return executable;
}

#else

std::unique_ptr<Executable> LegacyCompiler::compile(JITCompiler::wrapper_function) const {
	throw RuntimeException("Jit not initialised");
}

std::unique_ptr<Executable> LegacyCompiler::compile(std::list<CompilableFunction>&) const {
	throw RuntimeException("Jit not initialised");
}

std::shared_ptr<ir::IRGraph> LegacyCompiler::compileToIR(std::list<CompilableFunction>&, CompilationStatistics*) const {
	throw RuntimeException("Jit not initialised");
}

std::unique_ptr<Executable> LegacyCompiler::compileIR(const std::shared_ptr<ir::IRGraph>&, const std::string&,
                                                      CompilationStatistics*) const {
	throw RuntimeException("Jit not initialised");
}

#endif
} // namespace nautilus::compiler
