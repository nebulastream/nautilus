#pragma once

#include "ExecutionTrace.hpp"
#include "Snapshot.hpp"
#include "TraceContextBase.hpp"
#include "nautilus/tracing/TracingInterface.hpp"
#include "symbolic_execution/SymbolicExecutionContext.hpp"
#include "tag/TagRecorder.hpp"
#include <cstdint>
#include <functional>
#include <vector>

namespace nautilus {
class NautilusFunctionDefinition;
}

namespace nautilus::tracing {

/**
 * @brief A trace context for an inline traced region scope.
 *
 * A region records into the enclosing function's single ExecutionTrace while
 * maintaining its own delta environment: the region's staticVars and aliveVars
 * only hold statics pushed / refs incremented inside the region. Region
 * snapshots hash the region's delta combined with the parent's state hash P.
 *
 * On a recording engagement (recording_ == true), the region drives its own
 * local exploration of any internal branches via localCtx_, invoking the
 * region body multiple times (see traceRegionContinue()) so that branches
 * inside a region cost passes local to that region instead of re-running the
 * enclosing function (see docs/region.md). Only
 * traceConstant/traceCopy/traceAssignment/traceBinaryOp/traceUnaryOp/
 * traceTernaryOp/traceBool participate in local exploration — traceAlloca,
 * traceCall/traceIndirectCall/traceNautilusCall/traceNautilusFunctionPtr, and
 * traceReturnOperation still forward to the parent unconditionally (a
 * documented scope limit: these don't arise in the arithmetic/comparison
 * kernels this targets, and forwarding them preserves today's behavior
 * exactly rather than risking an unverified interaction).
 *
 * On a FOLLOW replay of an already-recorded region (recording_ == false),
 * every method still forwards to the parent, exactly as before: the body was
 * already fully resolved by an earlier recording engagement, so this
 * engagement is a transparent structural walk-through, not new exploration.
 */
class RegionTraceContext final : public TraceContextBase {
public:
	RegionTraceContext(TraceContextBase* parent, TagRecorder* recorder, bool recording);

	/// Re-arms this context for a fresh region engagement, restoring exactly the
	/// state the constructor establishes. Lets the root contexts keep a pool of
	/// instances (regions nest strictly LIFO, so the pool is indexed by nesting
	/// depth) instead of heap-allocating one per region entry: RegionTraceContext
	/// inherits several containers from TraceContextBase, and constructing them
	/// N times per trace is pure overhead when one region per branch is the
	/// target usage pattern.
	void reinitialize(TraceContextBase* parent, TagRecorder* recorder, bool recording);

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

	TraceContextBase* getRootContext() override;

	bool traceRegionBegin(TagAddress callSite) override;

	bool traceRegionContinue() override;

	void traceRegionEnd() override;

private:
	Snapshot recordSnapshot() override;

	// --- Local exploration (recording_ == true only) ---
	bool localIsFollowing() const;
	TypedValueRef& localFollow(Op op);
	template <typename OnCreation>
	TypedValueRef& localTraceOperation(Op op, OnCreation&& onCreation);

	/// The root's shared execution trace. A pointer rather than a reference so a
	/// pooled instance can be re-armed for a different trace by reinitialize().
	ExecutionTrace& trace() const {
		return *trace_;
	}

	TraceContextBase* parent_ = nullptr;
	TagRecorder* recorder_ = nullptr;
	TagAddress callSite_ = 0;
	uint64_t P_ = 0;
	ExecutionTrace* trace_ = nullptr;
	// True when this engagement *records* the region body into a fresh entry
	// block; false when it is a symbolic FOLLOW replay of an already-recorded
	// open (non-memoizable) region, in which case the body is transparently
	// followed and no entry block is created (issue: region internal branch).
	bool recording_ = false;

	// Local exploration state, meaningful only while recording_ == true. A
	// fresh SymbolicExecutionContext owned by this region drives exploration
	// of any branches structurally inside the region body, decoupled from the
	// parent's own mode/exploration (see docs/region.md). paused_ mirrors
	// LazyTraceContext's passive mode, scoped to this region: once the local
	// tree is exhausted for the current pass, all
	// further trace calls in that pass become no-ops until the body returns.
	// Sized for the branches structurally inside one region body, not a whole
	// function: the default (128) would allocate a large bucket array on every
	// region entry, which at one-region-per-branch is paid N times per trace.
	static constexpr size_t kExpectedRegionTags = 8;
	SymbolicExecutionContext localCtx_ {kExpectedRegionTags};
	bool paused_ = false;
	TypedValueRef dummyRef_ = {0, Type::v};
	// The region's entry block/position, captured once at first recording so
	// every local pass after the first can rewind the shared cursor back to
	// the start of the region body (mirrors ExecutionTrace::resetExecution(),
	// scoped to the region instead of the whole function).
	uint32_t entryBlockIndex_ = 0;
	// Lazily created the first time a second local pass turns out to be
	// needed, i.e. only for regions that actually contain an unresolved
	// internal branch. A branch-free region never allocates this block, so
	// region()'s IR footprint for the common case is unchanged from before
	// this feature.
	uint32_t exitBlockIndex_ = 0;
	bool hasExitBlock_ = false;

	// Nesting depth of region() calls served by *this* instance. A root-level
	// region entry constructs a fresh RegionTraceContext (see
	// LazyTraceContext::traceRegionBegin), so at depth 0 every member above is
	// already pristine and needs neither saving nor resetting — only a region()
	// nested inside this region's own body (depth > 0, see LocalExplorationState)
	// pays for the save/restore. Skipping it at depth 0 avoids constructing a
	// second SymbolicExecutionContext (and its reserve(128) tagMap allocation)
	// on the common, non-nested path.
	uint32_t depth_ = 0;

	// Nested `region()` calls (region() invoked from within this region's own
	// fn()) reach this same RegionTraceContext instance again, recursively,
	// via traceRegionBegin/traceRegionContinue/traceRegionEnd (region.md
	// §4.6 predates region-local exploration and reuses the instance rather
	// than creating a child context). The members above are therefore
	// per-invocation, not per-instance: save/restore them around a nested
	// call so the inner region's local exploration can't corrupt the outer
	// region's own in-progress pass state.
	struct LocalExplorationState {
		SymbolicExecutionContext localCtx;
		TagAddress callSite;
		bool paused;
		uint32_t entryBlockIndex;
		uint32_t exitBlockIndex;
		bool hasExitBlock;
	};
	std::vector<LocalExplorationState> savedLocalStates_;
};

} // namespace nautilus::tracing
