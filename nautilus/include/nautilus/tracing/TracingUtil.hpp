
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

TypedValueRef traceBinaryOp(Op operation, Type resultType, const TypedValueRef& leftState, const TypedValueRef& rightState);
TypedValueRef traceUnaryOp(Op operation, Type resultType, const TypedValueRef& inputState);

bool traceBool(const TypedValueRef& state);
TypedValueRef traceConstant(Type type, const ConstantLiteral& value);
template <typename T>
TypedValueRef traceConstant(T&& value) {
	if (inTracer()) {
		return traceConstant(to_type<T>(), createConstLiteral(value));
	}
	return {0, to_type<T>()};
}

TypedValueRef traceLoad(const TypedValueRef& src, Type resultType);
void traceStore(const TypedValueRef& target, const TypedValueRef& src, Type valueType);

TypedValueRef traceCast(const TypedValueRef& state, Type resultType);
void traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType);
TypedValueRef traceCopy(const TypedValueRef& state);

TypedValueRef traceCall(void* fptn, Type resultType, const std::vector<tracing::TypedValueRef>& arguments);

TypedValueRef registerFunctionArgument(Type type, size_t index);

void traceReturnOperation(Type type, const TypedValueRef& ref);
void traceValueDestruction(TypedValueRef ref);

void pushStaticVal(void* ptr);
void popStaticVal();
void allocateValRef(ValueRef ref);
void freeValRef(ValueRef ref);
} // namespace nautilus::tracing
