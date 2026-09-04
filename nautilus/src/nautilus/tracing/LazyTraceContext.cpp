
#include "LazyTraceContext.hpp"
#include "TraceOperation.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "symbolic_execution/SymbolicExecutionContext.hpp"
#include <cassert>
#include <fmt/format.h>

namespace fmt {
template <>
struct formatter<nautilus::tracing::ExecutionTrace> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::ExecutionTrace& trace, format_context& ctx) -> format_context::iterator;
};
} // namespace fmt

namespace nautilus::tracing {

// Thread-local LazyTraceContext object (not a pointer)
static thread_local LazyTraceContext completingTraceContext;

LazyTraceContext* LazyTraceContext::initialize(TagRecorder& tagRecorder, ExecutionTrace& executionTrace,
                                               SymbolicExecutionContext& symbolicExecutionContext,
                                               const engine::Options& options) {
	completingTraceContext.state.emplace(tagRecorder, executionTrace, symbolicExecutionContext, options);
	completingTraceContext.paused_ = false;
	completingTraceContext.parent_ = nullptr;
	completingTraceContext.session_ = &completingTraceContext;
	setActiveTracer(&completingTraceContext);
	return &completingTraceContext;
}

void LazyTraceContext::resume() {
	staticVars.clear();
	aliveVars.reset();
	activeDestructors.clear();
	paused_ = false;
}

TypedValueRef& LazyTraceContext::registerFunctionArgument(Type type, size_t index) {
	if (paused_) {
		return dummyRef_;
	}
	if (parent_ != nullptr) {
		throw RuntimeException("Invalid region(): a region body has no arguments of its own.");
	}
	return state->executionTrace.setArgument(type, index);
}

bool LazyTraceContext::isFollowing() {
	return state->symbolicExecutionContext.getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW;
}

TypedValueRef& LazyTraceContext::follow([[maybe_unused]] Op op) {
	auto& currentOperation = state->executionTrace.getCurrentOperation();
	auto consumedTag = currentOperation.tag;
	state->executionTrace.nextOperation();
	assert(currentOperation.op == op);
	// traceConstant/traceCopy's globalTagMap-collision branch (see their
	// definitions below) records a *reconciliation* ASSIGN immediately after
	// the primary operation, sharing its exact tag, so that a stale value ref
	// from an earlier, structurally-identical call site is patched to the
	// freshly recorded value -- folding two recorded operations into a single
	// traceConstant()/traceCopy() call. A later FOLLOW-mode replay only ever
	// issues one follow() call per call site, so without this the cursor
	// would desynchronize from the recorded operation stream by one entry as
	// soon as it stepped over such a pair, corrupting every subsequent follow()
	// in the block (this was the root cause of #384). Skip any run of
	// same-tagged reconciliation operations here to keep the cursor aligned.
	while (true) {
		auto& block = state->executionTrace.getCurrentBlock();
		if (state->executionTrace.currentOperationIndex >= block.operations.size()) {
			break;
		}
		if (!(block.operations[state->executionTrace.currentOperationIndex]->tag == consumedTag)) {
			break;
		}
		state->executionTrace.nextOperation();
	}
	return currentOperation.resultRef;
}

TypedValueRef& LazyTraceContext::traceConstant(Type type, const ConstantLiteral& constValue) {
	if (paused_) {
		return dummyRef_;
	}
	log::debug("Trace Constant");
	auto op = Op::CONST;
	if (isFollowing()) {
		return follow(op);
	}
	auto tag = recordSnapshot();
	auto globalTabIter = state->executionTrace.globalTagMap.find(tag);
	if (globalTabIter != state->executionTrace.globalTagMap.end()) {
		auto& ref = globalTabIter->second;
		auto* originalRef = state->executionTrace.getBlocks()[ref.blockIndex]->operations[ref.operationIndex];
		auto resultRef = state->executionTrace.addOperationWithResult(tag, op, type, {constValue});
		state->executionTrace.addAssignmentOperation(tag, originalRef->resultRef, resultRef, resultRef.type);
		return originalRef->resultRef;
	} else {
		return state->executionTrace.addOperationWithResult(tag, op, type, {constValue});
	}
}

template <typename OnCreation>
TypedValueRef& LazyTraceContext::traceOperation(Op op, OnCreation&& onCreation) {
	if (paused_) {
		return dummyRef_;
	}
	if (isFollowing()) {
		return follow(op);
	} else {
		auto tag = recordSnapshot();
		if (state->executionTrace.checkTag(tag)) {
			return onCreation(tag);
		} else {
			// Instead of throwing TraceTerminationException, enter passive mode.
			paused_ = true;
			return dummyRef_;
		}
	}
}

TypedValueRef& LazyTraceContext::traceAlloca(size_t size, size_t align) {
	auto op = Op::ALLOCA;
	auto resultType = Type::ptr;
	return traceOperation(op, [&, size, align](Snapshot& tag) -> TypedValueRef& {
		auto index = state->executionTrace.addAllocaSpec(size, align);
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {index});
	});
}

TypedValueRef& LazyTraceContext::traceCopy(const TypedValueRef& ref) {
	if (paused_) {
		return dummyRef_;
	}
	log::debug("Trace Copy");
	if (isFollowing()) {
		return follow(ASSIGN);
	}
	auto tag = recordSnapshot();
	auto& trace = state->executionTrace;
	auto globalTabIter = trace.globalTagMap.find(tag);
	if (globalTabIter != trace.globalTagMap.end()) {
		// This copy's call site was already reached by a different execution
		// path (e.g. the other arm of a native `cond ? a : b` ternary
		// copy-constructing from a different pre-existing value). The source
		// ref legitimately differs per branch, so reaching the same tag here
		// is not a genuine control-flow reconvergence and must not trigger a
		// premature merge (which would silently discard the diverging copy on
		// this path -- the root cause of the "double jump" miscompilation in
		// issue #95). Record a fresh copy of the current source value, and
		// reconcile it onto the canonical result ref established by the first
		// visit, mirroring traceConstant's handling of the same situation for
		// freshly-constructed literals.
		auto& originalIdentifier = globalTabIter->second;
		auto* originalOp =
		    trace.getBlocks()[originalIdentifier.blockIndex]->operations[originalIdentifier.operationIndex];
		auto resultRef = trace.getNextValueRef();
		trace.addAssignmentOperation(tag, {resultRef, ref.type}, ref, ref.type);
		trace.addAssignmentOperation(tag, originalOp->resultRef, {resultRef, ref.type}, ref.type);
		return originalOp->resultRef;
	}
	if (!trace.checkTag(tag)) {
		// Defer any remaining repeated tag to the control-flow-merge machinery.
		paused_ = true;
		return dummyRef_;
	}
	auto resultRef = trace.getNextValueRef();
	return trace.addAssignmentOperation(tag, {resultRef, ref.type}, ref, ref.type);
}

TypedValueRef& LazyTraceContext::traceCall(void* fptn, Type resultType,
                                           const std::vector<tracing::TypedValueRef>& arguments,
                                           FunctionAttributes fnAttrs) {
	if (paused_) {
		return dummyRef_;
	}
	// Name caches are session-wide: a call traced inside a region must normalize to
	// the same name as the same call traced outside one.
	auto mangledName = session_->getMangledName(fptn);
	auto functionName = session_->getFunctionName(fptn, mangledName);
	auto op = Op::CALL;
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* functionArguments =
		    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
		                                                                        .mangledName = mangledName,
		                                                                        .ptr = fptn,
		                                                                        .arguments = arguments,
		                                                                        .fnAttrs = fnAttrs,
		                                                                        .destructors = {}});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

TypedValueRef& LazyTraceContext::traceCallWithExceptionHandling(void* fptn, Type resultType,
                                                                const std::vector<tracing::TypedValueRef>& arguments,
                                                                FunctionAttributes fnAttrs, void* captureFunc) {
	if (paused_) {
		return dummyRef_;
	}
	auto mangledName = getMangledName(fptn);
	auto functionName = getFunctionName(fptn, mangledName);
	auto op = Op::CALL_WITH_EXCEPTION_HANDLING;
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* functionArguments =
		    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
		                                                                        .mangledName = mangledName,
		                                                                        .ptr = fptn,
		                                                                        .captureFunc = captureFunc,
		                                                                        .arguments = arguments,
		                                                                        .fnAttrs = fnAttrs,
		                                                                        .destructors = activeDestructors});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

TypedValueRef& LazyTraceContext::traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
                                                   const std::vector<tracing::TypedValueRef>& arguments,
                                                   FunctionAttributes fnAttrs, void* captureFunc) {
	if (paused_) {
		return dummyRef_;
	}
	auto op = Op::INDIRECT_CALL;
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* indirectCall = state->executionTrace.getArena().create<IndirectFunctionCall>(
		    IndirectFunctionCall {.fnPtr = fnPtrRef,
		                          .captureFunc = captureFunc,
		                          .arguments = arguments,
		                          .fnAttrs = fnAttrs,
		                          .destructors = {}});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {indirectCall});
	});
}

TypedValueRef&
LazyTraceContext::traceIndirectCallWithExceptionHandling(const TypedValueRef& fnPtrRef, Type resultType,
                                                         const std::vector<tracing::TypedValueRef>& arguments,
                                                         FunctionAttributes fnAttrs, void* captureFunc) {
	if (paused_) {
		return dummyRef_;
	}
	auto op = Op::INDIRECT_CALL_WITH_EXCEPTION_HANDLING;
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* functionArguments = state->executionTrace.getArena().create<IndirectFunctionCall>(
		    IndirectFunctionCall {.fnPtr = fnPtrRef,
		                          .captureFunc = captureFunc,
		                          .arguments = arguments,
		                          .fnAttrs = fnAttrs,
		                          .destructors = activeDestructors});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

const std::string& LazyTraceContext::registerNautilusFunction(const NautilusFunctionDefinition* definition,
                                                              std::function<void()> fwrapper, bool& newlyRegistered) {
	if (const auto it = session_->registeredFunctions.find(definition); it != session_->registeredFunctions.end()) {
		newlyRegistered = false;
		return it->second;
	}
	newlyRegistered = true;

	// Uniquify against names already claimed by *other* definitions. Two
	// NautilusFunctions may legitimately share a name; both must be traced,
	// and each needs its own name so the trace module, the IR and every
	// backend can tell them apart.
	std::string name = definition->name();
	if (session_->usedFunctionNames.contains(name)) {
		const std::string base = name;
		uint32_t suffix = 1;
		do {
			++suffix;
			name = base + "_" + std::to_string(suffix);
		} while (session_->usedFunctionNames.contains(name));
		log::warn("Two distinct NautilusFunctions are named '{}'; tracing the second as '{}'. Give them distinct "
		          "names to keep generated code readable.",
		          base, name);
	}
	session_->usedFunctionNames.insert(name);

	const auto [inserted, _] = session_->registeredFunctions.emplace(definition, std::move(name));
	session_->functionsToTrace.push_back(
	    compiler::CompilableFunction(inserted->second, std::move(fwrapper), definition->attributes(), definition));
	log::debug("Added function '{}' to functionsToTrace list. List now has {} functions", inserted->second,
	           session_->functionsToTrace.size());
	return inserted->second;
}

TypedValueRef& LazyTraceContext::traceNautilusCall(const NautilusFunctionDefinition* definition,
                                                   std::function<void()> fwrapper, Type resultType,
                                                   const std::vector<tracing::TypedValueRef>& arguments,
                                                   FunctionAttributes fnAttrs) {
	if (paused_) {
		return dummyRef_;
	}
	bool newlyRegistered = false;
	const auto& functionName = registerNautilusFunction(definition, fwrapper, newlyRegistered);
	auto op = Op::CALL;
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* functionArguments =
		    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
		                                                                        .mangledName = functionName,
		                                                                        .ptr = (void*) definition,
		                                                                        .kind = CalleeKind::Internal,
		                                                                        .arguments = arguments,
		                                                                        .fnAttrs = fnAttrs,
		                                                                        .destructors = {},
		                                                                        .isNautilusCall = true});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

TypedValueRef& LazyTraceContext::traceNautilusCallWithExceptionHandling(
    const NautilusFunctionDefinition* definition, std::function<void()> fwrapper, Type resultType,
    const std::vector<tracing::TypedValueRef>& arguments, FunctionAttributes fnAttrs) {
	if (paused_) {
		return dummyRef_;
	}
	bool newlyRegistered = false;
	const auto& functionName = registerNautilusFunction(definition, fwrapper, newlyRegistered);
	auto op = Op::CALL_WITH_EXCEPTION_HANDLING;
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* functionArguments =
		    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
		                                                                        .mangledName = functionName,
		                                                                        .ptr = (void*) definition,
		                                                                        .kind = CalleeKind::Internal,
		                                                                        .arguments = arguments,
		                                                                        .fnAttrs = fnAttrs,
		                                                                        .destructors = activeDestructors,
		                                                                        .isNautilusCall = true});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

TypedValueRef& LazyTraceContext::traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
                                                          std::function<void()> fwrapper) {
	if (paused_) {
		return dummyRef_;
	}
	bool newlyRegistered = false;
	const auto& functionName = registerNautilusFunction(definition, std::move(fwrapper), newlyRegistered);
	auto op = Op::FUNC_ADDR;
	auto resultType = Type::ptr;
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* functionArguments =
		    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
		                                                                        .mangledName = functionName,
		                                                                        .ptr = (void*) definition,
		                                                                        .kind = CalleeKind::Internal,
		                                                                        .arguments = {},
		                                                                        .fnAttrs = {},
		                                                                        .destructors = {}});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

void LazyTraceContext::traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) {
	if (paused_) {
		return;
	}
	if (isFollowing()) {
		follow(ASSIGN);
		return;
	}
	auto tag = recordSnapshot();
	auto& trace = state->executionTrace;

	// Tag identity is purely a call-stack return-address chain (TagRecorder.cpp),
	// so a tag match here does not by itself prove this ASSIGN is a genuine
	// control-flow re-entry (a loop back-edge or an if/else merge point): the
	// identical call-stack shape, with an identical alive-variable footprint,
	// can also be reached while assigning a *different* target -- e.g. two
	// structurally-identical sibling arguments of the same call (`f(0, x, 0)`)
	// evaluated by the same argument-evaluation loop, or two slots of the same
	// array assigned via the same loop body. Distinguish the two by checking
	// whether the operation already recorded at this tag assigned the same
	// target: if not, this is an unrelated assignment that merely collided,
	// so record it fresh instead of forcing a bogus merge that would discard
	// everything traced afterwards (issue #382). This mirrors how traceCopy
	// already reconciles instead of merging when a repeated call site's
	// *source* legitimately differs (issue #95/#384).
	if (auto it = trace.globalTagMap.find(tag); it != trace.globalTagMap.end()) {
		auto& existing = it->second;
		auto* existingOp = trace.getBlocks()[existing.blockIndex]->operations[existing.operationIndex];
		if (existingOp->op != ASSIGN || existingOp->resultRef.ref != target.ref) {
			trace.addAssignmentOperation(tag, target, source, resultType);
			return;
		}
	}
	if (!trace.checkTag(tag)) {
		paused_ = true;
		return;
	}
	trace.addAssignmentOperation(tag, target, source, resultType);
}

void LazyTraceContext::traceReturnOperation(Type resultType, const TypedValueRef& ref) {
	if (paused_) {
		return;
	}
	if (parent_ != nullptr) {
		throw RuntimeException("Invalid region(): a region body returns void and cannot return from the enclosing "
		                       "function; assign to a val<T> captured by reference instead.");
	}
	if (isFollowing()) {
		follow(RETURN);
	} else {
		auto tag = recordSnapshot();
		state->executionTrace.addReturn(tag, resultType, ref);
	}
}

TypedValueRef& LazyTraceContext::traceBinaryOp(Op op, Type resultType, const TypedValueRef& left,
                                               const TypedValueRef& right) {
	if (paused_) {
		return dummyRef_;
	}
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {left, right});
	});
}

TypedValueRef& LazyTraceContext::traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) {
	if (paused_) {
		return dummyRef_;
	}
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {input});
	});
}

TypedValueRef& LazyTraceContext::traceTernaryOp(Op op, Type resultType, const TypedValueRef& first,
                                                const TypedValueRef& second, const TypedValueRef& third) {
	if (paused_) {
		return dummyRef_;
	}
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {first, second, third});
	});
}

bool LazyTraceContext::traceBool(const TypedValueRef& value, const double probability) {
	if (paused_) {
		// In passive mode, return false to guarantee loop termination.
		return false;
	}

	bool result;
	bool shouldTerminate = false;

	if (state->symbolicExecutionContext.getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW) {
		auto recordResult = state->symbolicExecutionContext.followNoThrow();
		result = recordResult.branchDirection;
		shouldTerminate = recordResult.shouldTerminate;
	} else {
		// record
		auto tag = recordSnapshot();
		if (state->executionTrace.checkTag(tag)) {
			state->executionTrace.addCmpOperation(tag, value, probability);
			auto recordResult = state->symbolicExecutionContext.recordNoThrow(tag);
			result = recordResult.branchDirection;
			shouldTerminate = recordResult.shouldTerminate;
		} else {
			// Control flow merge/loop detected. Enter passive mode.
			paused_ = true;
			return false;
		}
	}

	if (shouldTerminate) {
		// The symbolic execution signals termination (SecondVisit).
		// Enter passive mode instead of throwing.
		paused_ = true;
		return false;
	}

	auto& currentOperation = state->executionTrace.getCurrentOperation();
	assert(currentOperation.op == CMP);

	uint32_t nextBlock;
	if (result) {
		nextBlock = std::get<BlockRef*>(currentOperation.input[1])->block;
	} else {
		nextBlock = std::get<BlockRef*>(currentOperation.input[2])->block;
	}
	state->executionTrace.setCurrentBlock(nextBlock);
	return result;
}

void LazyTraceContext::runScope(std::function<void()>& body) {
	auto& symbolicExecutionContext = state->symbolicExecutionContext;
	auto traceIteration = 0;
	while (symbolicExecutionContext.shouldContinue()) {
		traceIteration = traceIteration + 1;
		log::trace("Trace Iteration {}", traceIteration);
		log::trace("{}", state->executionTrace);

		// Prepare for the next iteration. Rewinding to entryBlock_ is what
		// resetExecution() does for a function scope (entryBlock_ == 0) and what a
		// region scope needs to replay from its own entry instead of the function's.
		symbolicExecutionContext.next();
		state->executionTrace.setCurrentBlock(entryBlock_);
		resume(); // Reset persistent state (staticVars, aliveVars, paused_)

		// Execute the scope body - it always returns normally.
		body();

		// A region scope has no terminator of its own; see traceScopeExit().
		if (parent_ != nullptr) {
			traceScopeExit();
		}

		// After each iteration, the static variable stack must be empty.
		// Since the body completes normally, all destructors fire in order.
		assert(staticVars.empty() && "static variable stack not empty after tracing iteration");
	}
	log::debug("Scope traced with {} iterations", traceIteration);
}

LazyTraceContext::RegionScopeState& LazyTraceContext::regionState() {
	if (!regionState_) {
		regionState_ = std::make_unique<RegionScopeState>();
	}
	return *regionState_;
}

LazyTraceContext& LazyTraceContext::acquireChildScope() {
	auto& region = regionState();
	if (!region.childScope) {
		region.childScope = std::make_unique<LazyTraceContext>();
	}
	return *region.childScope;
}

void LazyTraceContext::initRegionScope(LazyTraceContext& parent, uint32_t entry, uint32_t exit, TagRecorder& recorder,
                                       const RegionAttributes& attributes) {
	parent_ = &parent;
	session_ = parent.session_;
	entryBlock_ = entry;
	paused_ = false;
	staticVars.clear();
	aliveVars.reset();
	auto& region = regionState();
	region.exitBlock = exit;
	region.attributes = attributes;
	region.exitSnapshot.reset();
	// Regions recorded by a previous engagement of this pooled scope belong to that
	// engagement's body; their entries are unreachable from here.
	region.regionMemo.clear();
	region.symbolicExecutionContext.reset();
	state.emplace(recorder, parent.state->executionTrace, region.symbolicExecutionContext, parent.state->options);
}

void LazyTraceContext::traceScopeExit() {
	// Each of the skipped cases has already been terminated by other machinery: a pass
	// paused by a control-flow merge had its jump added by processControlFlowMerge, a
	// pass paused by a second visit ends on the CMP that paused it, and a pass that ran
	// entirely in FOLLOW mode is replaying a marker that is already recorded.
	if (paused_ || isFollowing()) {
		return;
	}
	auto& region = regionState();

	// Nothing created inside a region may outlive it. A val<T> constructed in the body
	// and still held when the body returns owns a ref this scope allocated, and there is
	// no way to hand that to the enclosing scope correctly: on a replay pass the body is
	// skipped, so the C++ object is never constructed, and once the enclosing scope's
	// exploration flips from FOLLOW to RECORD every operation it records afterwards would
	// take its inputs from an object that does not exist. Assigning to a val<T> declared
	// outside the region has none of that problem -- that ref is allocated before the
	// region and stays stable across it -- which is why it is the supported way to carry
	// a value out. See docs/region.md.
	if (aliveVars.size() > 0) {
		std::string escaped;
		aliveVars.forEachAlive([&escaped](uint32_t ref, uint32_t) {
			escaped += (escaped.empty() ? "" : ", ") + std::string("$") + std::to_string(ref);
		});
		throw RuntimeException(
		    "Invalid region() " + region.attributes.toString() +
		    ": a value created inside the region body outlives it (" + escaped +
		    "). Carry the value out through a val<T> declared outside the region and assigned to inside it, or trace "
		    "this function with engine.traceMode = \"exceptionBasedTracing\".");
	}

	auto snapshot = recordSnapshot();
	if (!region.exitSnapshot.has_value()) {
		region.exitSnapshot = snapshot;
	} else if (*region.exitSnapshot != snapshot) {
		// Same escape set, different snapshot: the remaining input to the hash is the
		// static-variable stack, so a captured static_val was written inside the body.
		throw RuntimeException(
		    "Invalid region() " + region.attributes.toString() +
		    ": the state alive at the end of the region body differs between the paths through it, so what escapes the "
		    "region would depend on which path was explored last. Build the escaping value on every path (assigning to "
		    "a val<T> declared outside the region merges across branches), or trace this function with "
		    "engine.traceMode = \"exceptionBasedTracing\".");
	}
	auto& trace = state->executionTrace;
	if (!trace.checkTag(snapshot)) {
		// This pass's tail was merged with an earlier pass's exit. The pass is over.
		paused_ = true;
		return;
	}
	trace.getBlock(region.exitBlock).predecessors.emplace_back(trace.getCurrentBlockIndex());
	trace.addJumpOperation(snapshot, region.exitBlock);
}

void LazyTraceContext::traceRegion(std::function<void()>& regionFunction, const RegionAttributes& attributes) {
	if (paused_) {
		return;
	}
	auto& trace = state->executionTrace;
	// A region is an operation to its enclosing scope and a function to its own body:
	// identified at its call site exactly like any other traced operation.
	auto key = recordSnapshot();

	if (isFollowing()) {
		// Memoized replay: the body was fully explored when it was first reached, so
		// skip it and continue where it handed control back. The tagged jump recorded
		// below is invisible here - the cursor traverses JMPs transparently - which is
		// why this looks the region up by key rather than following the operation.
		auto& memo = regionState().regionMemo;
		auto memoized = memo.find(key);
		if (memoized == memo.end()) {
			throw RuntimeException("Invalid region() " + attributes.toString() +
			                       ": replaying a recorded path reached a region() call site that was not recorded "
			                       "there. Trace this function with engine.traceMode = \"exceptionBasedTracing\", "
			                       "which traces region bodies inline.");
		}
		trace.setCurrentBlock(memoized->second.exitBlock);
		return;
	}

	if (!trace.checkTag(key)) {
		// Re-entering the same region call site in the same state is a control-flow
		// re-entry (a loop around the region); checkTag has merged, so this pass ends.
		paused_ = true;
		return;
	}

	auto entry = trace.createBlock();
	auto exit = trace.createBlock();
	trace.getBlock(entry).predecessors.emplace_back(trace.getCurrentBlockIndex());
	trace.addJumpOperation(key, entry);
	trace.setCurrentBlock(entry);

	// The recorder is rooted at this frame's return address, so tags inside the body are
	// the call path *from the region entry* and cannot collide with the enclosing
	// scope's. It is owned by the session because the Tag* it mints outlive the region.
	auto& recorder = session_->regionState().tagRecorders.emplace_back(
	    reinterpret_cast<TagAddress>(__builtin_return_address(0)), trace.getArena());
	auto& child = acquireChildScope();
	child.initRegionScope(*this, entry, exit, recorder, attributes);

	// The region's attributes are metadata on the enclosing trace, not an operation in it:
	// recorded once, here, against the blocks that bound the body about to be traced.
	trace.addRegion(attributes, entry, exit);

	setActiveTracer(&child);
	try {
		child.runScope(regionFunction);
	} catch (...) {
		setActiveTracer(this);
		throw;
	}
	setActiveTracer(this);

	if (trace.getBlock(exit).predecessors.empty()) {
		// No pass of the body ever ran to completion, so nothing reaches the block the
		// enclosing scope is about to continue in. Diagnose it here rather than let a
		// later phase fail on an unreachable block.
		throw RuntimeException("Invalid region() " + attributes.toString() +
		                       ": no path through the region body reached its end, so the enclosing function cannot "
		                       "continue after it.");
	}

	child.state.reset();

	trace.setCurrentBlock(exit);
	regionState().regionMemo[key] = RegionRecord {entry, exit};
}

std::unique_ptr<ExecutionTrace> LazyTraceContext::trace(std::function<void()>& traceFunction,
                                                        const engine::Options& options, Arena& arena) {
	log::debug("Initialize Completing Tracing");
	auto rootAddress = __builtin_return_address(0);
	auto tr = tracing::TagRecorder((tracing::TagAddress) rootAddress, arena);

	// The ExecutionTrace borrows the caller-provided arena for all
	// allocations; the arena must outlive the returned trace.
	auto executionTrace = std::make_unique<ExecutionTrace>(arena);
	SymbolicExecutionContext symbolicExecutionContext;

	// Initialize LazyTraceContext with references to our objects
	auto tc = initialize(tr, *executionTrace, symbolicExecutionContext, options);
	// Ensure the thread-local active tracer is cleared even if an exception
	// (e.g. RuntimeException from ExecutionTrace or from the traced function)
	// escapes the loop below - this variant has no try/catch by design.
	ActiveTracerGuard activeTracerGuard;

	// Symbolic execution loop: explore all execution paths.
	// No try/catch needed - the traced function always returns normally.
	if (tc->regionState_) {
		tc->regionState_->tagRecorders.clear();
		tc->regionState_->regionMemo.clear();
	}
	tc->entryBlock_ = 0;
	tc->parent_ = nullptr;
	tc->runScope(traceFunction);

	// Clean up: reset state pointer. activeTracer is cleared by ActiveTracerGuard.
	tc->state.reset();

	log::trace("Final trace: {}", *executionTrace);

	return executionTrace;
}

std::unique_ptr<TraceModule> LazyTraceContext::Trace(std::list<compiler::CompilableFunction>& functions,
                                                     const engine::Options& options, Arena& arena) {
	return completingTraceContext.startTrace(functions, options, arena);
}

std::unique_ptr<TraceModule> LazyTraceContext::startTrace(std::list<compiler::CompilableFunction>& functions,
                                                          const engine::Options& options, Arena& arena) {
	log::debug("Initialize Lazy Tracing");
	auto traceModule = std::make_unique<TraceModule>();
	functionsToTrace = functions;
	registeredFunctions.clear();
	usedFunctionNames.clear();
	setActiveTracer(this);
	// Ensure the thread-local active tracer is cleared even if an exception
	// escapes the per-function loop below.
	ActiveTracerGuard activeTracerGuard;

	bool isFirstFunction = true;
	while (!functionsToTrace.empty()) {
		auto currentFunction = functionsToTrace.front();
		functionsToTrace.pop_front();
		if (traceModule->hasFunction(currentFunction.getName())) {
			// Already traced under this name -- typically a NautilusFunction
			// sharing a name with a module-registered entry function. Record
			// this identity against the existing body anyway: its call sites
			// mint a function-table entry keyed on it, and that entry has to
			// resolve to the same function as the body's own.
			traceModule->addFunctionDefinition(currentFunction.getName(), currentFunction.getDefinition());
			log::debug("Function '{}' already traced, skipping.", currentFunction.getName());
			continue;
		}

		auto& executionTrace = traceModule->addNewFunction(currentFunction.getName(), arena);
		// Tag the first popped function as the entry point. Everything else in
		// the queue was `invoke`d by some other traced function; that alone
		// cannot tell us host-vs-device side (a plain NautilusFunction may be
		// invoked from either), so non-entry classification is left to a
		// downstream call-graph pass. `kernel` rides through unchanged from
		// NautilusKernelFunction.
		auto attributes = currentFunction.getAttributes();
		if (isFirstFunction) {
			attributes["entry"] = "true";
			isFirstFunction = false;
		}
		traceModule->setFunctionAttributes(currentFunction.getName(), attributes);
		// Carry the definition identity through to IR conversion, which uses it
		// to bind this body to the function-table id its call sites minted.
		traceModule->addFunctionDefinition(currentFunction.getName(), currentFunction.getDefinition());
		auto wrapperFunc = currentFunction.getFunction();

		auto rootAddress = __builtin_return_address(0);
		auto tr = tracing::TagRecorder((tracing::TagAddress) rootAddress, arena);
		SymbolicExecutionContext symbolicExecutionContext;
		state.emplace(tr, executionTrace, symbolicExecutionContext, options);

		// Region bookkeeping is scoped to one function trace: the recorders because the
		// Tag* they mint are only referenced by this function's trace, the memo because
		// its keys are those tags.
		if (regionState_) {
			regionState_->tagRecorders.clear();
			regionState_->regionMemo.clear();
		}
		entryBlock_ = 0;
		parent_ = nullptr;
		runScope(wrapperFunc);

		state.reset();
		log::trace("Final trace: {}", executionTrace);
	}

	// activeTracer is cleared by ActiveTracerGuard.
	return traceModule;
}

void LazyTraceContext::allocateValRef(ValueRef ref) {
	if (paused_) {
		return;
	}
	aliveVars.increment(ref);
}

void LazyTraceContext::freeValRef(ValueRef ref) {
	if (paused_) {
		return;
	}
	if (parent_ != nullptr && !aliveVars.isAlive(ref)) {
		// A value created outside this region and released inside it (e.g. moved into a
		// region-local variable) is counted by the scope that allocated it. Walk out to
		// that scope rather than decrementing a count this one never took.
		for (auto* scope = parent_; scope != nullptr; scope = scope->parent_) {
			if (scope->aliveVars.isAlive(ref)) {
				scope->aliveVars.decrement(ref);
				return;
			}
		}
	}
	aliveVars.decrement(ref);
}

void LazyTraceContext::pushStaticVal(void* valPtr, size_t size) {
	// Always maintain the static variable stack, even in passive mode.
	// Static variables may have been pushed before entering passive mode,
	// and their destructors will call popStaticVal after.
	staticVars.emplace_back(valPtr, size);
	if (!paused_ && log::options::getLogStaticVars()) {
		log::info("pushStaticVal: [{}]", formatStaticVars());
	}
}

void LazyTraceContext::popStaticVal() {
	// Always maintain the static variable stack, even in passive mode.
	if (!paused_ && log::options::getLogStaticVars()) {
		log::info("popStaticVal: [{}] (popping last)", formatStaticVars());
	}
	staticVars.pop_back();
}

std::string LazyTraceContext::formatStaticVars() const {
	std::string result;
	for (size_t i = 0; i < staticVars.size(); i++) {
		if (i > 0) {
			result += ", ";
		}
		result += std::to_string(getStaticVarValue(staticVars[i]));
	}
	return result;
}

Snapshot LazyTraceContext::recordSnapshot() {
	return {state->tagRecorder.createTag(), hashStaticVector(staticVars) ^ aliveVars.hash()};
}

} // namespace nautilus::tracing
