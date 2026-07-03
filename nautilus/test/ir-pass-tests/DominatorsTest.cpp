#include "IRGraphFixtures.hpp"
#include "nautilus/compiler/ir/passes/Dominators.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::testing {

using compiler::ir::BasicBlock;
using compiler::ir::BlockIdentifier;
using compiler::ir::Dominators;
using compiler::ir::FunctionOperation;

namespace {

const BasicBlock* findBlock(const FunctionOperation& fn, uint32_t id) {
	for (const auto* block : fn.getBasicBlocks()) {
		if (block->getIdentifier() == BlockIdentifier {id}) {
			return block;
		}
	}
	return nullptr;
}

} // namespace

TEST_CASE("Dominators: diamond shape (D1)") {
	auto ir = IRGraphFixtures::makeDiamondGraph();
	compiler::ir::rebuildPredecessorLists(*ir);
	const auto& fn = *ir->getFunctionOperations().front();
	Dominators dom(fn);

	const auto* entry = findBlock(fn, 0);
	const auto* thenBlock = findBlock(fn, 1);
	const auto* elseBlock = findBlock(fn, 2);
	const auto* merge = findBlock(fn, 3);

	REQUIRE(dom.dominates(entry, entry));
	REQUIRE(dom.dominates(entry, thenBlock));
	REQUIRE(dom.dominates(entry, elseBlock));
	REQUIRE(dom.dominates(entry, merge));

	REQUIRE_FALSE(dom.dominates(thenBlock, merge));
	REQUIRE_FALSE(dom.dominates(elseBlock, merge));
	REQUIRE_FALSE(dom.dominates(merge, thenBlock));
	REQUIRE_FALSE(dom.dominates(merge, elseBlock));
	REQUIRE_FALSE(dom.dominates(thenBlock, elseBlock));
	REQUIRE_FALSE(dom.dominates(elseBlock, thenBlock));
}

TEST_CASE("Dominators: natural loop shape (D1)") {
	auto ir = IRGraphFixtures::makeNaturalLoopGraph();
	compiler::ir::rebuildPredecessorLists(*ir);
	const auto& fn = *ir->getFunctionOperations().front();
	Dominators dom(fn);

	const auto* preheader = findBlock(fn, 0);
	const auto* header = findBlock(fn, 1);
	const auto* latch = findBlock(fn, 2);
	const auto* exit = findBlock(fn, 3);

	REQUIRE(dom.dominates(preheader, header));
	REQUIRE(dom.dominates(preheader, latch));
	REQUIRE(dom.dominates(preheader, exit));
	REQUIRE(dom.dominates(header, latch));
	REQUIRE(dom.dominates(header, exit));
	REQUIRE_FALSE(dom.dominates(latch, exit));
	REQUIRE_FALSE(dom.dominates(latch, header));
}

TEST_CASE("Dominators: chain is transitive (D1)") {
	auto ir = IRGraphFixtures::makeChainOfEmptyBlocks(4);
	compiler::ir::rebuildPredecessorLists(*ir);
	const auto& fn = *ir->getFunctionOperations().front();
	Dominators dom(fn);
	const auto& rpo = dom.reversePostOrder();
	REQUIRE(rpo.size() == 6); // entry + 4 empty blocks + ret block
	for (size_t i = 0; i < rpo.size(); ++i) {
		for (size_t j = i; j < rpo.size(); ++j) {
			REQUIRE(dom.dominates(rpo[i], rpo[j]));
		}
	}
}

TEST_CASE("Dominators: self-dominance (D1)") {
	auto ir = IRGraphFixtures::makeDiamondGraph();
	compiler::ir::rebuildPredecessorLists(*ir);
	const auto& fn = *ir->getFunctionOperations().front();
	Dominators dom(fn);
	for (const auto* block : dom.reversePostOrder()) {
		REQUIRE(dom.dominates(block, block));
	}
}

TEST_CASE("Dominators: detached cycle is unreachable (D2)") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<ir::IRGraph>("detached-cycle-test");
	auto& arena = irGraph->getArena();

	auto* entry = arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {0}, std::vector<ir::BasicBlockArgument*> {});
	entry->addOperation<ir::ReturnOperation>();

	// A self-referential two-block cycle, never reachable from entry.
	auto* cycleA =
	    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {1}, std::vector<ir::BasicBlockArgument*> {});
	auto* cycleB =
	    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {2}, std::vector<ir::BasicBlockArgument*> {});
	cycleA->addNextBlock(cycleB, std::span<ir::Operation* const> {});
	cycleB->addNextBlock(cycleA, std::span<ir::Operation* const> {});

	auto* fn = arena.create<ir::FunctionOperation>("execute", std::vector<ir::BasicBlock*> {entry, cycleA, cycleB},
	                                               std::vector<Type> {}, std::vector<std::string> {}, Type::v);
	irGraph->addFunctionOperation(fn);
	compiler::ir::rebuildPredecessorLists(*irGraph);

	// Construction itself must not throw on a graph with an unreachable cycle.
	Dominators dom(*fn);
	REQUIRE(dom.isReachable(entry));
	REQUIRE_FALSE(dom.isReachable(cycleA));
	REQUIRE_FALSE(dom.isReachable(cycleB));
	REQUIRE(dom.reversePostOrder().size() == 1);
	REQUIRE_FALSE(dom.dominates(entry, cycleA));
	REQUIRE_FALSE(dom.dominates(cycleA, cycleA));
}

TEST_CASE("Dominators: reverse post order is deterministic across constructions (D3)") {
	auto ir = IRGraphFixtures::makeNaturalLoopGraph();
	compiler::ir::rebuildPredecessorLists(*ir);
	const auto& fn = *ir->getFunctionOperations().front();
	Dominators domA(fn);
	Dominators domB(fn);
	REQUIRE(domA.reversePostOrder() == domB.reversePostOrder());
}

} // namespace nautilus::testing
