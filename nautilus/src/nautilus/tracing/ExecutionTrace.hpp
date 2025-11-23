
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

	// Delete copy operations (ExecutionTrace is move-only)
	ExecutionTrace(const ExecutionTrace&) = delete;
	ExecutionTrace& operator=(const ExecutionTrace&) = delete;

	// Default move operations
	ExecutionTrace(ExecutionTrace&&) = default;
	ExecutionTrace& operator=(ExecutionTrace&&) = default;

	TypedValueRef& addOperationWithResult(Snapshot& snapshot, Op& operation, Type& resultType,
	                                      std::initializer_list<InputVariant> inputs);

	void addCmpOperation(Snapshot& snapshot, const TypedValueRef& inputs, double probability);

	TypedValueRef& addAssignmentOperation(Snapshot&, const TypedValueRef& targetRef, const TypedValueRef& srcRef,
	                                      Type resultType);

	void addReturn(Snapshot&, Type type, const TypedValueRef& ref);

	bool checkTag(Snapshot& snapshot);

	void resetExecution();

	void nextOperation();

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
	 * @brief Creates a new block in the trace.
	 * @return
	 */
	uint16_t createBlock();

	/**
	 * @brief Returns the reference to a specific block
	 * @param blockIndex
	 * @return Block&
	 */
	Block& getBlock(uint16_t blockIndex);

	/**
	 * @brief Returns a reference to all blocks
	 * @return std::vector<Block>&
	 */
	std::vector<Block>& getBlocks();

	/**
	 * @brief Returns the index to the current block.
	 * @return uint32_t
	 */
	uint16_t getCurrentBlockIndex() const;

	void addOperation(Snapshot& snapshot, Op& operation, std::initializer_list<InputVariant> inputs);

	/**
	 * @brief Returns the current block
	 * @return Block&
	 */
	Block& getCurrentBlock();

	TraceOperation& getCurrentOperation();

	/**
	 * @brief Sets the current block
	 * @param index
	 */
	void setCurrentBlock(uint16_t index);

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
