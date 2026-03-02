
#pragma once

#include "nautilus/tracing/TraceOperation.hpp"
#include <memory>
#include <unordered_map>
#include <unordered_set>

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
		 * @brief Checks if a ValueRef is defined in a specific block before the given operation index.
		 * Uses precomputed definition indices for O(1) lookup.
		 * @param blockId the block to check
		 * @param ref the value reference to look for
		 * @param operationIndex the operation index that accesses the ValueRef
		 * @return true if ValueRef is defined locally
		 */
		bool isLocalValueRef(uint16_t blockId, uint16_t ref, uint32_t operationIndex) const;

		void processValueRef(Block& block, TypedValueRef& ref, uint32_t operationIndex);

		void processBlockRef(Block& block, BlockRef& blockRef, uint32_t operationIndex);

		/**
		 * @brief Eagerly propagates a non-local value reference upward through predecessor blocks.
		 * Adds the value as a block argument and to predecessor branch instructions,
		 * continuing until reaching a block where the value is locally defined.
		 * @param block the block where the non-local value was discovered
		 * @param ref the value reference to propagate
		 */
		void propagateValue(Block& block, TypedValueRef ref);

		/**
		 * @brief Removes the assignment operations from all blocks.
		 * Assignment operations are only required to infer SSA form.
		 */
		void removeAssignOperations();

		/**
		 * @brief Adds an argument to a block with O(1) deduplication via the argument set.
		 * @param block the block to add the argument to
		 * @param ref the value reference to add
		 */
		void addBlockArgument(Block& block, TypedValueRef ref);

	private:
		std::shared_ptr<ExecutionTrace> trace;
		std::unordered_set<uint16_t> processedBlocks;
		// Precomputed: blockId -> (valueRef -> first definition op index in that block).
		// Used for O(1) locality checks in isLocalValueRef and propagateValue.
		std::unordered_map<uint16_t, std::unordered_map<uint16_t, uint32_t>> blockFirstDefIndex;
		// Maintained: blockId -> set of argument refs for O(1) containment checks.
		// Updated alongside Block::arguments during value propagation.
		std::unordered_map<uint16_t, std::unordered_set<uint16_t>> blockArgSets;
	};
};

} // namespace nautilus::tracing
