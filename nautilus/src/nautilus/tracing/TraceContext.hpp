
#pragma once

#include "ExecutionTrace.hpp"
#include "TraceOperation.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
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
};

/**
 * @brief The trace context manages a thread local instance to record a symbolic execution trace of a given Nautilus
 * function. Tracing will be initialized with ether traceFunction or traceFunctionWithReturn.
 */
class TraceContext {
public:
	/**
	 * @brief Get a thread local reference to the trace context.
	 * If the trace context is not initialized this function returns a nullptr.
	 * @return TraceContext*
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

	TypedValueRef& traceCall(const std::string& functionName, const std::string& mangledName, void* fptn,
	                         Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
	                         FunctionAttributes fnAttrs);

	bool traceCmp(const TypedValueRef& targetRef);

	~TraceContext() = default;

	void resume();

	static TraceContext* initialize(TagRecorder& tagRecorder);

	static std::unique_ptr<ExecutionTrace> trace(std::function<void()>& traceFunction);

	std::vector<StaticVarHolder>& getStaticVars();
	void allocateValRef(ValueRef ref);
	void freeValRef(ValueRef ref);

private:
	explicit TraceContext(TagRecorder& tagRecorder);

	static void terminate();

	bool isFollowing();
	TypedValueRef& follow(Op op);
	template <typename OnCreation>
	TypedValueRef& traceOperation(Op op, OnCreation&& onCreation);
	Snapshot recordSnapshot();

	TagRecorder& tagRecorder;
	std::unique_ptr<ExecutionTrace> executionTrace;
	std::unique_ptr<SymbolicExecutionContext> symbolicExecutionContext;
	std::vector<StaticVarHolder> staticVars;
	AliveVariableHash aliveVars;
};

} // namespace nautilus::tracing
