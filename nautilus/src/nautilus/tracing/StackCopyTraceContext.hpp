
#pragma once

#include "ExceptionBasedTraceContext.hpp"
#include "fiber/ContextSwitch.hpp"
#include "fiber/FiberStack.hpp"
#include "nautilus/CompilableFunction.hpp"
#include <deque>
#include <functional>
#include <list>
#include <memory>
#include <unordered_set>
#include <vector>

namespace nautilus::tracing {
class ExecutionTrace;
class SymbolicExecutionContext;

/**
 * @brief Tracing context that executes every path through a function exactly once.
 *
 * The re-executing tracers (ExceptionBasedTraceContext, LazyTraceContext) explore one
 * path per execution of the traced function: every additional path replays the shared
 * decision prefix from the function entry (FOLLOW mode), so total tracing work is
 * O(paths x path length). This tracer removes the replay entirely: the traced function
 * runs on a dedicated side stack and at every freshly recorded CMP the live stack
 * region plus the small mutable tracer state (static-variable stack, alive-variable
 * hash, trace position) are snapshotted. When a path terminates, the oldest pending
 * snapshot is restored and execution resumes directly inside traceBool(), taking the
 * false branch. Each branch suffix therefore executes exactly once and total work is
 * O(size of the execution tree).
 *
 * Trace-output equivalence: the FOLLOW replay of the re-executing tracers performs no
 * trace mutations except bumping lastValueRef once per function argument (emulated on
 * every resume), and pending false-branches are explored in FIFO order of their CMP
 * discovery - exactly the order of this tracer's snapshot queue. The produced traces
 * are therefore byte-identical to the other tracers and validated against the same
 * reference files.
 *
 * Path termination reuses LazyTraceContext's passive mode: on a control-flow merge or
 * loop the tracer pauses (all trace methods become no-ops, traceBool returns false)
 * and the function runs to its natural return, so destructors fire in order and no
 * stack frames are abandoned mid-flight.
 *
 * Constraint: restoring stack bytes resurrects frames whose destructors already ran
 * on a previously completed path. This is safe for val<T> / static_val objects (their
 * destructors only touch tracer state that the snapshot restores) but NOT for
 * heap-owning plain C++ locals (e.g. a std::vector) that live across a traced branch:
 * their destructor would run once per explored path. Use forkTracing for such code.
 *
 * Selected via the engine option "engine.traceMode" = "stackCopyTracing".
 */
class StackCopyTraceContext final : public TraceContextBase {
public:
	// --- TracingInterface overrides ---

	TypedValueRef& registerFunctionArgument(Type type, size_t index) override;
	TypedValueRef& traceConstant(Type type, const ConstantLiteral& value) override;
	TypedValueRef& traceAlloca(size_t size, size_t align) override;
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
	TypedValueRef& traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
	                                 const std::vector<tracing::TypedValueRef>& arguments,
	                                 FunctionAttributes fnAttrs) override;
	TypedValueRef& traceNautilusCall(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper,
	                                 Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
	                                 FunctionAttributes fnAttrs) override;
	TypedValueRef& traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
	                                        std::function<void()> fwrapper) override;
	bool traceBool(const TypedValueRef& value, double probability) override;
	void allocateValRef(ValueRef ref) override;
	void freeValRef(ValueRef ref) override;
	void pushStaticVal(void* ptr, size_t size) override;
	void popStaticVal() override;

	// --- Non-interface public API ---

	// Out-of-line: the unique_ptr members require complete types at destruction.
	~StackCopyTraceContext() override;
	StackCopyTraceContext() = default;

	/// Resets persistent state between traced functions.
	void resume();

	/**
	 * @brief Single-function tracing entry point (benchmark harness signature).
	 * @param traceFunction The function to trace.
	 * @param options Engine options for configuration.
	 * @param arena Arena used to allocate the trace's Blocks and TraceOperations;
	 *              must outlive the returned trace.
	 */
	static std::unique_ptr<ExecutionTrace> trace(std::function<void()>& traceFunction, const engine::Options& options,
	                                             Arena& arena);

	/**
	 * @brief Multi-function tracing entry point. Traces all functions in the work-list,
	 * including nested Nautilus functions discovered during tracing.
	 */
	std::unique_ptr<TraceModule> startTrace(std::list<compiler::CompilableFunction>& functions,
	                                        const engine::Options& options, Arena& arena);
	static std::unique_ptr<TraceModule> Trace(std::list<compiler::CompilableFunction>& functions,
	                                          const engine::Options& options, Arena& arena);

private:
	/**
	 * @brief Suspended false-branch continuation of a recorded CMP.
	 *
	 * The stack copy covers [stackBase, side-stack top); together with the captured
	 * registers it allows resuming execution inside traceBool() at the CMP. The CMP is
	 * identified by its Snapshot rather than a raw (block, operation) position because
	 * later control-flow merges can move the operation to a different block; the
	 * trace's tag maps always hold its current location.
	 */
	struct PathSnapshot {
		detail::CapturedContext context;
		std::vector<std::byte> stackCopy;
		std::byte* stackBase = nullptr;
		Snapshot cmpTag;
		std::vector<StaticVarHolder> staticVarsCopy;
		AliveVariableHash aliveVarsCopy;
	};

	template <typename OnCreation>
	TypedValueRef& traceOperation(Op op, OnCreation&& onCreation);
	Snapshot recordSnapshot();

	/// Host-side scheduler: runs the traced function on the side stack and resumes
	/// pending snapshots in FIFO order until every path has been explored.
	void traceFunction(std::function<void()>& wrapper);
	/// Traces one function of the work-list (shared by trace() and startTrace()).
	void traceCurrentFunction(std::function<void()>& wrapper);
	/// Entry point on the side stack; never returns through the stack switch.
	static void fiberEntry(void* self);
	/// Provides the stable TagRecorder start address (its return address terminates
	/// every tag walk before it could leave the side stack) and runs the wrapper.
	__attribute__((noinline)) void runTraced();
	/// Switches back to the host scheduler after a path completed. Multi-shot: the
	/// surrounding fiber frames are part of every snapshot and restored consistently.
	[[noreturn]] void switchToHost();
	void captureStackInto(PathSnapshot& snap);

	// Persistent per-path state - reset via resume() / snapshot restore
	std::vector<StaticVarHolder> staticVars;
	AliveVariableHash aliveVars;
	bool paused_ = false;
	// Returned by all trace methods when paused. Safe because callers (val<T> constructors)
	// always copy the TypedValueRef by value - no one holds the reference across calls.
	TypedValueRef dummyRef_ = {0, Type::v};

	// Host/fiber machinery
	std::unique_ptr<detail::FiberStack> stack_;
	size_t requestedStackSize_ = 0;
	detail::CapturedContext hostContext_ = {};
	std::deque<PathSnapshot> pendingSnapshots_;
	std::exception_ptr pendingException_;
	std::function<void()>* currentWrapper_ = nullptr;
	ExecutionTrace* currentTrace_ = nullptr;
	const engine::Options* currentOptions_ = nullptr;
	std::unique_ptr<TagRecorder> tagRecorder_;
	std::unique_ptr<SymbolicExecutionContext> symbolicExecutionContext_;
	uint64_t pathCount_ = 0;
	// AddressSanitizer fake-stack handover tokens for the host/fiber switches.
	void* hostFakeStack_ = nullptr;
	void* fiberFakeStack_ = nullptr;
	const void* hostStackBottom_ = nullptr;
	size_t hostStackSize_ = 0;

	// Work-list for multi-function tracing
	std::list<compiler::CompilableFunction> functionsToTrace;
	std::unordered_set<std::string> registeredFunctions;
};

} // namespace nautilus::tracing
