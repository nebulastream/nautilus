#pragma once

#include <memory>
#include <nautilus/compiler/ir/IRGraph.hpp>
#include <nautilus/compiler/ir/blocks/BasicBlock.hpp>
#include <nautilus/compiler/ir/operations/IfOperation.hpp>
#include <nautilus/compiler/ir/operations/Operation.hpp>
#include <stack>
#include <unordered_map>
#include <unordered_set>

namespace NES::Nautilus::IR {

/**
 * @brief This phase removes branch-only-blocks from a given IR graph.
 */
class RemoveBrOnlyBlocksPhase {
public:
	/**
	 * @brief Applies the RemoveBrOnlyBlocksPhase to the supplied IR graph.
	 * @param IR graph that the RemoveBrOnlyBlocksPhase is applied to.
	 */
	void apply(std::shared_ptr<IR::IRGraph> ir);

private:
	/**
	 * @brief Internal context object contains phase logic and state.
	 */
	class RemoveBrOnlyBlocksPhaseContext {
	public:
		/**
		 * @brief Constructor for the context of the remove br-only-blocks phase.
		 *
		 * @param ir: IRGraph to which the remove br-only-blocks phase will be applied.
		 */
		RemoveBrOnlyBlocksPhaseContext(std::shared_ptr<IR::IRGraph> ir) : ir(ir) {};

		/**
		 * @brief Actually applies the remove br-only-blocks phase to the IR.
		 */
		void process();

	private:
		/**
		 * @brief Adds predecessor information for all blocks in the IR.
		 *
		 * @param currentBlock: Initially will be the body-block of the root operation.
		 */
		void addPredecessors(IR::BasicBlockPtr currentBlock);

		/**
		 * @brief Removes all br-only blocks from the IR without influencing its control- and data flow.
		 *
		 * @param currentBlock: Initially will be the body-block of the root operation.
		 */
		void removeBrOnlyBlocks(IR::BasicBlockPtr currentBlock);

		/**
		 * @brief Checks if the current block is a br-only-block and check whether subsequent blocks are also
		 * br-only-blocks. When a sequence of size 1-n br-only-blocks was found, updatePredecessorBlocks() is called.
		 *
		 * @param currentBlock: Current block that might be a br-only-block.
		 * @param candidates: Blocks that still need to be processed.
		 * @param visitedBlocks: Blocks that have already been processed and can be disregarded.
		 */
		void inline processPotentialBrOnlyBlock(IR::BasicBlockPtr currentBlock,
		                                        std::stack<IR::BasicBlockPtr>& candidates,
		                                        std::unordered_set<std::string> visitedBlocks);

	private:
		std::shared_ptr<IR::IRGraph> ir;
		std::unordered_set<std::string> visitedBlocks;
	};
};

} // namespace NES::Nautilus::IR
