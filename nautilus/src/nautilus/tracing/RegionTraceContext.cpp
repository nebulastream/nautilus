#include "RegionTraceContext.hpp"
#include "TraceContextBase.hpp"
#include "TraceOperation.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include <cassert>

namespace nautilus::tracing {

RegionTraceContext::RegionTraceContext(TraceContextBase* parent, TagRecorder* recorder, bool recording) {
	reinitialize(parent, recorder, recording);
}

void RegionTraceContext::reinitialize(TraceContextBase* parent, TagRecorder* recorder, bool recording) {
	parent_ = parent;
	recorder_ = recorder;
	recording_ = recording;
	trace_ = &parent->getRootContext()->getExecutionTrace();
	callSite_ = 0;
	P_ = parent_->currentStateHash();

	// Per-engagement state. Reset rather than re-constructed so a pooled
	// instance keeps its containers' storage (see the pool in the root
	// contexts' traceRegionBegin): that retention is the point of pooling.
	depth_ = 0;
	localCtx_.reset();
	paused_ = false;
	entryBlockIndex_ = 0;
	exitBlockIndex_ = 0;
	hasExitBlock_ = false;
	savedLocalStates_.clear();
	// The region's *delta* environment (region.md §4.2) must start empty:
	// a reused instance would otherwise inherit the previous region's
	// statics/alive refs into its snapshot hash and escape transfer.
	staticVars.clear();
	aliveVars.reset();
}

TypedValueRef& RegionTraceContext::registerFunctionArgument(Type type, size_t index) {
	return parent_->registerFunctionArgument(type, index);
}

bool RegionTraceContext::localIsFollowing() const {
	return localCtx_.getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW;
}

TypedValueRef& RegionTraceContext::localFollow(Op op) {
	auto& currentOperation = trace().getCurrentOperation();
	auto consumedTag = currentOperation.tag;
	trace().nextOperation();
	assert(currentOperation.op == op);
	(void) op;
	// Mirrors LazyTraceContext::follow's reconciliation-run skip (see its
	// comment) so a local pass's cursor stays aligned with the recorded
	// operation stream across traceConstant/traceCopy's globalTagMap-collision
	// reconciliation ASSIGNs.
	while (true) {
		auto& block = trace().getCurrentBlock();
		if (trace().currentOperationIndex >= block.operations.size()) {
			break;
		}
		if (!(block.operations[trace().currentOperationIndex]->tag == consumedTag)) {
			break;
		}
		trace().nextOperation();
	}
	return currentOperation.resultRef;
}

template <typename OnCreation>
TypedValueRef& RegionTraceContext::localTraceOperation(Op op, OnCreation&& onCreation) {
	if (paused_) {
		return dummyRef_;
	}
	if (localIsFollowing()) {
		return localFollow(op);
	}
	auto tag = getActiveTracer()->recordSnapshot();
	if (trace().checkTag(tag)) {
		return onCreation(tag);
	}
	// Control flow merge/loop detected within this region's own local
	// exploration. Enter (region-local) passive mode instead of throwing.
	paused_ = true;
	return dummyRef_;
}

TypedValueRef& RegionTraceContext::traceConstant(Type type, const ConstantLiteral& value) {
	if (!recording_) {
		return parent_->traceConstant(type, value);
	}
	if (paused_) {
		return dummyRef_;
	}
	auto op = Op::CONST;
	if (localIsFollowing()) {
		return localFollow(op);
	}
	auto tag = getActiveTracer()->recordSnapshot();
	auto globalTabIter = trace().globalTagMap.find(tag);
	if (globalTabIter != trace().globalTagMap.end()) {
		auto& ref = globalTabIter->second;
		auto* originalRef = trace().getBlocks()[ref.blockIndex]->operations[ref.operationIndex];
		auto resultRef = trace().addOperationWithResult(tag, op, type, {value});
		trace().addAssignmentOperation(tag, originalRef->resultRef, resultRef, resultRef.type);
		return originalRef->resultRef;
	}
	return trace().addOperationWithResult(tag, op, type, {value});
}

TypedValueRef& RegionTraceContext::traceAlloca(size_t size, size_t align) {
	return parent_->traceAlloca(size, align);
}

TypedValueRef& RegionTraceContext::traceCopy(const TypedValueRef& ref) {
	if (!recording_) {
		return parent_->traceCopy(ref);
	}
	if (paused_) {
		return dummyRef_;
	}
	if (localIsFollowing()) {
		return localFollow(ASSIGN);
	}
	auto tag = getActiveTracer()->recordSnapshot();
	auto globalTabIter = trace().globalTagMap.find(tag);
	if (globalTabIter != trace().globalTagMap.end()) {
		// See LazyTraceContext::traceCopy: a repeated call site with a
		// legitimately different source (e.g. sibling arguments of the same
		// call) is not a genuine reconvergence — reconcile onto the canonical
		// result ref instead of merging.
		auto& originalIdentifier = globalTabIter->second;
		auto* originalOp =
		    trace().getBlocks()[originalIdentifier.blockIndex]->operations[originalIdentifier.operationIndex];
		auto resultRef = trace().getNextValueRef();
		trace().addAssignmentOperation(tag, {resultRef, ref.type}, ref, ref.type);
		trace().addAssignmentOperation(tag, originalOp->resultRef, {resultRef, ref.type}, ref.type);
		return originalOp->resultRef;
	}
	if (!trace().checkTag(tag)) {
		paused_ = true;
		return dummyRef_;
	}
	auto resultRef = trace().getNextValueRef();
	return trace().addAssignmentOperation(tag, {resultRef, ref.type}, ref, ref.type);
}

TypedValueRef& RegionTraceContext::traceBinaryOp(Op op, Type resultType, const TypedValueRef& left,
                                                 const TypedValueRef& right) {
	if (!recording_) {
		return parent_->traceBinaryOp(op, resultType, left, right);
	}
	return localTraceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return trace().addOperationWithResult(tag, op, resultType, {left, right});
	});
}

TypedValueRef& RegionTraceContext::traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) {
	if (!recording_) {
		return parent_->traceUnaryOp(op, resultType, input);
	}
	return localTraceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return trace().addOperationWithResult(tag, op, resultType, {input});
	});
}

TypedValueRef& RegionTraceContext::traceTernaryOp(Op op, Type resultType, const TypedValueRef& first,
                                                  const TypedValueRef& second, const TypedValueRef& third) {
	if (!recording_) {
		return parent_->traceTernaryOp(op, resultType, first, second, third);
	}
	return localTraceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return trace().addOperationWithResult(tag, op, resultType, {first, second, third});
	});
}

void RegionTraceContext::traceReturnOperation(Type type, const TypedValueRef& ref) {
	parent_->traceReturnOperation(type, ref);
}

void RegionTraceContext::traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) {
	if (!recording_) {
		parent_->traceAssignment(target, source, resultType);
		return;
	}
	if (paused_) {
		return;
	}
	if (localIsFollowing()) {
		localFollow(ASSIGN);
		return;
	}
	auto tag = getActiveTracer()->recordSnapshot();
	// See LazyTraceContext::traceAssignment: a tag match here only proves a
	// genuine re-entry if the previously recorded op at this tag assigned the
	// *same* target; otherwise it's an unrelated assignment that merely
	// collided (issue #382) and must be recorded fresh, not merged.
	if (auto it = trace().globalTagMap.find(tag); it != trace().globalTagMap.end()) {
		auto& existing = it->second;
		auto* existingOp = trace().getBlocks()[existing.blockIndex]->operations[existing.operationIndex];
		if (existingOp->op != ASSIGN || existingOp->resultRef.ref != target.ref) {
			trace().addAssignmentOperation(tag, target, source, resultType);
			return;
		}
	}
	if (!trace().checkTag(tag)) {
		paused_ = true;
		return;
	}
	trace().addAssignmentOperation(tag, target, source, resultType);
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
	if (!recording_) {
		return parent_->traceBool(value, probability);
	}
	if (paused_) {
		// In (region-local) passive mode, return false to guarantee this
		// pass's remaining native control flow terminates.
		return false;
	}

	bool result;
	bool shouldTerminate = false;

	if (localCtx_.getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW) {
		auto recordResult = localCtx_.followNoThrow();
		result = recordResult.branchDirection;
		shouldTerminate = recordResult.shouldTerminate;
	} else {
		auto tag = getActiveTracer()->recordSnapshot();
		if (trace().checkTag(tag)) {
			trace().addCmpOperation(tag, value, probability);
			auto recordResult = localCtx_.recordNoThrow(tag);
			result = recordResult.branchDirection;
			shouldTerminate = recordResult.shouldTerminate;
		} else {
			// A position inside this region's own local exploration was
			// already visited (a loop back-edge settling, or a branch merge)
			// within this same pass. Enter region-local passive mode.
			paused_ = true;
			return false;
		}
	}

	if (shouldTerminate) {
		paused_ = true;
		return false;
	}

	auto& currentOperation = trace().getCurrentOperation();
	assert(currentOperation.op == CMP);

	uint32_t nextBlock;
	if (result) {
		nextBlock = std::get<BlockRef*>(currentOperation.input[1])->block;
	} else {
		nextBlock = std::get<BlockRef*>(currentOperation.input[2])->block;
	}
	trace().setCurrentBlock(nextBlock);
	return result;
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
	// On a recording engagement create a dedicated region entry block. On a
	// FOLLOW replay of a non-memoized (open) region the body's ops already live
	// in the trace, so creating a fresh (empty) entry block here would leave the
	// shared cursor in an empty block and the body's follow() reads past its end
	// (crash "Current operation index out of bounds" on regions with internal
	// control flow). The body is transparently followed instead.
	if (recording_) {
		// Only a *nested* region() call (depth > 0) has in-progress state of
		// this instance's to protect. At depth 0 the instance was just
		// constructed by the root's traceRegionBegin, so every member is
		// already pristine and the save + re-construction would be pure waste
		// (a second SymbolicExecutionContext, including its reserve(128)
		// tagMap allocation, per region entry on the common path).
		//
		// callSite_ must be captured here, *before* it is overwritten below:
		// a nested region() otherwise leaves the inner call site in place, so
		// the outer region's traceRegionEnd would file its memo entry under
		// the inner region's key (and a later inner-region entry under the
		// same P would then wrongly hit it and skip to the outer region's
		// continuation).
		if (depth_ > 0) {
			savedLocalStates_.push_back(LocalExplorationState {std::move(localCtx_), callSite_, paused_,
			                                                   entryBlockIndex_, exitBlockIndex_, hasExitBlock_});
			localCtx_ = SymbolicExecutionContext(kExpectedRegionTags);
			paused_ = false;
			hasExitBlock_ = false;
			exitBlockIndex_ = 0;
		}
		++depth_;
		callSite_ = callSite;

		trace().createRegionEntryBlock();
		entryBlockIndex_ = trace().getCurrentBlockIndex();
		// Mirrors the top-level `while (shouldContinue()) { next(); ...; fn(); }`
		// shape: next() must run once before the first pass so iterations != 0
		// by the time traceRegionContinue() checks shouldContinue() below
		// (iterations == 0 would otherwise always report "more to explore",
		// regardless of whether this pass actually recorded a branch).
		localCtx_.next();
	}
	return true;
}

namespace {
// True when @p block is already properly terminated (its last op is a CMP or
// JMP). A pass can end with the cursor sitting in such a block without ever
// hitting a natural `fn()` return there: reaching an already-recorded CMP
// position mid-pass triggers ExecutionTrace::checkTag's own
// processControlFlowMerge side effect, which builds a new merge block ending
// in that CMP (its real successors, still unexplored, get their own later
// local passes) and sets it as the current block before this region's
// paused_ short-circuits the rest of the pass. Appending another jump after
// an existing terminator would produce a block with two exits.
bool blockAlreadyTerminated(const Block& block) {
	if (block.operations.empty()) {
		return false;
	}
	auto op = block.operations.back()->op;
	return op == Op::CMP || op == Op::JMP;
}
} // namespace

bool RegionTraceContext::traceRegionContinue() {
	if (!recording_) {
		return false;
	}
	if (localCtx_.shouldContinue()) {
		// This pass leaves an unresolved decision behind (either the very
		// first pass just recorded a fresh branch, or an earlier pass did and
		// this one explored its other side but hit yet another fresh branch).
		// Converge this pass's tail into a lazily-created exit block — lazy so
		// a branch-free region never allocates one — then rewind for the next
		// pass exactly like ExecutionTrace::resetExecution() does at the
		// top level, scoped to this region's entry instead of block 0.
		if (!blockAlreadyTerminated(trace().getCurrentBlock())) {
			if (!hasExitBlock_) {
				exitBlockIndex_ = trace().createBlock();
				hasExitBlock_ = true;
			}
			trace().appendJump(&trace().getCurrentBlock(), &trace().getBlock(exitBlockIndex_));
		}
		localCtx_.next();
		trace().setCurrentBlock(entryBlockIndex_);
		// aliveVars must not carry an imbalance a paused_-truncated pass left
		// behind (a loop or merge inside the region can end a pass early,
		// skipping the freeValRef calls for temporaries whose destructors run
		// after paused_ was set) into the next pass's own bookkeeping — mirrors
		// LazyTraceContext::resume() resetting it once per top-level pass.
		aliveVars.reset();
		paused_ = false;
		return true;
	}
	if (hasExitBlock_ && !blockAlreadyTerminated(trace().getCurrentBlock())) {
		trace().appendJump(&trace().getCurrentBlock(), &trace().getBlock(exitBlockIndex_));
		trace().setCurrentBlock(exitBlockIndex_);
	}
	// else: either a single pass with no branch ever recorded (leave the
	// cursor exactly where fn() left it, matching today's zero-overhead
	// behavior for the common branch-free region), or the last pass's tail
	// already lands in a CMP/JMP-terminated block with no exit block to
	// converge into anyway (only possible if every pass ended pre-terminated,
	// i.e. hasExitBlock_ is false here too).
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
		getRootContext()->regionMemos[callSite_][P_] =
		    RegionExec {trace().currentOperationIndex, trace().currentBlockIndex};
	}
	// Restore this instance's own in-progress local-exploration state, saved in
	// traceRegionBegin() before a nested region() call reused it. Mirrors that
	// save exactly: only depth > 0 pushed, so only depth > 0 pops.
	--depth_;
	if (depth_ > 0) {
		auto saved = std::move(savedLocalStates_.back());
		savedLocalStates_.pop_back();
		localCtx_ = std::move(saved.localCtx);
		callSite_ = saved.callSite;
		paused_ = saved.paused;
		entryBlockIndex_ = saved.entryBlockIndex;
		exitBlockIndex_ = saved.exitBlockIndex;
		hasExitBlock_ = saved.hasExitBlock;
	}
}

Snapshot RegionTraceContext::recordSnapshot() {
	return {recorder_->createTag(), currentStateHash() ^ P_};
}

} // namespace nautilus::tracing
