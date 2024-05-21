
#pragma once
#include <memory>
#include <nautilus/compiler/ir/IRGraph.hpp>
#include <nautilus/compiler/ir/blocks/BasicBlock.hpp>
#include <nautilus/compiler/ir/operations/ConstIntOperation.hpp>
#include <nautilus/compiler/ir/operations/IfOperation.hpp>
#include <nautilus/compiler/ir/operations/Operation.hpp>
#include <stack>
#include <unordered_map>
#include <unordered_set>

namespace NES::Nautilus::IR {

/**
 * @brief This phase takes an IR graph with blocks that either have branch- or if-operations as terminator-operations.
 *        Subsequently, this phase detects which if-operations are loop operations, converts them and enriches them with
 *        relevant information.
 */
class LoopDetectionPhase {
public:
	/**
	 * @brief Applies the LoopDetectionPhase to the supplied IR graph.
	 * @param IR graph that the LoopDetectionPhase is applied to.
	 */
	void apply(std::shared_ptr<IR::IRGraph> ir);

private:
	/**
	 * @brief Internal context object contains phase logic and state.
	 */
	class LoopDetectionPhaseContext {
	public:
		/**
		 * @brief Constructor for the context of the LoopDetectionPhaseContext.
		 *
		 * @param ir: IRGraph to which LoopDetectionPhaseContext will be applied.
		 */
		LoopDetectionPhaseContext(std::shared_ptr<IR::IRGraph> ir) : ir(ir) {};

		/**
		 * @brief Actually applies the LoopDetectionPhaseContext to the IR.
		 */
		void process();

	private:
		/**
		 * @brief Iterates over IR graph, finds all loop-header-blocks, and marks them.
		 *
		 * @param currentBlock: Initially will be the body-block of the root operation.
		 */
		void findLoopHeadBlocks(IR::BasicBlockPtr currentBlock);

		/**
		 * @brief Traverses a branch of the IR graph until either an already visited block or the return block
		 *        is encountered. If the already visited block contains an if-operation and if we did not fully
		 *        exhaust the true-branch of that if-operation, we found a loop-header and can construct a loop
		 * operation.
		 *
		 * @param currentBlock: The block that we are currently processing.
		 * @param ifBlocks: Blocks that contain an if-operation as their terminator operation.
		 * @param visitedBlocks: Blocks that have already been processed.
		 * @param loopHeaderCandidates: ifBlocks with true-branches that have not been fully explored yet.
		 * @param priorBlock: We keep track of the previous block to assign the loop-end-block, when the currentBlock
		 *                    is a loop-header block and we are creating its loop-operation.
		 */
		void inline checkBranchForLoopHeadBlocks(IR::BasicBlockPtr& currentBlock,
		                                         std::stack<IR::BasicBlockPtr>& ifBlocks,
		                                         std::unordered_set<std::string>& visitedBlocks,
		                                         std::unordered_set<std::string>& loopHeaderCandidates,
		                                         IR::BasicBlockPtr& priorBlock);

		/**
		 * @brief Checks the loop-header-block and block that appears in front of the loop-header-block in the control
		 * flow (from root) for constant values that represent the loop-iteration-variable and/or the upperBound.
		 *
		 * @param loopHeaderBlock: The loopHeaderBlock can contain the definition for the upperBound.
		 * @param loopBeforeBlock: The loopBeforeBlock can contain the definition of the loop-iteration-variable and
		 *                          the definition of the upperBound.
		 * @param compareOp: Contains the loop-induction-variable, and the upperBound as inputs.
		 * @return A pair that may contain the values for the loop-induction-variable and the upperBound.
		 *         However, the pair might also contain nullptrs, which we must handle.
		 */
		std::pair<std::shared_ptr<ir::ConstIntOperation>, std::shared_ptr<ir::ConstIntOperation>>
		getCompareOpConstants(const BasicBlockPtr& loopHeaderBlock, const BasicBlockPtr& loopBeforeBlock,
		                      const std::shared_ptr < Operations::CompareOperation * compareOp);

		/**
		 * @brief Check the loopEndBlock for a constant operation that matches the step size used in countOp.
		 *
		 * @param loopEndBlock: We currently assume that the step size is defined as constant in the loopEndBlock.
		 * @param countOp: The stepSize is used to increment the loop-induction-variable in the countOp.
		 * @return std::shared_ptr<ir::ConstIntOperation>: stepSize
		 */
		std::shared_ptr<ir::ConstIntOperation> inline getStepSize(const BasicBlockPtr& loopEndBlock,
		                                                          const Operations::Operation*& countOp);

	private:
		std::shared_ptr<IR::IRGraph> ir;
		std::unordered_set<std::string> visitedBlocks;
	};
};

} // namespace NES::Nautilus::IR
