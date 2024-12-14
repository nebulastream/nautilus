
#pragma once

#include "nautilus/tracing/TraceOperation.hpp"
#include <memory>
#include <set>

namespace nautilus::tracing {

class ExecutionTrace;

class Block;

/**
 * @brief This phase converts a execution trace to SSA form.
 * This implies that, each value is only assigned and that all parameters to a basic block are passed by block
 * arguments.
 */
class SSACreationPhase {
public:
	/**
	 * @brief Applies the phase on a execution trace
	 * @param trace
	 * @return the modified execution trace.
	 */
	std::shared_ptr<ExecutionTrace> apply(std::shared_ptr<ExecutionTrace> trace);

private:
	/**
	 * @brief The SSACreationPhaseContext maintains local state, which is required to calculate the ssa from.
	 */
	class SSACreationPhaseContext {
	public:
		explicit SSACreationPhaseContext(std::shared_ptr<ExecutionTrace> trace);

		/*
		 * Starts the conversion of the trace to SSA from
		 */
		std::shared_ptr<ExecutionTrace> process();

	private:
		/**
		 * @brief Converts a single basic block to SSA form
		 * @param block reference to the basic block
		 */
		void processBlock(Block& block);
		Block& getReturnBlock();

		/**
		 * @brief Checks if an ValueRef is defined in a specific block by an argument or an operation before the current
		 * operationIndex
		 * @param block reference to the current basic block
		 * @param valRef reference to the the value ref we are looking fore
		 * @param operationIndex the operation index, which accesses the ValueRef
		 * @return true if Value Ref is defined locally.
		 */
		static bool isLocalValueRef(Block& block, TypedValueRef& valRef, Type ref_type, uint32_t operationIndex);

		void processValueRef(Block& block, TypedValueRef& type, Type ref_type, uint32_t operationIndex);

		void processBlockRef(Block& block, BlockRef& blockRef, uint32_t operationIndex);

		/**
		 * @brief Creates a mapping of old block ids to new block ids, taking the empty blocks to remove into account
		 * @param emptyBlocksToRemove the empty block ids to remove
		 * @return a map from old block ids to new block ids.
		 */
		std::unordered_map<uint16_t, uint16_t> creatingNewBlockIds(const std::set<uint16_t>& emptyBlocksToRemove) const;

		/**
		 * @brief Removes the assignment operations from all blocks.
		 * Assignment operations are only required to infer SSA form.
		 */
		void removeAssignOperations();

		/**
		 * @brief In this step we finalise the block arguments of all blocks and create unique variances of them.
		 */
		void makeBlockArgumentsUnique();

	private:
		std::shared_ptr<ExecutionTrace> trace;
		std::set<uint32_t> processedBlocks;
		//ValueRef maxValueRef = 1024;
	};
};

} // namespace nautilus::tracing
