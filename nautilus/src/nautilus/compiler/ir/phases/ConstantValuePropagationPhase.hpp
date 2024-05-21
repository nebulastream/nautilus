
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
class ConstantValuePropagationPhase {
public:
	/**
	 * @brief Applies the ConstantValuePropagationPhase to the supplied IR graph.
	 * @param IR graph that the ConstantValuePropagationPhase is applied to.
	 */
	void apply(std::shared_ptr<IR::IRGraph> ir);

private:
	/**
	 * @brief Internal context object contains phase logic and state.
	 */
	class ConstantValuePropagationPhaseContext {
	public:
		struct ConstOpInfo {
			Operations::Operation* operation;
			NES::Nautilus::IR::BasicBlockPtr definedInBlock;
			std::shared_ptr<Operations::ConstIntOperation> rootConstOp;
			int64_t constValue;
			bool isUsed;
			// std::vector<NES::Nautilus::IR::BasicBlockPtr> usedBy;
			bool isComplex;
		};

		/**
		 * @brief Constructor for the context of the remove br-only-blocks phase.
		 *
		 * @param ir: IRGraph to which the remove br-only-blocks phase will be applied.
		 */
		ConstantValuePropagationPhaseContext(std::shared_ptr<IR::IRGraph> ir) : ir(ir) {};

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
		void handleBlockInvocation(Operations::BasicBlockInvocation basicBlockInvocation);

		void detectComplexVariable(Operations::BasicBlockInvocation basicBlockInvocation);

		void removeConstOpFromBlock(const Operations::Operation*& constOp,
		                            const NES::Nautilus::IR::BasicBlockPtr& currentBlock);

		void replaceInputArgumentUsages(Operations::Operation* toReplace, Operations::Operation* replaceWith,
		                                BasicBlockPtr currentBlock);

		/**
		 * @brief Todo
		 *
		 * @param currentBlock: Current block that might be a br-only-block.
		 * @param candidates: Blocks that still need to be processed.
		 * @param visitedBlocks: Blocks that have already been processed and can be disregarded.
		 */
		void inline processBlock(IR::BasicBlockPtr currentBlock, std::stack<IR::BasicBlockPtr>& newBlocks,
		                         std::unordered_set<std::string> visitedBlocks);

		/**
		 * @brief Todo
		 *
		 * @param currentBlock: Current block that might be a br-only-block.
		 * @param candidates: Blocks that still need to be processed.
		 * @param visitedBlocks: Blocks that have already been processed and can be disregarded.
		 */
		void inline buildConstOpMap(IR::BasicBlockPtr currentBlock);

		void inline propagateConstantValues();

		void inline foldConstantValues(IR::BasicBlockPtr currentBlock);

	private:
		std::shared_ptr<IR::IRGraph> ir;
		std::unordered_map<std::string, ConstOpInfo> constOpMap;
		std::unordered_set<std::string> nonConstOpArgs;
		// std::unordered_map<std::string, std::vector<std::shared_ptr<Operations::ConstIntOperation>>>
		// blockToConstOpMap;
	};
};

} // namespace NES::Nautilus::IR
