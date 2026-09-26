#include "IRGraphFixtures.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/passes/BlockArgumentPruningPass.hpp"
#include "nautilus/compiler/ir/passes/DeadCodeEliminationPass.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include "nautilus/options.hpp"
#include <catch2/catch_all.hpp>
#include <span>

namespace nautilus::testing {

namespace {

using compiler::ir::BasicBlock;
using compiler::ir::BasicBlockArgument;
using compiler::ir::BlockIdentifier;
using compiler::ir::FunctionOperation;
using compiler::ir::getSuccessorInvocations;
using compiler::ir::IRGraph;
using compiler::ir::Operation;
using compiler::ir::OperationIdentifier;

BasicBlock* findBlock(const IRGraph& ir, uint32_t id) {
	for (auto* fn : ir.getFunctionOperations()) {
		for (auto* block : fn->getBasicBlocks()) {
			if (block->getIdentifier() == BlockIdentifier {id}) {
				return block;
			}
		}
	}
	return nullptr;
}

Operation* findOp(const IRGraph& ir, uint32_t id) {
	for (auto* fn : ir.getFunctionOperations()) {
		for (auto* block : fn->getBasicBlocks()) {
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
	}
	return nullptr;
}

void runPass(IRGraph& ir) {
	engine::Options opts;
	opts.setOption("ir.verifyAfterEachPass", true);
	opts.setOption("ir.failOnVerifyError", true);
	compiler::ir::IRPassManager mgr(opts);
	mgr.addPass(std::make_unique<compiler::ir::BlockArgumentPruningPass>());
	mgr.run(ir);
}

void requireVerifierClean(const IRGraph& ir) {
	auto result = compiler::ir::IRVerifier::verify(ir);
	INFO(result.toString());
	REQUIRE(result.ok());
}

} // namespace

TEST_CASE("BlockArgumentPruning: unused argument removed from the block and every invocation") {
	auto ir = IRGraphFixtures::makeDiamondGraph();
	auto* mergeBlock = findBlock(*ir, 3);
	REQUIRE(mergeBlock->getArguments().size() == 2);

	runPass(*ir);

	// The unused second argument is gone; the used-but-disagreeing first
	// argument (then passes 10, else passes 20) must survive.
	REQUIRE(mergeBlock->getArguments().size() == 1);
	REQUIRE(mergeBlock->getArguments()[0]->getIdentifier() == OperationIdentifier {20});
	for (uint32_t predId : {1u, 2u}) {
		auto invocations = getSuccessorInvocations(*findBlock(*ir, predId)->getTerminatorOp());
		REQUIRE(invocations.size() == 1);
		REQUIRE(invocations.front()->getArguments().size() == 1);
	}
	// Pruning only drops the argument slot; the now-unused feeding constants
	// are DCE's job, not this pass's.
	REQUIRE(findOp(*ir, 11) != nullptr);
	REQUIRE(findOp(*ir, 13) != nullptr);
	requireVerifierClean(*ir);
}

TEST_CASE("BlockArgumentPruning: shared-target if -- both invocations of the one predecessor shrink") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<IRGraph>("prune-shared-target");
	auto& arena = irGraph->getArena();

	// merge(used, unused): both invocations come from entry's if.
	auto* usedArg = arena.create<BasicBlockArgument>(OperationIdentifier {10}, Type::i32);
	auto* unusedArg = arena.create<BasicBlockArgument>(OperationIdentifier {11}, Type::i32);
	auto* mergeBlock =
	    arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {usedArg, unusedArg});
	mergeBlock->addOperation<ir::ReturnOperation>(usedArg);

	auto* condArg = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::b);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {condArg});
	auto* c1 = entry->addOperation<ir::ConstIntOperation>(OperationIdentifier {2}, 1, Type::i32);
	auto* c2 = entry->addOperation<ir::ConstIntOperation>(OperationIdentifier {3}, 2, Type::i32);
	auto* ifOp = arena.create<ir::IfOperation>(arena, condArg, 0.5);
	ifOp->setTrueBlockInvocation(mergeBlock);
	ifOp->setFalseBlockInvocation(mergeBlock);
	ifOp->getTrueBlockInvocation().addArgument(arena, c1);
	ifOp->getTrueBlockInvocation().addArgument(arena, c2);
	ifOp->getFalseBlockInvocation().addArgument(arena, c2);
	ifOp->getFalseBlockInvocation().addArgument(arena, c1);
	entry->addOperation(ifOp);

	auto* fn =
	    arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, mergeBlock},
	                                    std::vector<Type> {Type::b}, std::vector<std::string> {"cond"}, Type::i32);
	irGraph->addFunctionOperation(fn);

	runPass(*irGraph);

	REQUIRE(mergeBlock->getArguments().size() == 1);
	REQUIRE(mergeBlock->getArguments()[0] == usedArg);
	auto invocations = getSuccessorInvocations(*entry->getTerminatorOp());
	REQUIRE(invocations.size() == 2);
	for (auto* inv : invocations) {
		REQUIRE(inv->getArguments().size() == 1);
	}
	// The used argument's slot disagrees across the two edges (c1 vs c2)
	// and stays.
	REQUIRE(ifOp->getTrueBlockInvocation().getArguments()[0] == c1);
	REQUIRE(ifOp->getFalseBlockInvocation().getArguments()[0] == c2);
	requireVerifierClean(*irGraph);
}

TEST_CASE("BlockArgumentPruning: loop-invariant carried argument replaced by the preheader value") {
	auto ir = IRGraphFixtures::makeNaturalLoopGraph();
	auto* header = findBlock(*ir, 1);
	auto* limitInit = findOp(*ir, 12); // preheader's constant
	auto* cmp = findOp(*ir, 23);       // header's iv < limit compare
	REQUIRE(header->getArguments().size() == 3);

	runPass(*ir);

	// The latch threads `limit` back to the header unchanged (a
	// self-referential edge, ignored for agreement), the preheader passes
	// `limitInit`, and the preheader dominates the header: the slot is
	// pruned and the loop body reads the outer value directly.
	REQUIRE(header->getArguments().size() == 2);
	REQUIRE(cmp->getInputs()[1] == limitInit);
	// iv and acc are genuinely loop-carried (preheader and latch disagree)
	// and must survive.
	REQUIRE(header->getArguments()[0] == findOp(*ir, 20));
	REQUIRE(header->getArguments()[1] == findOp(*ir, 21));
	// Every invocation targeting the header shrank in sync.
	for (uint32_t predId : {0u, 2u}) {
		auto invocations = getSuccessorInvocations(*findBlock(*ir, predId)->getTerminatorOp());
		for (auto* inv : invocations) {
			if (inv->getBlock() == header) {
				REQUIRE(inv->getArguments().size() == 2);
			}
		}
	}
	requireVerifierClean(*ir);
}

TEST_CASE("BlockArgumentPruning: agreeing predecessors with a dominating value are folded") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<IRGraph>("prune-agreeing-diamond");
	auto& arena = irGraph->getArena();

	auto* mergeArg = arena.create<BasicBlockArgument>(OperationIdentifier {20}, Type::i32);
	auto* mergeBlock =
	    arena.create<BasicBlock>(arena, BlockIdentifier {3}, std::vector<BasicBlockArgument*> {mergeArg});
	mergeBlock->addOperation<ir::ReturnOperation>(mergeArg);

	auto* condArg = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::b);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {condArg});
	auto* shared = entry->addOperation<ir::ConstIntOperation>(OperationIdentifier {2}, 42, Type::i32);

	auto* thenBlock = arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {});
	thenBlock->addNextBlock(mergeBlock, std::vector<Operation*> {shared});
	auto* elseBlock = arena.create<BasicBlock>(arena, BlockIdentifier {2}, std::vector<BasicBlockArgument*> {});
	elseBlock->addNextBlock(mergeBlock, std::vector<Operation*> {shared});

	auto* ifOp = arena.create<ir::IfOperation>(arena, condArg, 0.5);
	ifOp->setTrueBlockInvocation(thenBlock);
	ifOp->setFalseBlockInvocation(elseBlock);
	entry->addOperation(ifOp);

	auto* fn =
	    arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, thenBlock, elseBlock, mergeBlock},
	                                    std::vector<Type> {Type::b}, std::vector<std::string> {"cond"}, Type::i32);
	irGraph->addFunctionOperation(fn);

	runPass(*irGraph);

	// Both arms pass the same entry-defined constant, and entry dominates
	// the merge block: the argument disappears and the return reads the
	// constant directly.
	REQUIRE(mergeBlock->getArguments().empty());
	REQUIRE(mergeBlock->getTerminatorOp()->getInputs()[0] == shared);
	requireVerifierClean(*irGraph);
}

TEST_CASE("BlockArgumentPruning: dominance failure -- unreachable region left untouched") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<IRGraph>("prune-unreachable-cycle");
	auto& arena = irGraph->getArena();

	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	entry->addOperation<ir::ReturnOperation>();

	// Detached cycle: cycleA(a) <-> cycleB. cycleB passes a value it
	// defines itself, so every edge agrees on it -- but nothing dominates
	// an unreachable block, so the slot must survive.
	auto* cycleArg = arena.create<BasicBlockArgument>(OperationIdentifier {10}, Type::i32);
	auto* cycleA = arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {cycleArg});
	auto* cycleB = arena.create<BasicBlock>(arena, BlockIdentifier {2}, std::vector<BasicBlockArgument*> {});
	cycleA->addOperation<ir::AddOperation>(OperationIdentifier {11}, cycleArg, cycleArg);
	cycleA->addNextBlock(cycleB, std::span<Operation* const> {});
	auto* cycleConst = cycleB->addOperation<ir::ConstIntOperation>(OperationIdentifier {12}, 1, Type::i32);
	cycleB->addNextBlock(cycleA, std::vector<Operation*> {cycleConst});

	auto* fn = arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, cycleA, cycleB},
	                                           std::vector<Type> {}, std::vector<std::string> {}, Type::v);
	irGraph->addFunctionOperation(fn);

	runPass(*irGraph);

	REQUIRE(cycleA->getArguments().size() == 1);
	REQUIRE(getSuccessorInvocations(*cycleB->getTerminatorOp()).front()->getArguments().size() == 1);
}

TEST_CASE("BlockArgumentPruning: entry-block arguments are never touched") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<IRGraph>("prune-entry-abi");
	auto& arena = irGraph->getArena();

	// The entry argument is completely unused -- still ABI, still kept.
	auto* inputArg = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {inputArg});
	auto* c = entry->addOperation<ir::ConstIntOperation>(OperationIdentifier {2}, 3, Type::i32);
	entry->addOperation<ir::ReturnOperation>(c);

	auto* fn =
	    arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry}, std::vector<Type> {Type::i32},
	                                    std::vector<std::string> {"x"}, Type::i32);
	irGraph->addFunctionOperation(fn);

	runPass(*irGraph);

	REQUIRE(entry->getArguments().size() == 1);
	requireVerifierClean(*irGraph);
}

TEST_CASE("BlockArgumentPruning: argument used only by dead code dies with it in a group run") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<IRGraph>("prune-with-dce");
	auto& arena = irGraph->getArena();

	auto* mergeArg = arena.create<BasicBlockArgument>(OperationIdentifier {20}, Type::i32);
	auto* mergeBlock =
	    arena.create<BasicBlock>(arena, BlockIdentifier {3}, std::vector<BasicBlockArgument*> {mergeArg});
	// The argument's only use is a cast nobody reads.
	mergeBlock->addOperation<ir::CastOperation>(OperationIdentifier {21}, mergeArg, Type::i64);

	auto* condArg = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::b);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {condArg});
	auto* ret = entry->addOperation<ir::ConstIntOperation>(OperationIdentifier {2}, 0, Type::i32);
	mergeBlock->addOperation<ir::ReturnOperation>(ret);

	auto* thenBlock = arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {});
	auto* thenVal = thenBlock->addOperation<ir::ConstIntOperation>(OperationIdentifier {10}, 1, Type::i32);
	thenBlock->addNextBlock(mergeBlock, std::vector<Operation*> {thenVal});
	auto* elseBlock = arena.create<BasicBlock>(arena, BlockIdentifier {2}, std::vector<BasicBlockArgument*> {});
	auto* elseVal = elseBlock->addOperation<ir::ConstIntOperation>(OperationIdentifier {11}, 2, Type::i32);
	elseBlock->addNextBlock(mergeBlock, std::vector<Operation*> {elseVal});

	auto* ifOp = arena.create<ir::IfOperation>(arena, condArg, 0.5);
	ifOp->setTrueBlockInvocation(thenBlock);
	ifOp->setFalseBlockInvocation(elseBlock);
	entry->addOperation(ifOp);

	auto* fn =
	    arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, thenBlock, elseBlock, mergeBlock},
	                                    std::vector<Type> {Type::b}, std::vector<std::string> {"cond"}, Type::i32);
	irGraph->addFunctionOperation(fn);

	// One fixed-point group: DCE removes the dead cast, which strands the
	// argument; the pruning pass then removes the slot on the next round.
	engine::Options opts;
	opts.setOption("ir.verifyAfterEachPass", true);
	opts.setOption("ir.failOnVerifyError", true);
	compiler::ir::IRPassManager mgr(opts);
	std::vector<std::unique_ptr<compiler::ir::IRPass>> group;
	group.push_back(std::make_unique<compiler::ir::DeadCodeEliminationPass>());
	group.push_back(std::make_unique<compiler::ir::BlockArgumentPruningPass>());
	mgr.addFixedPointGroup(std::move(group), 4);
	mgr.run(*irGraph);

	REQUIRE(findOp(*irGraph, 21) == nullptr); // the cast
	REQUIRE(mergeBlock->getArguments().empty());
	for (uint32_t predId : {1u, 2u}) {
		auto invocations = getSuccessorInvocations(*findBlock(*irGraph, predId)->getTerminatorOp());
		REQUIRE(invocations.front()->getArguments().empty());
	}
	requireVerifierClean(*irGraph);
}

TEST_CASE("BlockArgumentPruning: a value threaded through a chain of blocks folds to its definition") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<IRGraph>("prune-chain");
	auto& arena = irGraph->getArena();

	// entry: v = 42; u = 7; br b1(v, u)
	// b_k(x_k, y_k): br b_{k+1}(x_k, y_k)   for k = 1..chainLength-1
	// b_last(x, y): return x               -- y is threaded but never read
	constexpr size_t chainLength = 6;
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {});
	auto* v = entry->addOperation<ir::ConstIntOperation>(OperationIdentifier {1}, 42, Type::i32);
	auto* u = entry->addOperation<ir::ConstIntOperation>(OperationIdentifier {2}, 7, Type::i32);

	std::vector<BasicBlock*> blocks {entry};
	uint32_t nextId = 10;
	for (size_t k = 1; k <= chainLength; ++k) {
		auto* x = arena.create<BasicBlockArgument>(OperationIdentifier {nextId++}, Type::i32);
		auto* y = arena.create<BasicBlockArgument>(OperationIdentifier {nextId++}, Type::i32);
		auto* block = arena.create<BasicBlock>(arena, BlockIdentifier {static_cast<uint32_t>(k)},
		                                       std::vector<BasicBlockArgument*> {x, y});
		blocks.push_back(block);
	}
	entry->addNextBlock(blocks[1], std::vector<Operation*> {v, u});
	for (size_t k = 1; k < chainLength; ++k) {
		const auto& args = blocks[k]->getArguments();
		blocks[k]->addNextBlock(blocks[k + 1], std::vector<Operation*> {args[0], args[1]});
	}
	auto* last = blocks[chainLength];
	last->addOperation<ir::ReturnOperation>(last->getArguments()[0]);

	auto* fn = arena.create<FunctionOperation>("execute", blocks, std::vector<Type> {}, std::vector<std::string> {},
	                                           Type::i32);
	irGraph->addFunctionOperation(fn);

	runPass(*irGraph);

	// Every hop agreed on the value from the hop before, and entry dominates
	// the whole chain: all slots fold to `v` (the read one) or vanish (the
	// unread one), and the return names the constant directly.
	for (size_t k = 1; k <= chainLength; ++k) {
		REQUIRE(blocks[k]->getArguments().empty());
		auto invocations = getSuccessorInvocations(*blocks[k - 1]->getTerminatorOp());
		REQUIRE(invocations.size() == 1);
		REQUIRE(invocations.front()->getArguments().empty());
	}
	REQUIRE(last->getTerminatorOp()->getInputs()[0] == v);
	requireVerifierClean(*irGraph);
}

TEST_CASE("BlockArgumentPruning: idempotent on an already-pruned graph") {
	auto ir = IRGraphFixtures::makeNaturalLoopGraph();
	auto* header = findBlock(*ir, 1);

	runPass(*ir);
	REQUIRE(header->getArguments().size() == 2);

	runPass(*ir);
	REQUIRE(header->getArguments().size() == 2);
	requireVerifierClean(*ir);
}

// A throwing call inside a loop records the loop-carried copy of a live
// struct's address as its destructor operand (#477). That destructor is the
// argument's only consumer; pruning must still see it as a use and rewire it
// to the dominating value, or the landing pad later destroys an argument that
// no longer exists.
TEST_CASE("BlockArgumentPruning: destructor address operand is rewired, not dropped") {
	namespace ir = compiler::ir;
	auto irGraph = std::make_shared<IRGraph>("prune-destructor-address");
	auto& arena = irGraph->getArena();

	auto* exit = arena.create<BasicBlock>(arena, BlockIdentifier {3}, std::vector<BasicBlockArgument*> {});
	exit->addOperation<ir::ReturnOperation>();

	// header(iv, carriedAddress): if iv < limit ? body : exit
	auto* ivArg = arena.create<BasicBlockArgument>(OperationIdentifier {20}, Type::i32);
	auto* carriedAddress = arena.create<BasicBlockArgument>(OperationIdentifier {21}, Type::ptr);
	auto* header =
	    arena.create<BasicBlock>(arena, BlockIdentifier {1}, std::vector<BasicBlockArgument*> {ivArg, carriedAddress});

	// body: throwing call whose landing pad would destroy *carriedAddress.
	auto* body = arena.create<BasicBlock>(arena, BlockIdentifier {2}, std::vector<BasicBlockArgument*> {});
	ir::CalleeDescriptor callee;
	callee.key = reinterpret_cast<void*>(0x3000);
	callee.demangledName = "mayThrow";
	callee.resultType = Type::v;
	FunctionAttributes attrs;
	std::vector<ir::CallOperation::Destructor> destructors {
	    {carriedAddress, "dtorSymbol", "dtor", reinterpret_cast<void*>(0x4000)}};
	auto* call = body->addOperation<ir::CallOperation>(
	    "mayThrow", "mayThrow", reinterpret_cast<void*>(0x3000), OperationIdentifier {30},
	    std::span<Operation* const> {}, Type::v, attrs, irGraph->internCallee(callee), destructors, true);
	auto* one = body->addOperation<ir::ConstIntOperation>(OperationIdentifier {31}, 1, Type::i32);
	auto* nextIv = body->addOperation<ir::AddOperation>(OperationIdentifier {32}, ivArg, one);
	body->addNextBlock(header, std::vector<Operation*> {nextIv, carriedAddress});

	auto* limit = arena.create<BasicBlockArgument>(OperationIdentifier {1}, Type::i32);
	auto* entry = arena.create<BasicBlock>(arena, BlockIdentifier {0}, std::vector<BasicBlockArgument*> {limit});
	auto* address = entry->addOperation<ir::AllocaOperation>(OperationIdentifier {2}, 0);
	auto* zero = entry->addOperation<ir::ConstIntOperation>(OperationIdentifier {3}, 0, Type::i32);
	entry->addNextBlock(header, std::vector<Operation*> {zero, address});

	auto* cmp =
	    header->addOperation<ir::CompareOperation>(OperationIdentifier {22}, ivArg, limit, ir::CompareOperation::LT);
	auto* headerIf = arena.create<ir::IfOperation>(arena, cmp, 0.9);
	header->addOperation(headerIf);
	headerIf->setTrueBlockInvocation(body);
	headerIf->setFalseBlockInvocation(exit);

	auto* fn =
	    arena.create<FunctionOperation>("execute", std::vector<BasicBlock*> {entry, header, body, exit},
	                                    std::vector<Type> {Type::i32}, std::vector<std::string> {"limit"}, Type::v);
	irGraph->addFunctionOperation(fn);
	compiler::ir::rebuildPredecessorLists(*irGraph);
	requireVerifierClean(*irGraph);

	runPass(*irGraph);

	// The carried address agrees on every incoming edge (entry passes the
	// alloca, the back edge passes the argument itself), so the slot is
	// pruned -- and the destructor must now name the dominating alloca.
	REQUIRE(header->getArguments().size() == 1);
	REQUIRE(call->getDestructors().size() == 1);
	REQUIRE(call->getDestructors()[0].address == address);
	requireVerifierClean(*irGraph);
}

} // namespace nautilus::testing
