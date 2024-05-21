
#pragma once

#include "TraceOperation.hpp"
#include "nautilus/common/traceing.hpp"
#include <cinttypes>
#include <ostream>
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

	operation_identifier addOperation(TraceOperation&& operation);

	/**
	 * @brief Adds a argument to the block
	 * @param value reference
	 */
	void addArgument(TypedValueRef ref);

	friend std::ostream& operator<<(std::ostream& os, const Block& block);

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
	 * @brief Defines a list of operations this block contains.
	 */
	std::vector<TraceOperation> operations;

	/**
	 * @brief Indicates successors of this block.
	 */
	std::vector<uint16_t> predecessors;
};

} // namespace nautilus::tracing
