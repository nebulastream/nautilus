#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
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
};

} // namespace nautilus::testing
