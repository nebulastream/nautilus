
#pragma once

#include "Block.hpp"
#include "TraceOperation.hpp"
#include "tag/TagRecorder.hpp"
#include <memory>
#include <unordered_map>

namespace nautilus::tracing {

/**
 * @brief The execution trace captures the trace of a program
 */
class ExecutionTrace {
public:
	ExecutionTrace();

	~ExecutionTrace() = default;

	value_ref addOperationWithResult(Snapshot& snapshot, Op& operation, Type& resultType, std::vector<InputVariant>&& inputs);

	void addCmpOperation(Snapshot& snapshot, value_ref inputs);

	void destruct(value_ref inputs);

	void addAssignmentOperation(Snapshot&, value_ref targetRef, value_ref srcRef, Type resultType);

	void addReturn(Snapshot&, Type type, value_ref ref);

	bool checkTag(Snapshot& snapshot);

	void resetExecution();

	void nextOperation();

	/**
	 * @brief Adds arguments that are passed to the traced function
	 * @param argument
	 */
	value_ref setArgument(Type type, size_t index);

	/**
	 * @brief Returns all arguments of this trace.
	 * @return std::vector<ValueRef>
	 */
	const std::vector<TypedValueRef>& getArguments();

	/**
	 * @brief Creates a new block in the trace.
	 * @return
	 */
	uint16_t createBlock();

	/**
	 * @brief Returns the reference to a specific block
	 * @param blockIndex
	 * @return Block&
	 */
	Block& getBlock(uint16_t blockIndex) {
		return blocks[blockIndex];
	}

	/**
	 * @brief Returns a reference to all blocks
	 * @return std::vector<Block>&
	 */
	std::vector<Block>& getBlocks() {
		return blocks;
	}

	/**
	 * @brief Returns the index to the current block.
	 * @return uint32_t
	 */
	uint16_t getCurrentBlockIndex() const {
		return currentBlockIndex;
	}

	void addOperation(Snapshot& snapshot, Op& operation, Type& resultType, nautilus::tracing::value_ref targetRef, nautilus::tracing::value_ref srcRef);

	/**
	 * @brief Returns the current block
	 * @return Block&
	 */
	Block& getCurrentBlock() {
		return blocks[currentBlockIndex];
	}

	TraceOperation& getCurrentOperation();

	/**
	 * @brief Sets the current block
	 * @param index
	 */
	void setCurrentBlock(uint16_t index) {
		currentOperationIndex = 0;
		currentBlockIndex = index;
	}

	/**
	 * @brief Processes a control flow merge
	 * @param blockIndex
	 * @param operationIndex
	 * @return Block&
	 */
	Block& processControlFlowMerge(operation_identifier oi);

	/**
	 * @brief Returns the return reference
	 * @return  std::shared_ptr<OperationRef>
	 */
	std::vector<operation_identifier> getReturn();

	std::string toString() const;

	uint16_t getNextValueRef();

private:
	void addTag(Snapshot& snapshot, operation_identifier& identifier);

public:
	uint16_t currentBlockIndex;
	uint16_t currentOperationIndex;
	std::vector<Block> blocks;
	std::vector<operation_identifier> returnRefs;
	uint16_t lastValueRef = 0;
	std::unordered_map<Snapshot, operation_identifier> globalTagMap;
	std::unordered_map<Snapshot, operation_identifier> localTagMap;

	operation_identifier getNextOperationIdentifier();
};

} // namespace nautilus::tracing


