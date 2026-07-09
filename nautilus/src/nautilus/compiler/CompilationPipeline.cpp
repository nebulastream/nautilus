#include "nautilus/compiler/CompilationPipeline.hpp"
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
#include "nautilus/compiler/ir/passes/AlgebraicSimplificationPass.hpp"
#include "nautilus/compiler/ir/passes/BlockArgumentPruningPass.hpp"
#include "nautilus/compiler/ir/passes/BlockMergingPass.hpp"
#include "nautilus/compiler/ir/passes/ConstantBranchFoldingPass.hpp"
#include "nautilus/compiler/ir/passes/ConstantFoldingAndCopyPropagationPass.hpp"
#include "nautilus/compiler/ir/passes/DeadCodeEliminationPass.hpp"
#include "nautilus/compiler/ir/passes/EmptyBlockEliminationPass.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/compiler/ir/passes/IRStatistics.hpp"
#include "nautilus/compiler/ir/passes/LocalCSEPass.hpp"
#include "nautilus/compiler/ir/passes/LoopInvariantCodeMotionPass.hpp"
#include "nautilus/compiler/ir/passes/StrengthReductionPass.hpp"
#include "nautilus/compiler/ir/util/GraphVizUtil.hpp"
#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include "nautilus/tracing/tag/SourceLocationResolver.hpp"
#endif

namespace nautilus::compiler {

CompilationPipeline::CompilationPipeline(engine::Options options, common::ArenaPool& traceArenaPool,
                                         common::ArenaPool& irArenaPool)
    : options(std::move(options)), backends(CompilationBackendRegistry::getInstance()),
      traceArenaPool_(&traceArenaPool), irArenaPool_(&irArenaPool) {
}

CompilationPipeline::~CompilationPipeline() = default;

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

static constexpr auto TRACE_MODE_OPTION = "engine.traceMode";
static constexpr auto TRACE_MODE_LAZY = "lazyTracing";

std::shared_ptr<ir::IRGraph> CompilationPipeline::compileToIR(std::list<CompilableFunction>& functions,
                                                              const engine::ModuleOptions& moduleOptions,
                                                              CompilationStatistics* statistics) const {
	const CompilationUnitID compilationId = createCompilationUnitID();
	auto dumpHandler = DumpHandler(moduleOptions, compilationId);

	if (statistics != nullptr && !statistics->contains("compilation.unitId")) {
		statistics->set("compilation.unitId", compilationId);
	}

	std::unique_ptr<tracing::SourceLocationResolver> sourceLocationResolver;
	ir::IRPrintOptions irPrintOptions;
	if (moduleOptions.getOptionOrDefault("dump.sourceLocations", false)) {
		sourceLocationResolver = std::make_unique<tracing::SourceLocationResolver>();
		irPrintOptions.showSourceLocations = true;
		irPrintOptions.resolver = sourceLocationResolver.get();
	}

	const auto frontendStart = std::chrono::steady_clock::now();

	const auto tracingStart = std::chrono::steady_clock::now();
	auto traceMode = moduleOptions.getOptionOrDefault(TRACE_MODE_OPTION, std::string(TRACE_MODE_LAZY));
	// Acquire a fresh trace arena for the lifetime of this compile.  Each
	// concurrent compile() gets its own arena from the synchronized pool, so
	// there is no shared bump allocator to race on.  The handle stays alive
	// until the end of this function; on release the arena is softReset and
	// recycled back into the pool.  Trace data is dead once IR generation has
	// finished, so this is safe (the IRGraph owns a separate IR arena).
	auto traceArenaHandle = traceArenaPool_->acquire();
	common::Arena& arena = *traceArenaHandle;
	std::shared_ptr<tracing::TraceModule> traceModule =
	    (traceMode == TRACE_MODE_LAZY) ? tracing::LazyTraceContext::Trace(functions, moduleOptions, arena)
	                                   : tracing::ExceptionBasedTraceContext::Trace(functions, moduleOptions, arena);
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
	dumpHandler.dump("after_ir_creation", "nautilus", [&]() { return ir->toString(irPrintOptions); });
	if (moduleOptions.getOptionOrDefault("dump.graph", false)) {
		ir::createGraphVizFromIr(ir, moduleOptions, dumpHandler);
	}

	if (moduleOptions.getOptionOrDefault("ir.runPasses", true)) {
		ir::IRPassManager passManager(moduleOptions, &dumpHandler, statistics, &irPrintOptions);
		std::vector<std::unique_ptr<ir::IRPass>> group;
		if (!moduleOptions.getOptionOrDefault("ir.disableConstantFolding", false)) {
			group.push_back(std::make_unique<ir::ConstantFoldingAndCopyPropagationPass>());
		}
		// Canonicalizes and folds local algebraic identities; see design
		// §4.3-B. Runs right after constant folding so the constants it
		// produces are canonicalized to the right operand immediately.
		if (!moduleOptions.getOptionOrDefault("ir.disableAlgebraicSimplification", false)) {
			group.push_back(std::make_unique<ir::AlgebraicSimplificationPass>());
		}
		// Closes the loop constant folding/simplification opens: a compare
		// that folded to a constant bool still drives a conditional branch
		// until this pass turns it into an unconditional one and sweeps the
		// dead arm; see design §4.3-C.
		if (!moduleOptions.getOptionOrDefault("ir.disableConstantBranchFolding", false)) {
			group.push_back(std::make_unique<ir::ConstantBranchFoldingPass>());
		}
		if (!moduleOptions.getOptionOrDefault("ir.disableEmptyBlockElimination", false)) {
			group.push_back(std::make_unique<ir::EmptyBlockEliminationPass>());
		}
		// Collapses the single-predecessor seams trace-generated IR is full
		// of (and that branch folding just created more of) into straight-line
		// blocks; see design §4.3-D. Runs after the empty-block pass so
		// trivial hops are gone before whole blocks are spliced.
		if (!moduleOptions.getOptionOrDefault("ir.disableBlockMerging", false)) {
			group.push_back(std::make_unique<ir::BlockMergingPass>());
		}
		// Block-local CSE over the long straight-line blocks block merging
		// just produced; see design §4.3-F. Opt-in (default off) pending the
		// benchmark sweep that gates promoting it to default-on
		// (`ir.disableLocalCSE`); DCE below sweeps the duplicates it removes.
		if (moduleOptions.getOptionOrDefault("ir.enableLocalCSE", false)) {
			group.push_back(std::make_unique<ir::LocalCSEPass>());
		}
		// Opt-in (default off), unlike the two passes above: correct, but
		// measured to regress the BC interpreter's dispatch-bound cost model
		// (see StrengthReductionPass.hpp) -- may still be worth enabling for
		// an ALU-bound backend.
		if (moduleOptions.getOptionOrDefault("ir.enableStrengthReduction", false)) {
			group.push_back(std::make_unique<ir::StrengthReductionPass>());
		}
		// Sweeps constant-folding and strength-reduction residue (dead
		// feeding constants, the neutralized multiply) every round; see
		// design §4.3-A. Runs last in the group so it cleans up whatever the
		// passes above it produced that round.
		if (!moduleOptions.getOptionOrDefault("ir.disableDeadCodeElimination", false)) {
			group.push_back(std::make_unique<ir::DeadCodeEliminationPass>());
		}
		// The only pass that changes block-argument arity: prunes unused and
		// same-value pass-through arguments; see design §4.3-E. Runs last in
		// the group because every CFG change above can strand arguments, and
		// DCE's sweep is what turns "used only by dead code" into "unused".
		if (!moduleOptions.getOptionOrDefault("ir.disableBlockArgumentPruning", false)) {
			group.push_back(std::make_unique<ir::BlockArgumentPruningPass>());
		}
		// Re-run the whole group until a full round changes nothing (e.g.
		// empty-block elimination exposing a new copy-propagation
		// opportunity for constant folding), capped at `ir.maxPipelineIterations`.
		const auto maxIterations = static_cast<size_t>(moduleOptions.getOptionOrDefault("ir.maxPipelineIterations", 4));
		passManager.addFixedPointGroup(std::move(group), maxIterations);
		// Loop-invariant code motion runs once, after the cleanup group has
		// canonicalized the CFG (single preheaders/latches), and stays opt-in
		// (default off) as the highest-risk pass; see design §4.3-G / §4.4.
		if (moduleOptions.getOptionOrDefault("ir.enableLICM", false)) {
			passManager.addPass(std::make_unique<ir::LoopInvariantCodeMotionPass>());
		}
		passManager.run(*ir);
		dumpHandler.dump("after_ir_passes", "nautilus", [&]() { return ir->toString(irPrintOptions); });
	}

	if (statistics != nullptr) {
		statistics->recordTimingMs("frontend.totalMs", frontendStart);
	}
	return ir;
}

std::unique_ptr<Executable> CompilationPipeline::compileIR(const std::shared_ptr<ir::IRGraph>& ir,
                                                           const std::string& backendName,
                                                           const engine::ModuleOptions& moduleOptions,
                                                           CompilationStatistics* statistics) const {
	const CompilationUnitID compilationId = createCompilationUnitID();
	auto dumpHandler = DumpHandler(moduleOptions, compilationId);

	if (statistics != nullptr) {
		if (!statistics->contains("compilation.unitId")) {
			statistics->set("compilation.unitId", compilationId);
		}
		statistics->set("backend.name", backendName);
	}

	const auto backend = backends->getBackend(backendName);
	auto executable = backend->compile(ir, dumpHandler, moduleOptions, statistics);
	executable->setGeneratedFiles(dumpHandler.getGeneratedFiles());
	return executable;
}

#else

std::shared_ptr<ir::IRGraph> CompilationPipeline::compileToIR(std::list<CompilableFunction>&,
                                                              const engine::ModuleOptions&,
                                                              CompilationStatistics*) const {
	throw RuntimeException("Jit not initialised");
}

std::unique_ptr<Executable> CompilationPipeline::compileIR(const std::shared_ptr<ir::IRGraph>&, const std::string&,
                                                           const engine::ModuleOptions&, CompilationStatistics*) const {
	throw RuntimeException("Jit not initialised");
}

#endif
} // namespace nautilus::compiler
