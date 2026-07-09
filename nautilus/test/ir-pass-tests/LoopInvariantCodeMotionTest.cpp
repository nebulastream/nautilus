#include "IRGraphFixtures.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/compiler/ir/passes/LoopInvariantCodeMotionPass.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include "nautilus/options.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::testing {

namespace {

using namespace compiler::ir;

size_t countInBlock(BasicBlock* block, Operation::OperationType type) {
	size_t n = 0;
	for (const auto* op : block->getOperations()) {
		if (op->getOperationType() == type) {
			++n;
		}
	}
	return n;
}

size_t countTotal(const IRGraph& ir, Operation::OperationType type) {
	size_t n = 0;
	for (const auto* fn : ir.getFunctionOperations()) {
		for (const auto* block : fn->getBasicBlocks()) {
			for (const auto* op : block->getOperations()) {
				if (op->getOperationType() == type) {
					++n;
				}
			}
		}
	}
	return n;
}

BasicBlock* blockWithId(IRGraph& ir, uint32_t id) {
	for (auto* block : ir.getFunctionOperations().front()->getBasicBlocks()) {
		if (block->getIdentifier().getId() == id) {
			return block;
		}
	}
	return nullptr;
}

void runLICM(IRGraph& ir) {
	engine::Options opts;
	opts.setOption("ir.verifyAfterEachPass", true);
	opts.setOption("ir.failOnVerifyError", true);
	IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<LoopInvariantCodeMotionPass>());
	mgr.run(ir);
}

} // namespace

TEST_CASE("LICM: a loop-invariant constant is hoisted out of the latch into the preheader") {
	auto ir = IRGraphFixtures::makeNaturalLoopGraph();
	auto* preheader = blockWithId(*ir, 0);
	auto* latch = blockWithId(*ir, 2);
	REQUIRE(countInBlock(latch, Operation::OperationType::ConstIntOp) == 1);     // `one`
	REQUIRE(countInBlock(preheader, Operation::OperationType::ConstIntOp) == 3); // iv/acc/limit inits

	runLICM(*ir);

	// `one` moves to the preheader; the loop-carried adds stay in the latch.
	CHECK(countInBlock(latch, Operation::OperationType::ConstIntOp) == 0);
	CHECK(countInBlock(preheader, Operation::OperationType::ConstIntOp) == 4);
	CHECK(countInBlock(latch, Operation::OperationType::AddOp) == 2);
	REQUIRE(IRVerifier::verify(*ir).ok());
}

// Builds: preheader/entry(n) -> header(iv, acc) <-> latch, header -> exit(acc).
// The latch computes an invariant chain `base = n*two`, `scaled = base + one`
// (only `n` and constants feed it) alongside the genuinely loop-carried
// `nextIv`/`nextAcc`.
std::shared_ptr<IRGraph> makeChainLoop() {
	auto ir = std::make_shared<IRGraph>("licm-chain");
	auto& arena = ir->getArena();

	auto* n = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* iv = arena.create<BasicBlockArgument>(OperationIdentifier {20}, Type::i32);
	auto* acc = arena.create<BasicBlockArgument>(OperationIdentifier {21}, Type::i32);
	auto* res = arena.create<BasicBlockArgument>(OperationIdentifier {40}, Type::i32);

	auto* exit = arena.create<BasicBlock>(arena, BlockIdentifier {3}, std::vector<BasicBlockArgument*> {res});
	exit->addOperation<ReturnOperation>(res);

	auto* header = arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {iv, acc});
	auto* cmp = header->addOperation<CompareOperation>(OperationIdentifier {23}, iv, n, CompareOperation::LT);
	auto* headerIf = arena.create<IfOperation>(arena, cmp, 0.9);
	header->addOperation(headerIf);

	auto* latch = arena.create<BasicBlock>(arena, BlockIdentifier {2}, std::vector<BasicBlockArgument*> {});
	auto* one = latch->addOperation<ConstIntOperation>(OperationIdentifier {30}, int64_t {1}, Type::i32);
	auto* two = latch->addOperation<ConstIntOperation>(OperationIdentifier {31}, int64_t {2}, Type::i32);
	auto* base = latch->addOperation<MulOperation>(OperationIdentifier {32}, n, two);
	auto* scaled = latch->addOperation<AddOperation>(OperationIdentifier {33}, base, one);
	auto* nextIv = latch->addOperation<AddOperation>(OperationIdentifier {34}, iv, one);
	auto* nextAcc = latch->addOperation<AddOperation>(OperationIdentifier {35}, acc, scaled);
	latch->addNextBlock(header, std::vector<Operation*> {nextIv, nextAcc});

	headerIf->setTrueBlockInvocation(latch);
	headerIf->setFalseBlockInvocation(exit);
	headerIf->getFalseBlockInvocation().addArgument(arena, acc);

	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {n});
	auto* iv0 = entry->addOperation<ConstIntOperation>(OperationIdentifier {10}, int64_t {0}, Type::i32);
	auto* acc0 = entry->addOperation<ConstIntOperation>(OperationIdentifier {11}, int64_t {0}, Type::i32);
	entry->addNextBlock(header, std::vector<Operation*> {iv0, acc0});

	auto* fn =
	    arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, header, latch, exit},
	                                    std::vector<Type> {Type::i32}, std::vector<std::string> {"n"}, Type::i32);
	ir->addFunctionOperation(fn);
	return ir;
}

TEST_CASE("LICM: a chain of loop-invariant operations is hoisted transitively") {
	auto ir = makeChainLoop();
	auto* entry = blockWithId(*ir, 0);
	auto* latch = blockWithId(*ir, 2);
	const auto totalMul = countTotal(*ir, Operation::OperationType::MulOp);
	const auto totalAdd = countTotal(*ir, Operation::OperationType::AddOp);

	runLICM(*ir);

	// `base` (mul) and `scaled` (add, depends on the just-hoisted `base`) both
	// move to the preheader; `nextIv`/`nextAcc` stay in the latch.
	CHECK(countInBlock(entry, Operation::OperationType::MulOp) == 1);
	CHECK(countInBlock(latch, Operation::OperationType::MulOp) == 0);
	CHECK(countInBlock(entry, Operation::OperationType::AddOp) == 1);
	CHECK(countInBlock(latch, Operation::OperationType::AddOp) == 2);
	CHECK(countInBlock(latch, Operation::OperationType::ConstIntOp) == 0);
	// Nothing is created or destroyed -- operations are moved.
	CHECK(countTotal(*ir, Operation::OperationType::MulOp) == totalMul);
	CHECK(countTotal(*ir, Operation::OperationType::AddOp) == totalAdd);
	REQUIRE(IRVerifier::verify(*ir).ok());

	LoopInvariantCodeMotionPass pass;
	CHECK_FALSE(pass.apply(*ir)); // idempotent
}

TEST_CASE("LICM: division is hoisted only with a non-zero constant divisor") {
	auto ir = std::make_shared<IRGraph>("licm-div");
	auto& arena = ir->getArena();

	auto* n = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* m = arena.create<BasicBlockArgument>(OperationIdentifier {2}, Type::i32);
	auto* iv = arena.create<BasicBlockArgument>(OperationIdentifier {20}, Type::i32);
	auto* res = arena.create<BasicBlockArgument>(OperationIdentifier {40}, Type::i32);

	auto* exit = arena.create<BasicBlock>(arena, BlockIdentifier {3}, std::vector<BasicBlockArgument*> {res});
	exit->addOperation<ReturnOperation>(res);

	auto* header = arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {iv});
	auto* cmp = header->addOperation<CompareOperation>(OperationIdentifier {23}, iv, n, CompareOperation::LT);
	auto* headerIf = arena.create<IfOperation>(arena, cmp, 0.9);
	header->addOperation(headerIf);

	auto* latch = arena.create<BasicBlock>(arena, BlockIdentifier {2}, std::vector<BasicBlockArgument*> {});
	auto* c4 = latch->addOperation<ConstIntOperation>(OperationIdentifier {30}, int64_t {4}, Type::i32);
	auto* dvar = latch->addOperation<DivOperation>(OperationIdentifier {31}, n, m);    // divisor not constant
	auto* dconst = latch->addOperation<DivOperation>(OperationIdentifier {32}, n, c4); // divisor const != 0
	auto* one = latch->addOperation<ConstIntOperation>(OperationIdentifier {33}, int64_t {1}, Type::i32);
	auto* nextIv = latch->addOperation<AddOperation>(OperationIdentifier {34}, iv, one);
	latch->addNextBlock(header, std::vector<Operation*> {nextIv});
	(void) dvar;
	(void) dconst;

	headerIf->setTrueBlockInvocation(latch);
	headerIf->setFalseBlockInvocation(exit);
	headerIf->getFalseBlockInvocation().addArgument(arena, iv);

	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {n, m});
	auto* iv0 = entry->addOperation<ConstIntOperation>(OperationIdentifier {10}, int64_t {0}, Type::i32);
	entry->addNextBlock(header, std::vector<Operation*> {iv0});

	auto* fn = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, header, latch, exit},
	                                           std::vector<Type> {Type::i32, Type::i32},
	                                           std::vector<std::string> {"n", "m"}, Type::i32);
	ir->addFunctionOperation(fn);

	runLICM(*ir);

	// The constant-divisor division is hoisted; the variable-divisor one stays
	// pinned in the loop (it could still trap).
	CHECK(countInBlock(blockWithId(*ir, 0), Operation::OperationType::DivOp) == 1);
	CHECK(countInBlock(blockWithId(*ir, 2), Operation::OperationType::DivOp) == 1);
	REQUIRE(IRVerifier::verify(*ir).ok());
}

TEST_CASE("LICM: loads are never hoisted; loop-varying operations stay put") {
	auto ir = std::make_shared<IRGraph>("licm-load");
	auto& arena = ir->getArena();

	auto* p = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::ptr);
	auto* nb = arena.create<BasicBlockArgument>(OperationIdentifier {2}, Type::i32);
	auto* iv = arena.create<BasicBlockArgument>(OperationIdentifier {20}, Type::i32);
	auto* res = arena.create<BasicBlockArgument>(OperationIdentifier {40}, Type::i32);

	auto* exit = arena.create<BasicBlock>(arena, BlockIdentifier {3}, std::vector<BasicBlockArgument*> {res});
	exit->addOperation<ReturnOperation>(res);

	auto* header = arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {iv});
	auto* cmp = header->addOperation<CompareOperation>(OperationIdentifier {23}, iv, nb, CompareOperation::LT);
	auto* headerIf = arena.create<IfOperation>(arena, cmp, 0.9);
	header->addOperation(headerIf);

	auto* latch = arena.create<BasicBlock>(arena, BlockIdentifier {2}, std::vector<BasicBlockArgument*> {});
	auto* two = latch->addOperation<ConstIntOperation>(OperationIdentifier {29}, int64_t {2}, Type::i32);
	latch->addOperation<LoadOperation>(OperationIdentifier {30}, p, Type::i32);      // load of invariant address
	auto* xv = latch->addOperation<MulOperation>(OperationIdentifier {31}, iv, two); // loop-varying (uses iv)
	auto* one = latch->addOperation<ConstIntOperation>(OperationIdentifier {32}, int64_t {1}, Type::i32);
	auto* nextIv = latch->addOperation<AddOperation>(OperationIdentifier {33}, iv, one);
	latch->addNextBlock(header, std::vector<Operation*> {nextIv});
	(void) xv;

	headerIf->setTrueBlockInvocation(latch);
	headerIf->setFalseBlockInvocation(exit);
	headerIf->getFalseBlockInvocation().addArgument(arena, iv);

	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {p, nb});
	auto* iv0 = entry->addOperation<ConstIntOperation>(OperationIdentifier {10}, int64_t {0}, Type::i32);
	entry->addNextBlock(header, std::vector<Operation*> {iv0});

	auto* fn = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, header, latch, exit},
	                                           std::vector<Type> {Type::ptr, Type::i32},
	                                           std::vector<std::string> {"p", "nb"}, Type::i32);
	ir->addFunctionOperation(fn);

	runLICM(*ir);

	// Load stays in the loop; the iv-dependent multiply stays; only the
	// invariant constants leave.
	CHECK(countInBlock(blockWithId(*ir, 0), Operation::OperationType::LoadOp) == 0);
	CHECK(countInBlock(blockWithId(*ir, 2), Operation::OperationType::LoadOp) == 1);
	CHECK(countInBlock(blockWithId(*ir, 2), Operation::OperationType::MulOp) == 1); // xv stays
	CHECK(countInBlock(blockWithId(*ir, 2), Operation::OperationType::ConstIntOp) == 0);
	REQUIRE(IRVerifier::verify(*ir).ok());
}

// Two properly nested natural loops; `mul(N, N)` in the inner latch is
// invariant to both and must migrate all the way out to the function entry.
std::shared_ptr<IRGraph> makeNestedLoop() {
	auto ir = std::make_shared<IRGraph>("licm-nested");
	auto& arena = ir->getArena();

	auto* N = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* oiv = arena.create<BasicBlockArgument>(OperationIdentifier {20}, Type::i32);
	auto* iiv = arena.create<BasicBlockArgument>(OperationIdentifier {30}, Type::i32);
	auto* olv = arena.create<BasicBlockArgument>(OperationIdentifier {50}, Type::i32);
	auto* res = arena.create<BasicBlockArgument>(OperationIdentifier {60}, Type::i32);

	auto* exit = arena.create<BasicBlock>(arena, BlockIdentifier {6}, std::vector<BasicBlockArgument*> {res});
	exit->addOperation<ReturnOperation>(res);

	auto* outerHeader = arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {oiv});
	auto* innerPreheader = arena.create<BasicBlock>(arena, BlockIdentifier {2}, std::vector<BasicBlockArgument*> {});
	auto* innerHeader = arena.create<BasicBlock>(arena, BlockIdentifier {3}, std::vector<BasicBlockArgument*> {iiv});
	auto* innerLatch = arena.create<BasicBlock>(arena, BlockIdentifier {4}, std::vector<BasicBlockArgument*> {});
	auto* outerLatch = arena.create<BasicBlock>(arena, BlockIdentifier {5}, std::vector<BasicBlockArgument*> {olv});

	auto* ocmp = outerHeader->addOperation<CompareOperation>(OperationIdentifier {21}, oiv, N, CompareOperation::LT);
	auto* oif = arena.create<IfOperation>(arena, ocmp, 0.9);
	outerHeader->addOperation(oif);
	oif->setTrueBlockInvocation(innerPreheader);
	oif->setFalseBlockInvocation(exit);
	oif->getFalseBlockInvocation().addArgument(arena, oiv);

	auto* iiv0 = innerPreheader->addOperation<ConstIntOperation>(OperationIdentifier {22}, int64_t {0}, Type::i32);
	innerPreheader->addNextBlock(innerHeader, std::vector<Operation*> {iiv0});

	auto* icmp = innerHeader->addOperation<CompareOperation>(OperationIdentifier {31}, iiv, N, CompareOperation::LT);
	auto* iif = arena.create<IfOperation>(arena, icmp, 0.9);
	innerHeader->addOperation(iif);
	iif->setTrueBlockInvocation(innerLatch);
	iif->setFalseBlockInvocation(outerLatch);
	iif->getFalseBlockInvocation().addArgument(arena, oiv); // thread outer iv to the outer latch

	auto* ione = innerLatch->addOperation<ConstIntOperation>(OperationIdentifier {40}, int64_t {1}, Type::i32);
	innerLatch->addOperation<MulOperation>(OperationIdentifier {41}, N, N); // invariant to both loops
	auto* niiv = innerLatch->addOperation<AddOperation>(OperationIdentifier {42}, iiv, ione);
	innerLatch->addNextBlock(innerHeader, std::vector<Operation*> {niiv});

	auto* oone = outerLatch->addOperation<ConstIntOperation>(OperationIdentifier {51}, int64_t {1}, Type::i32);
	auto* noiv = outerLatch->addOperation<AddOperation>(OperationIdentifier {52}, olv, oone);
	outerLatch->addNextBlock(outerHeader, std::vector<Operation*> {noiv});

	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {N});
	auto* oiv0 = entry->addOperation<ConstIntOperation>(OperationIdentifier {10}, int64_t {0}, Type::i32);
	entry->addNextBlock(outerHeader, std::vector<Operation*> {oiv0});

	auto* fn = arena.create<FunctionOperation>(
	    "execute",
	    std::vector<BasicBlock*> {entry, outerHeader, innerPreheader, innerHeader, innerLatch, outerLatch, exit},
	    std::vector<Type> {Type::i32}, std::vector<std::string> {"N"}, Type::i32);
	ir->addFunctionOperation(fn);
	return ir;
}

TEST_CASE("LICM: a loop whose body contains a nested loop is conservatively skipped") {
	// v1 does not hoist out of a loop that contains a nested loop: doing so
	// would give the hoisted value a live range spanning the inner loop's back
	// edge, which the direct-lowering interpreter backends miscompile. The
	// `N*N` multiply must therefore stay put in the inner latch (block 4).
	auto ir = makeNestedLoop();
	REQUIRE(countInBlock(blockWithId(*ir, 4), Operation::OperationType::MulOp) == 1);

	compiler::ir::rebuildPredecessorLists(*ir);
	LoopInvariantCodeMotionPass pass;
	CHECK_FALSE(pass.apply(*ir)); // nothing is hoisted -> no change

	CHECK(countInBlock(blockWithId(*ir, 4), Operation::OperationType::MulOp) == 1);
	CHECK(countInBlock(blockWithId(*ir, 0), Operation::OperationType::MulOp) == 0);
	REQUIRE(IRVerifier::verify(*ir).ok());
}

} // namespace nautilus::testing
