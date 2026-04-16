
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/ir/passes/IRStatistics.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"

namespace nautilus::compiler::ir {

IRPassManager::IRPassManager(const engine::Options& options, compiler::DumpHandler* dumpHandler)
    : dumpHandler(dumpHandler), verifyBeforePipeline(options.getOptionOrDefault("ir.verifyBeforePipeline", false)),
      verifyAfterEachPass(options.getOptionOrDefault("ir.verifyAfterEachPass", false)),
      failOnVerifyError(options.getOptionOrDefault("ir.failOnVerifyError", false)),
      logStatistics(options.getOptionOrDefault("ir.logPassStatistics", false)),
      dumpAfterEachPass(options.getOptionOrDefault("ir.dumpAfterEachPass", false)) {
}

void IRPassManager::addPass(std::unique_ptr<IRPass> pass) {
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

	IRStatistics baseline;
	if (logStatistics) {
		baseline = computeStatistics(ir);
		log::info("[ir-passes] baseline {}", baseline.toString());
	}

	for (const auto& pass : passes) {
		const auto name = pass->getName();
		IRStatistics before;
		if (logStatistics) {
			before = computeStatistics(ir);
		}

		pass->apply(ir);

		if (logStatistics) {
			auto after = computeStatistics(ir);
			auto delta = after - before;
			log::info("[ir-passes] after {} {} (delta functions={} blocks={} operations={})", name, after.toString(),
			          delta.numFunctions, delta.numBlocks, delta.numOperations);
		}

		if (verifyAfterEachPass) {
			verifyAndReport(ir, name.c_str());
		}

		if (dumpAfterEachPass && dumpHandler != nullptr) {
			const std::string stageName = "after_" + name;
			dumpHandler->dump(stageName, "ir", [&]() { return ir.toString(); });
		}
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
