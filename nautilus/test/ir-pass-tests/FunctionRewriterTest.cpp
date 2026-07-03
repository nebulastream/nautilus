#include "IRGraphFixtures.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/passes/FunctionRewriter.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <catch2/catch_all.hpp>
#include <unordered_set>

namespace nautilus::testing {

using namespace compiler::ir; // NOLINT

namespace {

BasicBlock* findBlock(FunctionOperation& fn, uint32_t id) {
	for (auto* block : fn.getBasicBlocks()) {
		if (block->getIdentifier() == BlockIdentifier {id}) {
			return block;
		}
	}
	return nullptr;
}

Operation* findOp(FunctionOperation& fn, uint32_t id) {
	for (auto* block : fn.getBasicBlocks()) {
		for (auto* op : block->getOperations()) {
			if (op->getIdentifier() == OperationIdentifier {id}) {
				return op;
			}
		}
		for (auto* arg : block->getArguments()) {
			if (arg->getIdentifier() == OperationIdentifier {id}) {
				return arg;
			}
		}
	}
	return nullptr;
}

/// `StoreOperation` (like the terminator kinds) always carries the sentinel
/// identifier 0 -- its constructor doesn't accept one -- so it must be found
/// by operation kind rather than by id.
Operation* findOpByType(FunctionOperation& fn, Operation::OperationType type) {
	for (auto* block : fn.getBasicBlocks()) {
		for (auto* op : block->getOperations()) {
			if (op->getOperationType() == type) {
				return op;
			}
		}
	}
	return nullptr;
}

void requireVerifierClean(const IRGraph& ir) {
	auto result = IRVerifier::verify(ir);
	INFO(result.toString());
	REQUIRE(result.ok());
}

} // namespace

// ── M1: uniform use model ──────────────────────────────────────────────

TEST_CASE("FunctionRewriter M1: uses uniform over operand and invocation-argument edges") {
	auto ir = IRGraphFixtures::makeNaturalLoopGraph();
	rebuildPredecessorLists(*ir);
	auto* fn = const_cast<FunctionOperation*>(ir->getFunctionOperations().front());
	FunctionRewriter rewriter(*fn, ir->getArena());

	auto* header = findBlock(*fn, 1);
	auto* limitArg = findOp(*fn, 22); // header's loop-invariant argument
	// limitArg is used once by the header's compare (an op operand) and
	// once as the latch->header invocation argument that threads it
	// through unchanged.
	REQUIRE(rewriter.useCount(limitArg) == 2);
	bool sawOpUser = false;
	bool sawInvocationUser = false;
	for (const auto& use : rewriter.usesOf(limitArg)) {
		if (isa<BasicBlockInvocation>(use.user)) {
			sawInvocationUser = true;
		} else {
			sawOpUser = true;
		}
	}
	REQUIRE(sawOpUser);
	REQUIRE(sawInvocationUser);
	REQUIRE(rewriter.definingBlock(limitArg) == header);

	auto* ivArg = findOp(*fn, 20);
	REQUIRE(rewriter.definingBlock(ivArg) == header);
}

TEST_CASE("FunctionRewriter M1: unused value has no uses") {
	auto ir = IRGraphFixtures::makeDeadChainGraph();
	rebuildPredecessorLists(*ir);
	auto* fn = const_cast<FunctionOperation*>(ir->getFunctionOperations().front());
	FunctionRewriter rewriter(*fn, ir->getArena());

	auto* deadAdd = findOp(*fn, 3);
	REQUIRE_FALSE(rewriter.hasUses(deadAdd));
}

// ── M2: replaceAllUses ─────────────────────────────────────────────────

TEST_CASE("FunctionRewriter M2a: replace the value one diamond arm passes to the merge block") {
	auto ir = IRGraphFixtures::makeDiamondGraph();
	rebuildPredecessorLists(*ir);
	auto* fn = const_cast<FunctionOperation*>(ir->getFunctionOperations().front());
	FunctionRewriter rewriter(*fn, ir->getArena());

	auto* thenBlock = findBlock(*fn, 1);
	auto* thenUsed = findOp(*fn, 10);
	auto* replacement =
	    rewriter.createBeforeTerminator<ConstIntOperation>(thenBlock, rewriter.freshId(), 999, Type::i32);

	rewriter.replaceAllUses(thenUsed, replacement);
	REQUIRE(rewriter.useCount(thenUsed) == 0);
	REQUIRE(rewriter.useCount(replacement) == 1);
	REQUIRE_NOTHROW(rewriter.erase(thenUsed));

	requireVerifierClean(*ir);
	// the invocation now actually carries the replacement value.
	auto* terminator = thenBlock->getTerminatorOp();
	auto invocations = getSuccessorInvocations(*terminator);
	REQUIRE(invocations.size() == 1);
	REQUIRE(invocations.front()->getArguments()[0] == replacement);
}

TEST_CASE("FunctionRewriter M2b: mixed uses (op operand, invocation argument, return value) all rewired") {
	auto irGraph = std::make_shared<IRGraph>("mixed-uses-test");
	auto& arena = irGraph->getArena();

	auto* retBlockDirect = arena.create<BasicBlock>(arena, BlockIdentifier {2}, std::vector<BasicBlockArgument*> {});
	auto* argT = arena.create<BasicBlockArgument>(OperationIdentifier {30}, Type::i32);
	auto* retBlockViaArg =
	    arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {argT});
	retBlockViaArg->addOperation<ReturnOperation>(argT);

	auto* condArg = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::b);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {condArg});
	auto* c = entry->addOperation<ConstIntOperation>(OperationIdentifier {2}, 5, Type::i32);
	auto* unrelated = entry->addOperation<ConstIntOperation>(OperationIdentifier {3}, 7, Type::i32);
	entry->addOperation<AddOperation>(OperationIdentifier {4}, c, c);
	auto* ifOp = arena.create<IfOperation>(arena, condArg, 0.5);
	ifOp->setTrueBlockInvocation(retBlockDirect);
	ifOp->setFalseBlockInvocation(retBlockViaArg);
	ifOp->getFalseBlockInvocation().addArgument(arena, c);
	entry->addOperation(ifOp);
	retBlockDirect->addOperation<ReturnOperation>(c);

	auto* fn =
	    arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, retBlockViaArg, retBlockDirect},
	                                    std::vector<Type> {Type::b}, std::vector<std::string> {"cond"}, Type::i32);
	irGraph->addFunctionOperation(fn);
	rebuildPredecessorLists(*irGraph);
	requireVerifierClean(*irGraph);

	FunctionRewriter rewriter(*fn, arena);
	// c: 2 uses from the Add (both operands), 1 from the invocation
	// argument, 1 from retBlockDirect's return -- four edges total.
	REQUIRE(rewriter.useCount(c) == 4);
	REQUIRE(rewriter.useCount(unrelated) == 0);

	// Inserted immediately before `c` (not before the terminator) so it
	// dominates every one of c's existing uses, including the `Add` that
	// appears earlier in the block than the terminator.
	auto* replacement = rewriter.createBefore<ConstIntOperation>(c, rewriter.freshId(), 42, Type::i32);
	rewriter.replaceAllUses(c, replacement);

	REQUIRE(rewriter.useCount(c) == 0);
	REQUIRE(rewriter.useCount(replacement) == 4);
	REQUIRE(rewriter.useCount(unrelated) == 0); // unrelated op's count unaffected

	requireVerifierClean(*irGraph);
}

// ── M3: safe erase ─────────────────────────────────────────────────────

TEST_CASE("FunctionRewriter M3: erase throws on a live op and mutates nothing") {
	auto ir = IRGraphFixtures::makeDiamondGraph();
	rebuildPredecessorLists(*ir);
	auto* fn = const_cast<FunctionOperation*>(ir->getFunctionOperations().front());
	FunctionRewriter rewriter(*fn, ir->getArena());

	auto* thenUsed = findOp(*fn, 10); // still referenced by thenBlock's outgoing invocation
	const auto before = ir->toString();
	REQUIRE_THROWS_AS(rewriter.erase(thenUsed), RuntimeException);
	REQUIRE(ir->toString() == before);
}

TEST_CASE("FunctionRewriter M3: erase on a use-free op detaches it and decrements operand counts") {
	auto ir = IRGraphFixtures::makeDeadChainGraph();
	rebuildPredecessorLists(*ir);
	auto* fn = const_cast<FunctionOperation*>(ir->getFunctionOperations().front());
	auto* entry = findBlock(*fn, 0);
	FunctionRewriter rewriter(*fn, ir->getArena());

	auto* deadAdd = findOp(*fn, 3);
	auto* deadCast = findOp(*fn, 2);
	REQUIRE(rewriter.useCount(deadCast) == 2); // deadAdd reads it twice

	rewriter.erase(deadAdd);
	REQUIRE(rewriter.useCount(deadCast) == 0);
	const auto& ops = entry->getOperations();
	REQUIRE(std::find(ops.begin(), ops.end(), deadAdd) == ops.end());
	requireVerifierClean(*ir);
}

TEST_CASE("FunctionRewriter M3: erasing a terminator throws") {
	auto ir = IRGraphFixtures::makeDiamondGraph();
	rebuildPredecessorLists(*ir);
	auto* fn = const_cast<FunctionOperation*>(ir->getFunctionOperations().front());
	FunctionRewriter rewriter(*fn, ir->getArena());

	auto* entry = findBlock(*fn, 0);
	REQUIRE_THROWS_AS(rewriter.erase(entry->getTerminatorOp()), RuntimeException);
}

// ── M4: eraseIfDead cascade with purity boundary ────────────────────────

TEST_CASE("FunctionRewriter M4: eraseIfDead erases the maximal dead pure chain") {
	auto ir = IRGraphFixtures::makeDeadChainGraph();
	rebuildPredecessorLists(*ir);
	auto* fn = const_cast<FunctionOperation*>(ir->getFunctionOperations().front());
	FunctionRewriter rewriter(*fn, ir->getArena());

	auto* deadAdd = findOp(*fn, 3);
	auto* liveConst = findOp(*fn, 4);
	auto* storeOp = findOpByType(*fn, Operation::OperationType::StoreOp);

	REQUIRE(rewriter.eraseIfDead(deadAdd) == 3);           // add, cast, const
	REQUIRE(rewriter.definingBlock(liveConst) != nullptr); // untouched
	REQUIRE(rewriter.useCount(liveConst) == 2);            // store + return, unchanged
	requireVerifierClean(*ir);

	// A use-free but impure op (Store) must never be swept.
	REQUIRE(rewriter.eraseIfDead(storeOp) == 0);
	REQUIRE(rewriter.definingBlock(storeOp) != nullptr);

	// Idempotence: erasing the same already-erased root again is a no-op.
	REQUIRE(rewriter.eraseIfDead(deadAdd) == 0);
}

TEST_CASE("FunctionRewriter M4: a producer feeding both a dead and a live consumer survives") {
	auto irGraph = std::make_shared<IRGraph>("shared-producer-test");
	auto& arena = irGraph->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* producer = entry->addOperation<ConstIntOperation>(OperationIdentifier {1}, 3, Type::i32);
	entry->addOperation<CastOperation>(OperationIdentifier {2}, producer, Type::i64); // dead
	auto* liveUser = entry->addOperation<CastOperation>(OperationIdentifier {3}, producer, Type::i64);
	entry->addOperation<ReturnOperation>(liveUser);
	auto* fn = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry}, std::vector<Type> {},
	                                           std::vector<std::string> {}, Type::i64);
	irGraph->addFunctionOperation(fn);
	rebuildPredecessorLists(*irGraph);

	FunctionRewriter rewriter(*fn, arena);
	auto* deadUser = findOp(*fn, 2);
	REQUIRE(rewriter.eraseIfDead(deadUser) == 1); // only deadUser itself
	REQUIRE(rewriter.useCount(producer) == 1);    // liveUser still reads it
	REQUIRE(rewriter.definingBlock(producer) != nullptr);
	requireVerifierClean(*irGraph);
}

// ── M5: fresh-identifier authority ──────────────────────────────────────

TEST_CASE("FunctionRewriter M5: freshId never collides, even across erases") {
	auto ir = IRGraphFixtures::makeDiamondGraph();
	rebuildPredecessorLists(*ir);
	auto* fn = const_cast<FunctionOperation*>(ir->getFunctionOperations().front());
	auto* thenBlock = findBlock(*fn, 1);
	FunctionRewriter rewriter(*fn, ir->getArena());

	auto id1 = rewriter.freshId();
	auto id2 = rewriter.freshId();
	REQUIRE_FALSE(id1 == id2);

	auto* op1 = rewriter.createBeforeTerminator<ConstIntOperation>(thenBlock, id1, 1, Type::i32);
	auto* op2 = rewriter.createBeforeTerminator<ConstIntOperation>(thenBlock, id2, 2, Type::i32);
	requireVerifierClean(*ir); // V2 (unique identifiers) stays green

	rewriter.eraseIfDead(op1);
	rewriter.eraseIfDead(op2);
	auto id3 = rewriter.freshId();
	REQUIRE_FALSE(id3 == id1);
	REQUIRE_FALSE(id3 == id2);
}

// ── M6: creation & insertion ─────────────────────────────────────────────

TEST_CASE("FunctionRewriter M6: createBeforeTerminator lands immediately before the terminator") {
	auto ir = IRGraphFixtures::makeDiamondGraph();
	rebuildPredecessorLists(*ir);
	auto* fn = const_cast<FunctionOperation*>(ir->getFunctionOperations().front());
	auto* thenBlock = findBlock(*fn, 1);
	FunctionRewriter rewriter(*fn, ir->getArena());

	auto* thenUsed = findOp(*fn, 10);
	auto* newAdd = rewriter.createBeforeTerminator<AddOperation>(thenBlock, rewriter.freshId(), thenUsed, thenUsed);

	const auto& ops = thenBlock->getOperations();
	REQUIRE(ops[ops.size() - 2] == newAdd);
	REQUIRE(rewriter.useCount(thenUsed) == 3); // 1 (invocation) + 2 (new add's operands)
	requireVerifierClean(*ir);
}

TEST_CASE("FunctionRewriter M6: createBefore lands immediately before the given anchor") {
	auto ir = IRGraphFixtures::makeDeadChainGraph();
	rebuildPredecessorLists(*ir);
	auto* fn = const_cast<FunctionOperation*>(ir->getFunctionOperations().front());
	auto* entry = findBlock(*fn, 0);
	FunctionRewriter rewriter(*fn, ir->getArena());

	auto* liveConst = findOp(*fn, 4);
	auto* fresh = rewriter.createBefore<ConstIntOperation>(liveConst, rewriter.freshId(), 11, Type::i32);

	const auto& ops = entry->getOperations();
	auto it = std::find(ops.begin(), ops.end(), liveConst);
	REQUIRE(it != ops.begin());
	REQUIRE(*std::prev(it) == fresh);
}

// ── M7: atomic block-argument removal ───────────────────────────────────

TEST_CASE("FunctionRewriter M7: removeBlockArgument shrinks the block and every targeting invocation") {
	auto ir = IRGraphFixtures::makeDiamondGraph();
	rebuildPredecessorLists(*ir);
	auto* fn = const_cast<FunctionOperation*>(ir->getFunctionOperations().front());
	auto* mergeBlock = findBlock(*fn, 3);
	FunctionRewriter rewriter(*fn, ir->getArena());

	REQUIRE(mergeBlock->getArguments().size() == 2);
	rewriter.removeBlockArgument(mergeBlock, 1); // the unused second argument
	REQUIRE(mergeBlock->getArguments().size() == 1);

	for (auto* pred : {findBlock(*fn, 1), findBlock(*fn, 2)}) {
		auto invocations = getSuccessorInvocations(*pred->getTerminatorOp());
		REQUIRE(invocations.size() == 1);
		REQUIRE(invocations.front()->getArguments().size() == 1);
	}
	requireVerifierClean(*ir);
}

TEST_CASE("FunctionRewriter M7: shared-target-if -- both invocations of the single predecessor shrink") {
	auto ir = IRGraphFixtures::makeSharedTargetIfGraph();
	rebuildPredecessorLists(*ir);
	auto* fn = const_cast<FunctionOperation*>(ir->getFunctionOperations().front());
	auto* entry = fn->getEntryBlock();
	BasicBlock* mergeBlock = nullptr;
	for (auto* b : fn->getBasicBlocks()) {
		if (b != entry) {
			mergeBlock = b;
		}
	}
	FunctionRewriter rewriter(*fn, ir->getArena());

	// Add a second, unused argument fed by distinct per-edge values (both
	// invocations come from the same predecessor, `entry`).
	int callbackCount = 0;
	rewriter.addBlockArgument(mergeBlock, Type::i32, [&](BasicBlockInvocation&) {
		callbackCount++;
		return rewriter.createBeforeTerminator<ConstIntOperation>(entry, rewriter.freshId(), callbackCount, Type::i32);
	});
	REQUIRE(callbackCount == 2);
	REQUIRE(mergeBlock->getArguments().size() == 2);
	auto invocationsAfterAdd = getSuccessorInvocations(*entry->getTerminatorOp());
	REQUIRE(invocationsAfterAdd.size() == 2);
	for (auto* inv : invocationsAfterAdd) {
		REQUIRE(inv->getArguments().size() == 2);
	}
	requireVerifierClean(*ir);

	rewriter.removeBlockArgument(mergeBlock, 1);
	REQUIRE(mergeBlock->getArguments().size() == 1);
	auto invocationsAfterRemove = getSuccessorInvocations(*entry->getTerminatorOp());
	REQUIRE(invocationsAfterRemove.size() == 2);
	for (auto* inv : invocationsAfterRemove) {
		REQUIRE(inv->getArguments().size() == 1);
	}
	requireVerifierClean(*ir);
}

TEST_CASE("FunctionRewriter M7: removing a live argument throws and mutates nothing") {
	auto ir = IRGraphFixtures::makeDiamondGraph();
	rebuildPredecessorLists(*ir);
	auto* fn = const_cast<FunctionOperation*>(ir->getFunctionOperations().front());
	auto* mergeBlock = findBlock(*fn, 3);
	FunctionRewriter rewriter(*fn, ir->getArena());

	const auto before = ir->toString();
	REQUIRE_THROWS_AS(rewriter.removeBlockArgument(mergeBlock, 0), RuntimeException); // used by the return
	REQUIRE(ir->toString() == before);
}

// ── M8: atomic block-argument addition ──────────────────────────────────

TEST_CASE("FunctionRewriter M8: addBlockArgument on the natural loop -- distinct preheader/latch values") {
	auto ir = IRGraphFixtures::makeNaturalLoopGraph();
	rebuildPredecessorLists(*ir);
	auto* fn = const_cast<FunctionOperation*>(ir->getFunctionOperations().front());
	auto* header = findBlock(*fn, 1);
	auto* preheader = findBlock(*fn, 0);
	auto* latch = findBlock(*fn, 2);
	FunctionRewriter rewriter(*fn, ir->getArena());

	int callbackCount = 0;
	auto* ptrArg = rewriter.addBlockArgument(header, Type::ptr, [&](BasicBlockInvocation& inv) -> Operation* {
		callbackCount++;
		// Distinguish the edge by which block already-supplied invocation
		// argument (e.g. the loop-invariant `limitArg` value) is defined in.
		return rewriter.createBeforeTerminator<ConstIntOperation>(rewriter.definingBlock(inv.getArguments().front()),
		                                                          rewriter.freshId(), callbackCount, Type::ptr);
	});
	REQUIRE(callbackCount == 2); // exactly one call per invocation (preheader, latch)
	REQUIRE(header->getArguments().back() == ptrArg);

	auto preheaderInv = getSuccessorInvocations(*preheader->getTerminatorOp()).front();
	auto latchInv = getSuccessorInvocations(*latch->getTerminatorOp()).front();
	REQUIRE(preheaderInv->getArguments().size() == 4);
	REQUIRE(latchInv->getArguments().size() == 4);
	REQUIRE(preheaderInv->getArguments().back() != latchInv->getArguments().back());
	requireVerifierClean(*ir);
}

// ── M9: CFG primitives preserve the predecessor multiset ────────────────

TEST_CASE("FunctionRewriter M9: setInvocationTarget keeps predecessor multiset consistent") {
	auto ir = IRGraphFixtures::makeDiamondGraph();
	rebuildPredecessorLists(*ir);
	auto* fn = const_cast<FunctionOperation*>(ir->getFunctionOperations().front());
	auto* entry = findBlock(*fn, 0);
	auto* thenBlock = findBlock(*fn, 1);
	auto* elseBlock = findBlock(*fn, 2);
	FunctionRewriter rewriter(*fn, ir->getArena());

	auto* ifOp = as<IfOperation>(entry->getTerminatorOp());
	rewriter.setInvocationTarget(ifOp->getTrueBlockInvocation(), elseBlock);

	REQUIRE(thenBlock->getPredecessors().empty());
	int elseCount = 0;
	for (auto* p : elseBlock->getPredecessors()) {
		if (p == entry) {
			elseCount++;
		}
	}
	REQUIRE(elseCount == 2);
	requireVerifierClean(*ir);
}

TEST_CASE("FunctionRewriter M9: replaceTerminator drops the removed successor's predecessor entry") {
	auto ir = IRGraphFixtures::makeDiamondGraph();
	rebuildPredecessorLists(*ir);
	auto* fn = const_cast<FunctionOperation*>(ir->getFunctionOperations().front());
	auto* entry = findBlock(*fn, 0);
	auto* thenBlock = findBlock(*fn, 1);
	auto* elseBlock = findBlock(*fn, 2);
	FunctionRewriter rewriter(*fn, ir->getArena());

	auto& arena = ir->getArena();
	auto* newBranch = arena.create<BranchOperation>();
	newBranch->getNextBlockInvocation().setBlock(thenBlock);
	rewriter.replaceTerminator(entry, newBranch);

	REQUIRE(elseBlock->getPredecessors().empty());
	int thenCount = 0;
	for (auto* p : thenBlock->getPredecessors()) {
		if (p == entry) {
			thenCount++;
		}
	}
	REQUIRE(thenCount == 1);
	requireVerifierClean(*ir);
}

TEST_CASE("FunctionRewriter M9: detached-cycle -- eraseBlock succeeds on pred-free block, throws otherwise") {
	auto irGraph = std::make_shared<IRGraph>("detached-cycle-erase-test");
	auto& arena = irGraph->getArena();

	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	entry->addOperation<ReturnOperation>();

	auto* isolated = arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {});
	isolated->addOperation<ReturnOperation>();

	auto* cycleA = arena.create<BasicBlock>(arena, BlockIdentifier {2}, std::vector<BasicBlockArgument*> {});
	auto* cycleB = arena.create<BasicBlock>(arena, BlockIdentifier {3}, std::vector<BasicBlockArgument*> {});
	cycleA->addNextBlock(cycleB, std::span<Operation* const> {});
	cycleB->addNextBlock(cycleA, std::span<Operation* const> {});

	auto* fn = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, isolated, cycleA, cycleB},
	                                           std::vector<Type> {}, std::vector<std::string> {}, Type::v);
	irGraph->addFunctionOperation(fn);
	rebuildPredecessorLists(*irGraph);

	FunctionRewriter rewriter(*fn, arena);
	REQUIRE_THROWS_AS(rewriter.eraseBlock(entry), RuntimeException);
	REQUIRE_THROWS_AS(rewriter.eraseBlock(cycleA), RuntimeException); // has predecessor cycleB

	rewriter.eraseBlock(isolated);
	bool stillPresent = false;
	for (auto* b : fn->getBasicBlocks()) {
		if (b == isolated) {
			stillPresent = true;
		}
	}
	REQUIRE_FALSE(stillPresent);
}

// ── M10: no full-function rescans -- large-graph smoke test ────────────

TEST_CASE("FunctionRewriter M10: large straight-line graph smoke test") {
	constexpr size_t kPairs = 2500;
	auto irGraph = std::make_shared<IRGraph>("large-graph-smoke-test");
	auto& arena = irGraph->getArena();
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});

	uint32_t nextId = 1;
	std::vector<Operation*> deadConsts;
	deadConsts.reserve(kPairs);
	std::vector<Operation*> deadCasts;
	deadCasts.reserve(kPairs);
	for (size_t i = 0; i < kPairs; ++i) {
		auto* c =
		    entry->addOperation<ConstIntOperation>(OperationIdentifier {nextId++}, static_cast<int64_t>(i), Type::i32);
		auto* cast = entry->addOperation<CastOperation>(OperationIdentifier {nextId++}, c, Type::i64);
		deadConsts.push_back(c);
		deadCasts.push_back(cast);
	}
	auto* liveConst = entry->addOperation<ConstIntOperation>(OperationIdentifier {nextId++}, 1, Type::i32);
	entry->addOperation<ReturnOperation>(liveConst);

	auto* fn = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry}, std::vector<Type> {},
	                                           std::vector<std::string> {}, Type::i32);
	irGraph->addFunctionOperation(fn);
	rebuildPredecessorLists(*irGraph);

	FunctionRewriter rewriter(*fn, arena);
	for (size_t i = 0; i < 1000; ++i) {
		// Degenerate self-replace: exercises the replaceAllUses path at scale
		// without disturbing the graph's shape.
		rewriter.replaceAllUses(deadConsts[i], deadConsts[i]);
		REQUIRE(rewriter.eraseIfDead(deadCasts[i]) == 2); // cast + its const
	}

	requireVerifierClean(*irGraph);
}

} // namespace nautilus::testing
