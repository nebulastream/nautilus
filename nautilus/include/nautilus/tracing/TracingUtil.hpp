
#pragma once

#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/tracing/Operations.hpp"
#include "nautilus/tracing/TracingInterface.hpp"
#include "nautilus/tracing/TypedValueRef.hpp"
#include "nautilus/tracing/Types.hpp"
#include "nautilus/val_concepts.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iosfwd>
#include <vector>

namespace nautilus {
class NautilusFunctionDefinition;
}
namespace nautilus::tracing {

bool inTracer();

/// Get the active TracingInterface for the current thread, or nullptr if not tracing.
/// The returned pointer is valid for the lifetime of the current trace invocation.
TracingInterface* getActiveTracer();

/// Set the active TracingInterface for the current thread.
/// Pass nullptr to indicate that tracing is no longer active.
void setActiveTracer(TracingInterface* tracer);

TypedValueRef& traceBinaryOp(Op op, Type resultType, const TypedValueRef& left, const TypedValueRef& right);
TypedValueRef& traceUnaryOp(Op op, Type resultType, const TypedValueRef& input);
TypedValueRef& traceTernaryOp(Op op, Type resultType, const TypedValueRef& first, const TypedValueRef& second,
                              const TypedValueRef& third);

TypedValueRef& traceAlloca(const size_t size);

/// Traces a boolean branch with an associated taken-probability hint.
bool traceBool(const TypedValueRef& value, double probability);
TypedValueRef& traceConstant(Type type, const ConstantLiteral& value);
template <typename T>
TypedValueRef traceConstant(T&& value) {
	if (inTracer()) {
		return traceConstant(TypeResolver<T>::to_type(), createConstLiteral(value));
	}
	return {0, TypeResolver<T>::to_type()};
}

void traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType);
TypedValueRef traceCopy(const TypedValueRef& ref);

TypedValueRef& traceCall(void* fptn, Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
                         FunctionAttributes fnAttrs);

TypedValueRef& traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
                                 const std::vector<tracing::TypedValueRef>& arguments, FunctionAttributes fnAttrs);

TypedValueRef& traceNautilusCall(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper,
                                 Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
                                 FunctionAttributes fnAttrs);

TypedValueRef& traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper);

TypedValueRef& registerFunctionArgument(Type type, size_t index);

void traceReturnOperation(Type type, const TypedValueRef& ref);

void pushStaticVal(void* ptr, size_t size);
void popStaticVal();
void allocateValRef(ValueRef ref);
void freeValRef(ValueRef ref);

// The constant-folding tracer's public API — registry, Stage 2 widening,
// observability counters, runtime flags — used to live in this
// namespace. It moved to `nautilus::tracing::pe` and is declared in
// `<nautilus/partial_evaluation/api.hpp>`. Users enable it via the
// ENABLE_CONSTANT_TRACER CMake option.

} // namespace nautilus::tracing

// The RAII `nautilus::unroll_scope(max_unroll)` guard — the per-site
// counterpart to `setConstantUnrollLimit` above — lives in the
// partial-evaluation plugin. Pull it in here under the flag so users who
// include <nautilus/val.hpp> (which includes this header transitively)
// see the primitive.
#ifdef ENABLE_CONSTANT_TRACER
#include "nautilus/partial_evaluation/unroll_scope.hpp"
#endif
