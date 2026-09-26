#include "IRGraphFixtures.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/passes/LoopInfo.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::testing {

namespace {

using compiler::ir::BasicBlock;
using compiler::ir::BasicBlockArgument;
using compiler::ir::BlockIdentifier;
using compiler::ir::FunctionOperation;
using compiler::ir::IRGraph;
using compiler::ir::Operation;
using compiler::ir::OperationIdentifier;

BasicBlock* blockWithId(FunctionOperation& fn, uint32_t id) {
	for (auto* block : fn.getBasicBlocks()) {
		if (block->getIdentifier().getId() == id) {
			return block;
		}
	}
	return nullptr;
}

/// entry(0) -> outerHeader(1) -> innerPreheader(2) -> innerHeader(3) -> innerLatch(4) -> innerHeader
///                           \-> exit(6)                            \-> outerLatch(5) -> outerHeader
std::shared_ptr<IRGraph> makeNestedLoopGraph() {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<IRGraph>("loopinfo-nested");
	auto& arena = irGraph->getArena();

	auto* n = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* oiv = arena.create<BasicBlockArgument>(OperationIdentifier {20}, Type::i32);
	auto* iiv = arena.create<BasicBlockArgument>(OperationIdentifier {30}, Type::i32);
	auto* olv = arena.create<BasicBlockArgument>(OperationIdentifier {50}, Type::i32);

	auto* exit = arena.create<BasicBlock>(arena, BlockIdentifier {6}, std::vector<BasicBlockArgument*> {});
	exit->addOperation<ir::ReturnOperation>();

	auto* outerHeader = arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {oiv});
	auto* innerPreheader = arena.create<BasicBlock>(arena, BlockIdentifier {2}, std::vector<BasicBlockArgument*> {});
	auto* innerHeader = arena.create<BasicBlock>(arena, BlockIdentifier {3}, std::vector<BasicBlockArgument*> {iiv});
	auto* innerLatch = arena.create<BasicBlock>(arena, BlockIdentifier {4}, std::vector<BasicBlockArgument*> {});
	auto* outerLatch = arena.create<BasicBlock>(arena, BlockIdentifier {5}, std::vector<BasicBlockArgument*> {olv});

	auto* ocmp =
	    outerHeader->addOperation<ir::CompareOperation>(OperationIdentifier {21}, oiv, n, ir::CompareOperation::LT);
	auto* oif = arena.create<ir::IfOperation>(arena, ocmp, 0.9);
	outerHeader->addOperation(oif);
	oif->setTrueBlockInvocation(innerPreheader);
	oif->setFalseBlockInvocation(exit);

	auto* iiv0 = innerPreheader->addOperation<ir::ConstIntOperation>(OperationIdentifier {22}, int64_t {0}, Type::i32);
	innerPreheader->addNextBlock(innerHeader, std::vector<Operation*> {iiv0});

	auto* icmp =
	    innerHeader->addOperation<ir::CompareOperation>(OperationIdentifier {31}, iiv, n, ir::CompareOperation::LT);
	auto* iif = arena.create<ir::IfOperation>(arena, icmp, 0.9);
	innerHeader->addOperation(iif);
	iif->setTrueBlockInvocation(innerLatch);
	iif->setFalseBlockInvocation(outerLatch);
	iif->getFalseBlockInvocation().addArgument(arena, oiv);

	auto* ione = innerLatch->addOperation<ir::ConstIntOperation>(OperationIdentifier {40}, int64_t {1}, Type::i32);
	auto* niiv = innerLatch->addOperation<ir::AddOperation>(OperationIdentifier {41}, iiv, ione);
	innerLatch->addNextBlock(innerHeader, std::vector<Operation*> {niiv});

	auto* oone = outerLatch->addOperation<ir::ConstIntOperation>(OperationIdentifier {51}, int64_t {1}, Type::i32);
	auto* noiv = outerLatch->addOperation<ir::AddOperation>(OperationIdentifier {52}, olv, oone);
	outerLatch->addNextBlock(outerHeader, std::vector<Operation*> {noiv});

	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {n});
	auto* oiv0 = entry->addOperation<ir::ConstIntOperation>(OperationIdentifier {10}, int64_t {0}, Type::i32);
	entry->addNextBlock(outerHeader, std::vector<Operation*> {oiv0});

	auto* fn = arena.create<FunctionOperation>(
	    "execute",
	    std::vector<BasicBlock*> {entry, outerHeader, innerPreheader, innerHeader, innerLatch, outerLatch, exit},
	    std::vector<Type> {Type::i32}, std::vector<std::string> {"n"}, Type::v);
	irGraph->addFunctionOperation(fn);
	return irGraph;
}

/// entry(0) -> header(1) -if-> latchA(2) / latchB(3), both branching back to
/// header: a loop with two latches, which the natural-loop recognizer does
/// not accept, but a loop all the same.
std::shared_ptr<IRGraph> makeTwoLatchLoopGraph() {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<IRGraph>("loopinfo-two-latches");
	auto& arena = irGraph->getArena();

	auto* cond = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::b);
	auto* header = arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {});
	auto* latchA = arena.create<BasicBlock>(arena, BlockIdentifier {2}, std::vector<BasicBlockArgument*> {});
	auto* latchB = arena.create<BasicBlock>(arena, BlockIdentifier {3}, std::vector<BasicBlockArgument*> {});
	auto* headerIf = arena.create<ir::IfOperation>(arena, cond, 0.5);
	header->addOperation(headerIf);
	headerIf->setTrueBlockInvocation(latchA);
	headerIf->setFalseBlockInvocation(latchB);
	latchA->addNextBlock(header, std::vector<Operation*> {});
	latchB->addNextBlock(header, std::vector<Operation*> {});

	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {cond});
	entry->addNextBlock(header, std::vector<Operation*> {});

	auto* fn = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, header, latchA, latchB},
	                                           std::vector<Type> {Type::b}, std::vector<std::string> {"cond"}, Type::v);
	irGraph->addFunctionOperation(fn);
	return irGraph;
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
	CHECK_FALSE(compiler::ir::containsLoop(fn));
}

TEST_CASE("LoopInfo: nested loops are each recognized with their own body") {
	auto ir = makeNestedLoopGraph();
	compiler::ir::rebuildPredecessorLists(*ir);
	auto& fn = *ir->getFunctionOperations().front();

	auto loops = compiler::ir::findNaturalLoops(fn);
	REQUIRE(loops.size() == 2);

	// Headers come in block-list order: the outer loop first.
	const auto& outer = loops[0];
	CHECK(outer.header == blockWithId(fn, 1));
	CHECK(outer.preheader == blockWithId(fn, 0));
	CHECK(outer.latch == blockWithId(fn, 5));
	CHECK(outer.body.size() == 5);
	for (uint32_t id : {1u, 2u, 3u, 4u, 5u}) {
		CHECK(outer.body.contains(blockWithId(fn, id)));
	}
	CHECK_FALSE(outer.body.contains(blockWithId(fn, 0)));
	CHECK_FALSE(outer.body.contains(blockWithId(fn, 6)));

	// The inner loop's preheader is inside the outer loop -- and reachable
	// from the inner header around the outer back edge -- but that is a
	// forward edge into the inner header all the same: the inner header does
	// not dominate it.
	const auto& inner = loops[1];
	CHECK(inner.header == blockWithId(fn, 3));
	CHECK(inner.preheader == blockWithId(fn, 2));
	CHECK(inner.latch == blockWithId(fn, 4));
	CHECK(inner.body.size() == 2);
	CHECK(inner.body.contains(blockWithId(fn, 3)));
	CHECK(inner.body.contains(blockWithId(fn, 4)));

	CHECK(compiler::ir::containsLoop(fn));
}

TEST_CASE("LoopInfo: containsLoop sees cycles the natural-loop recognizer rejects") {
	auto ir = makeTwoLatchLoopGraph();
	compiler::ir::rebuildPredecessorLists(*ir);
	auto& fn = *ir->getFunctionOperations().front();

	// Three predecessor edges into the header: not a shape the passes handle.
	CHECK(compiler::ir::findNaturalLoops(fn).empty());
	// Still a loop, and an attribute analysis must not conclude it returns.
	CHECK(compiler::ir::containsLoop(fn));

	auto loop = IRGraphFixtures::makeNaturalLoopGraph();
	compiler::ir::rebuildPredecessorLists(*loop);
	CHECK(compiler::ir::containsLoop(*loop->getFunctionOperations().front()));
}

} // namespace nautilus::testing
