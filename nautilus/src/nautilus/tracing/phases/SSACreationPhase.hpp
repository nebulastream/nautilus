
#pragma once

#include "nautilus/tracing/TraceOperation.hpp"
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
		 * @brief Checks if a ValueRef is defined in the local scope of a block by an operation
		 * before operationIndex, or by a block argument.
		 */
		static bool isLocalValueRef(Block& block, TypedValueRef& ref, Type ref_type, uint32_t operationIndex);

		/**
		 * @brief Propagates a batch of non-local value references upward through predecessor
		 * blocks in a single traversal. Adds each ref as a block argument and updates branch
		 * instructions, stopping at blocks where the value is locally defined.
		 * @param startBlock the block where the non-local values were discovered
		 * @param refs the value references to propagate
		 */
		void propagateBatch(Block& startBlock, const std::vector<TypedValueRef>& refs);

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
		std::unordered_set<uint32_t> processedBlocks;
		// Precomputed set of value refs defined by operations in each block.
		// Used for O(1) locality checks during value propagation.
		std::unordered_map<uint16_t, std::unordered_set<uint16_t>> blockDefinitions;
	};
};

} // namespace nautilus::tracing
