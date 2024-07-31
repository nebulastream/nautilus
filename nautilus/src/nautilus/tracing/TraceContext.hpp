
#pragma once

#include "ExecutionTrace.hpp"
#include "TraceOperation.hpp"
#include "tag/Tag.hpp"
#include "tag/TagRecorder.hpp"
#include <cstdint>
#include <functional>
#include <memory>

namespace nautilus::tracing {
class OperationRef;

class ExecutionTrace;

class SymbolicExecutionContext;

struct StaticVarHolder {
	explicit StaticVarHolder(size_t* ptr) : ptr(ptr) {
	}

private:
	const size_t* ptr;

	friend uint64_t hashStaticVector(const std::vector<StaticVarHolder>& data);
};

constexpr size_t fnv_prime = 0x100000001b3;
constexpr size_t offset_basis = 0xcbf29ce484222325;

inline uint64_t hashStaticVector(const std::vector<StaticVarHolder>& data) {
	size_t hash = offset_basis;
	for (auto& value : data) {
		hash ^= *value.ptr;
		hash *= fnv_prime;
	}
	return hash;
}

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

	value_ref registerFunctionArgument(Type type, size_t index);

	void traceValueDestruction(value_ref target);

	/**
	 * @brief Trace a constant operation.
	 * @param valueReference reference to the const value.
	 * @param constValue constant value.
	 */
	value_ref traceConstValue(Type type, std::any constValue);

	value_ref traceCopy(value_ref ref);

	Tag* getTag();

	/**
	 * @brief Trace a unary operation, e.g., negate.
	 * @param op operation code.
	 * @param inputRef reference to the input.
	 * @param resultRef reference to the result.
	 */
	value_ref traceUnaryOperation(Op op, Type resultType, value_ref& inputRef);

	/**
	 * @brief Trace a binary operation, e.g., add, sub, div.
	 * @param op operation code-
	 * @param leftRef reference to the left input.
	 * @param rightRef reference to the right input.
	 * @param resultRef reference to the result.
	 */
	value_ref traceBinaryOperation(Op op, Type resultType, value_ref& leftRef, value_ref& rightRef);

	/**
	 * @brief Trace the return function.
	 * @param resultRef referent to the return value.
	 */
	void traceReturnOperation(Type type, value_ref ref);

	/**
	 * @brief Trace a value assignment.
	 * @param targetRef reference to the target value.
	 * @param sourceRef reference to the source value.
	 */
	void traceAssignment(value_ref targetRef, value_ref sourceRef, Type resultType);

	value_ref traceCall(const std::string& functionName, void* fptn, Type resultType,
	                    std::vector<tracing::value_ref> arguments);

	bool traceCmp(value_ref targetRef);

	value_ref traceLoad(value_ref src, Type resultType);

	value_ref traceCast(value_ref state, Type resultType);

	void traceStore(value_ref target, value_ref src, Type valueType);

	~TraceContext() = default;

	void pause() {
		active = false;
	}

	void resume() {
		staticVars.clear();
	}

	static TraceContext* initialize(TagRecorder& tagRecorder);

	static std::unique_ptr<ExecutionTrace> trace(std::function<void()>& traceFunction);

	std::vector<StaticVarHolder>& getStaticVars();

private:
	explicit TraceContext(TagRecorder& tagRecorder);

	static void terminate();

	bool isFollowing();

	Snapshot recordSnapshot();

	bool active = false;
	TagRecorder& tagRecorder;
	std::unique_ptr<ExecutionTrace> executionTrace;
	std::unique_ptr<SymbolicExecutionContext> symbolicExecutionContext;
	std::vector<StaticVarHolder> staticVars;
};

} // namespace nautilus::tracing
