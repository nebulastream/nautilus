#include "nautilus/compiler/CompilationPipeline.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/config.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fstream>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>

#ifdef ENABLE_COMPILER

#include "nautilus/CompilableFunction.hpp"
#include "nautilus/compiler/NirToIR.hpp"
#include "nautilus/compiler/ir/passes/AlgebraicSimplificationPass.hpp"
#include "nautilus/compiler/ir/passes/BlockArgumentPruningPass.hpp"
#include "nautilus/compiler/ir/passes/BlockMergingPass.hpp"
#include "nautilus/compiler/ir/passes/ConstantBranchFoldingPass.hpp"
#include "nautilus/compiler/ir/passes/ConstantFoldingAndCopyPropagationPass.hpp"
#include "nautilus/compiler/ir/passes/DeadCodeEliminationPass.hpp"
#include "nautilus/compiler/ir/passes/EmptyBlockEliminationPass.hpp"
#include "nautilus/compiler/ir/passes/ExceptionRegionPreparationPass.hpp"
#include "nautilus/compiler/ir/passes/FunctionAttributeInferencePass.hpp"
#include "nautilus/compiler/ir/passes/IRLocationPass.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/compiler/ir/passes/IRStatistics.hpp"
#include "nautilus/compiler/ir/passes/LocalCSEPass.hpp"
#include "nautilus/compiler/ir/passes/LoopInvariantCodeMotionPass.hpp"
#include "nautilus/compiler/ir/passes/NoThrowInferencePass.hpp"
#include "nautilus/compiler/ir/passes/StrengthReductionPass.hpp"
#include "nautilus/compiler/ir/util/GraphVizUtil.hpp"
#include "nautilus/conversion/Conversion.hpp"
#include "nautilus/nir/Printer.hpp"
#include "nautilus/nir/Verifier.hpp"
#include "nautilus/tracing/CopyAttribution.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/nir/NirTrace.hpp"
#include "nautilus/tracing/nir/NirTracer.hpp"
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
	// `<local date and time to the clock's precision>_#<7 hex digits>`. Every
	// compile makes two of these, so neither part may cost much: a
	// std::random_device opens the system's entropy source each time it is
	// constructed, and formatting the calendar part is most of the rest.
	const auto now = std::chrono::system_clock::now();
	const auto second = std::chrono::floor<std::chrono::seconds>(now);
	thread_local std::chrono::sys_seconds formattedSecond {};
	thread_local std::string formattedPrefix;
	if (formattedPrefix.empty() || second != formattedSecond) {
		formattedPrefix = fmt::format(fmt::runtime("{:%Y-%m-%d_%H-%M-%S}"), second);
		formattedSecond = second;
	}
	// The sub-second digits, as many as fmt prints for the clock's period.
	constexpr auto fractionDigits = [] {
		int digits = 0;
		for (auto den = std::chrono::system_clock::period::den; den > 1; den /= 10) {
			++digits;
		}
		return digits;
	}();
	const auto fraction = (now - second).count();

	// One process-wide splitmix64 sequence, seeded once: distinct suffixes for
	// every compile in this process, unpredictable across processes.
	static std::atomic<uint64_t> state = [] {
		std::random_device device;
		return (static_cast<uint64_t>(device()) << 32) | device();
	}();
	uint64_t mixed = state.fetch_add(0x9E3779B97F4A7C15ull, std::memory_order_relaxed) + 0x9E3779B97F4A7C15ull;
	mixed = (mixed ^ (mixed >> 30)) * 0xBF58476D1CE4E5B9ull;
	mixed = (mixed ^ (mixed >> 27)) * 0x94D049BB133111EBull;
	mixed ^= mixed >> 31;
	char suffix[7];
	for (auto& digit : suffix) {
		digit = "0123456789ABCDEF"[mixed & 0xF];
		mixed >>= 4;
	}

	if constexpr (fractionDigits == 0) {
		return fmt::format("{}_#{}", formattedPrefix, std::string_view(suffix, sizeof(suffix)));
	} else {
		return fmt::format("{}.{:0{}}_#{}", formattedPrefix, fraction, fractionDigits,
		                   std::string_view(suffix, sizeof(suffix)));
	}
}

namespace {

/// The cleanup that makes trace-generated IR small: registers the fixed-point
/// group and the analyses feeding it. At `ArgumentPruning` only the pruning
/// pass is registered (see `IROptimizationLevel` for why that one still pays
/// for a self-optimizing backend); at `None` nothing is.
void addOptimizationPasses(ir::IRPassManager& passManager, const engine::ModuleOptions& moduleOptions,
                           IROptimizationLevel level) {
	if (level == IROptimizationLevel::None) {
		return;
	}
	if (level == IROptimizationLevel::ArgumentPruning) {
		if (!moduleOptions.getOptionOrDefault("ir.disableBlockArgumentPruning", false)) {
			passManager.addPass(std::make_unique<ir::BlockArgumentPruningPass>());
		}
		return;
	}
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
	// measured to regress the bytecode interpreter's dispatch-bound cost model
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

	// Derive what each in-module function does to memory before the passes
	// that could act on it. Every traced call to a Nautilus function
	// declares the pessimistic default, so without this no call is ever
	// eligible for CSE, hoisting or elimination.
	//
	// It runs first, on pre-cleanup IR, and that is safe in one direction
	// only: the later passes remove and move operations, never add them,
	// so a derived attribute can become stale by being *more* pessimistic
	// than the final body -- which costs optimisation, never correctness.
	if (!moduleOptions.getOptionOrDefault("ir.disableAttributeInference", false)) {
		passManager.addPass(std::make_unique<ir::FunctionAttributeInferencePass>());
	}
	passManager.addFixedPointGroup(std::move(group), maxIterations);
	// Loop-invariant code motion runs once, after the cleanup group has
	// canonicalized the CFG (single preheaders/latches), and stays opt-in
	// (default off) as the highest-risk pass; see design §4.3-G / §4.4.
	if (moduleOptions.getOptionOrDefault("ir.enableLICM", false)) {
		passManager.addPass(std::make_unique<ir::LoopInvariantCodeMotionPass>());
	}
}

/// The `tracing.*` statistics of one trace, whichever tracer recorded it.
void recordTracingStatistics(CompilationStatistics& statistics, const tracing::TracingStatistics& t) {
	statistics.set("tracing.iterations", static_cast<int64_t>(t.iterations));
	statistics.set("tracing.replayedOperations", static_cast<int64_t>(t.replayedOperations));
	statistics.set("tracing.snapshots", static_cast<int64_t>(t.snapshots));
	statistics.set("tracing.operations", static_cast<int64_t>(t.operations));
	statistics.set("tracing.pausedOperations", static_cast<int64_t>(t.pausedOperations));
	statistics.set("tracing.copies", static_cast<int64_t>(t.copies()));
	statistics.set("tracing.copies.construct", static_cast<int64_t>(t.copiesConstruct));
	statistics.set("tracing.copies.assign", static_cast<int64_t>(t.copiesAssign));
	statistics.set("tracing.copies.explicit", static_cast<int64_t>(t.copiesExplicit));
	statistics.set("tracing.scopes", static_cast<int64_t>(t.scopes));
	statistics.set("tracing.maxScopeIterations", static_cast<int64_t>(t.maxScopeIterations));
	statistics.set("tracing.firstRun.ms", t.firstRunMs);
	statistics.set("tracing.rerun.ms", t.rerunMs);
	statistics.set("tracing.rerun.follow.ms", t.rerunFollowMs);
	statistics.set("tracing.rerun.record.ms", t.rerunRecordMs);
	statistics.set("tracing.rerun.paused.ms", t.rerunPausedMs);
}

/// Throws if `module` does not verify; `what` names the module in the message.
void verifyOrThrow(const nir::Module& module, const std::string& what, const nir::Module* from = nullptr) {
	auto diagnostics = nir::verify(module);
	if (diagnostics.empty()) {
		return;
	}
	std::string message = what + " does not verify:";
	for (const auto& d : diagnostics) {
		message += "\n  " + d.str();
	}
	message += "\n" + nir::print(module);
	if (from != nullptr) {
		message += "\nfrom\n" + nir::print(*from);
	}
	throw RuntimeException(message);
}

/// The conversion of a pre-SSA module (03-conversion-and-passes.md), with its statistics, the verification of its
/// result when `verify` is set and the recorded attribute bits when `attributesFile` names a file.
nir::Module convertToSSA(const nir::Module& pre, const engine::ModuleOptions& moduleOptions,
                         CompilationStatistics* statistics, bool verify, const std::string& attributesFile) {
	conversion::ConversionStatistics conversionStats;
	const auto conversionStart = std::chrono::steady_clock::now();
	nir::Module ssa;
	try {
		ssa = conversion::convert(pre, {}, &conversionStats);
	} catch (const std::exception& e) {
		if (!verify) {
			throw;
		}
		throw RuntimeException(std::string(e.what()) + "\nwhile converting\n" + nir::print(pre));
	}
	if (statistics != nullptr) {
		statistics->recordTimingMs("conversion.ms", conversionStart);
		statistics->set("conversion.instructions", static_cast<int64_t>(conversionStats.instructions));
		statistics->set("conversion.phis", static_cast<int64_t>(conversionStats.phis));
		statistics->set("conversion.removedPhis", static_cast<int64_t>(conversionStats.removedPhis));
		statistics->set("conversion.folded", static_cast<int64_t>(conversionStats.folded));
		statistics->set("conversion.cse", static_cast<int64_t>(conversionStats.cse));
		statistics->set("conversion.refoldSweeps", static_cast<int64_t>(conversionStats.refoldSweeps));
		statistics->set("conversion.refoldedInsts", static_cast<int64_t>(conversionStats.refoldedInsts));
		statistics->set("conversion.deadBlocks", static_cast<int64_t>(conversionStats.deadBlocks));
		statistics->set("conversion.deadValues", static_cast<int64_t>(conversionStats.deadValues));
		statistics->set("conversion.foldedBranches", static_cast<int64_t>(conversionStats.foldedBranches));
		statistics->set("conversion.loops", static_cast<int64_t>(conversionStats.loops));
	}
	if (verify) {
		verifyOrThrow(ssa, "the SSA NIR converted from the trace", &pre);
	}
	if (!attributesFile.empty()) {
		// the guarantee bits the attribute pre-pass derived, one line per internal function: name, bits
		std::ofstream out(attributesFile, std::ios::app);
		for (const auto& f : ssa.functions) {
			if (f.visibility == static_cast<uint8_t>(nir::Visibility::Internal)) {
				out << ssa.name(f.nameId) << '\t' << (f.attrs & nir::GUARANTEE_MASK) << '\n';
			}
		}
	}
	(void) moduleOptions;
	return ssa;
}

} // namespace

std::shared_ptr<ir::IRGraph> CompilationPipeline::traceToNir(std::list<CompilableFunction>& functions,
                                                             const engine::ModuleOptions& moduleOptions,
                                                             CompilationStatistics* statistics,
                                                             const CompilationUnitID& compilationId,
                                                             const DumpHandler& dumpHandler) const {
	const auto frontendStart = std::chrono::steady_clock::now();
	const auto tracingStart = std::chrono::steady_clock::now();
	// A trace arena per compile: concurrent compiles never share a bump allocator, and the arena is recycled into the
	// pool when the handle goes out of scope (the NIR module owns its data, so nothing refers to it afterwards).
	auto traceArenaHandle = traceArenaPool_->acquire();
	auto traced = tracing::nirtrace::NirTracer::Trace(functions, moduleOptions, *traceArenaHandle);
	if (statistics != nullptr) {
		statistics->recordTimingMs("tracing.ms", tracingStart);
		recordTracingStatistics(*statistics, traced.statistics);
	}
	dumpHandler.dump("after_nir_tracing", "nir", [&]() { return nir::print(traced.module); });

	// The verifier is a debugging tool here: `tracing.verifyNir`, or the `ir.verifyAfterEachPass` hook of the tests.
	const bool verify = moduleOptions.getOptionOrDefault("tracing.verifyNir", false) ||
	                    moduleOptions.getOptionOrDefault("ir.verifyAfterEachPass", false);
	if (verify) {
		verifyOrThrow(traced.module, "the pre-SSA NIR of the trace");
	}
	auto ssa = convertToSSA(traced.module, moduleOptions, statistics, verify,
	                        moduleOptions.getOptionOrDefault<std::string>("tracing.nirAttributesFile", ""));
	dumpHandler.dump("after_nir_conversion", "nir", [&]() { return nir::print(ssa); });

	// The graph carries the module to the backend: a NIR backend compiles it as it is, the others get the 0.3 IR built
	// from it (`lowerToIR`).
	auto carrier = std::make_shared<ir::IRGraph>(irArenaPool_->acquire(), compilationId);
	carrier->setNirModule(std::make_shared<const nir::Module>(std::move(ssa)));
	if (statistics != nullptr) {
		statistics->recordTimingMs("frontend.totalMs", frontendStart);
	}
	return carrier;
}

void CompilationPipeline::lowerToIR(ir::IRGraph& graph, const engine::ModuleOptions& moduleOptions,
                                    CompilationStatistics* statistics, DumpHandler& dumpHandler) const {
	const auto bridgeStart = std::chrono::steady_clock::now();
	const auto module = std::static_pointer_cast<const nir::Module>(graph.getNirModule());
	buildIRFromNir(*module, graph, moduleOptions);
	if (statistics != nullptr) {
		statistics->recordTimingMs("bridge.ms", bridgeStart);
		const auto snapshot = ir::computeStatistics(graph);
		statistics->set("ir.functions", static_cast<int64_t>(snapshot.numFunctions));
		statistics->set("ir.blocks", static_cast<int64_t>(snapshot.numBlocks));
		statistics->set("ir.blockArguments", static_cast<int64_t>(snapshot.numBlockArguments));
		statistics->set("ir.operations", static_cast<int64_t>(snapshot.numOperations));
	}
	ir::IRPrintOptions irPrintOptions;
	dumpHandler.dump("after_ir_creation", "nautilus", [&]() { return graph.toString(irPrintOptions); });
	if (!moduleOptions.getOptionOrDefault("ir.runPasses", true)) {
		return;
	}
	// The conversion of NIR has done what the optimization passes of 0.3 do; they run only when asked for. What every
	// backend needs, the exception regions its landing pads come from, always runs.
	ir::IRPassManager passManager(moduleOptions, &dumpHandler, statistics, &irPrintOptions);
	if (moduleOptions.getOptionOrDefault("ir.runOptimizationPasses", false)) {
		addOptimizationPasses(passManager, moduleOptions, IROptimizationLevel::Full);
	}
	passManager.addPass(std::make_unique<ir::ExceptionRegionPreparationPass>());
	if (moduleOptions.getOptionOrDefault("debug", false) || moduleOptions.getOptionOrDefault("perf", false)) {
		passManager.addPass(std::make_unique<ir::IRLocationPass>());
	}
	passManager.run(graph);
	dumpHandler.dump("after_ir_passes", "nautilus", [&]() { return graph.toString(irPrintOptions); });
}

std::shared_ptr<ir::IRGraph> CompilationPipeline::compileToIR(std::list<CompilableFunction>& functions,
                                                              const engine::ModuleOptions& moduleOptions,
                                                              CompilationStatistics* statistics,
                                                              IROptimizationLevel optimization) const {
	const CompilationUnitID compilationId = createCompilationUnitID();
	auto dumpHandler = DumpHandler(moduleOptions, compilationId);

	if (statistics != nullptr && !statistics->contains("compilation.unitId")) {
		statistics->set("compilation.unitId", compilationId);
	}

	// The default front end: the direct NIR tracer and the conversion. `tracing.nirTracer=false` selects the 0.3
	// pipeline below, kept as a fallback for one release.
	if (moduleOptions.getOptionOrDefault("tracing.nirTracer", true)) {
		return traceToNir(functions, moduleOptions, statistics, compilationId, dumpHandler);
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
	// Acquire a fresh trace arena for the lifetime of this compile.  Each
	// concurrent compile() gets its own arena from the synchronized pool, so
	// there is no shared bump allocator to race on.  The handle stays alive
	// until the end of this function; on release the arena is softReset and
	// recycled back into the pool.  Trace data is dead once IR generation has
	// finished, so this is safe (the IRGraph owns a separate IR arena).
	auto traceArenaHandle = traceArenaPool_->acquire();
	common::Arena& arena = *traceArenaHandle;
	auto traceModule = tracing::TraceContext::Trace(functions, moduleOptions, arena);
	if (statistics != nullptr) {
		statistics->recordTimingMs("tracing.ms", tracingStart);
		recordTracingStatistics(*statistics, traceModule->statistics);
	}
	dumpHandler.dump("after_tracing", "trace", [&]() { return traceModule->toString(); });
	// tracing.nirChain: the trace is also mapped to NIR and converted, for the `nirtbc` pseudo-backend of the tests
	const auto nirAttributesFile = moduleOptions.getOptionOrDefault<std::string>("tracing.nirAttributesFile", "");
	const bool nirChain = moduleOptions.getOptionOrDefault("tracing.nirChain", false);
	std::shared_ptr<const nir::Module> chainModule;
	if (const bool verifyNir = moduleOptions.getOptionOrDefault("tracing.verifyNir", false) || nirChain;
	    verifyNir || !nirAttributesFile.empty() || dumpHandler.shouldDump("after_nir_tracing")) {
		try {
			auto nirModule = tracing::nirtrace::toPreSSA(*traceModule);
			dumpHandler.dump("after_nir_tracing", "nir", [&]() { return nir::print(nirModule); });
			if (verifyNir) {
				verifyOrThrow(nirModule, "the pre-SSA NIR of the trace");
			}
			if (verifyNir || !nirAttributesFile.empty() || statistics != nullptr) {
				auto ssa = convertToSSA(nirModule, moduleOptions, statistics, verifyNir, nirAttributesFile);
				if (nirChain) {
					chainModule = std::make_shared<const nir::Module>(std::move(ssa));
				}
			}
		} catch (const std::exception&) {
			// a dump must not fail a compilation; the verification exists to fail it
			if (verifyNir) {
				throw;
			}
		}
	}
	// Not part of dump.all: resolving every copy's call stack through the debug information is slow.
	if (moduleOptions.getOptionOrDefault("dump.copySites", false)) {
		tracing::SourceLocationResolver copySiteResolver;
		dumpHandler.forceDump("copy_sites", "txt", tracing::formatCopySites(*traceModule, copySiteResolver));
	}

	const auto irGenStart = std::chrono::steady_clock::now();
	auto ir = tracing::TraceToIRConversionPhase().apply(std::move(traceModule), *irArenaPool_, compilationId);
	if (statistics != nullptr) {
		statistics->recordTimingMs("irGeneration.ms", irGenStart);
	}
	if (chainModule) {
		ir->setNirModule(std::move(chainModule));
	}
	if (statistics != nullptr) {
		const auto snapshot = ir::computeStatistics(*ir);
		statistics->set("ir.functions", static_cast<int64_t>(snapshot.numFunctions));
		statistics->set("ir.blocks", static_cast<int64_t>(snapshot.numBlocks));
		statistics->set("ir.blockArguments", static_cast<int64_t>(snapshot.numBlockArguments));
		statistics->set("ir.operations", static_cast<int64_t>(snapshot.numOperations));
	}
	dumpHandler.dump("after_ir_creation", "nautilus", [&]() { return ir->toString(irPrintOptions); });
	if (moduleOptions.getOptionOrDefault("dump.graph", false)) {
		ir::createGraphVizFromIr(ir, moduleOptions, dumpHandler);
	}

	if (moduleOptions.getOptionOrDefault("ir.runPasses", true)) {
		ir::IRPassManager passManager(moduleOptions, &dumpHandler, statistics, &irPrintOptions);
		// The optimization passes are for the backends that execute the IR as
		// it is; a backend that optimizes on its own (MLIR, through LLVM) gets
		// the same cleanups from its pipeline, so by default the graph gets
		// only what the backends compiling it ask for. The option pins it
		// either way.
		if (moduleOptions.hasOption("ir.runOptimizationPasses")) {
			optimization = moduleOptions.getOptionOrDefault("ir.runOptimizationPasses", true)
			                   ? IROptimizationLevel::Full
			                   : IROptimizationLevel::None;
		}
		addOptimizationPasses(passManager, moduleOptions, optimization);
		// Proves Nautilus-to-Nautilus calls noUnwind via whole-module
		// call-graph analysis, downgrading calls the trace-time heuristic
		// pessimistically marked exception-handling; see
		// NoThrowInferencePass.hpp. Must run before exception-region
		// preparation so a proven-noThrow function skips landing-pad
		// construction entirely.
		passManager.addPass(std::make_unique<ir::NoThrowInferencePass>());
		// Exception-region preparation: collects cleanup metadata for backends.
		// Terminal pass -- runs once after all optimisation. Every backend
		// lowers its landing pads from this side table, so it is not optional.
		passManager.addPass(std::make_unique<ir::ExceptionRegionPreparationPass>());
		// Records where every operation lands in a rendering of the final IR,
		// and publishes it on the graph for the backend. Strictly last: a map
		// is a snapshot, and any pass that mints or removes an operation after
		// it invalidates every line.
		//
		// Opt-in, because it renders the whole module to a string: only when
		// the MLIR backend is going to point DWARF line numbers at a
		// Nautilus-IR dump. `perf` needs this exactly as much as `debug`
		// does -- both are the MLIR backend's DebugInfoOptions::
		// emitDebugInfo() axis, mirrored here via raw options since this
		// file is backend-agnostic and cannot depend on the MLIR backend's
		// headers. Skipping it when only perf is set would not break
		// correctness (MLIRCompilationBackend falls back to computing the
		// map itself for a locationMap-less graph), only duplicate the work
		// IRLocationPass would otherwise have cached.
		const bool writesDwarfSource =
		    moduleOptions.getOptionOrDefault("debug", false) || moduleOptions.getOptionOrDefault("perf", false);
		if (writesDwarfSource) {
			passManager.addPass(std::make_unique<ir::IRLocationPass>());
		}
		passManager.run(*ir);
		dumpHandler.dump("after_ir_passes", "nautilus", [&]() { return ir->toString(irPrintOptions); });
	}

	if (statistics != nullptr) {
		statistics->recordTimingMs("frontend.totalMs", frontendStart);
	}
	return ir;
}

void CompilationPipeline::prepareFor(const std::shared_ptr<ir::IRGraph>& ir, const std::string& backendName,
                                     const engine::ModuleOptions& moduleOptions,
                                     CompilationStatistics* statistics) const {
	const auto backend = backends->getBackend(backendName);
	// A graph with a NIR module and no functions is the carrier of the NIR front end; one that has functions already
	// has its 0.3 IR (built for the backend of a previous tier, or by the 0.3 tracer).
	if (ir->getNirModule() == nullptr || !ir->getFunctionOperations().empty() || backend->compilesNir()) {
		return;
	}
	const auto compilationId = ir->getId();
	auto dumpHandler = DumpHandler(moduleOptions, compilationId);
	lowerToIR(*ir, moduleOptions, statistics, dumpHandler);
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

	prepareFor(ir, backendName, moduleOptions, statistics);
	const auto backend = backends->getBackend(backendName);
	auto executable = backend->compile(ir, dumpHandler, moduleOptions, statistics);
	executable->setGeneratedFiles(dumpHandler.getGeneratedFiles());
	return executable;
}

#else

std::shared_ptr<ir::IRGraph> CompilationPipeline::compileToIR(std::list<CompilableFunction>&,
                                                              const engine::ModuleOptions&, CompilationStatistics*,
                                                              IROptimizationLevel) const {
	throw RuntimeException("Jit not initialised");
}

std::unique_ptr<Executable> CompilationPipeline::compileIR(const std::shared_ptr<ir::IRGraph>&, const std::string&,
                                                           const engine::ModuleOptions&, CompilationStatistics*) const {
	throw RuntimeException("Jit not initialised");
}

void CompilationPipeline::prepareFor(const std::shared_ptr<ir::IRGraph>&, const std::string&,
                                     const engine::ModuleOptions&, CompilationStatistics*) const {
}

#endif

IROptimizationLevel CompilationPipeline::irOptimizationLevel(std::initializer_list<std::string> backendNames) const {
	bool any = false;
	auto level = IROptimizationLevel::None;
	for (const auto& name : backendNames) {
		if (!backends->hasBackend(name)) {
			continue;
		}
		any = true;
		level = std::max(level, backends->getBackend(name)->irOptimizationLevel());
	}
	return any ? level : IROptimizationLevel::Full;
}

} // namespace nautilus::compiler
