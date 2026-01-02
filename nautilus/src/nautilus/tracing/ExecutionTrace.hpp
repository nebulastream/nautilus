
#pragma once

#include "Block.hpp"
#include "TraceOperation.hpp"
#include "tag/SharedHashMap.hpp"
#include "tag/TagRecorder.hpp"
#include <memory>
#include <unordered_map>

namespace nautilus::tracing {

/**
 * @brief The execution trace captures the trace of a program
 */
class ExecutionTrace {
public:
	/**
	 * @brief Attaches a new block to the trace. The block will have a unique identifier
	 * @return identifier of the newly created block
	 */
	uint16_t createBlock();

	/**
	 * @brief Adds arguments that are passed to the traced function
	 * @param argument
	 */
	TypedValueRef& setArgument(Type type, size_t index);

	/**
	 * @brief Returns all arguments of this trace.
	 * @return std::vector<ValueRef>
	 */
	const std::vector<TypedValueRef>& getArguments();

	/**
	 * @brief Returns the reference to a specific block
	 * @param blockIndex
	 * @return Block&
	 */
	Block& getBlock(uint16_t blockIndex);
	const Block& getBlock(uint16_t blockIndex) const;

	/**
	 * @brief Returns a reference to all blocks
	 * @return std::vector<Block>&
	 */
	std::vector<Block>& getBlocks();
	const std::vector<Block>& getBlocks() const;

	/**
	 * @brief Returns the return reference
	 * @return  std::shared_ptr<OperationRef>
	 */
	std::vector<operation_identifier>& getReturns();
	const std::vector<operation_identifier>& getReturns() const;

	/**
	 * @brief Converts the execution trace to a string representation
	 * @return std::string String representation of the trace
	 */
	std::string toString() const;

private:
	std::vector<Block> blocks;
	std::vector<operation_identifier> returnRefs;
};

} // namespace nautilus::tracing
