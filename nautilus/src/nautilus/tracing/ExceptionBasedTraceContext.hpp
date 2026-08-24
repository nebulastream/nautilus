#pragma once

#include "ExecutionTrace.hpp"
#include "RegionTraceContext.hpp"
#include "TraceContextBase.hpp"
#include "TraceOperation.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/options.hpp"
#include "nautilus/tracing/TracingInterface.hpp"
#include "tag/TagRecorder.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <list>
#include <memory>
#include <unordered_set>
#include <vector>

namespace nautilus {
class NautilusFunctionDefinition;
}

namespace nautilus::tracing {
class ExecutionTrace;
class SymbolicExecutionContext;
class TraceModule;

/**
 * @brief The trace context manages a thread local instance to record a symbolic execution trace of a given Nautilus
 * function.
 *
 * Design Philosophy:
 * - ExceptionBasedTraceContext is a simple thread_local object (not a pointer) - zero heap allocation
 * - ExecutionTrace and SymbolicExecutionContext are allocated on the stack in trace()
 * - TraceState holds references to these stack objects and is created during initialization
 * - staticVars and aliveVars are persistent members that get reset between trace iterations
 * - Inherits from TraceContextBase so different implementations can be swapped per trace via setActiveTracer().
 *
 * Lifecycle:
 * 1. trace() allocates ExecutionTrace and SymbolicExecutionContext on its stack
 * 2. initialize() creates TraceState with references to these stack objects and registers itself
 *    as the active tracer via setActiveTracer(this)
 * 3. Multiple trace iterations execute, calling resume() to reset persistent state
 * 4. After tracing completes, setActiveTracer(nullptr) is called and ExecutionTrace is returned
 */
class ExceptionBasedTraceContext final : public TraceContextBase {
public:
	// --- TracingInterface overrides ---

	TypedValueRef& registerFunctionArgument(Type type, size_t index) override;

	TypedValueRef& traceConstant(Type type, const ConstantLiteral& value) override;

	/**
	 * @brief Main tracing entry point - allocates all objects on stack and executes symbolic tracing.
	 * @param functionsToTrace List of functions to trace
	 * @param options Engine options for configuration
	 * @param arena Arena used to allocate Blocks/TraceOperations for every
	 *              ExecutionTrace in the returned TraceModule; must outlive
	 *              the returned module.
	 * @return unique_ptr to TraceModule containing all function traces
	 */
	std::unique_ptr<TraceModule> startTrace(std::list<compiler::CompilableFunction>& functionsToTrace,
	                                        const engine::Options& options, Arena& arena);
	static std::unique_ptr<TraceModule> Trace(std::list<compiler::CompilableFunction>& functionsToTrace,
	                                          const engine::Options& options, Arena& arena);

	TypedValueRef& traceCopy(const TypedValueRef& ref) override;

	TypedValueRef& traceBinaryOp(Op op, Type resultType, const TypedValueRef& left,
	                             const TypedValueRef& right) override;

	TypedValueRef& traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) override;

	TypedValueRef& traceTernaryOp(Op op, Type resultType, const TypedValueRef& first, const TypedValueRef& second,
	                              const TypedValueRef& third) override;
	TypedValueRef& traceAlloca(size_t size, size_t align) override;
	void traceReturnOperation(Type type, const TypedValueRef& ref) override;

	void traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) override;

	TypedValueRef& traceCall(void* fptn, Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
	                         FunctionAttributes fnAttrs) override;

	TypedValueRef& traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
	                                 const std::vector<tracing::TypedValueRef>& arguments,
	                                 FunctionAttributes fnAttrs) override;

	bool traceBool(const TypedValueRef& value, double probability) override;

	void allocateValRef(ValueRef ref) override;
	void freeValRef(ValueRef ref) override;

	TypedValueRef& traceNautilusCall(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper,
	                                 Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
	                                 FunctionAttributes fnAttrs) override;

	TypedValueRef& traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
	                                        std::function<void()> fwrapper) override;

	TraceContextBase* getRootContext() override;

	bool traceRegionBegin(TagAddress callSite) override;

	bool traceRegionContinue() override;

	void traceRegionEnd() override;

	// --- Non-interface public API ---

	~ExceptionBasedTraceContext() override = default;

	/**
	 * @brief Resets persistent state between trace iterations.
	 * Clears staticVars and resets aliveVars hash/counts.
	 * Does NOT reset state (executionTrace/symbolicExecutionContext) - they persist across iterations.
	 */
	void resume();

	/**
	 * @brief Initialize the trace context with references to stack-allocated objects.
	 * Sets this context as the active tracer via setActiveTracer().
	 * @param tagRecorder Reference to TagRecorder for creating unique tags
	 * @param executionTrace Reference to stack-allocated ExecutionTrace
	 * @param symbolicExecutionContext Reference to stack-allocated SymbolicExecutionContext
	 * @param options Reference to engine options for configuration
	 * @return Pointer to initialized thread_local ExceptionBasedTraceContext
	 */
	static ExceptionBasedTraceContext* initialize(TagRecorder& tagRecorder, ExecutionTrace& executionTrace,
	                                              SymbolicExecutionContext& symbolicExecutionContext,
	                                              const engine::Options& options);

	/**
	 * @brief Main tracing entry point - executes symbolic tracing of the
	 * supplied function.
	 * @param traceFunction The function to trace.
	 * @param options Engine options for configuration.
	 * @param arena Arena used to allocate the trace's Blocks and
	 *              TraceOperations; must outlive the returned trace.
	 * @return unique_ptr to ExecutionTrace containing the complete trace.
	 */
	static std::unique_ptr<ExecutionTrace> trace(std::function<void()>& traceFunction, const engine::Options& options,
	                                             Arena& arena);

	/**
	 * @brief Default constructor - public to allow thread_local storage.
	 * Initializes with empty state (state == nullptr means not initialized).
	 */
	ExceptionBasedTraceContext() = default;

	bool isActive() const;

private:
	bool isFollowing();
	TypedValueRef& follow(Op op);
	template <typename OnCreation>
	TypedValueRef& traceOperation(Op op, OnCreation&& onCreation);
	Snapshot recordSnapshot() override;

	/// Stack of active traced regions. Each frame borrows a region context from
	/// regionPool_ (it does not own it) and records the active tracer that was
	/// in effect before the region body began (so it can be restored).
	struct RegionFrame {
		RegionTraceContext* region;
		TracingInterface* previous;
	};
	std::vector<RegionFrame> activeRegions_;

	/// Pool of region contexts, indexed by nesting depth. Regions nest strictly
	/// LIFO, so the context at index activeRegions_.size() is always free when a
	/// region is entered; it is re-armed via RegionTraceContext::reinitialize()
	/// rather than heap-allocated afresh. See the equivalent member on
	/// LazyTraceContext.
	std::vector<std::unique_ptr<RegionTraceContext>> regionPool_;

	std::list<compiler::CompilableFunction> functionsToTrace = std::list<compiler::CompilableFunction> {};
	std::unordered_set<std::string> registeredFunctions;
};

} // namespace nautilus::tracing
