
#pragma once

#include "ExecutionTrace.hpp"
#include "TraceContextBase.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "symbolic_execution/SymbolicExecutionContext.hpp"
#include <functional>
#include <list>
#include <memory>
#include <unordered_set>
#include <vector>

namespace nautilus::tracing {
class ExecutionTrace;
class TraceModule;

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
 *
 * This class also drives region()'s local branch exploration directly (see docs/region.md and
 * RegionFrame below), rather than delegating to a separate RegionTraceContext the way
 * ExceptionBasedTraceContext does. A region does not need its own ExecutionTrace or
 * TagRecorder-holding object: it shares this trace context's single ExecutionTrace and
 * differs from top-level tracing only in which SymbolicExecutionContext/staticVars/aliveVars/
 * paused flag/TagRecorder/hash-offset are "current" -- exactly what a RegionFrame holds. Every
 * trace* method below picks between "top RegionFrame" and "this object's own root state" via a
 * handful of small accessors (currentPaused()/setCurrentPaused()/currentSymbolicExecutionContext()/
 * currentEnv()), so the actual per-operation tracing logic (traceConstant, traceCopy, follow,
 * traceOperation, traceBool, ...) is written exactly once and used for both scopes -- unlike the
 * old RegionTraceContext, whose local-exploration methods were near-identical, separately
 * maintained copies of this class's own.
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

	TraceContextBase* getRootContext() override;

	bool traceRegionBegin(TagAddress callSite) override;

	bool traceRegionContinue() override;

	void traceRegionEnd() override;

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
	/// Region-aware form of isFollowing(): consults the innermost recording
	/// region frame's own SymbolicExecutionContext if one is active, else the
	/// root's -- see currentSymbolicExecutionContext().
	bool currentlyFollowing();
	TypedValueRef& follow(Op op);
	/// Region-aware per-operation tracing, shared by traceConstant/traceCopy/
	/// traceBinaryOp/traceUnaryOp/traceTernaryOp/traceAssignment/traceBool (via
	/// their own bodies) and this template: dispatches through currentPaused()/
	/// currentlyFollowing()/setCurrentPaused() so the exact same body serves
	/// both root-level and region-local tracing.
	template <typename OnCreation>
	TypedValueRef& traceOperation(Op op, OnCreation&& onCreation);
	/// Root-only variant of traceOperation, for the six ops that always forward
	/// straight to the true root regardless of any active region (traceAlloca,
	/// traceCall, traceIndirectCall, traceNautilusCall, traceNautilusFunctionPtr,
	/// traceReturnOperation) -- matching the old RegionTraceContext, which
	/// unconditionally forwarded these to parent_. Still calls the region-aware
	/// recordSnapshot() (matching the old getActiveTracer()->recordSnapshot()
	/// dispatch, which resolved to the *active* region's own snapshot even for a
	/// call that otherwise forwarded straight through it).
	template <typename OnCreation>
	TypedValueRef& rootTraceOperation(Op op, OnCreation&& onCreation);
	Snapshot recordSnapshot() override;
	uint64_t currentStateHash() const override;

	/**
	 * @brief One nested level of an active region() engagement.
	 *
	 * Holds everything a region needs that isn't the (always shared, root-owned)
	 * ExecutionTrace: its own local SymbolicExecutionContext driving branch
	 * exploration scoped to the region body, its own passive-mode flag, its own
	 * delta staticVars/aliveVars (region.md's "fresh tag/liveness domain"), and
	 * the bookkeeping traceRegionBegin/Continue/End need (callSite/P for
	 * memoization, entry/exit block indices for wiring passes together).
	 *
	 * regionFramePool_ below pools these by nesting depth exactly like the old
	 * regionPool_ pooled RegionTraceContext instances: entries persist across
	 * engagements and are reinitialize()'d rather than reconstructed, so a
	 * region's SymbolicExecutionContext (and its tag map's allocation) is paid
	 * for once per depth, not once per region entry.
	 */
	struct RegionFrame {
		explicit RegionFrame() : localCtx(kExpectedRegionTags) {
		}

		void reinitialize(TagRecorder* rec, TagAddress site, uint64_t parentP, bool isRecording) {
			localCtx.reset();
			paused = false;
			recorder = rec;
			callSite = site;
			P = parentP;
			entryBlockIndex = 0;
			exitBlockIndex = 0;
			hasExitBlock = false;
			recording = isRecording;
			// The region's *delta* environment (region.md §4.2) must start empty:
			// a reused frame would otherwise inherit the previous region's
			// statics/alive refs into its snapshot hash and escape transfer.
			staticVars.clear();
			aliveVars.reset();
		}

		// Sized for the branches structurally inside one region body, not a whole
		// function: the default (128) would allocate a large bucket array on
		// every region entry, which at one-region-per-branch is paid N times per
		// trace.
		static constexpr size_t kExpectedRegionTags = 8;

		SymbolicExecutionContext localCtx;
		bool paused = false;
		TagRecorder* recorder = nullptr;
		TagAddress callSite = 0;
		uint64_t P = 0;
		// The region's entry block/position, captured once at first recording so
		// every local pass after the first can rewind the shared cursor back to
		// the start of the region body (mirrors ExecutionTrace::resetExecution(),
		// scoped to the region instead of the whole function).
		uint32_t entryBlockIndex = 0;
		// Lazily created the first time a second local pass turns out to be
		// needed, i.e. only for regions that actually contain an unresolved
		// internal branch. A branch-free region never allocates this block, so
		// region()'s IR footprint for the common case is unchanged.
		uint32_t exitBlockIndex = 0;
		bool hasExitBlock = false;
		// True when this engagement *records* the region body; false on a FOLLOW
		// replay of an already-recorded open (non-memoizable) region, in which
		// case local exploration never runs and every op below just updates this
		// frame's env (staticVars/aliveVars) as a side effect of the shared
		// trace's own record/follow cursor doing its normal thing. Fixed once,
		// when the outermost region of a nesting chain begins (root RECORD vs
		// FOLLOW at that point), and inherited unchanged by every region nested
		// inside it -- not recomputed per nesting level, since a nested region
		// reached while running an already-recording outer region's body is
		// unconditionally "new work" too, regardless of the outer region's own
		// current *local* pass mode (which alternates between RECORD and FOLLOW
		// across its own passes; recording here is not that).
		bool recording = false;
		std::vector<StaticVarHolder> staticVars;
		AliveVariableHash aliveVars;
	};

	/// True if there is an active region frame right now (top of regionFramePool_,
	/// up to activeRegionDepth_). Env bookkeeping (staticVars/aliveVars/hash) uses
	/// this unconditionally; exploration state (paused/SymbolicExecutionContext)
	/// additionally requires that frame to be `recording` -- see currentPaused()
	/// and currentSymbolicExecutionContext().
	bool inActiveRegion() const {
		return activeRegionDepth_ > 0;
	}

	RegionFrame& topFrame() {
		return regionFramePool_[activeRegionDepth_ - 1];
	}

	const RegionFrame& topFrame() const {
		return regionFramePool_[activeRegionDepth_ - 1];
	}

	/// The staticVars/aliveVars currently being written to: the top region
	/// frame's own delta environment if any region is active (regardless of
	/// whether it is `recording` -- matches the old RegionTraceContext, whose
	/// allocateValRef/freeValRef/pushStaticVal/popStaticVal were never gated on
	/// recording_), or this object's own (root) environment otherwise.
	TraceEnv currentEnv() {
		if (inActiveRegion()) {
			auto& f = topFrame();
			return TraceEnv {f.staticVars, f.aliveVars};
		}
		return TraceEnv {staticVars, aliveVars};
	}

	/// The passive-mode flag currently in effect: the innermost *recording*
	/// region frame's own, or this object's root-level paused_ if no region is
	/// active or the active one isn't recording (a non-recording region defers
	/// every trace call to whatever scope is really doing the work, exactly like
	/// the old RegionTraceContext's `if (!recording_) return parent_->traceX(...)`).
	bool currentPaused() const {
		return inActiveRegion() && topFrame().recording ? topFrame().paused : paused_;
	}

	void setCurrentPaused(bool value) {
		if (inActiveRegion() && topFrame().recording) {
			topFrame().paused = value;
		} else {
			paused_ = value;
		}
	}

	/// The SymbolicExecutionContext currently driving record/follow decisions:
	/// the innermost *recording* region frame's local context, or this object's
	/// own root-level one.
	SymbolicExecutionContext& currentSymbolicExecutionContext() {
		if (inActiveRegion() && topFrame().recording) {
			return topFrame().localCtx;
		}
		return state->symbolicExecutionContext;
	}

	/// Pool of region frames, indexed by nesting depth. Regions nest strictly
	/// LIFO, so the frame at index activeRegionDepth_ is always free to
	/// reinitialize() when a region is entered.
	std::vector<RegionFrame> regionFramePool_;
	size_t activeRegionDepth_ = 0;

	// Passive mode state (root-level; see RegionFrame::paused for the region-local
	// equivalent).
	bool paused_ = false;
	// Returned by all trace methods when paused. Safe because callers (val<T> constructors)
	// always copy the TypedValueRef by value — no one holds the reference across calls.
	TypedValueRef dummyRef_ = {0, Type::v};

	// Work-list for multi-function tracing
	std::list<compiler::CompilableFunction> functionsToTrace;
	std::unordered_set<std::string> registeredFunctions;
};

} // namespace nautilus::tracing
