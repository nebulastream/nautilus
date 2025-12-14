
#pragma once

#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/tracing/Operations.hpp"
#include "nautilus/tracing/TypedValueRef.hpp"
#include "nautilus/tracing/Types.hpp"
#include "nautilus/val_concepts.hpp"
#include <any>
#include <array>
#include <cstdint>
#include <iosfwd>
#include <vector>

namespace nautilus::tracing {

// Forward declaration
class TraceContext;

bool inTracer();

// Get the current tracer context pointer (returns nullptr if not tracing)
// This can be cached locally to avoid repeated thread-local access
TraceContext* getTracerIfActive();

TypedValueRef& traceBinaryOp(Op operation, Type resultType, const TypedValueRef& leftState,
                             const TypedValueRef& rightState);
TypedValueRef& traceUnaryOp(Op operation, Type resultType, const TypedValueRef& inputState);

bool traceBool(const TypedValueRef& state);
TypedValueRef& traceConstant(Type type, const ConstantLiteral& value);
template <typename T>
TypedValueRef traceConstant(T&& value) {
	if (auto* ctx = getTracerIfActive()) {
		return traceConstant(TypeResolver<T>::to_type(), createConstLiteral(value));
	}
	return {0, TypeResolver<T>::to_type()};
}

void traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType);
TypedValueRef traceCopy(const TypedValueRef& state);

TypedValueRef& traceCall(void* fptn, Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
                         FunctionAttributes fnAttrs);

TypedValueRef& registerFunctionArgument(Type type, size_t index);

void traceReturnOperation(Type type, const TypedValueRef& ref);
void traceValueDestruction(TypedValueRef ref);

void pushStaticVal(void* ptr);
void popStaticVal();
void allocateValRef(ValueRef ref);
void freeValRef(ValueRef ref);
} // namespace nautilus::tracing
