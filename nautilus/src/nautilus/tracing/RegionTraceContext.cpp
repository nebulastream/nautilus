#include "RegionTraceContext.hpp"
#include "TraceContextBase.hpp"

namespace nautilus::tracing {

RegionTraceContext::RegionTraceContext(TraceContextBase* parent, TagRecorder* recorder, bool recording) {
	reinitialize(parent, recorder, recording);
}

void RegionTraceContext::reinitialize(TraceContextBase* parent, TagRecorder* recorder, bool recording) {
	parent_ = parent;
	recorder_ = recorder;
	recording_ = recording;
	callSite_ = 0;
	P_ = parent_->currentStateHash();

	// The region's *delta* environment (region.md §4.2) must start empty: a
	// reused instance would otherwise inherit the previous region's
	// statics/alive refs into its snapshot hash and escape transfer.
	staticVars.clear();
	aliveVars.reset();
}

TypedValueRef& RegionTraceContext::registerFunctionArgument(Type type, size_t index) {
	return parent_->registerFunctionArgument(type, index);
}

TypedValueRef& RegionTraceContext::traceConstant(Type type, const ConstantLiteral& value) {
	return parent_->traceConstant(type, value);
}

TypedValueRef& RegionTraceContext::traceAlloca(size_t size, size_t align) {
	return parent_->traceAlloca(size, align);
}

TypedValueRef& RegionTraceContext::traceCopy(const TypedValueRef& ref) {
	return parent_->traceCopy(ref);
}

TypedValueRef& RegionTraceContext::traceBinaryOp(Op op, Type resultType, const TypedValueRef& left,
                                                 const TypedValueRef& right) {
	return parent_->traceBinaryOp(op, resultType, left, right);
}

TypedValueRef& RegionTraceContext::traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) {
	return parent_->traceUnaryOp(op, resultType, input);
}

TypedValueRef& RegionTraceContext::traceTernaryOp(Op op, Type resultType, const TypedValueRef& first,
                                                  const TypedValueRef& second, const TypedValueRef& third) {
	return parent_->traceTernaryOp(op, resultType, first, second, third);
}

void RegionTraceContext::traceReturnOperation(Type type, const TypedValueRef& ref) {
	parent_->traceReturnOperation(type, ref);
}

void RegionTraceContext::traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) {
	parent_->traceAssignment(target, source, resultType);
}

TypedValueRef& RegionTraceContext::traceCall(void* fptn, Type resultType,
                                             const std::vector<tracing::TypedValueRef>& arguments,
                                             FunctionAttributes fnAttrs) {
	return parent_->traceCall(fptn, resultType, arguments, fnAttrs);
}

TypedValueRef& RegionTraceContext::traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
                                                     const std::vector<tracing::TypedValueRef>& arguments,
                                                     FunctionAttributes fnAttrs) {
	return parent_->traceIndirectCall(fnPtrRef, resultType, arguments, fnAttrs);
}

TypedValueRef& RegionTraceContext::traceNautilusCall(const NautilusFunctionDefinition* definition,
                                                     std::function<void()> fwrapper, Type resultType,
                                                     const std::vector<tracing::TypedValueRef>& arguments,
                                                     FunctionAttributes fnAttrs) {
	return parent_->traceNautilusCall(definition, std::move(fwrapper), resultType, arguments, fnAttrs);
}

TypedValueRef& RegionTraceContext::traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
                                                            std::function<void()> fwrapper) {
	return parent_->traceNautilusFunctionPtr(definition, std::move(fwrapper));
}

bool RegionTraceContext::traceBool(const TypedValueRef& value, const double probability) {
	return parent_->traceBool(value, probability);
}

void RegionTraceContext::allocateValRef(ValueRef ref) {
	aliveVars.increment(ref);
}

void RegionTraceContext::freeValRef(ValueRef ref) {
	aliveVars.decrement(ref);
}

TraceContextBase* RegionTraceContext::getRootContext() {
	return parent_->getRootContext();
}

bool RegionTraceContext::traceRegionBegin(TagAddress callSite) {
	callSite_ = callSite;
	return true;
}

bool RegionTraceContext::traceRegionContinue() {
	// No local exploration against this substrate (see the class comment):
	// the region body is invoked exactly once per engagement.
	return false;
}

void RegionTraceContext::traceRegionEnd() {
	if (!recording_) {
		// FOLLOW replay: the body was already recorded; there is nothing new to
		// memoize or escape-transfer, and a bogus continuation must not be cached.
		return;
	}
	if (aliveVars.size() > 0) {
		aliveVars.forEachAliveRef([this](ValueRef ref, uint32_t count) {
			for (uint32_t i = 0; i < count; ++i) {
				parent_->allocateValRef(ref);
			}
		});
	} else {
		auto& trace = getRootContext()->getExecutionTrace();
		getRootContext()->regionMemos[callSite_][P_] =
		    RegionExec {trace.currentOperationIndex, trace.currentBlockIndex};
	}
}

Snapshot RegionTraceContext::recordSnapshot() {
	return {recorder_->createTag(), currentStateHash() ^ P_};
}

} // namespace nautilus::tracing
