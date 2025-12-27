
#pragma once

#include "TraceOperation.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include <cinttypes>
#include <vector>

namespace nautilus::tracing {

struct operation_identifier {
	uint16_t blockIndex;
	uint16_t operationIndex;
};

/**
 * @brief Represents a basic block in a trace
 */
class Block {
public:
	/**
	 * @brief Indicates if this is a control flow merge block -> such blocks could be loop headers.
	 */
	enum class Type : uint8_t { Default, ControlFlowMerge };

	/**
	 * @brief creates a new block with a specific block id.
	 * @param blockId
	 */
	explicit Block(uint16_t blockId);

	/**
	 * @brief Adds an operation index to this block
	 * @param operationIndex Index into ExecutionTrace::operations vector
	 * @return operation_identifier The identifier for the added operation
	 */
	operation_identifier addOperation(uint32_t operationIndex);

	/**
	 * @brief Adds a argument to the block
	 * @param value reference
	 */
	void addArgument(TypedValueRef ref);

	/**
	 * @brief defines the id of this block.
	 */
	uint16_t blockId;

	/**
	 * @brief defines the type of this block.
	 */
	Type type;

	/**
	 * @brief indicates a list of arguments that this block receives.
	 */
	std::vector<TypedValueRef> arguments;

	/**
	 * @brief Defines a list of operation indices into ExecutionTrace::operations vector.
	 * Changed from std::vector<TraceOperation> to std::vector<uint32_t> for:
	 * - Simplified copy/move operations (4 bytes vs 64 bytes per operation)
	 * - Better cache locality (operations stored contiguously in ExecutionTrace)
	 * - Easier cross-block operation access
	 */
	std::vector<uint32_t> operations;

	/**
	 * @brief Indicates successors of this block.
	 */
	std::vector<uint16_t> predecessors;
};

} // namespace nautilus::tracing
