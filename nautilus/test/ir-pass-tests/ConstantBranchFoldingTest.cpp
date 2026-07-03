#include "IRGraphFixtures.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/passes/ConstantBranchFoldingPass.hpp"
#include "nautilus/compiler/ir/passes/ConstantFoldingAndCopyPropagationPass.hpp"
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

bool hasBlock(const IRGraph& ir, BlockIdentifier id) {
	for (const auto* fn : ir.getFunctionOperations()) {
		for (const auto* block : fn->getBasicBlocks()) {
			if (block->getIdentifier() == id) {
				return true;
			}
		}
	}
	return false;
}

void runPass(IRGraph& ir) {
	engine::Options opts;
	opts.setOption("ir.verifyAfterEachPass", true);
	opts.setOption("ir.failOnVerifyError", true);
	compiler::ir::IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<compiler::ir::ConstantBranchFoldingPass>());
	mgr.run(ir);
}

/// entry --if(ConstBooleanOperation(condValue))--> trueBlock(arg=trueVal) /
/// falseBlock(arg=falseVal), each target block returning its argument.
/// BlockIdentifier 0=entry, 1=trueBlock, 2=falseBlock.
std::shared_ptr<IRGraph> makeConstIfGraph(bool condValue, int64_t trueVal, int64_t falseVal) {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<ir::IRGraph>("const-if-test");
	auto& arena = irGraph->getArena();

	auto* trueArg = arena.create<ir::BasicBlockArgument>(ir::OperationIdentifier {10}, Type::i32);
	auto* trueBlock =
	    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {1}, std::vector<ir::BasicBlockArgument*> {trueArg});
	trueBlock->addOperation<ir::ReturnOperation>(trueArg);

	auto* falseArg = arena.create<ir::BasicBlockArgument>(ir::OperationIdentifier {11}, Type::i32);
	auto* falseBlock =
	    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {2}, std::vector<ir::BasicBlockArgument*> {falseArg});
	falseBlock->addOperation<ir::ReturnOperation>(falseArg);

	auto* entry = arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {0}, std::vector<ir::BasicBlockArgument*> {});
	auto* cond = entry->addOperation<ir::ConstBooleanOperation>(ir::OperationIdentifier {1}, condValue);
	auto* trueConst = entry->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {2}, trueVal, Type::i32);
	auto* falseConst = entry->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {3}, falseVal, Type::i32);
	auto* ifOp = arena.create<ir::IfOperation>(arena, cond, 0.5);
	ifOp->setTrueBlockInvocation(trueBlock);
	ifOp->setFalseBlockInvocation(falseBlock);
	ifOp->getTrueBlockInvocation().addArgument(arena, trueConst);
	ifOp->getFalseBlockInvocation().addArgument(arena, falseConst);
	entry->addOperation(ifOp);

	auto* fn =
	    arena.create<ir::FunctionOperation>("execute", std::vector<ir::BasicBlock*> {entry, trueBlock, falseBlock},
	                                        std::vector<Type> {}, std::vector<std::string> {}, Type::i32);
	irGraph->addFunctionOperation(fn);
	return irGraph;
}

} // namespace

TEST_CASE("ConstantBranchFolding: if(true) folds to a branch, taken argument preserved") {
	auto ir = makeConstIfGraph(true, 7, 9);
	REQUIRE(blockCount(*ir) == 3);

	runPass(*ir);

	// The false arm is now unreachable and swept.
	REQUIRE(blockCount(*ir) == 2);
	REQUIRE(hasBlock(*ir, BlockIdentifier {1}));
	REQUIRE_FALSE(hasBlock(*ir, BlockIdentifier {2}));

	auto* fn = ir->getFunctionOperations().front();
	auto* entry = fn->getEntryBlock();
	auto* branch = compiler::ir::dyn_cast<compiler::ir::BranchOperation>(entry->getTerminatorOp());
	REQUIRE(branch != nullptr);
	const auto& inv = branch->getNextBlockInvocation();
	REQUIRE(inv.getBlock()->getIdentifier() == BlockIdentifier {1});
	REQUIRE(inv.getArguments().size() == 1);
	auto* arg = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(inv.getArguments()[0]);
	REQUIRE(arg != nullptr);
	REQUIRE(arg->getValue() == 7);

	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE(result.ok());
}

TEST_CASE("ConstantBranchFolding: if(false) folds to a branch, taken argument preserved") {
	auto ir = makeConstIfGraph(false, 7, 9);
	REQUIRE(blockCount(*ir) == 3);

	runPass(*ir);

	REQUIRE(blockCount(*ir) == 2);
	REQUIRE_FALSE(hasBlock(*ir, BlockIdentifier {1}));
	REQUIRE(hasBlock(*ir, BlockIdentifier {2}));

	auto* fn = ir->getFunctionOperations().front();
	auto* entry = fn->getEntryBlock();
	auto* branch = compiler::ir::dyn_cast<compiler::ir::BranchOperation>(entry->getTerminatorOp());
	REQUIRE(branch != nullptr);
	const auto& inv = branch->getNextBlockInvocation();
	REQUIRE(inv.getBlock()->getIdentifier() == BlockIdentifier {2});
	auto* arg = compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(inv.getArguments()[0]);
	REQUIRE(arg != nullptr);
	REQUIRE(arg->getValue() == 9);

	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE(result.ok());
}

TEST_CASE("ConstantBranchFolding: both arms targeting the same block with identical args fold, "
          "regardless of condition constness") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<ir::IRGraph>("same-target-same-args-test");
	auto& arena = irGraph->getArena();

	auto* mergeArg = arena.create<ir::BasicBlockArgument>(ir::OperationIdentifier {10}, Type::i32);
	auto* mergeBlock =
	    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {1}, std::vector<ir::BasicBlockArgument*> {mergeArg});
	mergeBlock->addOperation<ir::ReturnOperation>(mergeArg);

	auto* condArg = arena.create<ir::BasicBlockArgument>(ir::OperationIdentifier {1}, Type::b);
	auto* entry =
	    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {0}, std::vector<ir::BasicBlockArgument*> {condArg});
	auto* sharedConst =
	    entry->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {2}, int64_t {42}, Type::i32);
	auto* ifOp = arena.create<ir::IfOperation>(arena, condArg, 0.5);
	ifOp->setTrueBlockInvocation(mergeBlock);
	ifOp->setFalseBlockInvocation(mergeBlock);
	ifOp->getTrueBlockInvocation().addArgument(arena, sharedConst);
	ifOp->getFalseBlockInvocation().addArgument(arena, sharedConst);
	entry->addOperation(ifOp);

	auto* fn =
	    arena.create<ir::FunctionOperation>("execute", std::vector<ir::BasicBlock*> {entry, mergeBlock},
	                                        std::vector<Type> {Type::b}, std::vector<std::string> {"cond"}, Type::i32);
	irGraph->addFunctionOperation(fn);

	runPass(*irGraph);

	REQUIRE(blockCount(*irGraph) == 2); // merge stays reachable, just via one edge now.
	auto* branch = compiler::ir::dyn_cast<compiler::ir::BranchOperation>(entry->getTerminatorOp());
	REQUIRE(branch != nullptr);
	REQUIRE(branch->getNextBlockInvocation().getBlock() == mergeBlock);
	// merge previously had 2 predecessor entries (one per If arm); now exactly 1.
	REQUIRE(mergeBlock->getPredecessors().size() == 1);

	auto result = compiler::ir::IRVerifier::verify(*irGraph);
	REQUIRE(result.ok());
}

TEST_CASE("ConstantBranchFolding: entry block is never removed even when trivially foldable") {
	auto ir = IRGraphFixtures::makeSingleBlockGraph();
	runPass(*ir);
	REQUIRE(blockCount(*ir) == 1);
	auto* fn = ir->getFunctionOperations().front();
	REQUIRE(fn->getEntryBlock() != nullptr);
}

TEST_CASE("ConstantBranchFolding: a block already unreachable from entry is swept") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<ir::IRGraph>("unreachable-block-test");
	auto& arena = irGraph->getArena();

	// `orphan` has no predecessors at all -- reachable from nothing.
	auto* orphan =
	    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {1}, std::vector<ir::BasicBlockArgument*> {});
	orphan->addOperation<ir::ReturnOperation>();

	auto* entry = arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {0}, std::vector<ir::BasicBlockArgument*> {});
	entry->addOperation<ir::ReturnOperation>();

	auto* fn = arena.create<ir::FunctionOperation>("execute", std::vector<ir::BasicBlock*> {entry, orphan},
	                                               std::vector<Type> {}, std::vector<std::string> {}, Type::v);
	irGraph->addFunctionOperation(fn);

	REQUIRE(blockCount(*irGraph) == 2);
	runPass(*irGraph);
	REQUIRE(blockCount(*irGraph) == 1);
	REQUIRE_FALSE(hasBlock(*irGraph, BlockIdentifier {1}));
}

TEST_CASE("ConstantBranchFolding: an unreachable cycle is swept") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<ir::IRGraph>("unreachable-cycle-test");
	auto& arena = irGraph->getArena();

	// cycleA <-> cycleB, neither reachable from entry: individually, neither
	// ever reaches zero predecessors on its own.
	auto* cycleA =
	    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {1}, std::vector<ir::BasicBlockArgument*> {});
	auto* cycleB =
	    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {2}, std::vector<ir::BasicBlockArgument*> {});
	cycleA->addNextBlock(cycleB, std::span<ir::Operation* const> {});
	cycleB->addNextBlock(cycleA, std::span<ir::Operation* const> {});

	auto* entry = arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {0}, std::vector<ir::BasicBlockArgument*> {});
	entry->addOperation<ir::ReturnOperation>();

	auto* fn = arena.create<ir::FunctionOperation>("execute", std::vector<ir::BasicBlock*> {entry, cycleA, cycleB},
	                                               std::vector<Type> {}, std::vector<std::string> {}, Type::v);
	irGraph->addFunctionOperation(fn);

	REQUIRE(blockCount(*irGraph) == 3);
	runPass(*irGraph);
	REQUIRE(blockCount(*irGraph) == 1);
	REQUIRE_FALSE(hasBlock(*irGraph, BlockIdentifier {1}));
	REQUIRE_FALSE(hasBlock(*irGraph, BlockIdentifier {2}));

	auto result = compiler::ir::IRVerifier::verify(*irGraph);
	REQUIRE(result.ok());
}

TEST_CASE("ConstantBranchFolding: idempotent on an already-folded graph") {
	auto ir = makeConstIfGraph(true, 1, 2);
	runPass(*ir);
	const auto countAfterFirst = blockCount(*ir);

	runPass(*ir);
	REQUIRE(blockCount(*ir) == countAfterFirst);

	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE(result.ok());
}

TEST_CASE("ConstantBranchFolding: interaction with constant folding -- compare to bool to straight line") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<ir::IRGraph>("interaction-test");
	auto& arena = irGraph->getArena();

	auto* trueArg = arena.create<ir::BasicBlockArgument>(ir::OperationIdentifier {10}, Type::i32);
	auto* trueBlock =
	    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {1}, std::vector<ir::BasicBlockArgument*> {trueArg});
	trueBlock->addOperation<ir::ReturnOperation>(trueArg);

	auto* falseArg = arena.create<ir::BasicBlockArgument>(ir::OperationIdentifier {11}, Type::i32);
	auto* falseBlock =
	    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {2}, std::vector<ir::BasicBlockArgument*> {falseArg});
	falseBlock->addOperation<ir::ReturnOperation>(falseArg);

	auto* entry = arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {0}, std::vector<ir::BasicBlockArgument*> {});
	// 5 < 3 -- constant-folds to `false`.
	auto* c5 = entry->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {1}, int64_t {5}, Type::i32);
	auto* c3 = entry->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {2}, int64_t {3}, Type::i32);
	auto* cmp =
	    entry->addOperation<ir::CompareOperation>(ir::OperationIdentifier {3}, c5, c3, ir::CompareOperation::LT);
	auto* trueConst = entry->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {4}, int64_t {100}, Type::i32);
	auto* falseConst =
	    entry->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {5}, int64_t {200}, Type::i32);
	auto* ifOp = arena.create<ir::IfOperation>(arena, cmp, 0.5);
	ifOp->setTrueBlockInvocation(trueBlock);
	ifOp->setFalseBlockInvocation(falseBlock);
	ifOp->getTrueBlockInvocation().addArgument(arena, trueConst);
	ifOp->getFalseBlockInvocation().addArgument(arena, falseConst);
	entry->addOperation(ifOp);

	auto* fn =
	    arena.create<ir::FunctionOperation>("execute", std::vector<ir::BasicBlock*> {entry, trueBlock, falseBlock},
	                                        std::vector<Type> {}, std::vector<std::string> {}, Type::i32);
	irGraph->addFunctionOperation(fn);

	engine::Options opts;
	opts.setOption("ir.verifyAfterEachPass", true);
	opts.setOption("ir.failOnVerifyError", true);
	compiler::ir::IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<compiler::ir::ConstantFoldingAndCopyPropagationPass>());
	mgr.addPass(std::make_unique<compiler::ir::ConstantBranchFoldingPass>());
	mgr.run(*irGraph);

	// 5 < 3 is false: the false arm (falseBlock, value 200) is taken and the
	// true arm is swept as unreachable, in one `run()` call.
	REQUIRE(blockCount(*irGraph) == 2);
	REQUIRE_FALSE(hasBlock(*irGraph, BlockIdentifier {1}));
	REQUIRE(hasBlock(*irGraph, BlockIdentifier {2}));

	auto* branch = compiler::ir::dyn_cast<compiler::ir::BranchOperation>(entry->getTerminatorOp());
	REQUIRE(branch != nullptr);
	auto* arg =
	    compiler::ir::dyn_cast<compiler::ir::ConstIntOperation>(branch->getNextBlockInvocation().getArguments()[0]);
	REQUIRE(arg != nullptr);
	REQUIRE(arg->getValue() == 200);

	auto result = compiler::ir::IRVerifier::verify(*irGraph);
	REQUIRE(result.ok());
}

} // namespace nautilus::testing
