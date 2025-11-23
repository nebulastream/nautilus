
#pragma once

#include "ExecutionTrace.hpp"
#include "TraceOperation.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/options.hpp"
#include "tag/Tag.hpp"
#include "tag/TagRecorder.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <memory>

namespace nautilus::tracing {
class ExecutionTrace;
class SymbolicExecutionContext;
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
	ExecutionTrace& executionTrace;
	SymbolicExecutionContext& symbolicExecutionContext;
	const engine::Options& options;
	std::unordered_map<void*, uint32_t> normalizedFunctionNameCache; // Maps function pointers to normalized indices
	uint32_t nextNormalizedFunctionIndex = 0;                        // Counter for normalized function names

	TraceState(TagRecorder& tr, ExecutionTrace& et, SymbolicExecutionContext& sec, const engine::Options& opts);
};

/**
 * @brief The trace context manages a thread local instance to record a symbolic execution trace of a given Nautilus
 * function.
 *
 * Design Philosophy:
 * - TraceContext is a simple thread_local object (not a pointer) - zero heap allocation
 * - ExecutionTrace and SymbolicExecutionContext are allocated on the stack in trace()
 * - TraceState holds references to these stack objects and is created during initialization
 * - staticVars and aliveVars are persistent members that get reset between trace iterations
 *
 * Lifecycle:
 * 1. trace() allocates ExecutionTrace and SymbolicExecutionContext on its stack
 * 2. initialize() creates TraceState with references to these stack objects
 * 3. Multiple trace iterations execute, calling resume() to reset persistent state
 * 4. After tracing completes, state is reset and ExecutionTrace is moved into unique_ptr for return
 */
class TraceContext {
public:
	/**
	 * @brief Get a thread local reference to the trace context.
	 * If the trace context is not initialized (state == nullptr) this function returns nullptr.
	 * @return TraceContext* pointer to thread_local TraceContext if initialized, nullptr otherwise
	 */
	static TraceContext* get();

	static TraceContext* getIfActive();

	static bool shouldTrace();

	TypedValueRef& registerFunctionArgument(Type type, size_t index);

	void traceValueDestruction(TypedValueRef target);

	/**
	 * @brief Trace a constant operation.
	 * @param valueReference reference to the const value.
	 * @param constValue constant value.
	 */
	TypedValueRef& traceConstValue(Type type, const ConstantLiteral& constValue);

	TypedValueRef& traceCopy(const TypedValueRef& ref);

	/**
	 * @brief Trace a unary operation, e.g., negate.
	 * @param op operation code.
	 * @param inputRef reference to the input.
	 * @param resultRef reference to the result.
	 */
	TypedValueRef& traceOperation(Op op, Type resultType, std::initializer_list<InputVariant> inputRef);

	/**
	 * @brief Trace the return function.
	 * @param resultRef referent to the return value.
	 */
	void traceReturnOperation(Type type, const TypedValueRef& ref);

	/**
	 * @brief Trace a value assignment.
	 * @param targetRef reference to the target value.
	 * @param sourceRef reference to the source value.
	 */
	void traceAssignment(const TypedValueRef& targetRef, const TypedValueRef& sourceRef, Type resultType);

	TypedValueRef& traceCall(void* fptn, Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
	                         FunctionAttributes fnAttrs);

	bool traceCmp(const TypedValueRef& targetRef, double probability);

	~TraceContext() = default;

	/**
	 * @brief Resets persistent state between trace iterations.
	 * Clears staticVars and resets aliveVars hash/counts.
	 * Does NOT reset state (executionTrace/symbolicExecutionContext) - they persist across iterations.
	 */
	void resume();

	/**
	 * @brief Initialize the trace context with references to stack-allocated objects.
	 * @param tagRecorder Reference to TagRecorder for creating unique tags
	 * @param executionTrace Reference to stack-allocated ExecutionTrace
	 * @param symbolicExecutionContext Reference to stack-allocated SymbolicExecutionContext
	 * @param options Reference to engine options for configuration
	 * @return Pointer to initialized thread_local TraceContext
	 */
	static TraceContext* initialize(TagRecorder& tagRecorder, ExecutionTrace& executionTrace,
	                                SymbolicExecutionContext& symbolicExecutionContext, const engine::Options& options);

	/**
	 * @brief Main tracing entry point - allocates all objects on stack and executes symbolic tracing.
	 * @param traceFunction The function to trace
	 * @param options Engine options for configuration
	 * @return unique_ptr to ExecutionTrace containing the complete trace
	 */
	static std::unique_ptr<ExecutionTrace> trace(std::function<void()>& traceFunction,
	                                             const engine::Options& options = engine::Options());

	std::vector<StaticVarHolder>& getStaticVars();
	void allocateValRef(ValueRef ref);
	void freeValRef(ValueRef ref);

	std::string getMangledName(void* fnptr);
	std::string getFunctionName(void* fnptr, const std::string& mangledNamed);

	/**
	 * @brief Default constructor - public to allow thread_local storage.
	 * Initializes with empty state (state == nullptr means not initialized).
	 */
	TraceContext() = default;

private:
	bool isFollowing();
	TypedValueRef& follow(Op op);
	template <typename OnCreation>
	TypedValueRef& traceOperation(Op op, OnCreation&& onCreation);
	Snapshot recordSnapshot();

	// Injected state - holds references to stack-allocated objects (ExecutionTrace, SymbolicExecutionContext)
	// nullptr when not tracing, set during initialize()
	std::unique_ptr<TraceState> state;

	// Persistent state - reset between trace iterations via resume()
	std::vector<StaticVarHolder> staticVars; // Tracks static variable states for snapshot hashing
	AliveVariableHash aliveVars;             // Tracks alive variables with incremental hash (256KB)
	std::unordered_map<void*, std::string> mangledNameCache;
};

} // namespace nautilus::tracing
