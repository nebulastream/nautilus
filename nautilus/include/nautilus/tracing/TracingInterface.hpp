
#pragma once

#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/tracing/Operations.hpp"
#include "nautilus/tracing/TypedValueRef.hpp"
#include "nautilus/tracing/Types.hpp"
#include <cstddef>
#include <vector>

namespace nautilus::tracing {

/**
 * @brief Abstract interface for tracing operations.
 *
 * This interface decouples the val<T> tracing layer (TracingUtil) from any concrete
 * tracing implementation. The active implementation is set once per trace and stored
 * as a thread-local pointer, so all operations within a single trace go through the
 * same implementation while different traces can use different ones.
 *
 * Concrete implementations (e.g. TraceContext) are set via setActiveTracer() before
 * executing a traced function and cleared afterwards.
 */
class TracingInterface {
public:
	virtual ~TracingInterface() = default;

	/// Register a function argument at the given index.
	virtual TypedValueRef& registerFunctionArgument(Type type, size_t index) = 0;

	/// Trace a constant value of the given type.
	virtual TypedValueRef& traceConstant(Type type, const ConstantLiteral& value) = 0;

	/// Trace a copy of an existing traced value.
	virtual TypedValueRef& traceCopy(const TypedValueRef& ref) = 0;

	/// Trace a binary operation (e.g. ADD, MUL, EQ, ...).
	virtual TypedValueRef& traceBinaryOp(Op op, Type resultType, const TypedValueRef& left,
	                                     const TypedValueRef& right) = 0;

	/// Trace a unary operation (e.g. NEGATE, CAST, ...).
	virtual TypedValueRef& traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) = 0;

	/// Trace a ternary operation (e.g. SELECT).
	virtual TypedValueRef& traceTernaryOp(Op op, Type resultType, const TypedValueRef& first,
	                                      const TypedValueRef& second, const TypedValueRef& third) = 0;

	/// Trace a return statement.
	virtual void traceReturnOperation(Type type, const TypedValueRef& ref) = 0;

	/// Trace a value assignment from source to target.
	virtual void traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) = 0;

	/// Trace a runtime function call.
	virtual TypedValueRef& traceCall(void* fptn, Type resultType, const std::vector<TypedValueRef>& arguments,
	                                 FunctionAttributes fnAttrs) = 0;

	/// Trace a conditional branch. Returns the taken branch direction.
	virtual bool traceBool(const TypedValueRef& value, double probability) = 0;

	/// Increment the reference count of a value.
	virtual void allocateValRef(ValueRef ref) = 0;

	/// Decrement the reference count of a value.
	virtual void freeValRef(ValueRef ref) = 0;

	/// Push a static variable onto the static-variable stack used for snapshot hashing.
	/// @param ptr   Pointer to the raw value inside the static_val.
	/// @param size  sizeof(T) — only this many bytes are valid at ptr.
	virtual void pushStaticVal(void* ptr, size_t size) = 0;

	/// Pop the top static variable from the static-variable stack.
	virtual void popStaticVal() = 0;
};

} // namespace nautilus::tracing
