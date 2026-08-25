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
	depth_ = 0;
	savedCallSites_.clear();

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
	// Save the enclosing call's callSite_ before overwriting it below, but only
	// for an actual nested call (depth_ > 0) -- at depth 0 this instance was
	// just (re)armed by the root's own traceRegionBegin, so there is nothing
	// of a prior engagement left to protect. See the depth_ comment in the
	// header for why callSite_ specifically needs this and P_/recording_ don't.
	if (depth_ > 0) {
		savedCallSites_.push_back(callSite_);
	}
	++depth_;
	callSite_ = callSite;
	return true;
}

bool RegionTraceContext::traceRegionContinue() {
	// No local exploration against this substrate (see the class comment):
	// the region body is invoked exactly once per engagement.
	return false;
}

void RegionTraceContext::traceRegionEnd() {
	--depth_;
	if (depth_ > 0) {
		// A nested region() ending: the whole nesting chain shares this one
		// instance, so the enclosing region's delta environment *is* this same
		// aliveVars. Values that outlive the inner region are therefore already
		// accounted for in the right place and must not be handed to parent_
		// yet -- doing that credited the root once per boundary crossed for a
		// value that only ever receives one freeValRef(), leaving a phantom
		// alive ref that skews the root's hash (and so every later snapshot)
		// for the rest of the pass. There is nothing to memoize here either:
		// only a top-level traceRegionBegin performs a memo lookup, so an entry
		// filed by a nested region could only ever be read back by a later
		// *top-level* entry at the same call site, which would match it against
		// a P computed fresh at that site while this key carries the P
		// inherited from the enclosing chain.
		callSite_ = savedCallSites_.back();
		savedCallSites_.pop_back();
		return;
	}

	// FOLLOW replay: the body was already recorded, so there is nothing new to
	// memoize or escape-transfer, and a bogus continuation must not be cached.
	if (!recording_) {
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
	// Forwards too, unlike everything else this class merely delegates:
	// recordSnapshot() is dispatched polymorphically via getActiveTracer()
	// from *parent_'s own* traceOperation/traceBool/etc bodies (see
	// TracingUtil.cpp's trampolines and ExceptionBasedTraceContext's use of
	// getActiveTracer()->recordSnapshot()), not called directly by this
	// class. Those callers restart the *whole* enclosing function from
	// scratch on every unresolved branch (ExceptionBasedTraceContext has no
	// region-local exploration loop of its own -- see traceRegionContinue()
	// below), so a region-scoped tag/hash here would recompute P_ fresh on
	// every such restart while the tags it's XORed against are expected to
	// stay stable across restarts for checkTag() dedup to work. The result
	// was silent exponential blowup (thousands of duplicate blocks) for any
	// branch inside a region, not a no-op. Delegating here instead gives the
	// branch the exact same identity it would have if the region weren't
	// there, matching every other method on this class.
	return parent_->recordSnapshot();
}

} // namespace nautilus::tracing
