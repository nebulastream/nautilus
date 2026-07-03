
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/ir/passes/IRStatistics.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"
#include <algorithm>
#include <chrono>

namespace nautilus::compiler::ir {

namespace {

/// Adds @p elapsedMs to the existing double-valued entry at @p key, or
/// creates it. `CompilationStatistics::add` only accumulates `int64_t`
/// counters, so per-pass timing (a duration, i.e. a `double`) is
/// accumulated by hand here to let it sum across a fixed-point group's
/// iterations.
void addMs(compiler::CompilationStatistics& stats, const std::string& key, double elapsedMs) {
	if (const auto* existing = stats.find(key)) {
		stats.set(key, std::get<double>(*existing) + elapsedMs);
	} else {
		stats.set(key, elapsedMs);
	}
}

} // namespace

IRPassManager::IRPassManager(const engine::Options& options, compiler::DumpHandler* dumpHandler,
                             compiler::CompilationStatistics* statistics, const IRPrintOptions* printOptions)
    : dumpHandler(dumpHandler), statistics(statistics),
      verifyBeforePipeline(options.getOptionOrDefault("ir.verifyBeforePipeline", false)),
      verifyAfterEachPass(options.getOptionOrDefault("ir.verifyAfterEachPass", false)),
      failOnVerifyError(options.getOptionOrDefault("ir.failOnVerifyError", false)),
      dumpAfterEachPass(options.getOptionOrDefault("ir.dumpAfterEachPass", false)), printOptions(printOptions) {
}

void IRPassManager::addPass(std::unique_ptr<IRPass> pass) {
	if (pass == nullptr) {
		return;
	}
	PassGroup group;
	group.maxIterations = 1;
	group.passes.push_back(std::move(pass));
	groups.push_back(std::move(group));
}

void IRPassManager::addFixedPointGroup(std::vector<std::unique_ptr<IRPass>> passes, size_t maxIterations) {
	if (passes.empty()) {
		return;
	}
	PassGroup group;
	group.maxIterations = std::max<size_t>(1, maxIterations);
	group.passes = std::move(passes);
	groups.push_back(std::move(group));
}

void IRPassManager::run(IRGraph& ir) {
	// Establish the predecessor invariant. Passes maintain it from there
	// via the wiring helpers (`replaceSuccessor`, `addNextBlock`, etc.).
	rebuildPredecessorLists(ir);

	if (verifyBeforePipeline) {
		verifyAndReport(ir, "pipeline-start");
	}

	const auto pipelineStart = std::chrono::steady_clock::now();

	if (statistics != nullptr) {
		const auto baseline = computeStatistics(ir);
		statistics->set("irPasses.baseline.functions", static_cast<int64_t>(baseline.numFunctions));
		statistics->set("irPasses.baseline.blocks", static_cast<int64_t>(baseline.numBlocks));
		statistics->set("irPasses.baseline.operations", static_cast<int64_t>(baseline.numOperations));
	}

	size_t totalIterations = 0;

	for (const auto& group : groups) {
		for (size_t iteration = 0; iteration < group.maxIterations; ++iteration) {
			++totalIterations;
			bool anyChangeThisRound = false;

			for (const auto& pass : group.passes) {
				const auto name = pass->getName();

				IRStatistics before;
				std::chrono::steady_clock::time_point passStart;
				if (statistics != nullptr) {
					before = computeStatistics(ir);
					passStart = std::chrono::steady_clock::now();
				}

				const bool changed = pass->apply(ir);
				anyChangeThisRound = anyChangeThisRound || changed;

				if (statistics != nullptr) {
					const auto elapsedMs =
					    std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - passStart).count();
					addMs(*statistics, "irPasses." + name + ".ms", elapsedMs);
					auto after = computeStatistics(ir);
					// `before - after` yields the signed-free delta representation
					// used by IRStatistics: a positive "delta" means the pass
					// removed entities. Preserve that convention so downstream
					// readers can interpret values consistently. Accumulated
					// (not overwritten) so a pass that runs several times inside
					// a fixed-point group reports its total effect.
					auto delta = before - after;
					statistics->add("irPasses." + name + ".blocksDelta", static_cast<int64_t>(delta.numBlocks));
					statistics->add("irPasses." + name + ".operationsDelta", static_cast<int64_t>(delta.numOperations));
				}

				if (verifyAfterEachPass) {
					verifyAndReport(ir, name.c_str());
				}

				if (changed && dumpAfterEachPass && dumpHandler != nullptr) {
					const std::string stageName = "after_" + name;
					// Default-constructed IRPrintOptions produces byte-identical
					// output to the no-arg toString().
					const IRPrintOptions opts = printOptions != nullptr ? *printOptions : IRPrintOptions {};
					dumpHandler->dump(stageName, "ir", [&]() { return ir.toString(opts); });
				}
			}

			if (!anyChangeThisRound) {
				break; // fixed point reached; no need to spend further iterations.
			}
		}
	}

	if (statistics != nullptr) {
		statistics->set("irPasses.pipelineIterations", static_cast<int64_t>(totalIterations));
		statistics->recordTimingMs("irPasses.totalMs", pipelineStart);
	}
}

void IRPassManager::verifyAndReport(const IRGraph& ir, const char* stage) {
	auto result = IRVerifier::verify(ir);
	if (result.ok()) {
		return;
	}
	const auto summary = result.toString();
	if (failOnVerifyError) {
		throw RuntimeException(std::string("IR verification failed at ") + stage + ":\n" + summary);
	}
	log::error("[ir-passes] verification failed at {}:\n{}", stage, summary);
}

} // namespace nautilus::compiler::ir
