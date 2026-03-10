
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
		 * @brief Eagerly propagates a non-local value reference upward through predecessor blocks.
		 * Adds the value as a block argument and to predecessor branch instructions,
		 * continuing until reaching a block where the value is locally defined.
		 * @param block the block where the non-local value was discovered
		 * @param ref the value reference to propagate
		 */
		void propagateValue(Block& block, TypedValueRef ref);

		/**
		 * @brief Returns the set of value refs defined by operations in the given block,
		 * computing and caching it on first access.
		 */
		const std::unordered_set<ValueRef>& getOrBuildDefinitions(uint16_t blockId);

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
		// O(1) block-visit tracking; the iterative version never erases, so unordered_set is safe.
		std::unordered_set<uint32_t> processedBlocks;
		// Lazy cache: maps blockId → set of value refs produced by operations in that block.
		// Entries are built on first access rather than upfront.
		std::unordered_map<uint16_t, std::unordered_set<ValueRef>> blockDefinitions;
		// Tracks (blockId, ref) pairs that have already been propagated,
		// replacing the O(n) std::find on predBlock.arguments.
		std::unordered_set<uint64_t> propagatedValues;
		// Reused across propagateValue calls to avoid repeated heap allocation.
		std::vector<uint16_t> propWorklist;
	};
};

} // namespace nautilus::tracing
