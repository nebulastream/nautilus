#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <memory>
#include <stack>
#include <unordered_map>
#include <unordered_set>

namespace NES::Nautilus::IR {

/**
 * @brief This phase removes branch-only-blocks from a given IR graph.
 */
class RedundantOperationRemovalPhase {
public:
	/**
	 * @brief Applies the RedundantOperationRemovalPhase to the supplied IR graph.
	 * @param IR graph that the RedundantOperationRemovalPhase is applied to.
	 */
	void apply(std::shared_ptr<IR::IRGraph> ir);

private:
	/**
	 * @brief Internal context object contains phase logic and state.
	 */
	class RedundantOperationRemovalPhaseContext {
	public:
		/**
		 * @brief Constructor for the context of the remove br-only-blocks phase.
		 *
		 * @param ir: IRGraph to which the remove br-only-blocks phase will be applied.
		 */
		RedundantOperationRemovalPhaseContext(std::shared_ptr<IR::IRGraph> ir) : ir(ir) {};

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
		// void addPredecessors(IR::BasicBlockPtr currentBlock);
		void removeRedundantOperations(IR::BasicBlockPtr currentBlock);

	private:
		std::shared_ptr<IR::IRGraph> ir;
		std::unordered_map<std::string, Operations::Operation*> operationMap;
	};
};

} // namespace NES::Nautilus::IR
