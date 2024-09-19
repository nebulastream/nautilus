
#pragma once

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

bool inTracer();

value_ref traceBinaryOp(Op operation, Type resultType, const TypedValueRef& leftState, const TypedValueRef& rightState);
value_ref traceUnaryOp(Op operation, Type resultType, const TypedValueRef& inputState);

bool traceBool(const TypedValueRef& state);

void traceStore(const TypedValueRef& target, const TypedValueRef& src, Type valueType);

value_ref traceLoad(const TypedValueRef& src, Type resultType);

value_ref traceCopy(const TypedValueRef& state);

value_ref registerFunctionArgument(Type type, size_t index);

void traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType);

value_ref traceConstant(Type type, const ConstantLiteral& value);

template <typename T>
value_ref traceConstant(T value) {
	if (!inTracer())
		return {0, to_type<T>()};
	return traceConstant(to_type<T>(), createConstLiteral(value));
}

void traceReturnOperation(Type type, const value_ref& ref);

void traceValueDestruction(value_ref ref);

value_ref traceCast(const value_ref& state, Type resultType);

value_ref traceCall(void* fptn, Type resultType, const std::vector<tracing::value_ref>& arguments);

void pushStaticVal(void* ptr);
void popStaticVal();
void allocateValRef(ValueRef ref);
void freeValRef(ValueRef ref);
} // namespace nautilus::tracing
