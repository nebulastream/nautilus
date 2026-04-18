
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/ir/passes/IRStatistics.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"
#include <chrono>

namespace nautilus::compiler::ir {

IRPassManager::IRPassManager(const engine::Options& options, compiler::DumpHandler* dumpHandler,
                             compiler::CompilationStatistics* statistics)
    : dumpHandler(dumpHandler), statistics(statistics),
      verifyBeforePipeline(options.getOptionOrDefault("ir.verifyBeforePipeline", false)),
      verifyAfterEachPass(options.getOptionOrDefault("ir.verifyAfterEachPass", false)),
      failOnVerifyError(options.getOptionOrDefault("ir.failOnVerifyError", false)),
      dumpAfterEachPass(options.getOptionOrDefault("ir.dumpAfterEachPass", false)) {
}

void IRPassManager::addPass(std::shared_ptr<IRPass> pass) {
	if (pass != nullptr) {
		passes.push_back(std::move(pass));
	}
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

	for (const auto& pass : passes) {
		const auto name = pass->getName();

		IRStatistics before;
		std::chrono::steady_clock::time_point passStart;
		if (statistics != nullptr) {
			before = computeStatistics(ir);
			passStart = std::chrono::steady_clock::now();
		}

		pass->apply(ir);

		if (statistics != nullptr) {
			statistics->recordTimingMs("irPasses." + name + ".ms", passStart);
			auto after = computeStatistics(ir);
			// `before - after` yields the signed-free delta representation
			// used by IRStatistics: a positive "delta" means the pass
			// removed entities. Preserve that convention so downstream
			// readers can interpret values consistently.
			auto delta = before - after;
			statistics->set("irPasses." + name + ".blocksDelta", static_cast<int64_t>(delta.numBlocks));
			statistics->set("irPasses." + name + ".operationsDelta", static_cast<int64_t>(delta.numOperations));
		}

		if (verifyAfterEachPass) {
			verifyAndReport(ir, name.c_str());
		}

		if (dumpAfterEachPass && dumpHandler != nullptr) {
			const std::string stageName = "after_" + name;
			dumpHandler->dump(stageName, "ir", [&]() { return ir.toString(); });
		}
	}

	if (statistics != nullptr) {
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
