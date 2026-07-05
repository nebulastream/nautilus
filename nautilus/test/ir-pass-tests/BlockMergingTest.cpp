#include "IRGraphFixtures.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/passes/BlockMergingPass.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/options.hpp"
#include <catch2/catch_all.hpp>
#include <span>

namespace nautilus::testing {

namespace {

using compiler::ir::BasicBlock;
using compiler::ir::BasicBlockArgument;
using compiler::ir::BlockIdentifier;
using compiler::ir::FunctionOperation;
using compiler::ir::IRGraph;
using compiler::ir::Operation;
using compiler::ir::OperationIdentifier;

size_t blockCount(const IRGraph& ir) {
	size_t n = 0;
	for (const auto* fn : ir.getFunctionOperations()) {
		n += fn->getBasicBlocks().size();
	}
	return n;
}

void runPass(IRGraph& ir) {
	engine::Options opts;
	opts.setOption("ir.verifyAfterEachPass", true);
	opts.setOption("ir.failOnVerifyError", true);
	compiler::ir::IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<compiler::ir::BlockMergingPass>());
	mgr.run(ir);
}

void requireVerifierClean(const IRGraph& ir) {
	auto result = compiler::ir::IRVerifier::verify(ir);
	INFO(result.toString());
	REQUIRE(result.ok());
}

} // namespace

TEST_CASE("BlockMerging: a chain of single-predecessor blocks collapses into one block") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<IRGraph>("merge-chain");
	auto& arena = irGraph->getArena();

	auto* exitArg = arena.create<BasicBlockArgument>(OperationIdentifier {40}, Type::i32);
	auto* exitBlock = arena.create<BasicBlock>(arena, BlockIdentifier {3}, std::vector<BasicBlockArgument*> {exitArg});
	exitBlock->addOperation<ir::ReturnOperation>(exitArg);

	auto* midArg2 = arena.create<BasicBlockArgument>(OperationIdentifier {30}, Type::i32);
	auto* mid2 = arena.create<BasicBlock>(arena, BlockIdentifier {2}, std::vector<BasicBlockArgument*> {midArg2});
	auto* add2 = mid2->addOperation<ir::AddOperation>(OperationIdentifier {31}, midArg2, midArg2);
	mid2->addNextBlock(exitBlock, std::vector<Operation*> {add2});

	auto* midArg1 = arena.create<BasicBlockArgument>(OperationIdentifier {20}, Type::i32);
	auto* mid1 = arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {midArg1});
	auto* add1 = mid1->addOperation<ir::AddOperation>(OperationIdentifier {21}, midArg1, midArg1);
	mid1->addNextBlock(mid2, std::vector<Operation*> {add1});

	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* c = entry->addOperation<ir::ConstIntOperation>(OperationIdentifier {10}, 5, Type::i32);
	entry->addNextBlock(mid1, std::vector<Operation*> {c});

	auto* fn = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, mid1, mid2, exitBlock},
	                                           std::vector<Type> {}, std::vector<std::string> {}, Type::i32);
	irGraph->addFunctionOperation(fn);

	runPass(*irGraph);

	REQUIRE(blockCount(*irGraph) == 1);
	REQUIRE(fn->getEntryBlock() == entry);
	// const, add, add, return -- spliced in order, arguments substituted
	// through the whole chain.
	const auto& ops = entry->getOperations();
	REQUIRE(ops.size() == 4);
	REQUIRE(ops[1] == add1);
	REQUIRE(ops[2] == add2);
	REQUIRE(add1->getInputs()[0] == c);
	REQUIRE(add1->getInputs()[1] == c);
	REQUIRE(add2->getInputs()[0] == add1);
	REQUIRE(entry->getTerminatorOp()->getInputs()[0] == add2);
	requireVerifierClean(*irGraph);
}

TEST_CASE("BlockMerging: a block with two predecessor edges is not merged") {
	// Diamond: then/else each end in a branch to merge, but merge has two
	// predecessor edges; entry ends in an if. Nothing is mergeable.
	auto ir = IRGraphFixtures::makeDiamondGraph();
	runPass(*ir);
	REQUIRE(blockCount(*ir) == 4);
	requireVerifierClean(*ir);

	// Same for the duplicate-predecessor shape: one predecessor block, two
	// edges.
	auto sharedTarget = IRGraphFixtures::makeSharedTargetIfGraph();
	runPass(*sharedTarget);
	REQUIRE(blockCount(*sharedTarget) == 2);
	requireVerifierClean(*sharedTarget);
}

TEST_CASE("BlockMerging: a self-loop is not merged") {
	auto irGraph = std::make_shared<IRGraph>("merge-self-loop");
	auto& arena = irGraph->getArena();

	auto* loop = arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {});
	loop->addNextBlock(loop, std::span<Operation* const> {});
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	entry->addNextBlock(loop, std::span<Operation* const> {});

	auto* fn = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, loop}, std::vector<Type> {},
	                                           std::vector<std::string> {}, Type::v);
	irGraph->addFunctionOperation(fn);

	// `loop` branches to itself (self-merge rejected) and has two
	// predecessor edges anyway; `entry`'s target has two predecessor edges.
	runPass(*irGraph);
	REQUIRE(blockCount(*irGraph) == 2);
}

TEST_CASE("BlockMerging: a natural loop keeps its shape (latch -> header not merged)") {
	auto ir = IRGraphFixtures::makeNaturalLoopGraph();
	runPass(*ir);
	// preheader -> header: header has two predecessor edges (preheader,
	// latch). latch -> header: same. header -> latch / exit: header ends in
	// an if. Nothing merges.
	REQUIRE(blockCount(*ir) == 4);
	requireVerifierClean(*ir);
}

TEST_CASE("BlockMerging: argument uses in ops and nested invocations are both substituted") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<IRGraph>("merge-arg-substitution");
	auto& arena = irGraph->getArena();

	auto* exitArg = arena.create<BasicBlockArgument>(OperationIdentifier {30}, Type::i32);
	auto* exitBlock = arena.create<BasicBlock>(arena, BlockIdentifier {2}, std::vector<BasicBlockArgument*> {exitArg});
	exitBlock->addOperation<ir::ReturnOperation>(exitArg);

	// mid's argument feeds a regular op (the add) *and* the nested
	// invocation (it is passed straight on to exit).
	auto* midArg = arena.create<BasicBlockArgument>(OperationIdentifier {20}, Type::i32);
	auto* mid = arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {midArg});
	auto* add = mid->addOperation<ir::AddOperation>(OperationIdentifier {21}, midArg, midArg);
	mid->addNextBlock(exitBlock, std::vector<Operation*> {midArg});

	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* c = entry->addOperation<ir::ConstIntOperation>(OperationIdentifier {10}, 7, Type::i32);
	entry->addNextBlock(mid, std::vector<Operation*> {c});

	auto* fn = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, mid, exitBlock},
	                                           std::vector<Type> {}, std::vector<std::string> {}, Type::i32);
	irGraph->addFunctionOperation(fn);

	runPass(*irGraph);

	REQUIRE(blockCount(*irGraph) == 1);
	// Both use kinds were rewired to the constant: the add's operands and
	// the (former) exit block's incoming value, which after the full
	// collapse is the return's operand.
	REQUIRE(add->getInputs()[0] == c);
	REQUIRE(add->getInputs()[1] == c);
	REQUIRE(entry->getTerminatorOp()->getInputs()[0] == c);
	requireVerifierClean(*irGraph);
}

TEST_CASE("BlockMerging: idempotent on an already-merged graph") {
	auto ir = IRGraphFixtures::makeChainOfEmptyBlocks(3);
	runPass(*ir);
	const auto countAfterFirst = blockCount(*ir);
	REQUIRE(countAfterFirst == 1);

	runPass(*ir);
	REQUIRE(blockCount(*ir) == countAfterFirst);
	requireVerifierClean(*ir);
}

} // namespace nautilus::testing
