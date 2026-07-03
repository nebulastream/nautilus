#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nautilus::testing {

/**
 * @brief Hand-built IR graphs used by the pass and verifier tests.
 *
 * Every builder returns a `shared_ptr<IRGraph>` that owns the arena the
 * graph was allocated in. The graph contains a single function named
 * "execute" with the block layout described per-builder. Predecessor
 * lists are left empty on return; tests that need them should call
 * `compiler::ir::rebuildPredecessorLists` (which is what the pass
 * manager does at the start of `run()`).
 */
class IRGraphFixtures {
public:
	/// Returns a graph with a single block containing only a void
	/// `return`. Entry block, no successors, no predecessors.
	static std::shared_ptr<compiler::ir::IRGraph> makeSingleBlockGraph() {
		auto ir = std::make_shared<compiler::ir::IRGraph>("single-block-test");
		auto& arena = ir->getArena();
		auto* entry = arena.create<compiler::ir::BasicBlock>(arena, compiler::ir::BlockIdentifier {0},
		                                                     std::vector<compiler::ir::BasicBlockArgument*> {});
		entry->addOperation<compiler::ir::ReturnOperation>();

		auto* fn =
		    arena.create<compiler::ir::FunctionOperation>("execute", std::vector<compiler::ir::BasicBlock*> {entry},
		                                                  std::vector<Type> {}, std::vector<std::string> {}, Type::v);
		ir->addFunctionOperation(fn);
		return ir;
	}

	/// Entry -> empty branch-only block -> return block. The intermediate
	/// block carries no block arguments and can therefore be eliminated.
	static std::shared_ptr<compiler::ir::IRGraph> makeGraphWithEmptyBlock() {
		auto ir = std::make_shared<compiler::ir::IRGraph>("empty-block-test");
		auto& arena = ir->getArena();

		auto* retBlock = arena.create<compiler::ir::BasicBlock>(arena, compiler::ir::BlockIdentifier {2},
		                                                        std::vector<compiler::ir::BasicBlockArgument*> {});
		retBlock->addOperation<compiler::ir::ReturnOperation>();

		auto* emptyBlock = arena.create<compiler::ir::BasicBlock>(arena, compiler::ir::BlockIdentifier {1},
		                                                          std::vector<compiler::ir::BasicBlockArgument*> {});
		emptyBlock->addNextBlock(retBlock, std::span<compiler::ir::Operation* const> {});

		auto* entry = arena.create<compiler::ir::BasicBlock>(arena, compiler::ir::BlockIdentifier {0},
		                                                     std::vector<compiler::ir::BasicBlockArgument*> {});
		entry->addNextBlock(emptyBlock, std::span<compiler::ir::Operation* const> {});

		auto* fn = arena.create<compiler::ir::FunctionOperation>(
		    "execute", std::vector<compiler::ir::BasicBlock*> {entry, emptyBlock, retBlock}, std::vector<Type> {},
		    std::vector<std::string> {}, Type::v);
		ir->addFunctionOperation(fn);
		return ir;
	}

	/// Entry -> N empty branch-only blocks -> return block. The chain
	/// collapses to a single edge from entry straight to the return
	/// block.
	static std::shared_ptr<compiler::ir::IRGraph> makeChainOfEmptyBlocks(size_t n) {
		auto ir = std::make_shared<compiler::ir::IRGraph>("chain-empty-test");
		auto& arena = ir->getArena();

		auto* retBlock = arena.create<compiler::ir::BasicBlock>(arena, compiler::ir::BlockIdentifier {uint32_t(n + 1)},
		                                                        std::vector<compiler::ir::BasicBlockArgument*> {});
		retBlock->addOperation<compiler::ir::ReturnOperation>();

		std::vector<compiler::ir::BasicBlock*> chain;
		chain.reserve(n);
		compiler::ir::BasicBlock* prev = retBlock;
		for (size_t i = n; i >= 1; --i) {
			auto* b = arena.create<compiler::ir::BasicBlock>(arena, compiler::ir::BlockIdentifier {uint32_t(i)},
			                                                 std::vector<compiler::ir::BasicBlockArgument*> {});
			b->addNextBlock(prev, std::span<compiler::ir::Operation* const> {});
			chain.push_back(b);
			prev = b;
		}

		auto* entry = arena.create<compiler::ir::BasicBlock>(arena, compiler::ir::BlockIdentifier {0},
		                                                     std::vector<compiler::ir::BasicBlockArgument*> {});
		entry->addNextBlock(prev, std::span<compiler::ir::Operation* const> {});

		std::vector<compiler::ir::BasicBlock*> allBlocks {entry};
		for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
			allBlocks.push_back(*it);
		}
		allBlocks.push_back(retBlock);

		auto* fn = arena.create<compiler::ir::FunctionOperation>("execute", std::move(allBlocks), std::vector<Type> {},
		                                                         std::vector<std::string> {}, Type::v);
		ir->addFunctionOperation(fn);
		return ir;
	}

	/// Malformed: a block whose last operation is a non-terminator.
	static std::shared_ptr<compiler::ir::IRGraph> makeMalformedGraph_NoTerminator() {
		auto ir = std::make_shared<compiler::ir::IRGraph>("malformed-no-terminator");
		auto& arena = ir->getArena();
		auto* entry = arena.create<compiler::ir::BasicBlock>(arena, compiler::ir::BlockIdentifier {0},
		                                                     std::vector<compiler::ir::BasicBlockArgument*> {});
		// Append a constant but no terminator — the verifier should flag
		// this as the block's last op not being a terminator.
		entry->addOperation<compiler::ir::ConstIntOperation>(compiler::ir::OperationIdentifier {1}, 42, Type::i32);

		auto* fn =
		    arena.create<compiler::ir::FunctionOperation>("execute", std::vector<compiler::ir::BasicBlock*> {entry},
		                                                  std::vector<Type> {}, std::vector<std::string> {}, Type::v);
		ir->addFunctionOperation(fn);
		return ir;
	}

	/// Malformed: two blocks sharing the same BlockIdentifier.
	static std::shared_ptr<compiler::ir::IRGraph> makeMalformedGraph_DuplicateBlockId() {
		auto ir = std::make_shared<compiler::ir::IRGraph>("malformed-duplicate-id");
		auto& arena = ir->getArena();

		auto* b1 = arena.create<compiler::ir::BasicBlock>(arena, compiler::ir::BlockIdentifier {0},
		                                                  std::vector<compiler::ir::BasicBlockArgument*> {});
		b1->addOperation<compiler::ir::ReturnOperation>();

		auto* b2 = arena.create<compiler::ir::BasicBlock>(arena, compiler::ir::BlockIdentifier {0},
		                                                  std::vector<compiler::ir::BasicBlockArgument*> {});
		b2->addOperation<compiler::ir::ReturnOperation>();

		auto* fn =
		    arena.create<compiler::ir::FunctionOperation>("execute", std::vector<compiler::ir::BasicBlock*> {b1, b2},
		                                                  std::vector<Type> {}, std::vector<std::string> {}, Type::v);
		ir->addFunctionOperation(fn);
		return ir;
	}

	/// entry(cond: bool) --if--> then / else, each defining a private const and
	/// an "unused" const, both branching into merge(used: i32, unused: i32),
	/// which returns `used`. Neither arm dominates the merge block; the merge
	/// block does not dominate either arm; entry dominates everything. The
	/// second merge argument is fed by every predecessor but read by nothing,
	/// so it is a removal candidate for block-argument pruning tests (M7).
	static std::shared_ptr<compiler::ir::IRGraph> makeDiamondGraph() {
		namespace ir = compiler::ir;
		auto irGraph = std::make_shared<ir::IRGraph>("diamond-test");
		auto& arena = irGraph->getArena();

		auto* mergeUsedArg = arena.create<ir::BasicBlockArgument>(ir::OperationIdentifier {20}, Type::i32);
		auto* mergeUnusedArg = arena.create<ir::BasicBlockArgument>(ir::OperationIdentifier {21}, Type::i32);
		auto* mergeBlock = arena.create<ir::BasicBlock>(
		    arena, ir::BlockIdentifier {3}, std::vector<ir::BasicBlockArgument*> {mergeUsedArg, mergeUnusedArg});
		mergeBlock->addOperation<ir::ReturnOperation>(mergeUsedArg);

		auto* thenBlock =
		    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {1}, std::vector<ir::BasicBlockArgument*> {});
		auto* thenUsed = thenBlock->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {10}, 10, Type::i32);
		auto* thenUnused = thenBlock->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {11}, 100, Type::i32);
		thenBlock->addNextBlock(mergeBlock, std::vector<ir::Operation*> {thenUsed, thenUnused});

		auto* elseBlock =
		    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {2}, std::vector<ir::BasicBlockArgument*> {});
		auto* elseUsed = elseBlock->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {12}, 20, Type::i32);
		auto* elseUnused = elseBlock->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {13}, 200, Type::i32);
		elseBlock->addNextBlock(mergeBlock, std::vector<ir::Operation*> {elseUsed, elseUnused});

		auto* condArg = arena.create<ir::BasicBlockArgument>(ir::OperationIdentifier {1}, Type::b);
		auto* entry = arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {0},
		                                           std::vector<ir::BasicBlockArgument*> {condArg});
		auto* ifOp = arena.create<ir::IfOperation>(arena, condArg, 0.5);
		ifOp->setTrueBlockInvocation(thenBlock);
		ifOp->setFalseBlockInvocation(elseBlock);
		entry->addOperation(ifOp);

		auto* fn = arena.create<ir::FunctionOperation>(
		    "execute", std::vector<ir::BasicBlock*> {entry, thenBlock, elseBlock, mergeBlock},
		    std::vector<Type> {Type::b}, std::vector<std::string> {"cond"}, Type::i32);
		irGraph->addFunctionOperation(fn);
		return irGraph;
	}

	/// entry(cond: bool) --if--> both arms target the *same* merge block (the
	/// duplicate-predecessor shape): merge's predecessor list must list entry
	/// twice, once per invocation.
	static std::shared_ptr<compiler::ir::IRGraph> makeSharedTargetIfGraph() {
		namespace ir = compiler::ir;
		auto irGraph = std::make_shared<ir::IRGraph>("shared-target-if-test");
		auto& arena = irGraph->getArena();

		auto* mergeArg = arena.create<ir::BasicBlockArgument>(ir::OperationIdentifier {10}, Type::i32);
		auto* mergeBlock = arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {1},
		                                                std::vector<ir::BasicBlockArgument*> {mergeArg});
		mergeBlock->addOperation<ir::ReturnOperation>(mergeArg);

		auto* condArg = arena.create<ir::BasicBlockArgument>(ir::OperationIdentifier {1}, Type::b);
		auto* entry = arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {0},
		                                           std::vector<ir::BasicBlockArgument*> {condArg});
		auto* trueConst = entry->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {2}, 1, Type::i32);
		auto* falseConst = entry->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {3}, 2, Type::i32);
		auto* ifOp = arena.create<ir::IfOperation>(arena, condArg, 0.5);
		ifOp->setTrueBlockInvocation(mergeBlock);
		ifOp->setFalseBlockInvocation(mergeBlock);
		ifOp->getTrueBlockInvocation().addArgument(arena, trueConst);
		ifOp->getFalseBlockInvocation().addArgument(arena, falseConst);
		entry->addOperation(ifOp);

		auto* fn = arena.create<ir::FunctionOperation>("execute", std::vector<ir::BasicBlock*> {entry, mergeBlock},
		                                               std::vector<Type> {Type::b}, std::vector<std::string> {"cond"},
		                                               Type::i32);
		irGraph->addFunctionOperation(fn);
		return irGraph;
	}

	/// preheader -> header(iv, acc, limit) <-> latch, header -> exit(result).
	/// `iv`/`acc` are loop-carried (a new value every iteration via the
	/// latch); `limit` is loop-invariant (the latch passes the same object
	/// straight back to header, unchanged) -- the shape
	/// `computeHeaderPassThroughMap` / strength reduction expect.
	static std::shared_ptr<compiler::ir::IRGraph> makeNaturalLoopGraph() {
		namespace ir = compiler::ir;
		auto irGraph = std::make_shared<ir::IRGraph>("natural-loop-test");
		auto& arena = irGraph->getArena();

		auto* exitResultArg = arena.create<ir::BasicBlockArgument>(ir::OperationIdentifier {40}, Type::i32);
		auto* exitBlock = arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {3},
		                                               std::vector<ir::BasicBlockArgument*> {exitResultArg});
		exitBlock->addOperation<ir::ReturnOperation>(exitResultArg);

		auto* ivArg = arena.create<ir::BasicBlockArgument>(ir::OperationIdentifier {20}, Type::i32);
		auto* accArg = arena.create<ir::BasicBlockArgument>(ir::OperationIdentifier {21}, Type::i32);
		auto* limitArg = arena.create<ir::BasicBlockArgument>(ir::OperationIdentifier {22}, Type::i32);
		auto* header = arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {1},
		                                            std::vector<ir::BasicBlockArgument*> {ivArg, accArg, limitArg});
		auto* cmp = header->addOperation<ir::CompareOperation>(ir::OperationIdentifier {23}, ivArg, limitArg,
		                                                       ir::CompareOperation::LT);
		auto* headerIf = arena.create<ir::IfOperation>(arena, cmp, 0.9);
		header->addOperation(headerIf);

		auto* latch =
		    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {2}, std::vector<ir::BasicBlockArgument*> {});
		auto* one = latch->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {30}, 1, Type::i32);
		auto* nextIv = latch->addOperation<ir::AddOperation>(ir::OperationIdentifier {31}, ivArg, one);
		auto* nextAcc = latch->addOperation<ir::AddOperation>(ir::OperationIdentifier {32}, accArg, ivArg);
		latch->addNextBlock(header, std::vector<ir::Operation*> {nextIv, nextAcc, limitArg});

		headerIf->setTrueBlockInvocation(latch);
		headerIf->setFalseBlockInvocation(exitBlock);
		headerIf->getFalseBlockInvocation().addArgument(arena, accArg);

		auto* preheader =
		    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {0}, std::vector<ir::BasicBlockArgument*> {});
		auto* ivInit = preheader->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {10}, 0, Type::i32);
		auto* accInit = preheader->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {11}, 0, Type::i32);
		auto* limitInit = preheader->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {12}, 10, Type::i32);
		preheader->addNextBlock(header, std::vector<ir::Operation*> {ivInit, accInit, limitInit});

		auto* fn = arena.create<ir::FunctionOperation>(
		    "execute", std::vector<ir::BasicBlock*> {preheader, header, latch, exitBlock}, std::vector<Type> {},
		    std::vector<std::string> {}, Type::i32);
		irGraph->addFunctionOperation(fn);
		return irGraph;
	}

	/// Single-block function: a pure chain `const -> cast -> add` with zero
	/// uses (must never survive a DCE-style sweep) next to a live constant
	/// that is returned, plus a use-free `StoreOperation` (impure -- must
	/// never be counted as dead despite having no uses).
	static std::shared_ptr<compiler::ir::IRGraph> makeDeadChainGraph() {
		namespace ir = compiler::ir;
		auto irGraph = std::make_shared<ir::IRGraph>("dead-chain-test");
		auto& arena = irGraph->getArena();

		auto* entry =
		    arena.create<ir::BasicBlock>(arena, ir::BlockIdentifier {0}, std::vector<ir::BasicBlockArgument*> {});

		auto* deadConst = entry->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {1}, 5, Type::i32);
		auto* deadCast = entry->addOperation<ir::CastOperation>(ir::OperationIdentifier {2}, deadConst, Type::i64);
		entry->addOperation<ir::AddOperation>(ir::OperationIdentifier {3}, deadCast, deadCast);

		auto* liveConst = entry->addOperation<ir::ConstIntOperation>(ir::OperationIdentifier {4}, 42, Type::i32);
		auto* allocaOp = entry->addOperation<ir::AllocaOperation>(ir::OperationIdentifier {5}, 0u);
		entry->addOperation<ir::StoreOperation>(liveConst, allocaOp);
		entry->addOperation<ir::ReturnOperation>(liveConst);

		auto* fn = arena.create<ir::FunctionOperation>("execute", std::vector<ir::BasicBlock*> {entry},
		                                               std::vector<Type> {}, std::vector<std::string> {}, Type::i32,
		                                               std::vector<ir::AllocaSpec> {ir::AllocaSpec {8, 8}});
		irGraph->addFunctionOperation(fn);
		return irGraph;
	}
};

} // namespace nautilus::testing
