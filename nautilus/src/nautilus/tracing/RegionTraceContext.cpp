#include "RegionTraceContext.hpp"
#include "TraceContextBase.hpp"

namespace nautilus::tracing {

RegionTraceContext::RegionTraceContext(TraceContextBase* parent, TagRecorder* recorder)
    : parent_(parent), recorder_(recorder), trace_(parent->getRootContext()->getExecutionTrace()) {
	callSite_ = 0;
	P_ = parent_->currentStateHash();
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
	trace_.createRegionEntryBlock();
	return true;
}

void RegionTraceContext::traceRegionEnd() {
	if (aliveVars.size() > 0) {
		aliveVars.forEachAliveRef([this](ValueRef ref, uint32_t count) {
			for (uint32_t i = 0; i < count; ++i) {
				parent_->allocateValRef(ref);
			}
		});
	} else {
		getRootContext()->regionMemos[callSite_][P_] =
		    RegionExec {trace_.currentOperationIndex, trace_.currentBlockIndex};
	}
}

Snapshot RegionTraceContext::recordSnapshot() {
	return {recorder_->createTag(), currentStateHash() ^ P_};
}

} // namespace nautilus::tracing
