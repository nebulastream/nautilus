
#pragma once

#include "ExecutionTrace.hpp"
#include "TraceOperation.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/options.hpp"
#include "nautilus/tracing/TracingInterface.hpp"
#include "tag/Tag.hpp"
#include "tag/TagRecorder.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace nautilus {
class NautilusFunctionDefinition;
}

namespace nautilus::tracing {
class ExecutionTrace;
class SymbolicExecutionContext;
class TraceModule;
struct StaticVarHolder {
	explicit StaticVarHolder(const void* ptr, size_t size) : ptr(ptr), size(size) {
	}

private:
	const void* ptr;
	size_t size;
	friend uint64_t hashStaticVector(const std::vector<StaticVarHolder>& data);
	friend size_t getStaticVarValue(const StaticVarHolder& holder);
};

inline size_t getStaticVarValue(const StaticVarHolder& holder) {
	size_t result = 0;
	std::memcpy(&result, holder.ptr, holder.size);
	return result;
}

/**
 * @brief Efficiently tracks reference counts and computes an incremental hash of alive variables.
 *
 * ValueRefs are dense small integers (ExecutionTrace::getNextValueRef() is a plain incrementing
 * counter), so reference counts are stored in a vector indexed by id rather than a hash map.
 * The hash reflects both which variables are alive and their reference counts, updated
 * incrementally in O(1) time.
 *
 * Implementation details:
 * - Uses XOR-based hashing for O(1) incremental updates
 * - Each variable ID is mixed with a constant multiplier for better hash distribution
 * - The hash incorporates both variable identity (ID) and reference count
 * - Uses a growable vector indexed by id - no allocation on increment/decrement beyond
 *   the amortized growth needed to cover the highest id seen so far
 * - A separate alive-count is maintained so size() stays O(1) even though zero-count
 *   entries are not removed from the vector
 *
 * Performance characteristics:
 * - increment(): O(1) amortized - vector index + two XOR operations, two multiplications
 * - decrement(): O(1) - vector index + two XOR operations, two multiplications
 * - hash(): O(1) - returns cached value
 * - size(): O(1) - returns cached alive count
 *
 * @note Changed from a hash map (which erased entries to bound its size) to a vector indexed
 * by ValueRef. This trades peak-alive-sized memory for maxRef-sized memory in exchange for
 * removing the malloc/free pair that the map's insert/erase pair cost per traced value.
 */
class AliveVariableHash {
	static constexpr uint64_t HASH_MULTIPLIER = 0x9e3779b97f4a7c15; // Golden ratio constant for good mixing

	std::vector<uint32_t> counts;
	size_t aliveCount = 0;
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
	 * @param id Variable identifier (32-bit value)
	 */
	inline void increment(uint32_t id) noexcept {
		if (id >= counts.size()) {
			counts.resize(id + 1, 0);
		}
		uint32_t& c = counts[id];
		alive_hash ^= (id * HASH_MULTIPLIER) * c;
		if (c == 0) {
			++aliveCount;
		}
		++c;
		alive_hash ^= (id * HASH_MULTIPLIER) * c;
	}

	/**
	 * @brief Decrements the reference count for a variable and updates the hash.
	 *
	 * The hash is updated by XOR-ing out the old contribution ((id * HASH_MULTIPLIER) * old_count)
	 * and XOR-ing in the new contribution ((id * HASH_MULTIPLIER) * new_count).
	 *
	 * @param id Variable identifier (32-bit value), previously passed to increment()
	 */
	inline void decrement(uint32_t id) noexcept {
		assert(id < counts.size() && counts[id] > 0 && "decrement() on a variable that is not alive");
		uint32_t& c = counts[id];
		alive_hash ^= (id * HASH_MULTIPLIER) * c;
		--c;
		alive_hash ^= (id * HASH_MULTIPLIER) * c;
		if (c == 0) {
			--aliveCount;
		}
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
	 * @brief Returns the number of currently-alive variables (non-zero reference count).
	 * @return Number of currently-alive variables
	 */
	inline size_t size() const noexcept {
		return aliveCount;
	}

	/**
	 * @brief Resets all reference counts and hash to initial state.
	 *
	 * Zeroes every slot in the backing vector rather than shrinking it, so the vector's
	 * capacity - and thus the highest id it can hold without reallocating - is retained
	 * across trace iterations.
	 */
	inline void reset() noexcept {
		std::fill(counts.begin(), counts.end(), 0);
		aliveCount = 0;
		alive_hash = 0;
	}
};

/**
 * @brief State that requires initialization for tracing operations.
 * This is initialized in the trace context when tracing begins and reset when it ends.
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
 * @brief Common base class for trace context implementations.
 *
 * Holds shared state (TraceState, mangledNameCache) and provides
 * getMangledName() / getFunctionName() so they are not duplicated
 * across ExceptionBasedTraceContext and LazyTraceContext.
 */
class TraceContextBase : public TracingInterface {
public:
	~TraceContextBase() override = default;

	std::string getMangledName(void* fnptr);
	std::string getFunctionName(void* fnptr, const std::string& mangledName);
	void registerDestructor(const TypedValueRef& address, void* destructor) override;
	void unregisterDestructor(const TypedValueRef& address) override;

protected:
	// Injected state - holds references to stack-allocated objects (ExecutionTrace, SymbolicExecutionContext).
	// Empty when not tracing and stored inline to avoid a per-trace heap allocation.
	std::optional<TraceState> state;

	std::unordered_map<void*, std::string> mangledNameCache;
	std::vector<FunctionCall::Destructor> activeDestructors;
};

/**
 * @brief The trace context manages a thread local instance to record a symbolic execution trace of a given Nautilus
 * function.
 *
 * Design Philosophy:
 * - ExceptionBasedTraceContext is a simple thread_local object (not a pointer) - zero heap allocation
 * - ExecutionTrace and SymbolicExecutionContext are allocated on the stack in trace()
 * - TraceState holds references to these stack objects and is created during initialization
 * - staticVars and aliveVars are persistent members that get reset between trace iterations
 * - Inherits from TraceContextBase so different implementations can be swapped per trace via setActiveTracer().
 *
 * Lifecycle:
 * 1. trace() allocates ExecutionTrace and SymbolicExecutionContext on its stack
 * 2. initialize() creates TraceState with references to these stack objects and registers itself
 *    as the active tracer via setActiveTracer(this)
 * 3. Multiple trace iterations execute, calling resume() to reset persistent state
 * 4. After tracing completes, setActiveTracer(nullptr) is called and ExecutionTrace is returned
 */
class ExceptionBasedTraceContext final : public TraceContextBase {
public:
	// --- TracingInterface overrides ---

	TypedValueRef& registerFunctionArgument(Type type, size_t index) override;

	TypedValueRef& traceConstant(Type type, const ConstantLiteral& value) override;

	/**
	 * @brief Main tracing entry point - allocates all objects on stack and executes symbolic tracing.
	 * @param functionsToTrace List of functions to trace
	 * @param options Engine options for configuration
	 * @param arena Arena used to allocate Blocks/TraceOperations for every
	 *              ExecutionTrace in the returned TraceModule; must outlive
	 *              the returned module.
	 * @return unique_ptr to TraceModule containing all function traces
	 */
	std::unique_ptr<TraceModule> startTrace(std::list<compiler::CompilableFunction>& functionsToTrace,
	                                        const engine::Options& options, Arena& arena);
	static std::unique_ptr<TraceModule> Trace(std::list<compiler::CompilableFunction>& functionsToTrace,
	                                          const engine::Options& options, Arena& arena);

	TypedValueRef& traceCopy(const TypedValueRef& ref) override;

	TypedValueRef& traceBinaryOp(Op op, Type resultType, const TypedValueRef& left,
	                             const TypedValueRef& right) override;

	TypedValueRef& traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) override;

	TypedValueRef& traceTernaryOp(Op op, Type resultType, const TypedValueRef& first, const TypedValueRef& second,
	                              const TypedValueRef& third) override;
	TypedValueRef& traceAlloca(size_t size, size_t align) override;
	void traceReturnOperation(Type type, const TypedValueRef& ref) override;

	void traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) override;

	TypedValueRef& traceCall(void* fptn, Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
	                         FunctionAttributes fnAttrs) override;
	TypedValueRef& traceCallWithExceptionHandling(void* fptn, Type resultType,
	                                              const std::vector<tracing::TypedValueRef>& arguments,
	                                              FunctionAttributes fnAttrs) override;

	TypedValueRef& traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
	                                 const std::vector<tracing::TypedValueRef>& arguments,
	                                 FunctionAttributes fnAttrs) override;

	bool traceBool(const TypedValueRef& value, double probability) override;

	void allocateValRef(ValueRef ref) override;
	void freeValRef(ValueRef ref) override;

	TypedValueRef& traceNautilusCall(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper,
	                                 Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
	                                 FunctionAttributes fnAttrs) override;

	TypedValueRef& traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
	                                        std::function<void()> fwrapper) override;

	void pushStaticVal(void* ptr, size_t size) override;
	void popStaticVal() override;

	// --- Non-interface public API ---

	~ExceptionBasedTraceContext() override = default;

	/**
	 * @brief Resets persistent state between trace iterations.
	 * Clears staticVars and resets aliveVars hash/counts.
	 * Does NOT reset state (executionTrace/symbolicExecutionContext) - they persist across iterations.
	 */
	void resume();

	/**
	 * @brief Initialize the trace context with references to stack-allocated objects.
	 * Sets this context as the active tracer via setActiveTracer().
	 * @param tagRecorder Reference to TagRecorder for creating unique tags
	 * @param executionTrace Reference to stack-allocated ExecutionTrace
	 * @param symbolicExecutionContext Reference to stack-allocated SymbolicExecutionContext
	 * @param options Reference to engine options for configuration
	 * @return Pointer to initialized thread_local ExceptionBasedTraceContext
	 */
	static ExceptionBasedTraceContext* initialize(TagRecorder& tagRecorder, ExecutionTrace& executionTrace,
	                                              SymbolicExecutionContext& symbolicExecutionContext,
	                                              const engine::Options& options);

	/**
	 * @brief Main tracing entry point - executes symbolic tracing of the
	 * supplied function.
	 * @param traceFunction The function to trace.
	 * @param options Engine options for configuration.
	 * @param arena Arena used to allocate the trace's Blocks and
	 *              TraceOperations; must outlive the returned trace.
	 * @return unique_ptr to ExecutionTrace containing the complete trace.
	 */
	static std::unique_ptr<ExecutionTrace> trace(std::function<void()>& traceFunction, const engine::Options& options,
	                                             Arena& arena);

	/**
	 * @brief Default constructor - public to allow thread_local storage.
	 * Initializes with empty state (state == nullptr means not initialized).
	 */
	ExceptionBasedTraceContext() = default;

	bool isActive() const;

private:
	bool isFollowing();
	TypedValueRef& follow(Op op);
	template <typename OnCreation>
	TypedValueRef& traceOperation(Op op, OnCreation&& onCreation);
	Snapshot recordSnapshot();
	std::string formatStaticVars() const;

	// Persistent state - reset between trace iterations via resume()
	std::vector<StaticVarHolder> staticVars; // Tracks static variable states for snapshot hashing
	AliveVariableHash aliveVars;             // Tracks alive variables with incremental hash
	std::list<compiler::CompilableFunction> functionsToTrace = std::list<compiler::CompilableFunction> {};
	std::unordered_set<std::string> registeredFunctions;
};

} // namespace nautilus::tracing
