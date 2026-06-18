
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

	/// Trace a stack allocation of @p size bytes with @p align byte alignment.
	/// Each call appends a fresh entry to the function's alloca table on the
	/// execution trace; the returned ref points to that entry's index.
	virtual TypedValueRef& traceAlloca(size_t size, size_t align) = 0;

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

	// --- Explicit control-flow primitives ---
	//
	// These power the closure-style If/While/For constructs in control_flow.hpp.
	// Unlike traceBool, they emit CMP/JMP blocks directly in a single pass and
	// never enqueue a symbolic execution path, so a function built purely from
	// explicit control flow is traced in exactly one iteration. They are not
	// supported by every tracer (e.g. lazy tracing) and default to throwing.

	/// Emit a conditional branch on @p condition with taken-probability @p
	/// probability, returning the ids of the created true/false blocks. Implementations
	/// must reject use while symbolic re-execution is in progress (i.e. mixing with
	/// implicit native control flow in the same function).
	virtual ExplicitCmpBlocks emitExplicitCmp(const TypedValueRef& condition, double probability) = 0;

	/// Create a fresh control-flow-merge block (used for If merges and loop headers).
	virtual uint32_t openMergeBlock() = 0;

	/// Make @p blockId the block subsequent operations are emitted into.
	virtual void switchToBlock(uint32_t blockId) = 0;

	/// Return the id of the block operations are currently emitted into.
	virtual uint32_t currentBlock() = 0;

	/// Emit an unconditional jump from @p fromBlock to @p targetBlock.
	virtual void jumpTo(uint32_t fromBlock, uint32_t targetBlock) = 0;

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
