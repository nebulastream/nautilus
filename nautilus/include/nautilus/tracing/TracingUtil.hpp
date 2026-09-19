
#pragma once

#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/common/RegionAttributes.hpp"
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

TypedValueRef& traceCallWithExceptionHandling(void* fptn, Type resultType,
                                              const std::vector<tracing::TypedValueRef>& arguments,
                                              FunctionAttributes fnAttrs, void* captureFunc = nullptr);

void registerDestructor(const TypedValueRef& address, void* destructor);
void unregisterDestructor(const TypedValueRef& address);

TypedValueRef& traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
                                 const std::vector<tracing::TypedValueRef>& arguments, FunctionAttributes fnAttrs,
                                 void* captureFunc = nullptr);

TypedValueRef& traceIndirectCallWithExceptionHandling(const TypedValueRef& fnPtrRef, Type resultType,
                                                      const std::vector<tracing::TypedValueRef>& arguments,
                                                      FunctionAttributes fnAttrs, void* captureFunc = nullptr);

TypedValueRef& traceNautilusCall(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper,
                                 Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
                                 FunctionAttributes fnAttrs);

TypedValueRef& traceNautilusCallWithExceptionHandling(const NautilusFunctionDefinition* definition,
                                                      std::function<void()> fwrapper, Type resultType,
                                                      const std::vector<tracing::TypedValueRef>& arguments,
                                                      FunctionAttributes fnAttrs);

TypedValueRef& traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper);

TypedValueRef& registerFunctionArgument(Type type, size_t index);

void traceReturnOperation(Type type, const TypedValueRef& ref);

/// Traces @p regionFunction as an isolated tracing region described by @p attributes;
/// see docs/region.md.
///
/// Forced inline (like TagRecorder::createTag(), for the same reason) so this dispatch never
/// shows up as its own stack frame: LazyTraceContext::traceRegion roots a new region scope's
/// tags at __builtin_return_address(0), which is only call-site-specific if its immediate
/// caller is region()'s own (per-lambda) instantiation. A real, out-of-line frame for this
/// forwarder sits at the exact same address for every region() call in the program regardless
/// of optimization level, since it is not templated on the region body -- and since whether
/// the compiler collapses that frame away is otherwise optimization-level-dependent, two
/// sibling region() calls opened directly inside another region's body could root their tags
/// at that one shared address and collide (issue #450), which only surfaced in unoptimized
/// builds.
[[gnu::always_inline]] inline void traceRegion(std::function<void()>& regionFunction,
                                               const RegionAttributes& attributes) {
	if (auto* tracer = getActiveTracer()) {
		tracer->traceRegion(regionFunction, attributes);
	} else {
		regionFunction();
	}
}

void pushStaticVal(void* ptr, size_t size);
void popStaticVal();
void allocateValRef(ValueRef ref);
void freeValRef(ValueRef ref);

} // namespace nautilus::tracing
