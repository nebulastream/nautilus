
#pragma once

#include "Block.hpp"
#include "TraceOperation.hpp"
#include "nautilus/common/Arena.hpp"
#include "tag/TagRecorder.hpp"
#include <initializer_list>
#include <memory>
#include <unordered_map>
#include <vector>

namespace nautilus::tracing {

using Arena = common::Arena;

class ExecutionTrace;

/// Bundles a traced function's execution trace with its metadata.
struct TraceFunctionDefinition {
	std::string name;
	std::unique_ptr<ExecutionTrace> trace;
	std::unordered_map<std::string, std::string> attributes;
};

/**
 * @brief Container for traced functions, each represented by a TraceFunctionDefinition.
 *
 * TraceModule manages a collection of function traces used to support tracing of nested
 * Nautilus functions, where the root "execute" function may call other Nautilus functions
 * that need to be traced and compiled separately.
 *
 * @note This class is move-only (copy operations are deleted)
 * @see ExecutionTrace
 * @see TraceContext::startTrace
 */
class TraceModule {
public:
	TraceModule() = default;
	~TraceModule() = default;

	TraceModule(const TraceModule&) = delete;
	TraceModule& operator=(const TraceModule&) = delete;
	TraceModule(TraceModule&&) = default;
	TraceModule& operator=(TraceModule&&) = default;

	/**
	 * @brief Creates and adds a new function trace to the module.
	 *
	 * Creates a new ExecutionTrace for the given function name that
	 * allocates its Blocks and TraceOperations from the supplied Arena.
	 * If a function with the same name already exists, it will be replaced.
	 *
	 * @param functionName Name of the function to trace.
	 * @param arena Arena backing the trace's allocations; must outlive
	 *              the returned trace and the enclosing TraceModule.
	 * @return Reference to the newly created ExecutionTrace.
	 */
	ExecutionTrace& addNewFunction(std::string_view functionName, Arena& arena);

	/// Associates generic attributes with a previously added function.
	void setFunctionAttributes(const std::string& functionName,
	                           const std::unordered_map<std::string, std::string>& attrs);

	/// Returns the TraceFunctionDefinition for a function, or nullptr if not found.
	TraceFunctionDefinition* getFunctionDefinition(const std::string& functionName);
	const TraceFunctionDefinition* getFunctionDefinition(const std::string& functionName) const;

	/// Convenience: returns the execution trace for a function, or nullptr.
	ExecutionTrace* getFunction(const std::string& functionName);
	const ExecutionTrace* getFunction(const std::string& functionName) const;

	/// Convenience: returns the attributes for a function (empty map if none).
	const std::unordered_map<std::string, std::string>& getFunctionAttributes(const std::string& functionName) const;

	bool hasFunction(const std::string& functionName) const;

	/// Returns all function names in sorted order.
	std::vector<std::string> getFunctionNames() const;

	std::string toString() const;

private:
	std::unordered_map<std::string, TraceFunctionDefinition> functions;
};

/**
 * @brief The execution trace captures the trace of a program.
 *
 * An ExecutionTrace allocates all of its Blocks and TraceOperations from an
 * externally provided Arena.  Callers (the trace contexts, the engine, etc.)
 * are responsible for the Arena's lifetime and may reuse a single Arena
 * across multiple trace/compile cycles to amortise allocation costs.
 */
class ExecutionTrace {
public:
	/**
	 * @brief Constructs a new execution trace that allocates its Blocks and
	 * TraceOperations from the supplied Arena.
	 *
	 * The Arena must outlive the ExecutionTrace.  Long-lived components
	 * (e.g. a JIT compiler or Engine) can keep a single Arena alive across
	 * many trace/compile cycles and call Arena::softReset() between them
	 * to reuse the memory.
	 *
	 * @param arena Non-owning reference to the Arena to allocate from.
	 */
	explicit ExecutionTrace(Arena& arena);

	/**
	 * Destroys the trace, including explicit destruction of all
	 * arena-allocated Blocks and TraceOperations.  The Arena itself is left
	 * untouched so the caller can softReset() and reuse it for the next
	 * trace.
	 */
	~ExecutionTrace();

	// ExecutionTrace is neither copyable nor movable: Block and TraceOperation
	// instances hold pointers into the Arena, and moving them would leave
	// those pointers dangling.  Callers that need ownership transfer should
	// heap-allocate the trace via std::unique_ptr<ExecutionTrace>, which
	// moves the pointer rather than the object.
	ExecutionTrace(const ExecutionTrace&) = delete;
	ExecutionTrace& operator=(const ExecutionTrace&) = delete;
	ExecutionTrace(ExecutionTrace&&) = delete;
	ExecutionTrace& operator=(ExecutionTrace&&) = delete;

	/**
	 * @brief Adds an operation with a result to the trace
	 * @param snapshot The current execution snapshot
	 * @param operation The operation to add
	 * @param resultType The type of the result value
	 * @param inputs The input operands for the operation
	 * @return TypedValueRef& Reference to the resulting value
	 */
	TypedValueRef& addOperationWithResult(Snapshot& snapshot, Op& operation, Type& resultType,
	                                      std::initializer_list<InputVariant> inputs);

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
	uint32_t createBlock();

	/**
	 * @brief Returns the reference to a specific block
	 * @param blockIndex
	 * @return Block&
	 */
	Block& getBlock(uint32_t blockIndex);

	/**
	 * @brief Returns a reference to all blocks.
	 *
	 * Blocks are owned by the trace's Arena; the vector stores non-owning
	 * pointers whose target addresses are stable for the lifetime of the
	 * trace.
	 * @return std::vector<Block*>&
	 */
	std::vector<Block*>& getBlocks();

	/**
	 * @brief Returns a reference to the underlying arena used to allocate
	 * blocks and operations.  Phases that need to mint new TraceOperation /
	 * Block instances (e.g. during SSA construction) should use this arena
	 * instead of allocating on the heap directly.
	 */
	Arena& getArena();

	/**
	 * @brief Returns the index to the current block.
	 * @return uint32_t
	 */
	uint32_t getCurrentBlockIndex() const;

	/**
	 * @brief Adds an operation without a result to the trace
	 * @param snapshot The current execution snapshot
	 * @param operation The operation to add
	 * @param inputs The input operands for the operation
	 */
	void addOperation(Snapshot& snapshot, Op& operation, std::initializer_list<InputVariant> inputs);

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
	void setCurrentBlock(uint32_t index);

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
	 * @return ValueRef The next value reference ID
	 */
	ValueRef getNextValueRef();

private:
	/**
	 * @brief Adds a tag for the given snapshot
	 * @param snapshot The snapshot to tag
	 * @param identifier The operation identifier to associate with the tag
	 */
	void addTag(Snapshot& snapshot, operation_identifier& identifier);

public:
	/**
	 * @brief Non-owning pointer to the arena supplied at construction time
	 * that backs all Block and TraceOperation allocations for this trace.
	 * The pointed-to arena must outlive the ExecutionTrace.
	 */
	Arena* arena;
	uint32_t currentBlockIndex;
	ValueRef currentOperationIndex;
	/// Non-owning pointers to arena-allocated Block instances.
	std::vector<Block*> blocks;
	std::vector<operation_identifier> returnRefs;
	ValueRef lastValueRef = 0;
	std::unordered_map<Snapshot, operation_identifier> globalTagMap;
	std::unordered_map<Snapshot, operation_identifier> localTagMap;

	/**
	 * @brief Gets the next available operation identifier
	 * @return operation_identifier The next operation identifier
	 */
	operation_identifier getNextOperationIdentifier();
};

} // namespace nautilus::tracing
