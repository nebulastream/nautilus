
#pragma once

#include "ExceptionBasedTraceContext.hpp"
#include <functional>
#include <memory>

namespace nautilus::tracing {
class ExecutionTrace;
class SymbolicExecutionContext;

/**
 * @brief Exception-free tracing context that always completes function execution.
 *
 * This is an alternative to ExceptionBasedTraceContext that eliminates the need for TraceTerminationException.
 * Instead of throwing an exception when a path is fully explored or a merge/loop is detected,
 * it enters "passive mode" where all tracing operations become no-ops and traceBool() returns
 * false to guide the function to its natural exit.
 *
 * Key differences from ExceptionBasedTraceContext:
 * - No exceptions thrown during tracing - the traced function always returns normally
 * - Uses SymbolicExecutionContext::recordNoThrow() instead of record()
 * - When termination is detected, enters passive mode (paused_ = true)
 * - In passive mode, traceBool() returns false (guarantees loop termination)
 * - All other trace methods return dummyRef_ or are no-ops in passive mode
 *
 * Both LazyTraceContext and ExceptionBasedTraceContext produce identical ExecutionTrace output.
 * The choice between them is made via the engine option "engine.traceMode" (values: "exceptionBasedTracing",
 * "lazyTracing").
 */
class LazyTraceContext final : public TracingInterface {
public:
	// --- TracingInterface overrides ---

	TypedValueRef& registerFunctionArgument(Type type, size_t index) override;
	TypedValueRef& traceConstant(Type type, const ConstantLiteral& value) override;
	TypedValueRef& traceAlloca(size_t allocSize) override;
	TypedValueRef& traceCopy(const TypedValueRef& ref) override;
	TypedValueRef& traceBinaryOp(Op op, Type resultType, const TypedValueRef& left,
	                             const TypedValueRef& right) override;
	TypedValueRef& traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) override;
	TypedValueRef& traceTernaryOp(Op op, Type resultType, const TypedValueRef& first, const TypedValueRef& second,
	                              const TypedValueRef& third) override;
	void traceReturnOperation(Type type, const TypedValueRef& ref) override;
	void traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) override;
	TypedValueRef& traceCall(void* fptn, Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
	                         FunctionAttributes fnAttrs) override;
	bool traceBool(const TypedValueRef& value, double probability) override;
	void allocateValRef(ValueRef ref) override;
	void freeValRef(ValueRef ref) override;
	void pushStaticVal(void* ptr, size_t size) override;
	void popStaticVal() override;

	// --- Non-interface public API ---

	~LazyTraceContext() override = default;

	/**
	 * @brief Resets persistent state between trace iterations.
	 * Also resets the paused_ flag.
	 */
	void resume();

	/**
	 * @brief Initialize the completing trace context with references to stack-allocated objects.
	 */
	static LazyTraceContext* initialize(TagRecorder& tagRecorder, ExecutionTrace& executionTrace,
	                                    SymbolicExecutionContext& symbolicExecutionContext,
	                                    const engine::Options& options);

	/**
	 * @brief Main tracing entry point - allocates all objects on stack and executes symbolic tracing.
	 * Unlike ExceptionBasedTraceContext::trace(), this method never uses try/catch - the traced function always
	 * returns normally.
	 * @param traceFunction The function to trace
	 * @param options Engine options for configuration
	 * @return unique_ptr to ExecutionTrace containing the complete trace
	 */
	static std::unique_ptr<ExecutionTrace> trace(std::function<void()>& traceFunction,
	                                             const engine::Options& options = engine::Options());

	LazyTraceContext() = default;

private:
	bool isFollowing();
	TypedValueRef& follow(Op op);
	template <typename OnCreation>
	TypedValueRef& traceOperation(Op op, OnCreation&& onCreation);
	Snapshot recordSnapshot();
	std::string formatStaticVars() const;
	std::string getMangledName(void* fnptr);
	std::string getFunctionName(void* fnptr, const std::string& mangledNamed);

	// Injected state - holds references to stack-allocated objects
	std::unique_ptr<TraceState> state;

	// Persistent state - reset between trace iterations via resume()
	std::vector<StaticVarHolder> staticVars;
	AliveVariableHash aliveVars;
	std::unordered_map<void*, std::string> mangledNameCache;

	// Passive mode state
	bool paused_ = false;
	TypedValueRef dummyRef_ = {0, Type::v};
};

} // namespace nautilus::tracing
