#pragma once

#include "AliveVariableHash.hpp"
#include "ExecutionTrace.hpp"
#include "TraceOperation.hpp"
#include "TracingStatistics.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/options.hpp"
#include "nautilus/tracing/TracingInterface.hpp"
#include "symbolic_execution/SymbolicExecutionContext.hpp"
#include "tag/Tag.hpp"
#include "tag/TagRecorder.hpp"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <deque>
#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace nautilus {
class NautilusFunctionDefinition;
}

namespace nautilus::tracing {
class TraceModule;

/**
 * @brief State that requires initialization for tracing operations.
 * This is initialized in the trace context when tracing begins and reset when it ends.
 * Holds references to stack-allocated objects.
 */
struct TraceState {
	TagRecorder& tagRecorder;
	ExecutionTrace& executionTrace;
	SymbolicExecutionContext& symbolicExecutionContext;
	const engine::Options& options;
	/// `engine.normalizeFunctionNames`, read once: it is consulted on every traced call.
	bool normalizeFunctionNames;
	/// `dump.copySites`: remember where each recorded copy was made (ExecutionTrace::copySites).
	bool recordCopySites;
	std::unordered_map<void*, uint32_t> normalizedFunctionNameCache; // Maps function pointers to normalized indices
	uint32_t nextNormalizedFunctionIndex = 0;                        // Counter for normalized function names

	TraceState(TagRecorder& tr, ExecutionTrace& et, SymbolicExecutionContext& sec, const engine::Options& opts);
};

/**
 * @brief Records a symbolic execution trace of a Nautilus function.
 *
 * The traced function is executed once per explored path. When a path reaches a point that
 * was already explored (a fully explored branch, a control-flow merge or a loop back-edge),
 * the context enters "passive mode" instead of aborting the execution: every trace method
 * becomes a no-op that returns dummyRef_, and traceBool() returns false so the function runs
 * to its natural exit. The traced function therefore always returns normally, which keeps
 * the C++ destructors of its locals (and of any val<T> it holds) running in order.
 *
 * Lifecycle:
 * 1. startTrace() pops a function off the work-list and creates its ExecutionTrace,
 *    TagRecorder and SymbolicExecutionContext; TraceState holds references to them.
 * 2. runScope() re-invokes the function once per explored path, calling resume() before
 *    each pass to reset the per-pass state (staticVars, aliveVars, destructors, paused_).
 * 3. Nautilus functions invoked while tracing are appended to the work-list and traced in turn.
 */
class TraceContext final : public TracingInterface {
public:
	// --- TracingInterface overrides ---

	TypedValueRef& registerFunctionArgument(Type type, size_t index) override;
	TypedValueRef& traceConstant(Type type, const ConstantLiteral& value) override;
	TypedValueRef& traceAlloca(size_t size, size_t align) override;
	TypedValueRef& traceCopy(const TypedValueRef& ref) override;
	void noteExplicitCopy() override {
		nextCopyExplicit_ = true;
	}
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
	void registerDestructor(const TypedValueRef& address, void* destructor) override;
	void unregisterDestructor(const TypedValueRef& address) override;
	void transferDestructor(const TypedValueRef& from, const TypedValueRef& to) override;

	// --- Non-interface public API ---

	~TraceContext() override = default;

	/**
	 * @brief Resets persistent state between trace iterations.
	 * Also resets the paused_ flag.
	 */
	void resume();

	/**
	 * @brief Initialize the thread-local trace context with references to stack-allocated objects
	 * and register it as the active tracer.
	 */
	static TraceContext* initialize(TagRecorder& tagRecorder, ExecutionTrace& executionTrace,
	                                SymbolicExecutionContext& symbolicExecutionContext, const engine::Options& options);

	/**
	 * @brief Single-function tracing entry point. Traces @p traceFunction alone; Nautilus
	 * functions it invokes are recorded as calls but not traced.
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

	TraceContext() = default;

private:
	bool isFollowing();
	TypedValueRef& follow(Op op);
	template <typename OnCreation>
	TypedValueRef& traceOperation(Op op, OnCreation&& onCreation);
	Snapshot recordSnapshot();
	std::string formatStaticVars() const;
	/// The normalized name ("runtimeFuncN") of the native callee @p fnptr when
	/// `engine.normalizeFunctionNames` is set, empty otherwise. Indices follow the order
	/// callees are first traced in, so this is assigned at trace time; every other name is
	/// resolved once tracing is done (see resolveCalleeNames). Session-wide, so a region
	/// scope numbers a callee the same way its enclosing function does.
	std::string normalizedFunctionName(void* fnptr);

	/// Fills in the symbol and display names of every native callee and destructor recorded
	/// in @p trace. Tracing records only their pointers: resolving a name costs a dladdr
	/// symbol-table scan, which is paid here once per callee instead of once per traced call
	/// site and scope, and is cached process-wide beyond that (see resolveFunctionName). When
	/// !shouldResolveCalleeNames() the names stay empty, and the function table names the
	/// callee `fnN` in the IR and in generated code.
	static void resolveCalleeNames(ExecutionTrace& trace, const engine::Options& options);

	/// `engine.resolveFunctionNames`, which defaults to on only when the compiled code will
	/// be inspected: `debug`, `perf` or `perf.sample`.
	static bool shouldResolveCalleeNames(const engine::Options& options);

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
	void initRegionScope(TraceContext& parent, uint32_t entry, uint32_t exit, TagRecorder& recorder,
	                     const RegionAttributes& attributes);

	/// Returns the pooled context used for regions opened by this scope, creating it on
	/// first use. Regions nest strictly LIFO and a scope traces at most one region at a
	/// time, so one slot per scope covers a whole nesting chain and each depth's
	/// SymbolicExecutionContext (and its tag map) is allocated once per thread.
	TraceContext& acquireChildScope();

	/// A region recorded in the enclosing trace: the block its body starts in and the
	/// block the enclosing scope continues in afterwards.
	struct RegionRecord {
		uint32_t entryBlock;
		uint32_t exitBlock;
	};

	// Injected state - holds references to the objects of the function or region scope being traced.
	// Empty when not tracing and stored inline to avoid a per-trace heap allocation.
	std::optional<TraceState> state;

	std::vector<FunctionCall::Destructor> activeDestructors;

	/// The block a pass of this scope rewinds to before re-invoking the body.
	/// 0 for a function scope (the trace's own entry block).
	uint32_t entryBlock_ = 0;

	/// Non-null exactly for a region scope: the scope that opened this region.
	/// Read on the freeValRef hot path, so it stays a plain member here.
	TraceContext* parent_ = nullptr;

	/// Everything a scope needs only once region() is involved, held behind one pointer
	/// and allocated on first use.
	///
	/// This is deliberately not inlined into the object. A TraceContext is otherwise
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
		std::unique_ptr<TraceContext> childScope;
	};

	/// Sized for the branches structurally inside one region body, not a whole function.
	static constexpr size_t kRegionExpectedTags = 8;

	/// Returns this scope's region state, allocating it on first use.
	RegionScopeState& regionState();

	/// The context that owns the cross-scope bookkeeping shared by every scope of
	/// one tracing session: the function work-list, the registered-function set and
	/// the normalized function-name cache. Always the outermost context;
	/// `this` for a function scope.
	TraceContext* session_ = this;

	/// Statistics (session-owned; see session_) and the pass timer behind the re-run split. Only the outermost scope is
	/// timed: a region's passes run inside a pass of the enclosing scope and are part of its time.
	enum class Phase : uint8_t { None, Follow, Record, Paused };
	TracingStatistics stats_;
	Phase phase_ = Phase::None;
	bool rerunPass_ = false;
	bool nextCopyExplicit_ = false;
	std::chrono::steady_clock::time_point phaseStart_, passStart_;
	void beginPass(bool rerun);
	void enterPhase(Phase phase);
	void endPass();
	void accountPhase(std::chrono::steady_clock::time_point now);
	/// Ends the pass: the traced calls that follow do nothing but return a dummy.
	void pause() {
		paused_ = true;
		enterPhase(Phase::Paused);
	}

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

	/// Name and registration site of the function whose body is currently being traced
	/// (session-owned; see session_). Read by the "Invalid region()" diagnostics so a
	/// rejected region body says which enclosing function it came from, not just where the
	/// region() call site itself sits.
	std::string currentFunctionName_;
	SourceLocation currentFunctionLocation_;

	/// " in function 'name' (registered at file:line:column)" for the function currently
	/// being traced, or empty before any function has started. Used to extend an
	/// "Invalid region()" diagnostic with the enclosing function's identity.
	std::string describeCurrentFunction() const;

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
