
#pragma once

#include "Block.hpp"
#include "TraceOperation.hpp"
#include <unordered_map>

namespace nautilus::tracing {

/**
 * @brief The execution trace captures the trace of a program
 */
class ExecutionTrace {
public:
	/**
	 * @brief Constructs a new execution trace
	 */
	ExecutionTrace();

	~ExecutionTrace() = default;

	// Delete copy operations (ExecutionTrace is move-only)
	ExecutionTrace(const ExecutionTrace&) = delete;
	ExecutionTrace& operator=(const ExecutionTrace&) = delete;

	// Default move operations
	ExecutionTrace(ExecutionTrace&&) = default;
	ExecutionTrace& operator=(ExecutionTrace&&) = default;

	/**
	 * @brief Adds an operation with a result to the trace
	 * @param snapshot The current execution snapshot
	 * @param operation The operation to add
	 * @param resultType The type of the result value
	 * @param inputs Optional input variants for the operation
	 * @return TypedValueRef& Reference to the resulting value
	 */
	template <typename... Inputs>
	TypedValueRef& addOperationWithResult(Snapshot& snapshot, Op operation, Type resultType, Inputs&&... inputs) {
		uint32_t globalOpIndex =
		    addOperationToBlock(snapshot, operation, resultType, TypedValueRef(getNextValueRef(), resultType),
		                        std::forward<Inputs>(inputs)...);

		auto operationIdentifier = getNextOperationIdentifier();
		addTag(snapshot, operationIdentifier);
		return operations[globalOpIndex].resultRef;
	}

	/**
	 * @brief Adds a comparison operation to the trace with branch probability
	 * @param snapshot The current execution snapshot
	 * @param inputs The input value to compare
	 * @param probability The branch probability for this comparison
	 */
	void addCmpOperation(Snapshot& snapshot, TypedValueRef inputs, float probability);

	/**
	 * @brief Adds an assignment operation to the trace
	 * @param snapshot The current execution snapshot
	 * @param targetRef The target value reference
	 * @param srcRef The source value reference
	 * @param resultType The type of the result
	 * @return TypedValueRef& Reference to the resulting value
	 */
	TypedValueRef& addAssignmentOperation(Snapshot&, TypedValueRef targetRef, TypedValueRef srcRef, Type resultType);

	/**
	 * @brief Adds a return operation to the trace
	 * @param snapshot The current execution snapshot
	 * @param type The type of the return value
	 * @param ref The value reference being returned
	 */
	void addReturn(Snapshot&, Type type, TypedValueRef ref);

	/**
	 * @brief Checks if a tag exists for the given snapshot
	 * @param snapshot The snapshot to check
	 * @return bool True if the tag exists, false otherwise
	 */
	bool checkTag(Snapshot& snapshot);

	/**
	 * @brief Resets the execution state of the trace
	 */
	void resetExecution();

	/**
	 * @brief Advances to the next operation in the trace
	 */
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

	/**
	 * @brief Adds an operation without a result to the trace
	 * @param snapshot The current execution snapshot
	 * @param operation The operation to add
	 * @param inputs Optional input variants for the operation
	 */
	template <typename... Inputs>
	void addOperation(Snapshot& snapshot, Op operation, Inputs&&... inputs) {
		addOperationToBlock(snapshot, operation, Type::v, TypedValueRef(0, Type::v), std::forward<Inputs>(inputs)...);
	}

	/**
	 * @brief Returns the current block
	 * @return Block&
	 */
	Block& getCurrentBlock();

	/**
	 * @brief Gets an operation by its block and operation index
	 * @param blockIndex The block index
	 * @param operationIndex The operation index within the block
	 * @return TraceOperation& Reference to the operation
	 */
	TraceOperation& getOperation(uint16_t blockIndex, uint16_t operationIndex);

	/**
	 * @brief Gets an operation by its identifier
	 * @param identifier The operation identifier
	 * @return TraceOperation& Reference to the operation
	 */
	TraceOperation& getOperation(const operation_identifier& identifier);

	/**
	 * @brief Returns the current operation being traced
	 * @return TraceOperation& Reference to the current operation
	 */
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

	/**
	 * @brief Converts the execution trace to a string representation
	 * @return std::string String representation of the trace
	 */
	std::string toString() const;

	/**
	 * @brief Gets the next available value reference identifier
	 * @return uint16_t The next value reference ID
	 */
	uint16_t getNextValueRef();

	FunctionCall& getFunctionCall(FunctionCallId index) {
		return functionCalls[index.id];
	}

	FunctionCallId addFunctionCall(FunctionCall&& functionCall) {
		functionCalls.push_back(std::move(functionCall));
		return FunctionCallId(static_cast<uint32_t>(functionCalls.size() - 1));
	}

	BlockRef& getBlockRef(BlockRefId index) {
		return blockRefs[index.id];
	}

	BlockRefId addBlockRef(BlockRef&& blockRef) {
		blockRefs.push_back(std::move(blockRef));
		return BlockRefId(static_cast<uint32_t>(blockRefs.size() - 1));
	}

	ConstantLiteral& getConstantLiteral(ConstantLiteralId index) {
		return constantLiterals[index.id];
	}

	ConstantLiteralId addConstantLiteral(ConstantLiteral&& constantLiteral) {
		constantLiterals.push_back(std::move(constantLiteral));
		return ConstantLiteralId(static_cast<uint32_t>(constantLiterals.size() - 1));
	}

private:
	/**
	 * @brief Adds a tag for the given snapshot
	 * @param snapshot The snapshot to tag
	 * @param identifier The operation identifier to associate with the tag
	 */
	void addTag(Snapshot& snapshot, operation_identifier& identifier);

	/**
	 * @brief Internal helper to add an operation to the operations vector and current block
	 * Constructs a TraceOperation in-place with perfect forwarding of arguments
	 * @tparam Args Types of arguments to forward to TraceOperation constructor
	 * @param args Arguments to forward to TraceOperation constructor
	 * @return uint32_t The global operation index in the operations vector
	 */
	template <typename... Args>
	uint32_t addOperationToBlock(Args&&... args) {
		if (blocks.empty()) {
			createBlock();
		}
		auto& block = getCurrentBlock();
		uint32_t globalOpIndex = operations.size();
		operations.emplace_back(std::forward<Args>(args)...);
		block.addOperation(globalOpIndex);
		return globalOpIndex;
	}

public:
	uint16_t currentBlockIndex;
	uint16_t currentOperationIndex;
	std::vector<Block> blocks;

	/**
	 * @brief Central storage for all operations across all blocks.
	 * Blocks reference operations via indices into this vector.
	 * This design enables:
	 * - Efficient copy/move of blocks (only copying indices, not full operations)
	 * - Better cache locality (all operations in contiguous memory)
	 * - Simpler cross-block operation access
	 */
	std::vector<TraceOperation> operations;

	std::vector<operation_identifier> returnRefs;
	uint16_t lastValueRef = 0;
	std::unordered_map<Snapshot, operation_identifier> globalTagMap;
	std::unordered_map<Snapshot, operation_identifier> localTagMap;
	std::vector<FunctionCall> functionCalls;
	std::vector<BlockRef> blockRefs;
	std::vector<ConstantLiteral> constantLiterals;

	/**
	 * @brief Gets the next available operation identifier
	 * @return operation_identifier The next operation identifier
	 */
	operation_identifier getNextOperationIdentifier();
};

} // namespace nautilus::tracing
