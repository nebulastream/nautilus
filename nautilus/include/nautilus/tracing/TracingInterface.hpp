
#pragma once

#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/tracing/Operations.hpp"
#include "nautilus/tracing/TypedValueRef.hpp"
#include "nautilus/tracing/Types.hpp"
#include <cstddef>
#include <functional>
#include <vector>

namespace nautilus {
class NautilusFunctionDefinition;
}

namespace nautilus::tracing {

/**
 * @brief Abstract interface for tracing operations.
 *
 * This interface decouples the val<T> tracing layer (TracingUtil) from any concrete
 * tracing implementation. The active implementation is set once per trace and stored
 * as a thread-local pointer, so all operations within a single trace go through the
 * same implementation while different traces can use different ones.
 *
 * Concrete implementations (e.g. ExceptionBasedTraceContext) are set via setActiveTracer() before
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
	virtual TypedValueRef& traceAlloca(size_t allocSize) = 0;

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

	/// Trace a call through a runtime function pointer value (indirect call).
	virtual TypedValueRef& traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
	                                         const std::vector<TypedValueRef>& arguments,
	                                         FunctionAttributes fnAttrs) = 0;

	/// Trace a call to a nested Nautilus function. Registers the function for later tracing.
	virtual TypedValueRef& traceNautilusCall(const NautilusFunctionDefinition* definition,
	                                         std::function<void()> fwrapper, Type resultType,
	                                         const std::vector<TypedValueRef>& arguments,
	                                         FunctionAttributes fnAttrs) = 0;

	/// Get the address of a Nautilus function as a function pointer value.
	/// Registers the function for later tracing and returns a ptr-typed value.
	virtual TypedValueRef& traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
	                                                std::function<void()> fwrapper) = 0;

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

	/// Return an opaque pointer identifying the current execution position
	/// (call stack). The backing type is nautilus::tracing::Tag (a
	/// pointer-interned trie node from the TagRecorder); two calls from the
	/// same source line compare equal by pointer. Exposed here as
	/// `const void*` so that the full Tag definition stays internal to the
	/// src/ tree. Used by Stage 2 of the constant-folding tracer to detect
	/// back-edges where a Constant's value has changed between visits.
	/// O(stack depth).
	virtual const void* currentTag() = 0;

	// -----------------------------------------------------------------
	// Lifecycle hooks — default no-ops. Subclasses (the partial-
	// evaluation plugin being the current user) override these to run
	// trace-scoped setup / teardown without core needing to know about
	// the subclass's existence.
	// -----------------------------------------------------------------

	/// Called by setActiveTracer() right after this instance becomes the
	/// thread's active tracer. Hook point for per-trace state init
	/// (e.g. zeroing observability counters at trace start). Default
	/// no-op.
	virtual void onActivate() noexcept {
	}

	/// Called by setActiveTracer() right before this instance is
	/// replaced / cleared. Hook point for per-trace state teardown
	/// (e.g. flushing out any registries a subclass owns). Default
	/// no-op.
	virtual void onDeactivate() noexcept {
	}

	/// Called inside the multi-function work-list loop at the top of
	/// each inner function's trace iteration — after the TraceState is
	/// re-initialized, before the traced C++ runs. Hook for per-
	/// function defensive sweeps. Default no-op.
	virtual void beforeInnerFunction() noexcept {
	}

	/// Declares whether this tracer is the partial-evaluation mode.
	/// Core's `setActiveTracer()` reads this after swapping the
	/// active-tracer pointer and syncs the PE runtime flag
	/// (`pe::setConstantTracerEnabled`) so `LazyTracedRef`'s fold
	/// fast paths and the Constant registry stay off outside PE mode
	/// without any explicit toggling by user code. Default false;
	/// `PELazyTraceContext` is the only override that returns true.
	virtual bool isPartialEvaluationMode() const noexcept {
		return false;
	}
};

} // namespace nautilus::tracing
