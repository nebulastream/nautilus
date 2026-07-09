#include "IRGraphFixtures.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::testing {

namespace {

bool messageContains(const compiler::ir::VerificationResult& result, const std::string& needle) {
	for (const auto& err : result.errors) {
		if (err.message.find(needle) != std::string::npos) {
			return true;
		}
	}
	return false;
}

} // namespace

TEST_CASE("IRVerifier: well-formed single-block graph is ok") {
	auto ir = IRGraphFixtures::makeSingleBlockGraph();
	compiler::ir::rebuildPredecessorLists(*ir);
	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE(result.ok());
}

TEST_CASE("IRVerifier: block without terminator is flagged") {
	auto ir = IRGraphFixtures::makeMalformedGraph_NoTerminator();
	compiler::ir::rebuildPredecessorLists(*ir);
	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE_FALSE(result.ok());
	// One of the errors should mention that the block's last op is not a terminator.
	bool found = false;
	for (const auto& err : result.errors) {
		if (err.message.find("terminator") != std::string::npos) {
			found = true;
			break;
		}
	}
	REQUIRE(found);
}

TEST_CASE("IRVerifier: duplicate block identifiers are flagged") {
	auto ir = IRGraphFixtures::makeMalformedGraph_DuplicateBlockId();
	compiler::ir::rebuildPredecessorLists(*ir);
	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE_FALSE(result.ok());
	bool found = false;
	for (const auto& err : result.errors) {
		if (err.message.find("duplicate") != std::string::npos) {
			found = true;
			break;
		}
	}
	REQUIRE(found);
}

TEST_CASE("IRVerifier: chain-of-empty fixture is well-formed") {
	auto ir = IRGraphFixtures::makeChainOfEmptyBlocks(4);
	compiler::ir::rebuildPredecessorLists(*ir);
	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE(result.ok());
}

TEST_CASE("IRVerifier: input pointing at an operation outside the function is flagged (#327)") {
	auto ir = std::make_shared<compiler::ir::IRGraph>("stale-input-test");
	auto& arena = ir->getArena();
	auto* entry = arena.create<compiler::ir::BasicBlock>(arena, compiler::ir::BlockIdentifier {0},
	                                                     std::vector<compiler::ir::BasicBlockArgument*> {});
	// A constant that lives in the arena but in no block's operation list —
	// the shape a pass leaves behind when it removes an operation without
	// rewiring its consumers (a stale input edge).
	auto* detached =
	    arena.create<compiler::ir::ConstIntOperation>(arena, compiler::ir::OperationIdentifier {1}, 7, Type::i32);
	entry->addOperation<compiler::ir::ReturnOperation>(detached);

	auto* fn =
	    arena.create<compiler::ir::FunctionOperation>("execute", std::vector<compiler::ir::BasicBlock*> {entry},
	                                                  std::vector<Type> {}, std::vector<std::string> {}, Type::i32);
	ir->addFunctionOperation(fn);

	compiler::ir::rebuildPredecessorLists(*ir);
	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE_FALSE(result.ok());
	bool found = false;
	for (const auto& err : result.errors) {
		if (err.message.find("not defined in function") != std::string::npos) {
			found = true;
			break;
		}
	}
	REQUIRE(found);
}

// ── V1: invocation arity == target block arity ──────────────────────────

TEST_CASE("IRVerifier V1: invocation with one argument too few is flagged") {
	auto ir = IRGraphFixtures::makeDiamondGraph();
	compiler::ir::rebuildPredecessorLists(*ir);
	auto& fn = *const_cast<compiler::ir::FunctionOperation*>(ir->getFunctionOperations().front());
	auto* thenBlock = fn.getBasicBlocks()[1];
	auto* terminator = thenBlock->getTerminatorOp();
	auto* branch = compiler::ir::as<compiler::ir::BranchOperation>(terminator);
	auto& inv = branch->getNextBlockInvocation();
	auto args = std::vector<compiler::ir::Operation*>(inv.getArguments().begin(), inv.getArguments().end());
	inv.clearArguments();
	// Re-add all but one argument -- one short of the merge block's arity.
	for (size_t i = 0; i + 1 < args.size(); ++i) {
		inv.addArgument(ir->getArena(), args[i]);
	}

	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE_FALSE(result.ok());
	REQUIRE(messageContains(result, "argument(s)"));
}

TEST_CASE("IRVerifier V1: every fixture satisfies invocation arity") {
	for (auto& ir : {IRGraphFixtures::makeDiamondGraph(), IRGraphFixtures::makeSharedTargetIfGraph(),
	                 IRGraphFixtures::makeNaturalLoopGraph(), IRGraphFixtures::makeDeadChainGraph()}) {
		compiler::ir::rebuildPredecessorLists(*ir);
		REQUIRE(compiler::ir::IRVerifier::verify(*ir).ok());
	}
}

// ── V2: operation/block-argument identifiers unique per function ───────

TEST_CASE("IRVerifier V2: two operations sharing an identifier are flagged (#321)") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<ir::IRGraph>("v2-duplicate-op-id");
	auto& arena = irGraph->getArena();
	auto* entry = arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {0}, std::vector<ir::BasicBlockArgument*> {});
	auto* c1 = entry->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {5}, 1, Type::i32);
	// Same identifier (5) reused for a second, distinct constant.
	auto* c2 = entry->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {5}, 2, Type::i32);
	auto* add = entry->addOperation<ir::AddOperation>(ir::OperationIdentifier {6}, c1, c2);
	entry->addOperation<ir::ReturnOperation>(add);
	auto* fn = arena.create<ir::FunctionOperation>("execute", std::vector<ir::BasicBlock*> {entry},
	                                               std::vector<Type> {}, std::vector<std::string> {}, Type::i32);
	irGraph->addFunctionOperation(fn);
	compiler::ir::rebuildPredecessorLists(*irGraph);

	auto result = compiler::ir::IRVerifier::verify(*irGraph);
	REQUIRE_FALSE(result.ok());
	REQUIRE(messageContains(result, "duplicate operation identifier"));
}

TEST_CASE("IRVerifier V2: an operation and a block argument sharing an identifier are flagged") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<ir::IRGraph>("v2-duplicate-arg-op-id");
	auto& arena = irGraph->getArena();
	auto* arg = arena.create<ir::BasicBlockArgument>(ir::OperationIdentifier {7}, Type::i32);
	auto* entry =
	    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {0}, std::vector<ir::BasicBlockArgument*> {arg});
	// Same identifier (7) as the block argument.
	auto* c1 = entry->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {7}, 1, Type::i32);
	auto* add = entry->addOperation<ir::AddOperation>(ir::OperationIdentifier {8}, arg, c1);
	entry->addOperation<ir::ReturnOperation>(add);
	auto* fn =
	    arena.create<ir::FunctionOperation>("execute", std::vector<ir::BasicBlock*> {entry},
	                                        std::vector<Type> {Type::i32}, std::vector<std::string> {"a"}, Type::i32);
	irGraph->addFunctionOperation(fn);
	compiler::ir::rebuildPredecessorLists(*irGraph);

	auto result = compiler::ir::IRVerifier::verify(*irGraph);
	REQUIRE_FALSE(result.ok());
	REQUIRE(messageContains(result, "duplicate operation identifier"));
}

// ── V3: def dominates use (reachable blocks) ─────────────────────────────

TEST_CASE("IRVerifier V3: use before def in the same block is flagged") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<ir::IRGraph>("v3-use-before-def");
	auto& arena = irGraph->getArena();
	auto* entry = arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {0}, std::vector<ir::BasicBlockArgument*> {});
	// `add` is appended before `c1`, but reads `c1` -- a use-before-def.
	auto* c1 = arena.create<ir::ConstIntOperation>(arena, ir::OperationIdentifier {2}, 5, Type::i32);
	auto* add = entry->addOperation<ir::AddOperation>(ir::OperationIdentifier {1}, c1, c1);
	entry->addOperation(c1);
	entry->addOperation<ir::ReturnOperation>(add);
	auto* fn = arena.create<ir::FunctionOperation>("execute", std::vector<ir::BasicBlock*> {entry},
	                                               std::vector<Type> {}, std::vector<std::string> {}, Type::i32);
	irGraph->addFunctionOperation(fn);
	compiler::ir::rebuildPredecessorLists(*irGraph);

	auto result = compiler::ir::IRVerifier::verify(*irGraph);
	REQUIRE_FALSE(result.ok());
	REQUIRE(messageContains(result, "does not dominate its use"));
}

TEST_CASE("IRVerifier V3: merge block using a value defined in only one diamond arm is flagged") {
	auto ir = IRGraphFixtures::makeDiamondGraph();
	compiler::ir::rebuildPredecessorLists(*ir);
	auto& fn = *const_cast<compiler::ir::FunctionOperation*>(ir->getFunctionOperations().front());
	auto* thenBlock = fn.getBasicBlocks()[1];
	auto* mergeBlock = fn.getBasicBlocks()[3];
	auto* thenConst = thenBlock->getOperations().front();
	// Illegally reference the `then`-arm-only constant directly from the
	// merge block's return, bypassing the block-argument mechanism.
	auto* mergeReturn = compiler::ir::as<compiler::ir::ReturnOperation>(mergeBlock->getTerminatorOp());
	mergeReturn->setReturnValue(thenConst);

	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE_FALSE(result.ok());
	REQUIRE(messageContains(result, "does not dominate its use"));
}

TEST_CASE("IRVerifier V3: value defined in a loop body used in the preheader is flagged") {
	auto ir = IRGraphFixtures::makeNaturalLoopGraph();
	compiler::ir::rebuildPredecessorLists(*ir);
	auto& fn = *const_cast<compiler::ir::FunctionOperation*>(ir->getFunctionOperations().front());
	auto* preheader = fn.getBasicBlocks()[0];
	auto* latch = fn.getBasicBlocks()[2];
	// `nextIv`, the second op in latch, is defined only inside the loop body.
	auto* nextIv = latch->getOperations()[1];
	auto* preheaderBranch = compiler::ir::as<compiler::ir::BranchOperation>(preheader->getTerminatorOp());
	preheaderBranch->getNextBlockInvocation().replaceArgument(
	    preheaderBranch->getNextBlockInvocation().getArguments()[0], nextIv);

	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE_FALSE(result.ok());
	REQUIRE(messageContains(result, "does not dominate its use"));
}

TEST_CASE("IRVerifier V3: loop-carried arguments and detached cycles are accepted") {
	auto loopIr = IRGraphFixtures::makeNaturalLoopGraph();
	compiler::ir::rebuildPredecessorLists(*loopIr);
	REQUIRE(compiler::ir::IRVerifier::verify(*loopIr).ok());

	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<ir::IRGraph>("v3-detached-cycle");
	auto& arena = irGraph->getArena();
	auto* entry = arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {0}, std::vector<ir::BasicBlockArgument*> {});
	entry->addOperation<ir::ReturnOperation>();
	auto* cycleA =
	    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {1}, std::vector<ir::BasicBlockArgument*> {});
	auto* cycleB =
	    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {2}, std::vector<ir::BasicBlockArgument*> {});
	// A use-before-def inside the detached cycle, which V3 must not flag
	// since neither block is reachable from entry.
	auto* danglingConst = arena.create<ir::ConstIntOperation>(arena, ir::OperationIdentifier {1}, 1, Type::i32);
	cycleA->addOperation<ir::AddOperation>(ir::OperationIdentifier {2}, danglingConst, danglingConst);
	cycleA->addNextBlock(cycleB, std::span<ir::Operation* const> {});
	cycleB->addOperation(danglingConst);
	cycleB->addNextBlock(cycleA, std::span<ir::Operation* const> {});
	auto* fn = arena.create<ir::FunctionOperation>("execute", std::vector<ir::BasicBlock*> {entry, cycleA, cycleB},
	                                               std::vector<Type> {}, std::vector<std::string> {}, Type::v);
	irGraph->addFunctionOperation(fn);
	compiler::ir::rebuildPredecessorLists(*irGraph);
	REQUIRE(compiler::ir::IRVerifier::verify(*irGraph).ok());
}

// ── V4: predecessor multiset == incoming edges ──────────────────────────

TEST_CASE("IRVerifier V4: removing one duplicate predecessor entry is flagged") {
	auto ir = IRGraphFixtures::makeSharedTargetIfGraph();
	compiler::ir::rebuildPredecessorLists(*ir);
	auto& fn = *const_cast<compiler::ir::FunctionOperation*>(ir->getFunctionOperations().front());
	auto* mergeBlock = fn.getBasicBlocks()[1];
	auto* entry = fn.getEntryBlock();
	mergeBlock->removePredecessor(entry); // removes one of the two entries.

	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE_FALSE(result.ok());
	REQUIRE(messageContains(result, "listed"));
}

TEST_CASE("IRVerifier V4: a stale extra predecessor entry is flagged") {
	auto ir = IRGraphFixtures::makeDiamondGraph();
	compiler::ir::rebuildPredecessorLists(*ir);
	auto& fn = *const_cast<compiler::ir::FunctionOperation*>(ir->getFunctionOperations().front());
	auto* mergeBlock = fn.getBasicBlocks()[3];
	auto* thenBlock = fn.getBasicBlocks()[1];
	mergeBlock->addPredecessor(thenBlock); // thenBlock now listed twice, but only targets it once.

	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE_FALSE(result.ok());
	REQUIRE(messageContains(result, "listed"));
}

TEST_CASE("IRVerifier V4: shared-target-if fixture lists its predecessor exactly twice") {
	auto ir = IRGraphFixtures::makeSharedTargetIfGraph();
	compiler::ir::rebuildPredecessorLists(*ir);
	REQUIRE(compiler::ir::IRVerifier::verify(*ir).ok());
}

// ── V5: entry block has no predecessors; args match the signature ──────

TEST_CASE("IRVerifier V5: a branch retargeted to the entry block is flagged") {
	auto ir = IRGraphFixtures::makeDiamondGraph();
	compiler::ir::rebuildPredecessorLists(*ir);
	auto& fn = *const_cast<compiler::ir::FunctionOperation*>(ir->getFunctionOperations().front());
	auto* thenBlock = fn.getBasicBlocks()[1];
	auto* entry = fn.getEntryBlock();
	thenBlock->replaceSuccessor(fn.getBasicBlocks()[3], entry);

	auto result = compiler::ir::IRVerifier::verify(*ir);
	REQUIRE_FALSE(result.ok());
	REQUIRE(messageContains(result, "entry block has"));
}

TEST_CASE("IRVerifier V5: entry argument count mismatch is flagged") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<ir::IRGraph>("v5-arity-mismatch");
	auto& arena = irGraph->getArena();
	auto* entry = arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {0}, std::vector<ir::BasicBlockArgument*> {});
	entry->addOperation<ir::ReturnOperation>();
	// The signature declares one i32 input, but the entry block has none.
	auto* fn =
	    arena.create<ir::FunctionOperation>("execute", std::vector<ir::BasicBlock*> {entry},
	                                        std::vector<Type> {Type::i32}, std::vector<std::string> {"a"}, Type::v);
	irGraph->addFunctionOperation(fn);
	compiler::ir::rebuildPredecessorLists(*irGraph);

	auto result = compiler::ir::IRVerifier::verify(*irGraph);
	REQUIRE_FALSE(result.ok());
	REQUIRE(messageContains(result, "argument(s), but the function signature declares"));
}

TEST_CASE("IRVerifier V5: entry argument stamp mismatch is flagged") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<ir::IRGraph>("v5-stamp-mismatch");
	auto& arena = irGraph->getArena();
	auto* arg = arena.create<ir::BasicBlockArgument>(ir::OperationIdentifier {1}, Type::i64);
	auto* entry =
	    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {0}, std::vector<ir::BasicBlockArgument*> {arg});
	entry->addOperation<ir::ReturnOperation>();
	// Signature declares slot 0 as i32; the entry block's argument is i64.
	auto* fn =
	    arena.create<ir::FunctionOperation>("execute", std::vector<ir::BasicBlock*> {entry},
	                                        std::vector<Type> {Type::i32}, std::vector<std::string> {"a"}, Type::v);
	irGraph->addFunctionOperation(fn);
	compiler::ir::rebuildPredecessorLists(*irGraph);

	auto result = compiler::ir::IRVerifier::verify(*irGraph);
	REQUIRE_FALSE(result.ok());
	REQUIRE(messageContains(result, "has stamp"));
}

// ── V6: edge stamp agreement ─────────────────────────────────────────────

TEST_CASE("IRVerifier V6: an invocation passing i64 into a ptr block argument is flagged") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<ir::IRGraph>("v6-stamp-mismatch");
	auto& arena = irGraph->getArena();
	auto* targetArg = arena.create<ir::BasicBlockArgument>(ir::OperationIdentifier {10}, Type::ptr);
	auto* targetBlock =
	    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {1}, std::vector<ir::BasicBlockArgument*> {targetArg});
	targetBlock->addOperation<ir::ReturnOperation>();

	auto* entry = arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {0}, std::vector<ir::BasicBlockArgument*> {});
	auto* i64Const = entry->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {1}, 42, Type::i64);
	entry->addNextBlock(targetBlock, std::vector<ir::Operation*> {i64Const});

	auto* fn = arena.create<ir::FunctionOperation>("execute", std::vector<ir::BasicBlock*> {entry, targetBlock},
	                                               std::vector<Type> {}, std::vector<std::string> {}, Type::v);
	irGraph->addFunctionOperation(fn);
	compiler::ir::rebuildPredecessorLists(*irGraph);

	auto result = compiler::ir::IRVerifier::verify(*irGraph);
	REQUIRE_FALSE(result.ok());
	REQUIRE(messageContains(result, "target's parameter stamp is"));
}

TEST_CASE("IRVerifier V6: every fixture agrees on edge stamps") {
	for (auto& ir : {IRGraphFixtures::makeDiamondGraph(), IRGraphFixtures::makeSharedTargetIfGraph(),
	                 IRGraphFixtures::makeNaturalLoopGraph(), IRGraphFixtures::makeDeadChainGraph()}) {
		compiler::ir::rebuildPredecessorLists(*ir);
		REQUIRE(compiler::ir::IRVerifier::verify(*ir).ok());
	}
}

} // namespace nautilus::testing
