
#pragma once

#include "ExceptionBasedTraceContext.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/tracing/predicate/PathPredicateStore.hpp"
#include <functional>
#include <list>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace nautilus::tracing {
class ExecutionTrace;
class SymbolicExecutionContext;

/**
 * @brief Tracing context with a stable operation-identity scheme and lightweight
 * path-predicate pruning.
 *
 * Background:
 *   The pre-existing tracers (ExceptionBasedTraceContext, LazyTraceContext) key
 *   each operation by a Snapshot of (Tag*, staticValueHash ^ aliveVars.hash()).
 *   The `aliveVars` half of that hash depends on the ValueRef of every live
 *   `val<T>` at the time of the operation, so the same source-level CMP
 *   executed under different incoming ValueRefs gets a different Snapshot.
 *   That is what produces the documented O(M^N) path-explosion in
 *   `pathExplosion_postCallBranch_*` and related shapes (see
 *   `test/common/PathExplosionFunctions.hpp:57-67`).
 *
 * Fixes in this context:
 *   1. **Tag-stable Snapshot** — `recordSnapshot()` drops `aliveVars.hash()`
 *      from the identity mix.  Same source op, same static-variable state,
 *      same Tag*.  This collapses the M-way fan-out at post-call CMPs to a
 *      single tag whose true/false arms are each explored once.
 *
 *   2. **Path-predicate pruning** — `traceBool()` derives a Predicate from the
 *      CMP that produced the bool and consults `PathPredicateStore` before
 *      handing the decision to `SymbolicExecutionContext`.  Arms that are
 *      provably dead under the current path constraints are short-circuited
 *      via the same passive-mode mechanism as `LazyTraceContext`, so no
 *      iterations are spent on them.
 *
 *   3. **Per-iteration predicate accounting** — predicates are pushed when a
 *      branch is committed (in `traceBool`) and the active stack is reset at
 *      the top of each new iteration via `resume()`.
 *
 * NOT in scope for this context (planned follow-ups):
 *   - Per-function sub-traces with implicit call-boundary detection and
 *     emitted CALL ops.  The Tag* still incorporates call-site identity via
 *     stack unwinding, so chained inlined calls (`baseline_threeCallsNoBranch`)
 *     still expand at trace level.  Removing that requires either
 *     `std::source_location` plumbing through the val<T> operators or a
 *     dladdr-based scope_id; both are larger changes left to a follow-up PR.
 *
 *   - Snapshot-and-resume execution.  This context still re-executes the
 *     traced function once per branch combination, like the existing tracers.
 *
 * Selection: pass `engine.traceMode = "scopedTracing"` to the engine options.
 */
class ScopedTraceContext final : public TraceContextBase {
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

	~ScopedTraceContext() override = default;

	/// Resets persistent state between trace iterations.
	void resume();

	/// Initializes the context with references to stack-allocated trace objects.
	static ScopedTraceContext* initialize(TagRecorder& tagRecorder, ExecutionTrace& executionTrace,
	                                      SymbolicExecutionContext& symbolicExecutionContext,
	                                      const engine::Options& options);

	/// Main tracing entry point — runs the trace driver loop with no exceptions.
	static std::unique_ptr<ExecutionTrace> trace(std::function<void()>& traceFunction, const engine::Options& options,
	                                             Arena& arena);

	/// Multi-function tracing entry point.  Traces all functions on the work-list,
	/// including nested Nautilus functions discovered during tracing.
	std::unique_ptr<TraceModule> startTrace(std::list<compiler::CompilableFunction>& functions,
	                                        const engine::Options& options, Arena& arena);
	static std::unique_ptr<TraceModule> Trace(std::list<compiler::CompilableFunction>& functions,
	                                          const engine::Options& options, Arena& arena);

	ScopedTraceContext() = default;

	/// Test-only accessor for the predicate store.  Useful for asserting that
	/// the store is balanced (size == 0) at the end of a trace.
	const PathPredicateStore& predicates() const noexcept {
		return predicates_;
	}

private:
	bool isFollowing();
	TypedValueRef& follow(Op op);
	template <typename OnCreation>
	TypedValueRef& traceOperation(Op op, OnCreation&& onCreation);
	Snapshot recordSnapshot();
	std::string formatStaticVars() const;

	/// Derives a Predicate from the CMP op that produced @p boolRef, when that
	/// CMP compares a non-constant value to an integer constant.  Returns an
	/// invalid predicate if no useful constraint can be extracted (e.g. the
	/// comparison is between two non-constants).
	PathPredicateStore::Predicate derivePredicate(const TypedValueRef& boolRef) const;

	/// Inverts a comparison op: EQ <-> NEQ, LT <-> GTE, LTE <-> GT.  Used when
	/// the false arm of a CMP is taken so we can push the negated predicate
	/// without inverting `lit`.
	static Op negateOp(Op op) noexcept;

	// Persistent state - reset between trace iterations via resume()
	std::vector<StaticVarHolder> staticVars;
	AliveVariableHash aliveVars; // tracked for backwards-compat APIs, not used in recordSnapshot

	// Per-trace predicate store; pushed/popped around branch decisions.
	PathPredicateStore predicates_;

	// Passive mode state
	bool paused_ = false;
	// Returned by all trace methods when paused. Safe because callers (val<T> constructors)
	// always copy the TypedValueRef by value — no one holds the reference across calls.
	TypedValueRef dummyRef_ = {0, Type::v};

	// Work-list for multi-function tracing
	std::list<compiler::CompilableFunction> functionsToTrace;
	std::unordered_set<std::string> registeredFunctions;
};

} // namespace nautilus::tracing
