#include "IRGraphFixtures.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/passes/LoopInfo.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::testing {

namespace {

using compiler::ir::BasicBlock;
using compiler::ir::BlockIdentifier;
using compiler::ir::FunctionOperation;
using compiler::ir::IRGraph;

BasicBlock* blockWithId(FunctionOperation& fn, uint32_t id) {
	for (auto* block : fn.getBasicBlocks()) {
		if (block->getIdentifier().getId() == id) {
			return block;
		}
	}
	return nullptr;
}

} // namespace

TEST_CASE("LoopInfo: findNaturalLoops recognizes the single-latch/single-preheader loop") {
	auto ir = IRGraphFixtures::makeNaturalLoopGraph();
	compiler::ir::rebuildPredecessorLists(*ir);
	auto& fn = *ir->getFunctionOperations().front();

	auto loops = compiler::ir::findNaturalLoops(fn);
	REQUIRE(loops.size() == 1);

	const auto& loop = loops.front();
	CHECK(loop.header == blockWithId(fn, 1));
	CHECK(loop.preheader == blockWithId(fn, 0));
	CHECK(loop.latch == blockWithId(fn, 2));

	// Body is header + latch; preheader and exit are outside it.
	CHECK(loop.body.contains(blockWithId(fn, 1)));
	CHECK(loop.body.contains(blockWithId(fn, 2)));
	CHECK_FALSE(loop.body.contains(blockWithId(fn, 0)));
	CHECK_FALSE(loop.body.contains(blockWithId(fn, 3)));

	// The recognized edges must be the actual preheader/latch invocations.
	CHECK(loop.preheaderInv->getBlock() == loop.header);
	CHECK(loop.latchInv->getBlock() == loop.header);
}

TEST_CASE("LoopInfo: pass-through header argument is invariant, real induction variables are not") {
	auto ir = IRGraphFixtures::makeNaturalLoopGraph();
	compiler::ir::rebuildPredecessorLists(*ir);
	auto& fn = *ir->getFunctionOperations().front();
	auto loops = compiler::ir::findNaturalLoops(fn);
	REQUIRE(loops.size() == 1);
	const auto& loop = loops.front();

	// Header arguments are {iv, acc, limit}: `limit` is threaded back unchanged
	// (a pass-through), `iv`/`acc` get a fresh value on the back edge.
	const auto& headerArgs = loop.header->getArguments();
	REQUIRE(headerArgs.size() == 3);
	auto passThrough = compiler::ir::computeHeaderPassThroughMap(loop.header, loop.latchInv, loop.body);

	CHECK(passThrough.contains(headerArgs[2]));       // limit -> invariant
	CHECK_FALSE(passThrough.contains(headerArgs[0])); // iv    -> varying
	CHECK_FALSE(passThrough.contains(headerArgs[1])); // acc   -> varying

	auto definingBlock = compiler::ir::computeDefiningBlocks(fn);
	CHECK(compiler::ir::isLoopInvariant(headerArgs[2], definingBlock, loop.body, passThrough));
	CHECK_FALSE(compiler::ir::isLoopInvariant(headerArgs[0], definingBlock, loop.body, passThrough));
}

TEST_CASE("LoopInfo: a function with no loops yields no natural loops") {
	auto ir = IRGraphFixtures::makeDiamondGraph();
	compiler::ir::rebuildPredecessorLists(*ir);
	auto& fn = *ir->getFunctionOperations().front();
	CHECK(compiler::ir::findNaturalLoops(fn).empty());
}

} // namespace nautilus::testing
