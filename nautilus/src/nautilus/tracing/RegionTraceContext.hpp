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
class SymbolicExecutionContext;

/**
 * @brief A trace context for an inline traced region scope.
 *
 * A region records into the enclosing function's single ExecutionTrace while
 * maintaining its own delta environment: the region's staticVars and aliveVars
 * only hold statics pushed / refs incremented inside the region. Region
 * snapshots hash the region's delta combined with the parent's state hash P.
 * All tracing operations are delegated to the parent's recording path so they
 * land in the shared ExecutionTrace; only snapshots (recorder + delta + P) are
 * produced here via recordSnapshot().
 */
class RegionTraceContext final : public TraceContextBase {
public:
	RegionTraceContext(TraceContextBase* parent, TagRecorder* recorder, TagAddress callSite);

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

	void traceRegionEnd() override;

private:
	Snapshot recordSnapshot() override;

	TraceContextBase* parent_;
	TagRecorder* recorder_;
	TagAddress callSite_;
	uint64_t P_;
	ExecutionTrace& trace_;
	// Refs made alive inside the region, used to escape-transfer on region exit.
	std::vector<ValueRef> aliveDeltaRefs_;
};

} // namespace nautilus::tracing
