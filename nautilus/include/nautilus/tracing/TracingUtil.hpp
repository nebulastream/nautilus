
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

/// RAII guard that clears the active tracer on scope exit. Use in tracing entry
/// points so an exception escaping the symbolic-execution loop (RuntimeException
/// from ExecutionTrace, TagCreationException, or any exception from the traced
/// function) still deregisters the thread-local tracer.
struct ActiveTracerGuard {
	ActiveTracerGuard() = default;
	~ActiveTracerGuard() noexcept {
		setActiveTracer(nullptr);
	}
	ActiveTracerGuard(const ActiveTracerGuard&) = delete;
	ActiveTracerGuard& operator=(const ActiveTracerGuard&) = delete;
	ActiveTracerGuard(ActiveTracerGuard&&) = delete;
	ActiveTracerGuard& operator=(ActiveTracerGuard&&) = delete;
};

TypedValueRef& traceBinaryOp(Op op, Type resultType, const TypedValueRef& left, const TypedValueRef& right);
TypedValueRef& traceUnaryOp(Op op, Type resultType, const TypedValueRef& input);
TypedValueRef& traceTernaryOp(Op op, Type resultType, const TypedValueRef& first, const TypedValueRef& second,
                              const TypedValueRef& third);

TypedValueRef& traceAlloca(size_t size, size_t align);

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

/// Returns the positionId most recently published by the trace context
/// (via recordSnapshot or registerFunctionArgument).  Used by
/// TypedValueRefHolder construction to bind each val<T> to a source-position
/// identity that survives the symbolic-execution iteration boundary.
uint64_t currentPositionId();

/// Sentinel for argument positions; high bit set so the value can never
/// collide with a real Tag* pointer (user-space pointers on supported
/// platforms have the high bit clear).
inline constexpr uint64_t ARG_POSITION_SENTINEL_BASE = 1ULL << 63;

void allocateValRef(uint64_t positionId);
void freeValRef(uint64_t positionId);

} // namespace nautilus::tracing
