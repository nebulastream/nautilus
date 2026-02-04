
#pragma once

#include "ExecutionTrace.hpp"
#include "TraceOperation.hpp"
#include "TracerTrace.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/options.hpp"
#include "tag/Tag.hpp"
#include "tag/TagRecorder.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <memory>

namespace nautilus::tracing {

/**
 * @brief External entrypoint for creating an ExecutionTrace of a function.
 * Will select a TraceContext implementation for each invocation.
 * The tracer can be force set in the options via "engine.force_tracer"
 * @param traceFunction The function to trace.
 * @param options Engine options for configuration.
 * @return unique_ptr to ExecutionTrace containing the complete trace.
 */
std::unique_ptr<ExecutionTrace> trace(std::function<void()>& traceFunction,
                                      const engine::Options& options = engine::Options());

class ExecutionTrace;
class SymbolicExecutionState;
struct StaticVarHolder {
	explicit StaticVarHolder(size_t* ptr) : ptr(ptr) {
	}

private:
	const size_t* ptr;
	friend uint64_t hashStaticVector(const std::vector<StaticVarHolder>& data);
};

/**
 * @brief Efficiently tracks reference counts and computes an incremental hash of alive variables.
 *
 * This class maintains reference counts for up to 65536 variables and computes a hash that reflects
 * both which variables are alive and their reference counts. The hash is updated incrementally in O(1)
 * time on each increment/decrement operation, avoiding the need to recompute the entire hash.
 *
 * Implementation details:
 * - Uses XOR-based hashing for O(1) incremental updates
 * - Each variable ID is mixed with a constant multiplier for better hash distribution
 * - The hash incorporates both variable identity (ID) and reference count
 * - Memory footprint: 256KB (only counts array, no precomputed hash table)
 *
 * Performance characteristics:
 * - increment(): O(1) - two XOR operations, two multiplications
 * - decrement(): O(1) - two XOR operations, two multiplications
 * - hash(): O(1) - returns cached value
 *
 * @note This replaces the previous vector-based approach which required O(n) hashing on snapshot.
 */
class AliveVariableHash {
	static constexpr size_t N = 1u << 16;
	static constexpr uint64_t HASH_MULTIPLIER = 0x9e3779b97f4a7c15; // Golden ratio constant for good mixing

	std::array<uint32_t, N> counts {};
	uint64_t alive_hash = 0;

public:
	/**
	 * @brief Default constructor. No initialization needed as counts are zero-initialized.
	 */
	AliveVariableHash() = default;

	/**
	 * @brief Increments the reference count for a variable and updates the hash.
	 *
	 * The hash is updated by XOR-ing out the old contribution ((id * HASH_MULTIPLIER) * old_count)
	 * and XOR-ing in the new contribution ((id * HASH_MULTIPLIER) * new_count).
	 *
	 * @param id Variable identifier (16-bit value)
	 */
	inline void increment(uint16_t id) noexcept {
		uint32_t& c = counts[id];
		alive_hash ^= (id * HASH_MULTIPLIER) * c;
		++c;
		alive_hash ^= (id * HASH_MULTIPLIER) * c;
	}

	/**
	 * @brief Decrements the reference count for a variable and updates the hash.
	 *
	 * The hash is updated by XOR-ing out the old contribution ((id * HASH_MULTIPLIER) * old_count)
	 * and XOR-ing in the new contribution ((id * HASH_MULTIPLIER) * new_count).
	 *
	 * @param id Variable identifier (16-bit value)
	 */
	inline void decrement(uint16_t id) noexcept {
		uint32_t& c = counts[id];
		alive_hash ^= (id * HASH_MULTIPLIER) * c;
		--c;
		alive_hash ^= (id * HASH_MULTIPLIER) * c;
	}

	/**
	 * @brief Returns the current hash value representing the state of alive variables.
	 *
	 * The hash reflects both which variables have non-zero reference counts and the
	 * magnitude of those counts. This value is maintained incrementally and can be
	 * retrieved in O(1) time.
	 *
	 * @return 64-bit hash value representing current variable state
	 */
	inline uint64_t hash() const noexcept {
		return alive_hash;
	}

	/**
	 * @brief Resets all reference counts and hash to initial state.
	 *
	 * This efficiently clears all counts without creating a temporary object.
	 * Optimized: if hash is already 0, we assume counts are already 0 and skip the fill.
	 */
	inline void reset() noexcept {
		if (alive_hash != 0) {
			counts.fill(0);
			alive_hash = 0;
		}
	}
};

/**
 * @brief State that requires initialization for tracing operations.
 * This is allocated when tracing begins and freed when it ends.
 * Holds references to stack-allocated objects.
 */
struct TraceState {
	TagRecorder& tagRecorder;
	Trace& executionTrace;
	const engine::Options& options;
	std::unordered_map<void*, uint32_t> normalizedFunctionNameCache; // Maps function pointers to normalized indices
	uint32_t nextNormalizedFunctionIndex = 0;                        // Counter for normalized function names

	TraceState(TagRecorder& tr, Trace& tt, const engine::Options& opts);
};

/**
 * @brief The trace context manages a thread local instance to record a trace of a given Nautilus function.
 *
 * TraceContext itself is an abstract class, as there are multiple tracer implementations.
 * Subclasses mainly need to specify the control flow handling
 *
 * Design Philosophy:
 * - TraceContext is a simple thread_local object (not a pointer) - zero heap allocation
 * - Trace (shared memory, no heap allocs) is allocated on the stack in trace()
 * - TraceState holds references to these stack objects and is created during initialization
 * - staticVars and aliveVars are persistent members that get reset between trace iterations
 *
 * Lifecycle:
 * 1. trace() allocates Trace on its stack
 * 2. initialize() creates TraceState with references to these stack objects
 * 3. Multiple trace iterations execute, calling resume() to reset persistent state
 * 4. After tracing completes, Trace is converted to ExecutionTrace and returned
 */
class TraceContext {
public:
	/**
	 * @brief Main tracing entry point - allocates all objects on stack and executes tracing.
	 * @param traceFunction The function to trace.
	 * @param options Engine options for configuration.
	 * @return unique_ptr to ExecutionTrace containing the complete trace.
	 */
	virtual std::unique_ptr<ExecutionTrace> trace(std::function<void()>& traceFunction,
	                                              const engine::Options& options) = 0;

	/**
	 * @brief Get a thread local reference to the active trace context.
	 * If the trace context is not initialized (state == nullptr) this function returns nullptr.
	 * @return TraceContext* pointer to thread_local TraceContext if initialized, nullptr otherwise.
	 */
	static TraceContext* get();

	/**
	 * @brief Register a single argument for the function to be traced.
	 * @param type the argument type.
	 * @param index the index in the argument list (important if the same arg is registered multiple times by re-running
	 * the target function).
	 */
	virtual TypedValueRef registerFunctionArgument(Type type, size_t index) = 0;

	virtual void traceValueDestruction(TypedValueRef target);

	/**
	 * @brief Trace a constant operation.
	 * @param type the type of the value.
	 * @param constValue constant value.
	 */
	virtual TypedValueRef traceConstValue(Type type, const ConstantLiteral& constValue);

	/**
	 * @brief Trace a unary operation, e.g., negate.
	 * @param op operation code.
	 * @param inputRef reference to the input.
	 */
	virtual TypedValueRef traceOperation(Op op, Type resultType, std::vector<TypedValueRef> inputRef);

	/**
	 * @brief Trace a copy of another value.
	 * @param ref reference to the source value.
	 */
	virtual TypedValueRef traceCopy(const TypedValueRef& ref);

	/**
	 * @brief Trace the return function.
	 * @param ref referent to the return value.
	 */
	virtual void traceReturnOperation(Type type, const TypedValueRef& ref) = 0;

	/**
	 * @brief Trace a value assignment.
	 * @param targetRef reference to the target value.
	 * @param sourceRef reference to the source value.
	 */
	virtual void traceAssignment(const TypedValueRef& targetRef, const TypedValueRef& sourceRef, Type resultType);

	/**
	 * @brief Trace a call operation.
	 * @param fptn runtime address pointer of the called function.
	 * @param resultType Return type of the called function.
	 * @param arguments Argument references passed as arguments to the call.
	 * @param fnAttrs function attributes for the compiler.
	 */
	virtual TypedValueRef traceCall(void* fptn, Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
	                                FunctionAttributes fnAttrs);

	/**
	 * @brief trace a compare operation (control flow).
	 * @param targetRef the boolean value to check.
	 * @param probability probability of the value being true (for branch prediction optimization).
	 * @return the boolean result to evaluate to during the execution.
	 */
	virtual bool traceCmp(const TypedValueRef& targetRef, double probability) = 0;

	virtual ~TraceContext() = default;

	/**
	 * @brief Resets persistent state between trace iterations.
	 * Clears staticVars and resets aliveVars hash/counts.
	 * Does NOT reset state (executionTrace/symbolicExecutionContext) - they persist across iterations.
	 */
	void resume();

	/**
	 * resets the state af the active trace context
	 * run after tracing a function (NOT after every iteration)
	 */
	virtual void resetState();

	/**
	 * @brief Initialize the trace context with references to stack-allocated objects.
	 * @param tagRecorder Reference to TagRecorder for creating unique tags
	 * @param tracerTrace Reference to stack-allocated Trace
	 * @param options Reference to engine options for configuration
	 * @return Pointer to initialized thread_local TraceContext
	 */
	TraceContext* initialize(TagRecorder& tagRecorder, Trace& tracerTrace, const engine::Options& options);

	std::vector<StaticVarHolder>& getStaticVars();
	void allocateValRef(ValueRef ref);
	void freeValRef(ValueRef ref);

	virtual void suggestInvertedBranch();

	/**
	 * @brief Default constructor - public to allow thread_local storage.
	 * Initializes with empty state (state == nullptr means not initialized).
	 */
	TraceContext() = default;

protected:
	/**
	 * @brief Generic operation trace function to be implemented by a concrete trace context.
	 * @param op opcode of the operation.
	 * @param onCreation generator function for creating the operation object.
	 */
	virtual TypedValueRef traceOperation(
	    Op op,
	    const std::function<std::pair<TypedValueRef, Trace::TracerOperationIdentifier>(Snapshot&)>& onCreation) = 0;

	Snapshot recordSnapshot();

	// Injected state - holds references to stack-allocated objects (ExecutionTrace, SymbolicExecutionContext)
	// nullptr when not tracing, set during initialize()
	std::unique_ptr<TraceState> state;

	// Persistent state - reset between trace iterations via resume()
	std::vector<StaticVarHolder> staticVars; // Tracks static variable states for snapshot hashing
	AliveVariableHash aliveVars;             // Tracks alive variables with incremental hash (256KB)
};

} // namespace nautilus::tracing
