#include "IRGraphFixtures.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::testing {

namespace {

void requireVerifierClean(const std::shared_ptr<compiler::ir::IRGraph>& ir) {
	compiler::ir::rebuildPredecessorLists(*ir);
	auto result = compiler::ir::IRVerifier::verify(*ir);
	INFO(result.toString());
	REQUIRE(result.ok());
}

} // namespace

TEST_CASE("IRGraphFixtures: makeDiamondGraph is verifier-clean") {
	requireVerifierClean(IRGraphFixtures::makeDiamondGraph());
}

TEST_CASE("IRGraphFixtures: makeSharedTargetIfGraph is verifier-clean") {
	auto ir = IRGraphFixtures::makeSharedTargetIfGraph();
	requireVerifierClean(ir);

	// The duplicate-predecessor shape: entry must appear twice in merge's
	// predecessor list (once per invocation of the shared-target if).
	const auto& fn = *ir->getFunctionOperations().front();
	const compiler::ir::BasicBlock* mergeBlock = nullptr;
	const compiler::ir::BasicBlock* entry = fn.getEntryBlock();
	for (const auto* block : fn.getBasicBlocks()) {
		if (block != entry) {
			mergeBlock = block;
		}
	}
	REQUIRE(mergeBlock != nullptr);
	int count = 0;
	for (const auto* pred : mergeBlock->getPredecessors()) {
		if (pred == entry) {
			count++;
		}
	}
	REQUIRE(count == 2);
}

TEST_CASE("IRGraphFixtures: makeNaturalLoopGraph is verifier-clean") {
	requireVerifierClean(IRGraphFixtures::makeNaturalLoopGraph());
}

TEST_CASE("IRGraphFixtures: makeDeadChainGraph is verifier-clean") {
	requireVerifierClean(IRGraphFixtures::makeDeadChainGraph());
}

} // namespace nautilus::testing
