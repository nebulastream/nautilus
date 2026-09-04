
#pragma once

#include "ExceptionBasedTraceContext.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "symbolic_execution/SymbolicExecutionContext.hpp"
#include <deque>
#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>

namespace nautilus::tracing {
class ExecutionTrace;

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
class LazyTraceContext final : public TraceContextBase {
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
	TypedValueRef& traceCallWithExceptionHandling(void* fptn, Type resultType,
	                                              const std::vector<tracing::TypedValueRef>& arguments,
	                                              FunctionAttributes fnAttrs, void* captureFunc = nullptr) override;
	TypedValueRef& traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
	                                 const std::vector<tracing::TypedValueRef>& arguments, FunctionAttributes fnAttrs,
	                                 void* captureFunc = nullptr) override;

	TypedValueRef& traceIndirectCallWithExceptionHandling(const TypedValueRef& fnPtrRef, Type resultType,
	                                                      const std::vector<tracing::TypedValueRef>& arguments,
	                                                      FunctionAttributes fnAttrs,
	                                                      void* captureFunc = nullptr) override;
	TypedValueRef& traceNautilusCall(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper,
	                                 Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
	                                 FunctionAttributes fnAttrs) override;
	TypedValueRef& traceNautilusCallWithExceptionHandling(const NautilusFunctionDefinition* definition,
	                                                      std::function<void()> fwrapper, Type resultType,
	                                                      const std::vector<tracing::TypedValueRef>& arguments,
	                                                      FunctionAttributes fnAttrs) override;
	TypedValueRef& traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
	                                        std::function<void()> fwrapper) override;
	bool traceBool(const TypedValueRef& value, double probability) override;
	void traceRegion(std::function<void()>& regionFunction, const RegionAttributes& attributes) override;
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
	 * @brief Main tracing entry point.  Unlike ExceptionBasedTraceContext::trace(),
	 * this method never uses try/catch - the traced function always returns normally.
	 * @param traceFunction The function to trace.
	 * @param options Engine options for configuration.
	 * @param arena Arena used to allocate the trace's Blocks and TraceOperations;
	 *              must outlive the returned trace.
	 * @return unique_ptr to ExecutionTrace containing the complete trace.
	 */
	static std::unique_ptr<ExecutionTrace> trace(std::function<void()>& traceFunction, const engine::Options& options,
	                                             Arena& arena);

	/**
	 * @brief Multi-function tracing entry point. Traces all functions in the work-list,
	 * including nested Nautilus functions discovered during tracing.
	 * @param functions Initial list of functions to trace.
	 * @param options Engine options for configuration.
	 * @param arena Arena backing all traces in the returned module; must
	 *              outlive the returned module.
	 * @return unique_ptr to TraceModule containing all function traces.
	 */
	std::unique_ptr<TraceModule> startTrace(std::list<compiler::CompilableFunction>& functions,
	                                        const engine::Options& options, Arena& arena);
	static std::unique_ptr<TraceModule> Trace(std::list<compiler::CompilableFunction>& functions,
	                                          const engine::Options& options, Arena& arena);

	LazyTraceContext() = default;

private:
	bool isFollowing();
	TypedValueRef& follow(Op op);
	template <typename OnCreation>
	TypedValueRef& traceOperation(Op op, OnCreation&& onCreation);
	Snapshot recordSnapshot();
	std::string formatStaticVars() const;

	/**
	 * @brief Runs the symbolic-execution loop of one *trace scope* to completion.
	 *
	 * A scope is a body that is explored path by path against its own
	 * SymbolicExecutionContext, TagRecorder, staticVars/aliveVars and passive-mode
	 * flag -- i.e. exactly the state this object holds. Tracing a whole function is
	 * running a scope whose entry block is block 0; a region (see docs/region.md) is
	 * the same loop over a scope whose entry block is a freshly created block inside
	 * the enclosing scope's trace. `resetExecution()` is by definition
	 * `setCurrentBlock(0)`, so the two differ only in `entryBlock_`.
	 *
	 * @param body The scope body, re-invoked once per explored path.
	 */
	void runScope(std::function<void()>& body);

	/**
	 * @brief Terminates one completed pass of a region scope.
	 *
	 * A function scope needs no equivalent: its body already emits a tagged RETURN per
	 * completed pass. A region body has no terminator of its own, so this records a
	 * tagged jump to the region's exit block. Because the jump is tagged, a second pass
	 * that ends in a different block hits the same tag and is merged by the ordinary
	 * control-flow-merge machinery -- the same code that merges the arms of an `if`.
	 *
	 * This is also where a value created inside the body that outlives it is rejected;
	 * see the check itself for why that cannot be supported.
	 */
	void traceScopeExit();

	/// Prepares this (possibly pooled) context to trace the body of the region @p attributes
	/// describes, opened by @p parent and recorded into @p parent's trace between @p entry
	/// and @p exit.
	void initRegionScope(LazyTraceContext& parent, uint32_t entry, uint32_t exit, TagRecorder& recorder,
	                     const RegionAttributes& attributes);

	/// Returns the pooled context used for regions opened by this scope, creating it on
	/// first use. Regions nest strictly LIFO and a scope traces at most one region at a
	/// time, so one slot per scope covers a whole nesting chain and each depth's
	/// SymbolicExecutionContext (and its tag map) is allocated once per thread.
	LazyTraceContext& acquireChildScope();

	/// A region recorded in the enclosing trace: the block its body starts in and the
	/// block the enclosing scope continues in afterwards.
	struct RegionRecord {
		uint32_t entryBlock;
		uint32_t exitBlock;
	};

	/// The block a pass of this scope rewinds to before re-invoking the body.
	/// 0 for a function scope (the trace's own entry block).
	uint32_t entryBlock_ = 0;

	/// Non-null exactly for a region scope: the scope that opened this region.
	/// Read on the freeValRef hot path, so it stays a plain member here.
	LazyTraceContext* parent_ = nullptr;

	/// Everything a scope needs only once region() is involved, held behind one pointer
	/// and allocated on first use.
	///
	/// This is deliberately not inlined into the object. A LazyTraceContext is otherwise
	/// small and its hot members (state, staticVars, aliveVars, paused_) are touched on
	/// every traced operation and every val<T> construction; carrying ~250 bytes of
	/// region state inline pushed them apart and cost 9-13% on tracing benchmarks that
	/// never use a region at all. Functions that use no region never allocate this.
	struct RegionScopeState {
		/// Region scopes only: the block the enclosing scope resumes in.
		uint32_t exitBlock = 0;

		/// Region scopes only: what the region() call site whose body this scope traces
		/// said about itself -- its optional name and its source location (see
		/// docs/region.md). Carried here so a body this scope has to reject can be
		/// reported against the call site the user wrote.
		RegionAttributes attributes;

		/// Region scopes only: the exit snapshot of the first completed pass. Every later
		/// completed pass must agree with it, or what escapes the region would depend on
		/// which path happened to be explored last (see docs/region.md).
		std::optional<Snapshot> exitSnapshot;

		/// The refs still alive at the end of the first completed pass, ascending -- the
		/// values that escape the region. Compared directly against every later pass
		/// rather than relying on exitSnapshot alone: that snapshot folds the escape set
		/// into an XOR hash together with the static-variable hash, so two different
		/// escape sets can collide, and a static-variable change can cancel an escape
		/// change. Comparing the sets also lets the diagnostic name the refs involved.

		/// Regions opened *by* this scope, keyed by their call-site snapshot. Consulted
		/// when this scope replays a recorded path and reaches the region again: the body
		/// is not re-executed, the cursor jumps straight to the region's exit block.
		std::unordered_map<Snapshot, RegionRecord> regionMemo;

		/// The SymbolicExecutionContext this scope's state refers to when it is a region
		/// scope. Owned here (a function scope's is owned by its caller's stack frame) and
		/// reset rather than reconstructed, so a pooled scope keeps its tag map
		/// allocation.
		SymbolicExecutionContext symbolicExecutionContext {kRegionExpectedTags};

		/// TagRecorders of every region traced during the current function; only the
		/// session's copy is used. A TagRecorder's trie root is a member Tag and its nodes
		/// carry pointers to it, while the Tag* it mints are stored in the trace and in
		/// the tag map -- both of which outlive the region. So recorders must live as long
		/// as the trace they tag, not as long as the region engagement, and must not move
		/// once created.
		std::deque<TagRecorder> tagRecorders;

		/// The pooled context used for regions opened by this scope. Regions nest strictly
		/// LIFO and a scope traces at most one region at a time, so one slot per scope
		/// covers a whole nesting chain and each depth's SymbolicExecutionContext (and its
		/// tag map) is allocated once per thread.
		std::unique_ptr<LazyTraceContext> childScope;
	};

	/// Sized for the branches structurally inside one region body, not a whole function.
	static constexpr size_t kRegionExpectedTags = 8;

	/// Returns this scope's region state, allocating it on first use.
	RegionScopeState& regionState();

	/// The context that owns the cross-scope bookkeeping shared by every scope of
	/// one tracing session: the function work-list, the registered-function set and
	/// the (mangled/normalized) function-name caches. Always the outermost context;
	/// `this` for a function scope.
	LazyTraceContext* session_ = this;

	// Persistent state - reset between trace iterations via resume()
	std::vector<StaticVarHolder> staticVars;
	AliveVariableHash aliveVars;

	// Passive mode state
	bool paused_ = false;
	// Returned by all trace methods when paused. Safe because callers (val<T> constructors)
	// always copy the TypedValueRef by value — no one holds the reference across calls.
	TypedValueRef dummyRef_ = {0, Type::v};

	// Work-list for multi-function tracing (session-owned; see session_)
	std::list<compiler::CompilableFunction> functionsToTrace;
	/// Definition identity -> the name that definition is traced under.
	///
	/// Keyed on the NautilusFunctionDefinition, not on its name: two distinct
	/// NautilusFunctions may share a name, and deduping by name meant the
	/// second was never traced while every call to it dispatched into the
	/// first one's body, with no diagnostic. The stored name is uniquified
	/// against `usedFunctionNames` so both bodies get traced and emitted.
	std::unordered_map<const void*, std::string> registeredFunctions;
	std::unordered_set<std::string> usedFunctionNames;

	/// Returns the trace-unique name for @p definition, registering it for
	/// tracing on first sight. @p newlyRegistered reports whether this call
	/// was the first.
	const std::string& registerNautilusFunction(const NautilusFunctionDefinition* definition,
	                                            std::function<void()> fwrapper, bool& newlyRegistered);

	/// Allocated on the first region() this scope opens; null for a scope that never
	/// sees one. Placed after the hot members on purpose -- see RegionScopeState.
	std::unique_ptr<RegionScopeState> regionState_;
};

} // namespace nautilus::tracing
