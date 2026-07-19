
#pragma once

#include "nautilus/tracing/TraceOperation.hpp"
#include <cstddef>
#include <memory>
#include <span>
#include <unordered_set>
#include <vector>

namespace nautilus::tracing {

class ExecutionTrace;
class TraceModule;

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

	/**
	 * @brief Applies the phase on all functions in a trace module
	 * @param traceModule
	 * @return the modified trace module with all functions converted to SSA.
	 */
	std::shared_ptr<TraceModule> apply(std::shared_ptr<TraceModule> traceModule);

private:
	/**
	 * @brief The SSACreationPhaseContext maintains local state, which is required to calculate the ssa from.
	 */
	class SSACreationPhaseContext {
	public:
		explicit SSACreationPhaseContext(ExecutionTrace& trace);

		/*
		 * Starts the conversion of the trace to SSA from
		 */
		void process();

	private:
		struct AssignmentDefinitionRange {
			ValueRef* data;
			size_t count;
		};

		/**
		 * @brief Converts a single basic block to SSA form
		 * @param block reference to the basic block
		 */
		void processBlock(Block& block);
		Block& getReturnBlock();

		void processValueRef(Block& block, TypedValueRef ref);

		/**
		 * @brief Eagerly propagates a non-local value reference upward through predecessor blocks.
		 * Adds the value as a block argument and to predecessor branch instructions,
		 * continuing until reaching a block where the value is locally defined.
		 * @param block the block where the non-local value was discovered
		 * @param ref the value reference to propagate
		 */
		void propagateValue(Block& block, TypedValueRef ref);
		void initializePropagation();

		bool isDefinedInBlock(uint32_t blockId, ValueRef ref);
		void indexDefinitions(uint32_t blockId);
		void validateValueRef(ValueRef ref) const;

		/**
		 * @brief Removes the assignment operations from all blocks.
		 * Assignment operations are only required to infer SSA form.
		 */
		void removeAssignOperations();

	private:
		ExecutionTrace& trace;
		// Arena-backed O(1) block-visit tracking.
		std::span<uint8_t> processedBlocks;
		// Arena-backed table indexed by ValueRef. The entry is the block where
		// the value is available at the current point, or NO_BLOCK.
		std::span<uint32_t> availableInBlock;
		// Non-ASSIGN operation ids are globally unique, so their defining block
		// is recorded directly. ASSIGN may redefine an id in several blocks and
		// therefore uses a compact, lazily built per-block index.
		std::span<uint32_t> uniqueDefinitionBlock;
		std::span<AssignmentDefinitionRange> assignmentDefinitionRanges;
		// Tracks (blockId, ref) pairs that have already been propagated,
		// replacing the O(n) std::find on predBlock.arguments.
		std::unordered_set<uint64_t> propagatedValues;
		bool propagationInitialized = false;
		// Reused across propagateValue calls to avoid repeated heap allocation.
		std::vector<uint32_t> propWorklist;
	};
};

} // namespace nautilus::tracing
