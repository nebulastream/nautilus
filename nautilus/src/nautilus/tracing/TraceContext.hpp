
#pragma once

#include "ExecutionTrace.hpp"
#include "TraceOperation.hpp"
#include "tag/Tag.hpp"
#include "tag/TagRecorder.hpp"
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

using DynamicValueMap = std::vector<uint8_t>;

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

	TypedValueRef registerFunctionArgument(Type type, size_t index);

	void traceValueDestruction(TypedValueRef target);

	/**
	 * @brief Trace a constant operation.
	 * @param valueReference reference to the const value.
	 * @param constValue constant value.
	 */
	TypedValueRef traceConstValue(Type type, const ConstantLiteral& constValue);

	TypedValueRef traceCopy(const TypedValueRef& ref);

	Tag* getTag();

	/**
	 * @brief Trace a unary operation, e.g., negate.
	 * @param op operation code.
	 * @param inputRef reference to the input.
	 * @param resultRef reference to the result.
	 */
	TypedValueRef traceUnaryOperation(Op op, Type resultType, const TypedValueRef& inputRef);

	/**
	 * @brief Trace a binary operation, e.g., add, sub, div.
	 * @param op operation code-
	 * @param leftRef reference to the left input.
	 * @param rightRef reference to the right input.
	 * @param resultRef reference to the result.
	 */
	TypedValueRef traceBinaryOperation(Op op, Type resultType, const TypedValueRef& leftRef, const TypedValueRef& rightRef);

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

	TypedValueRef traceCall(const std::string& functionName, const std::string& mangledName, void* fptn, Type resultType, const std::vector<tracing::TypedValueRef>& arguments);

	bool traceCmp(const TypedValueRef& targetRef);

	TypedValueRef traceLoad(const TypedValueRef& src, Type resultType);

	TypedValueRef traceCast(const TypedValueRef& state, Type resultType);

	void traceStore(const TypedValueRef& target, const TypedValueRef& src, Type valueType);

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

	Snapshot recordSnapshot();

	TagRecorder& tagRecorder;
	std::unique_ptr<ExecutionTrace> executionTrace;
	std::unique_ptr<SymbolicExecutionContext> symbolicExecutionContext;
	std::vector<StaticVarHolder> staticVars;
	DynamicValueMap dynamicVars;
};

} // namespace nautilus::tracing
