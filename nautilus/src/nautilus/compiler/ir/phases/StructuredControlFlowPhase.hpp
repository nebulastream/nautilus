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
 *        Subsequently, this phase matches if-operations with their correct corresponding merge-blocks.
 *        Also, if-operations are enriched with information on which blocks branch to the merge-blocks.
 *        This is required to correctly create MLIR YieldOps.
 */
class StructuredControlFlowPhase {
public:
	/**
	 * @brief Applies the StructuredControlFlowPhase to the supplied IR graph.
	 * @param IR graph that the StructuredControlFlowPhase is applied to.
	 */
	void apply(std::shared_ptr<IR::IRGraph> ir);

private:
	/**
	 * @brief Internal context object contains phase logic and state.
	 */
	class StructuredControlFlowPhaseContext {
	public:
		/**
		 * @brief Helper struct used to keep track of the currently active branch of an if-operation.
		 */
		struct IfOpCandidate {
			std::shared_ptr<ir::IfOperation> ifOp;
			bool isTrueBranch;
		};

		/**
		 * @brief Constructor for the context of the StructuredControlFlowPhase.
		 *
		 * @param ir: IRGraph to which StructuredControlFlowPhase will be applied.
		 */
		StructuredControlFlowPhaseContext(std::shared_ptr<IR::IRGraph> ir) : ir(ir) {};

		/**
		 * @brief Actually applies the StructuredControlFlowPhase to the IR.
		 */
		void process();

	private:
		/**
		 * @brief Iterates over IR graph, and connects all if-operations with their corresponding merge-blocks.
		 *
		 * @param currentBlock: Initially will be the body-block of the root operation.
		 */
		void createIfOperations(IR::BasicBlockPtr currentBlock);

		/**
		 * @brief Checks if the given currentBlocks is a merge-block with open edges (open: not yet traversed).
		 *
		 * @param currentBlock: Initially will be the body-block of the root operation.
		 * @param ifOperations: A stack that contains all visited if-operations
		                        that have not been matched with merge-blocks yet.
		 * @param mergeBlockNumVisits: A HashMap that keeps track on how many times we visited a merge-block already.
		 * @param newVisit: Signals whether we reached the currentBlock via a new edge.
		 * @return true, if the currentBlock is a merge-block with open edges, and false if not.
		 */
		bool inline mergeBlockCheck(IR::BasicBlockPtr& currentBlock,
		                            std::stack<std::unique_ptr<IfOpCandidate>>& ifOperations,
		                            std::unordered_map<std::string, uint32_t>& candidateEdgeCounter, bool newVisit,
		                            const std::unordered_set<IR::BasicBlockPtr>& loopBlockWithVisitedBody);

	private:
		std::shared_ptr<IR::IRGraph> ir;
		std::unordered_set<std::string> visitedBlocks;
	};
};

} // namespace NES::Nautilus::IR
