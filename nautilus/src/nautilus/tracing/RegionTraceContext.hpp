#pragma once

#include "ExecutionTrace.hpp"
#include "Snapshot.hpp"
#include "TraceContextBase.hpp"
#include "nautilus/tracing/TracingInterface.hpp"
#include "tag/TagRecorder.hpp"
#include <cstdint>
#include <functional>
#include <vector>

namespace nautilus {
class NautilusFunctionDefinition;
}

namespace nautilus::tracing {

/**
 * @brief A trace context for an inline traced region scope, driven by ExceptionBasedTraceContext.
 *
 * A region records into the enclosing function's single ExecutionTrace while
 * maintaining its own delta environment: the region's staticVars and aliveVars
 * only hold statics pushed / refs incremented inside the region. Region
 * snapshots hash the region's delta combined with the parent's state hash P.
 *
 * Every trace method forwards straight to parent_ -- this context exists for
 * tag/liveness isolation and memoized replay (see docs/region.md), not for
 * bounding branch-tracing cost the way LazyTraceContext's own inline region
 * frames do (see docs/region.md and LazyTraceContext.hpp's RegionFrame): a
 * region driven from the exception-based substrate always costs whatever
 * re-tracing the enclosing function would already cost without the region.
 */
class RegionTraceContext final : public TraceContextBase {
public:
	RegionTraceContext(TraceContextBase* parent, TagRecorder* recorder, bool recording);

	/// Re-arms this context for a fresh region engagement, restoring exactly the
	/// state the constructor establishes. Lets the root context keep a pool of
	/// instances (regions nest strictly LIFO, so the pool is indexed by nesting
	/// depth) instead of heap-allocating one per region entry: RegionTraceContext
	/// inherits several containers from TraceContextBase, and constructing them
	/// N times per trace is pure overhead when one region per branch is the
	/// target usage pattern.
	void reinitialize(TraceContextBase* parent, TagRecorder* recorder, bool recording);

	// --- TracingInterface overrides: all forward to parent_ ---

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

	TraceContextBase* parent_ = nullptr;
	TagRecorder* recorder_ = nullptr;
	TagAddress callSite_ = 0;
	uint64_t P_ = 0;
	// Gates whether traceRegionEnd() memoizes / transfers escapes: false when
	// this engagement is a FOLLOW replay of an already-recorded open
	// (non-memoizable) region, in which case a bogus continuation must not be
	// cached (the body was already fully recorded by an earlier engagement).
	bool recording_ = false;
};

} // namespace nautilus::tracing
