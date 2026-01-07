
#pragma once

#include "Block.hpp"
#include "TraceOperation.hpp"
#include "tag/TagRecorder.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace nautilus::tracing {

class ExecutionTrace;

/**
 * @brief Container for multiple execution traces, mapping function names to their traces.
 *
 * TraceModule manages a collection of execution traces, where each trace corresponds to a
 * different function. This is used to support tracing of nested Nautilus functions, where
 * the root "execute" function may call other Nautilus functions that need to be traced
 * and compiled separately.
 *
 * The module maintains ownership of all ExecutionTrace objects through unique_ptr, ensuring
 * proper lifetime management. It is move-only to prevent accidental copying of large trace data.
 *
 * @note This class is move-only (copy operations are deleted)
 * @see ExecutionTrace
 * @see TraceContext::startTrace
 */
class TraceModule {
public:
	TraceModule() = default;
	~TraceModule() = default;

	// Delete copy operations (TraceModule is move-only)
	TraceModule(const TraceModule&) = delete;
	TraceModule& operator=(const TraceModule&) = delete;

	// Default move operations
	TraceModule(TraceModule&&) = default;
	TraceModule& operator=(TraceModule&&) = default;

	/**
	 * @brief Creates and adds a new function trace to the module.
	 *
	 * Creates a new ExecutionTrace for the given function name and stores it in the module.
	 * If a function with the same name already exists, it will be replaced.
	 *
	 * @param functionName Name of the function to trace (e.g., "execute", "helper_function")
	 * @return Reference to the newly created ExecutionTrace
	 * @note The returned reference remains valid until the TraceModule is destroyed or moved
	 */
	ExecutionTrace& addNewFunction(std::string_view functionName);

	/**
	 * @brief Retrieves the execution trace for a specific function.
	 *
	 * @param functionName Name of the function to retrieve
	 * @return Pointer to the ExecutionTrace if found, nullptr otherwise
	 * @note The returned pointer is owned by the TraceModule and should not be deleted
	 */
	ExecutionTrace* getFunction(const std::string& functionName);

	bool hasFunction(const std::string& functionName) const;
	/**
	 * @brief Retrieves the execution trace for a specific function (const version).
	 *
	 * @param functionName Name of the function to retrieve
	 * @return Pointer to the ExecutionTrace if found, nullptr otherwise
	 * @note The returned pointer is owned by the TraceModule and should not be deleted
	 */
	const ExecutionTrace* getFunction(const std::string& functionName) const;

	/**
	 * @brief Converts all function traces to a string representation.
	 *
	 * Formats all execution traces in the module with function names as headers.
	 * Useful for debugging and dumping trace information.
	 *
	 * @return String containing all function traces with headers like "Function: <name>"
	 * @note The output format is consistent with ExecutionTrace::toString()
	 */
	std::string toString() const;

	/**
	 * @brief Gets a list of all function names in this module.
	 *
	 * @return Vector of function names contained in the module
	 * @note The order of names is not guaranteed
	 */
	std::vector<std::string> getFunctionNames() const;

	/**
	 * @brief Returns an iterator to the beginning of the functions map
	 */
	auto begin() {
		return functions.begin();
	}
	auto begin() const {
		return functions.begin();
	}

	/**
	 * @brief Returns an iterator to the end of the functions map
	 */
	auto end() {
		return functions.end();
	}
	auto end() const {
		return functions.end();
	}

private:
	/// Maps function names to their execution traces
	std::unordered_map<std::string, std::unique_ptr<ExecutionTrace>> functions;
};

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
	 * @param inputs The input operands for the operation
	 * @return TypedValueRef& Reference to the resulting value
	 */
	TypedValueRef& addOperationWithResult(Snapshot& snapshot, Op& operation, Type& resultType,
	                                      std::vector<InputVariant> inputs);

	/**
	 * @brief Adds a comparison operation to the trace with branch probability
	 * @param snapshot The current execution snapshot
	 * @param inputs The input value to compare
	 * @param probability The branch probability for this comparison
	 */
	void addCmpOperation(Snapshot& snapshot, const TypedValueRef& inputs, const double probability);

	/**
	 * @brief Adds an assignment operation to the trace
	 * @param snapshot The current execution snapshot
	 * @param targetRef The target value reference
	 * @param srcRef The source value reference
	 * @param resultType The type of the result
	 * @return TypedValueRef& Reference to the resulting value
	 */
	TypedValueRef& addAssignmentOperation(Snapshot&, const TypedValueRef& targetRef, const TypedValueRef& srcRef,
	                                      Type resultType);

	/**
	 * @brief Adds a return operation to the trace
	 * @param snapshot The current execution snapshot
	 * @param type The type of the return value
	 * @param ref The value reference being returned
	 */
	void addReturn(Snapshot&, Type type, const TypedValueRef& ref);

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
	 * @param inputs The input operands for the operation
	 */
	void addOperation(Snapshot& snapshot, Op& operation, std::vector<InputVariant> inputs);

	/**
	 * @brief Returns the current block
	 * @return Block&
	 */
	Block& getCurrentBlock();

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

private:
	/**
	 * @brief Adds a tag for the given snapshot
	 * @param snapshot The snapshot to tag
	 * @param identifier The operation identifier to associate with the tag
	 */
	void addTag(Snapshot& snapshot, operation_identifier& identifier);

public:
	uint16_t currentBlockIndex;
	uint16_t currentOperationIndex;
	std::vector<Block> blocks;
	std::vector<operation_identifier> returnRefs;
	uint16_t lastValueRef = 0;
	std::unordered_map<Snapshot, operation_identifier> globalTagMap;
	std::unordered_map<Snapshot, operation_identifier> localTagMap;

	/**
	 * @brief Gets the next available operation identifier
	 * @return operation_identifier The next operation identifier
	 */
	operation_identifier getNextOperationIdentifier();
};

} // namespace nautilus::tracing
