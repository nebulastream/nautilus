#include "nautilus/compiler/CompilationPipeline.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/common/Deterministic.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/config.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/link/LinkSession.hpp"
#include "nautilus/logging.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <random>
#include <set>
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
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/nir/Printer.hpp"
#include "nautilus/nir/Verifier.hpp"
#include "nautilus/passes/Passes.hpp"
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
	if (common::deterministicMode()) {
		// the same shape as below, from a counter: no clock, no entropy source, no dependence on when a second ends
		static std::atomic<uint64_t> counter {0};
		return fmt::format("1970-01-01_00-00-00.000000000_#{:07X}", counter.fetch_add(1) & 0xFFFFFFF);
	}
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
	// kept as the plain struct and turned into entries when the statistics are read
	statistics.defer([t](CompilationStatistics& s) {
		s.set("tracing.iterations", static_cast<int64_t>(t.iterations));
		s.set("tracing.replayedOperations", static_cast<int64_t>(t.replayedOperations));
		s.set("tracing.snapshots", static_cast<int64_t>(t.snapshots));
		s.set("tracing.operations", static_cast<int64_t>(t.operations));
		s.set("tracing.pausedOperations", static_cast<int64_t>(t.pausedOperations));
		s.set("tracing.copies", static_cast<int64_t>(t.copies()));
		s.set("tracing.copies.construct", static_cast<int64_t>(t.copiesConstruct));
		s.set("tracing.copies.assign", static_cast<int64_t>(t.copiesAssign));
		s.set("tracing.copies.explicit", static_cast<int64_t>(t.copiesExplicit));
		s.set("tracing.scopes", static_cast<int64_t>(t.scopes));
		s.set("tracing.maxScopeIterations", static_cast<int64_t>(t.maxScopeIterations));
		s.set("tracing.firstRun.ms", t.firstRunMs);
		s.set("tracing.rerun.ms", t.rerunMs);
		s.set("tracing.rerun.follow.ms", t.rerunFollowMs);
		s.set("tracing.rerun.record.ms", t.rerunRecordMs);
		s.set("tracing.rerun.paused.ms", t.rerunPausedMs);
		// the share of the pass time that went into re-runs (0 when nothing ran twice): the target of R1-R4
		const double passes = t.firstRunMs + t.rerunMs;
		s.set("tracing.rerun.share", passes > 0 ? t.rerunMs / passes : 0.0);
		s.set("tracing.unwoundPasses", static_cast<int64_t>(t.unwoundPasses));
		s.set("tracing.unwindFallbacks", static_cast<int64_t>(t.unwindFallbacks));
		s.set("tracing.constructs", static_cast<int64_t>(t.constructs));
		s.set("tracing.constructRestarts", static_cast<int64_t>(t.constructRestarts));
	});
}

/// The warnings of the tracer (docs/diagnostics.md) as `diagnostics.count` and
/// `diagnostics.<n>.{code,message,hint,location}`.
void recordTracingDiagnostics(CompilationStatistics& statistics, const std::vector<tracing::TracingDiagnostic>& list) {
	statistics.set("diagnostics.count", static_cast<int64_t>(list.size()));
	for (size_t i = 0; i < list.size(); ++i) {
		const auto prefix = "diagnostics." + std::to_string(i) + ".";
		statistics.set(prefix + "code", list[i].code);
		statistics.set(prefix + "message", list[i].message);
		statistics.set(prefix + "hint", list[i].hint);
		statistics.set(prefix + "location", list[i].location());
	}
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
		statistics->defer([conversionStats](CompilationStatistics& s) {
			s.set("conversion.instructions", static_cast<int64_t>(conversionStats.instructions));
			s.set("conversion.phis", static_cast<int64_t>(conversionStats.phis));
			s.set("conversion.removedPhis", static_cast<int64_t>(conversionStats.removedPhis));
			s.set("conversion.folded", static_cast<int64_t>(conversionStats.folded));
			s.set("conversion.cse", static_cast<int64_t>(conversionStats.cse));
			s.set("conversion.refoldSweeps", static_cast<int64_t>(conversionStats.refoldSweeps));
			s.set("conversion.refoldedInsts", static_cast<int64_t>(conversionStats.refoldedInsts));
			s.set("conversion.deadBlocks", static_cast<int64_t>(conversionStats.deadBlocks));
			s.set("conversion.deadValues", static_cast<int64_t>(conversionStats.deadValues));
			s.set("conversion.foldedBranches", static_cast<int64_t>(conversionStats.foldedBranches));
			s.set("conversion.loops", static_cast<int64_t>(conversionStats.loops));
			// no optional pass has run yet; the passes that do set the key themselves (03 §6)
			if (!s.containsDirect("irPasses.pipelineIterations")) {
				s.set("irPasses.pipelineIterations", int64_t {0});
			}
		});
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

/// The options of passes that 0.3 had and 0.4 does not: the conversion does what they did (03 §2.1). They are accepted
/// and ignored, with one warning each per process.
void warnRemovedPassOptions(const engine::ModuleOptions& options) {
	static constexpr std::string_view removed[] = {"ir.disableConstantFolding",
	                                               "ir.disableAlgebraicSimplification",
	                                               "ir.disableConstantBranchFolding",
	                                               "ir.disableEmptyBlockElimination",
	                                               "ir.disableBlockMerging",
	                                               "ir.disableDeadCodeElimination",
	                                               "ir.disableBlockArgumentPruning",
	                                               "ir.disableAttributeInference",
	                                               "ir.enableLocalCSE"};
	static std::mutex mutex;
	static std::set<std::string_view> warned;
	for (const auto name : removed) {
		if (!options.hasOption(name)) {
			continue;
		}
		const std::lock_guard<std::mutex> lock(mutex);
		if (warned.insert(name).second) {
			log::warn("The option '{}' belongs to an IR pass that 0.4 does not have: the conversion folds, simplifies, "
			          "merges, removes dead code and infers attributes on its own. The option is ignored.",
			          std::string(name));
		}
	}
}

/// What a pipeline of passes did, added to the statistics under `irPasses.*`.
void recordPassStatistics(CompilationStatistics& statistics, const passes::PipelineReport& report) {
	auto addMs = [&](const std::string& key, double ms) {
		const auto* existing = statistics.find(key);
		statistics.set(key, ms + (existing != nullptr && std::holds_alternative<double>(*existing)
		                              ? std::get<double>(*existing)
		                              : 0.0));
	};
	// read first: the first read expands the deferred groups, one of which sets the key to 0
	auto addCount = [&](const std::string& key, uint64_t n) {
		const auto* existing = statistics.find(key);
		statistics.set(key, static_cast<int64_t>(n) + (existing != nullptr && std::holds_alternative<int64_t>(*existing)
		                                                   ? std::get<int64_t>(*existing)
		                                                   : int64_t {0}));
	};
	addCount("irPasses.pipelineIterations", report.iterations);
	addMs("irPasses.totalMs", report.totalMs);
	for (const auto& pass : report.passes) {
		const std::string prefix = "irPasses." + pass.name;
		addMs(prefix + ".ms", pass.ms);
		addCount(prefix + ".runs", pass.runs);
		addCount(prefix + ".changes", pass.changes);
	}
}

/// Gives the inlining pass the modules that the import link of a compile resolved (13 §4.4.3).
class LinkImportView final : public passes::ImportView {
public:
	explicit LinkImportView(const link::ImportLink& link) : link_(link) {
	}
	Exporter exporter(const nir::Module&, uint32_t importIndex) const override {
		Exporter result;
		if (importIndex >= link_.bindings.size()) {
			return result;
		}
		const auto& binding = link_.bindings[importIndex];
		result.name = binding.module;
		result.hash = binding.exporterHash;
		if (binding.exporter != nullptr) {
			result.module = binding.exporter->module();
		}
		return result;
	}

private:
	const link::ImportLink& link_;
};

std::unique_ptr<passes::Pass> pluginPass(const ext::PassInfo& info, std::string backend) {
	return std::make_unique<passes::FunctionPass>(
	    info.name,
	    [run = info.run, backend = std::move(backend)](const nir::Module& module, passes::PassContext& context) {
		    return run(module, ext::PassEnvironment {context.options, backend});
	    });
}

/// Whether the optional passes run in a compile (03 §5): not at all with `ir.runPasses=false` or
/// `ir.runOptimizationPasses=false`, always with `ir.runOptimizationPasses=true`, and otherwise unless the compile is
/// latency-critical (the tier-0 compile of a module that is promoted).
bool optionalPassesRun(const engine::ModuleOptions& options, bool latencyCritical) {
	if (!options.getOptionOrDefault("ir.runPasses", true)) {
		return false;
	}
	if (options.hasOption("ir.runOptimizationPasses")) {
		return options.getOptionOrDefault("ir.runOptimizationPasses", true);
	}
	return !latencyCritical;
}

/// Runs the passes of one compile for `backendName`: the optional plugin passes of `afterConversion`, the optional core
/// passes, and the passes of `beforeLowering` (the required ones always). Returns `graph` itself when no pass changed
/// the module, else a graph with the new module: the module of `graph` is shared by the tiers of a module and stays as
/// the conversion made it.
std::shared_ptr<ir::IRGraph> runLoweringPasses(const std::shared_ptr<ir::IRGraph>& graph,
                                               const std::string& backendName, const engine::ModuleOptions& options,
                                               CompilationStatistics* statistics, const DumpHandler& dumpHandler,
                                               bool latencyCritical) {
	auto& registry = ext::ExtensionRegistry::instance();
	const bool afterPlugins = registry.hasPassesAt(ext::PassPoint::AfterConversion);
	const bool beforePlugins = registry.hasPassesAt(ext::PassPoint::BeforeLowering);
	if (!options.hasIrOptions() && !afterPlugins && !beforePlugins) {
		return graph; // the default compile: nothing is configured, nothing is asked
	}
	const bool optional = optionalPassesRun(options, latencyCritical);
	const auto which = optional ? passes::optionalPassesOf(options) : passes::OptionalPasses {};

	passes::PassManager manager;
	if (optional && afterPlugins) {
		std::vector<std::unique_ptr<passes::Pass>> group;
		for (const auto* info : registry.passesAt(ext::PassPoint::AfterConversion)) {
			if (info->requirement == ext::PassRequirement::Optional) {
				group.push_back(pluginPass(*info, backendName));
			}
		}
		manager.addGroup(std::move(group), 1);
	}
	manager.addGroup(passes::makeOptionalPasses(which), which.maxIterations);
	if (beforePlugins) {
		std::vector<std::unique_ptr<passes::Pass>> group;
		for (const auto* info : registry.passesAt(ext::PassPoint::BeforeLowering)) {
			if (optional || info->requirement == ext::PassRequirement::Required) {
				group.push_back(pluginPass(*info, backendName));
			}
		}
		manager.addGroup(std::move(group), 1);
	}
	if (manager.empty()) {
		return graph;
	}

	const auto module = std::static_pointer_cast<const nir::Module>(graph->getNirModule());
	std::vector<std::string> notes;
	std::vector<passes::PassContext::InlinedImport> inlinedImports;
	passes::PassContext context {options};
	context.notes = statistics != nullptr ? &notes : nullptr;
	context.inlinedImports = &inlinedImports;
	std::optional<LinkImportView> imports;
	if (const auto* link = static_cast<const link::ImportLink*>(graph->getImportLink().get()); link != nullptr) {
		imports.emplace(*link);
		context.imports = &*imports;
	}
	const bool verify = options.getOptionOrDefault("tracing.verifyNir", false) ||
	                    options.getOptionOrDefault("ir.verifyAfterEachPass", false);
	passes::PipelineReport report;
	auto result = manager.run(*module, context, report, [&](const nir::Module& changed, std::string_view pass) {
		if (verify) {
			verifyOrThrow(changed, "the module after the pass " + std::string(pass), module.get());
		}
	});
	if (statistics != nullptr) {
		recordPassStatistics(*statistics, report);
		if (!notes.empty()) {
			std::string text;
			for (const auto& n : notes) {
				text += n + "\n";
			}
			statistics->set("irPasses.inline.decisions", text);
		}
		if (!inlinedImports.empty()) {
			std::string text;
			for (const auto& i : inlinedImports) {
				text += i.module + "." + i.function + "=" + (i.hash ? i.hash->hex() : std::string("unknown")) + ";";
			}
			statistics->set("irPasses.inline.importedModules", text);
		}
	}
	if (!result) {
		return graph;
	}
	auto derived = std::make_shared<ir::IRGraph>(graph->getId());
	auto changed = std::make_shared<const nir::Module>(std::move(*result));
	dumpHandler.dump("after_nir_passes", "nir", [&]() { return nir::print(*changed); });
	derived->setNirModule(std::move(changed));
	derived->setImportLink(graph->getImportLink());
	return derived;
}

/// The plugin passes that run on the module the conversion made, in every compile and once for all tiers of a module:
/// the *required* passes of `afterConversion`. They are part of what the module means (the partition pass of the GPU
/// plugin), so they run whatever the options say.
nir::Module runRequiredPluginPasses(nir::Module module, const engine::ModuleOptions& options,
                                    CompilationStatistics* statistics, bool verify) {
	passes::PassManager manager;
	std::vector<std::unique_ptr<passes::Pass>> group;
	for (const auto* info : ext::ExtensionRegistry::instance().passesAt(ext::PassPoint::AfterConversion)) {
		if (info->requirement == ext::PassRequirement::Required) {
			group.push_back(pluginPass(*info, {}));
		}
	}
	manager.addGroup(std::move(group), 1);
	if (manager.empty()) {
		return module;
	}
	passes::PassContext context {options};
	passes::PipelineReport report;
	auto result = manager.run(module, context, report, [&](const nir::Module& changed, std::string_view pass) {
		if (verify) {
			verifyOrThrow(changed, "the module after the pass " + std::string(pass), &module);
		}
	});
	if (statistics != nullptr) {
		recordPassStatistics(*statistics, report);
	}
	return result ? std::move(*result) : std::move(module);
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
		recordTracingDiagnostics(*statistics, traced.diagnostics);
	}
	dumpHandler.dump("after_nir_tracing", "nir", [&]() { return nir::print(traced.module); });

	// The verifier is a debugging tool here: `tracing.verifyNir`, or the `ir.verifyAfterEachPass` hook of the tests.
	const bool verify = moduleOptions.getOptionOrDefault("tracing.verifyNir", false) ||
	                    moduleOptions.getOptionOrDefault("ir.verifyAfterEachPass", false);
	if (verify) {
		verifyOrThrow(traced.module, "the pre-SSA NIR of the trace");
	}
	if (moduleOptions.hasIrOptions()) {
		warnRemovedPassOptions(moduleOptions);
	}
	auto ssa = convertToSSA(traced.module, moduleOptions, statistics, verify,
	                        moduleOptions.getOptionOrDefault<std::string>("tracing.nirAttributesFile", ""));
	dumpHandler.dump("after_nir_conversion", "nir", [&]() { return nir::print(ssa); });
	if (ext::ExtensionRegistry::instance().hasPassesAt(ext::PassPoint::AfterConversion)) {
		ssa = runRequiredPluginPasses(std::move(ssa), moduleOptions, statistics, verify);
	}

	// The graph carries the module to the backend: a NIR backend compiles it as it is, the others get the 0.3 IR built
	// from it (`lowerToIR`).
	// A module that is named or imports others takes part in the module registry of its engine (13 §4.4): the session
	// of this compile resolves its imports against the compiled modules and the backends call through the slots.
	auto* linkSession = engine::details::LinkSession::current();
	std::shared_ptr<const void> importLink;
	if (linkSession != nullptr) {
		importLink = link::prepareModule(*linkSession, ssa);
	}
	auto carrier = std::make_shared<ir::IRGraph>(irArenaPool_->acquire(), compilationId);
	auto finalModule = std::make_shared<const nir::Module>(std::move(ssa));
	if (linkSession != nullptr) {
		link::noteCompiledModule(*linkSession, finalModule);
	}
	carrier->setNirModule(std::move(finalModule));
	carrier->setImportLink(std::move(importLink));
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

	if (statistics != nullptr && !statistics->containsDirect("compilation.unitId")) {
		statistics->set("compilation.unitId", compilationId);
	}

	// The default front end: the direct NIR tracer and the conversion. `tracing.nirTracer=false` selects the 0.3
	// pipeline below, kept as a fallback for one release, and so does `dump.copySites`, which needs its tags.
	// `debug`, `perf` and `perf.sample` stay on this pipeline: the NIR tracer records the debug sections they need (the
	// regions and function locations, 13 §4.9) when one of them is set, and the backends read them (M4.8).
	// A backend pinned with `engine.backend` can ask for the 0.3 pipeline too (`CompilationBackend::needs03Frontend`).
	const auto pinnedBackend = moduleOptions.getOptionOrDefault<std::string>("engine.backend", "");
	const bool backendNeeds03 = !pinnedBackend.empty() && backends->hasBackend(pinnedBackend) &&
	                            backends->getBackend(pinnedBackend)->needs03Frontend();
	const bool needs03Trace = backendNeeds03 || moduleOptions.getOptionOrDefault("dump.copySites", false);
	if (moduleOptions.getOptionOrDefault("tracing.nirTracer", true) && !needs03Trace) {
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

CompilationUnitID CompilationPipeline::unitIdOf(const ir::IRGraph& ir) {
	return ir.getId();
}

bool CompilationPipeline::carriesOnlyNir(const ir::IRGraph& ir) {
	return ir.getNirModule() != nullptr && ir.getFunctionOperations().empty();
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

std::unique_ptr<Executable>
CompilationPipeline::compileIR(const std::shared_ptr<ir::IRGraph>& ir, const std::string& backendName,
                               const engine::ModuleOptions& moduleOptions, CompilationStatistics* statistics,
                               const CompilationUnitID& unitId, const nirbackend::CancellationToken* cancellation,
                               bool latencyCritical) const {
	const CompilationUnitID compilationId = unitId.empty() ? createCompilationUnitID() : unitId;
	auto dumpHandler = DumpHandler(moduleOptions, compilationId);

	if (statistics != nullptr) {
		if (!statistics->containsDirect("compilation.unitId")) {
			statistics->set("compilation.unitId", compilationId);
		}
		statistics->set("backend.name", backendName);
	}

	// the passes of this compile (03 §5) work on a copy of the module: the graph's own is shared by the tiers
	auto graph = ir;
	if (carriesOnlyNir(*ir)) {
		graph = runLoweringPasses(ir, backendName, moduleOptions, statistics, dumpHandler, latencyCritical);
	}
	prepareFor(graph, backendName, moduleOptions, statistics);
	const auto backend = backends->getBackend(backendName);
	auto executable = backend->compile(graph, dumpHandler, moduleOptions, statistics, cancellation);
	if (executable == nullptr) {
		return nullptr; // cancelled
	}
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
                                                           const engine::ModuleOptions&, CompilationStatistics*,
                                                           const CompilationUnitID&,
                                                           const nirbackend::CancellationToken*, bool) const {
	throw RuntimeException("Jit not initialised");
}

CompilationUnitID CompilationPipeline::unitIdOf(const ir::IRGraph&) {
	return {};
}

bool CompilationPipeline::carriesOnlyNir(const ir::IRGraph&) {
	return false;
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
