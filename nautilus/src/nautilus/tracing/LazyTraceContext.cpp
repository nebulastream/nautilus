
#include "LazyTraceContext.hpp"
#include "TraceOperation.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include <cassert>
#include <fmt/format.h>

namespace fmt {
template <>
struct formatter<nautilus::tracing::ExecutionTrace> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::ExecutionTrace& trace, format_context& ctx) -> format_context::iterator;
};
} // namespace fmt

namespace nautilus::tracing {

namespace {
std::string formatStaticVarsOf(const std::vector<StaticVarHolder>& vars) {
	std::string result;
	for (size_t i = 0; i < vars.size(); i++) {
		if (i > 0) {
			result += ", ";
		}
		result += std::to_string(getStaticVarValue(vars[i]));
	}
	return result;
}

// True when @p block is already properly terminated (its last op is a CMP or
// JMP). A region-local pass can end with the cursor sitting in such a block
// without ever hitting a natural `fn()` return there: reaching an
// already-recorded CMP position mid-pass triggers ExecutionTrace::checkTag's
// own processControlFlowMerge side effect, which builds a new merge block
// ending in that CMP (its real successors, still unexplored, get their own
// later local passes) and sets it as the current block before this region's
// paused state short-circuits the rest of the pass. Appending another jump
// after an existing terminator would produce a block with two exits.
bool blockAlreadyTerminated(const Block& block) {
	if (block.operations.empty()) {
		return false;
	}
	auto op = block.operations.back()->op;
	return op == Op::CMP || op == Op::JMP;
}

// Reports a malformed trace as an actionable error instead of the crash or
// silent corruption it would otherwise cause several phases downstream.
// Region-local exploration (see traceRegionContinue) is the only thing that
// builds blocks outside the linear top-level flow, so a break in these
// invariants is attributable to a region() in the traced function.
//
// These are backstops, not an expression of what regions support: a region
// body may contain arbitrary control flow, with or without further operations
// after it, and may carry live values out. If one of these does fire it is a
// bug in region tracing rather than a usage error, so the message says so and
// names the escape hatch.
[[noreturn]] void throwMalformedRegionTrace(const std::string& detail) {
	throw RuntimeException(
	    "Malformed trace built by region() under engine.traceMode = \"lazyTracing\": " + detail +
	    " This is a bug in region-local branch exploration, not a limit on what a region body may contain; "
	    "continuing would emit a trace later phases cannot consume. Setting engine.traceMode = "
	    "\"exceptionBasedTracing\" traces the same code without region-local exploration (and without its "
	    "branch-tracing speedup) and is a workaround. Please report the traced function. See docs/region.md.");
}

// Verifies the shape invariants the rest of the pipeline relies on. An empty
// block reaches ConstantBranchFoldingPass as an unchecked operations.back(),
// so leaving one behind turns into a segfault several phases later, with
// nothing left pointing back at the region that produced it.
void verifyNoEmptyBlocks(ExecutionTrace& trace) {
	const auto& blocks = trace.getBlocks();
	for (size_t index = 0; index < blocks.size(); index++) {
		if (blocks[index]->operations.empty()) {
			throwMalformedRegionTrace("basic block " + std::to_string(index) + " was left empty and unterminated.");
		}
	}
}
} // namespace

// Thread-local LazyTraceContext object (not a pointer)
static thread_local LazyTraceContext completingTraceContext;

LazyTraceContext* LazyTraceContext::initialize(TagRecorder& tagRecorder, ExecutionTrace& executionTrace,
                                               SymbolicExecutionContext& symbolicExecutionContext,
                                               const engine::Options& options) {
	completingTraceContext.state.emplace(tagRecorder, executionTrace, symbolicExecutionContext, options);
	completingTraceContext.paused_ = false;
	setActiveTracer(&completingTraceContext);
	return &completingTraceContext;
}

void LazyTraceContext::resume() {
	setActiveTracer(this);
	staticVars.clear();
	// Region memo/recorder caches are cleared here at the top of EVERY
	// symbolic-execution pass (not just at trace start): a RegionExec's
	// continuation indices reference the trace layout of the pass that recorded
	// it, and the P inputs (staticVars, aliveVars) are reset per pass. Keeping a
	// stale entry across the RECORD->FOLLOW boundary could jump the replay cursor
	// to a continuation that no longer matches the current parent state (hash
	// collision / ABA). Safety, not hygiene (see docs/region.md).
	regionRecorders.clear();
	regionMemos.clear();
	// Any region frames left active from an abandoned pass (e.g. a nested trace
	// unwound through an exception from elsewhere) must not leak into the next
	// pass; the pooled RegionFrame storage itself is untouched and reused via
	// reinitialize() on the next traceRegionBegin(). No-op on the nominal path,
	// where every region already popped its own frame via traceRegionEnd().
	activeRegionDepth_ = 0;
	tracedAnyRegion_ = false;
	aliveVars.reset();
	paused_ = false;
}

TypedValueRef& LazyTraceContext::registerFunctionArgument(Type type, size_t index) {
	if (paused_) {
		return dummyRef_;
	}
	return state->executionTrace.setArgument(type, index);
}

bool LazyTraceContext::isFollowing() {
	return state->symbolicExecutionContext.getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW;
}

TypedValueRef& LazyTraceContext::follow(Op op) {
	auto& currentOperation = state->executionTrace.getCurrentOperation();
	auto consumedTag = currentOperation.tag;
	state->executionTrace.nextOperation();
	if (currentOperation.op != op) [[unlikely]] {
		// The replay cursor no longer lines up with the recorded operation
		// stream. When this function used region(), that is the known
		// region-shape limitation surfacing (a region's recorded continuation
		// no longer matches what replay walks into); report it as such while
		// the cause is still attributable. Otherwise it is an internal
		// invariant break, which was previously only an assert -- and so went
		// silently unnoticed in release builds, corrupting the trace instead.
		if (tracedAnyRegion_) {
			throwMalformedRegionTrace("replaying the trace desynchronized from the recorded operation stream.");
		}
		throw RuntimeException("Trace replay desynchronized from the recorded operation stream.");
	}
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
	// Shared verbatim by both root-level and region-local tracing: both operate
	// on this same state->executionTrace cursor, so a single implementation
	// suffices (unlike the old separate RegionTraceContext::localFollow, which
	// was this exact function operating on the identical object via a
	// differently-named accessor).
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

bool LazyTraceContext::currentlyFollowing() {
	return currentSymbolicExecutionContext().getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW;
}

TypedValueRef& LazyTraceContext::traceConstant(Type type, const ConstantLiteral& constValue) {
	if (currentPaused()) {
		return dummyRef_;
	}
	log::debug("Trace Constant");
	auto op = Op::CONST;
	if (currentlyFollowing()) {
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
	}
	return state->executionTrace.addOperationWithResult(tag, op, type, {constValue});
}

template <typename OnCreation>
TypedValueRef& LazyTraceContext::traceOperation(Op op, OnCreation&& onCreation) {
	if (currentPaused()) {
		return dummyRef_;
	}
	if (currentlyFollowing()) {
		return follow(op);
	}
	auto tag = recordSnapshot();
	if (state->executionTrace.checkTag(tag)) {
		return onCreation(tag);
	}
	setCurrentPaused(true);
	return dummyRef_;
}

template <typename OnCreation>
TypedValueRef& LazyTraceContext::rootTraceOperation(Op op, OnCreation&& onCreation) {
	if (paused_) {
		return dummyRef_;
	}
	if (isFollowing()) {
		return follow(op);
	}
	auto tag = recordSnapshot();
	if (state->executionTrace.checkTag(tag)) {
		return onCreation(tag);
	}
	paused_ = true;
	return dummyRef_;
}

TypedValueRef& LazyTraceContext::traceAlloca(size_t size, size_t align) {
	auto op = Op::ALLOCA;
	auto resultType = Type::ptr;
	return rootTraceOperation(op, [&, size, align](Snapshot& tag) -> TypedValueRef& {
		auto index = state->executionTrace.addAllocaSpec(size, align);
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {index});
	});
}

TypedValueRef& LazyTraceContext::traceCopy(const TypedValueRef& ref) {
	if (currentPaused()) {
		return dummyRef_;
	}
	log::debug("Trace Copy");
	if (currentlyFollowing()) {
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
		setCurrentPaused(true);
		return dummyRef_;
	}
	auto resultRef = trace.getNextValueRef();
	return trace.addAssignmentOperation(tag, {resultRef, ref.type}, ref, ref.type);
}

TypedValueRef& LazyTraceContext::traceCall(void* fptn, Type resultType,
                                           const std::vector<tracing::TypedValueRef>& arguments,
                                           FunctionAttributes fnAttrs) {
	auto mangledName = getMangledName(fptn);
	auto functionName = getFunctionName(fptn, mangledName);
	auto op = Op::CALL;
	return rootTraceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* functionArguments =
		    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
		                                                                        .mangledName = mangledName,
		                                                                        .ptr = fptn,
		                                                                        .arguments = arguments,
		                                                                        .fnAttrs = fnAttrs});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

TypedValueRef& LazyTraceContext::traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
                                                   const std::vector<tracing::TypedValueRef>& arguments,
                                                   FunctionAttributes fnAttrs) {
	auto op = Op::INDIRECT_CALL;
	return rootTraceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* indirectCall = state->executionTrace.getArena().create<IndirectFunctionCall>(
		    IndirectFunctionCall {.fnPtr = fnPtrRef, .arguments = arguments, .fnAttrs = fnAttrs});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {indirectCall});
	});
}

TypedValueRef& LazyTraceContext::traceNautilusCall(const NautilusFunctionDefinition* definition,
                                                   std::function<void()> fwrapper, Type resultType,
                                                   const std::vector<tracing::TypedValueRef>& arguments,
                                                   FunctionAttributes fnAttrs) {
	if (paused_) {
		return dummyRef_;
	}
	auto functionName = definition->name();
	auto mangledName = getMangledName((void*) definition);
	if (registeredFunctions.insert(functionName).second) {
		functionsToTrace.push_back(compiler::CompilableFunction(functionName, fwrapper, definition->attributes()));
		log::debug("Added function '{}' to functionsToTrace list. List now has {} functions", functionName,
		           functionsToTrace.size());
	}
	auto op = Op::CALL;
	return rootTraceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* functionArguments =
		    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
		                                                                        .mangledName = functionName,
		                                                                        .ptr = (void*) definition,
		                                                                        .arguments = arguments,
		                                                                        .fnAttrs = fnAttrs});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

TypedValueRef& LazyTraceContext::traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
                                                          std::function<void()> fwrapper) {
	if (paused_) {
		return dummyRef_;
	}
	auto functionName = definition->name();
	if (registeredFunctions.insert(functionName).second) {
		functionsToTrace.push_back(
		    compiler::CompilableFunction(functionName, std::move(fwrapper), definition->attributes()));
		log::debug("Added function '{}' to functionsToTrace list (via FUNC_ADDR). List now has {} functions",
		           functionName, functionsToTrace.size());
	}
	auto op = Op::FUNC_ADDR;
	auto resultType = Type::ptr;
	return rootTraceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* functionArguments =
		    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
		                                                                        .mangledName = functionName,
		                                                                        .ptr = (void*) definition,
		                                                                        .arguments = {},
		                                                                        .fnAttrs = {}});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

void LazyTraceContext::traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) {
	if (currentPaused()) {
		return;
	}
	if (currentlyFollowing()) {
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
		setCurrentPaused(true);
		return;
	}
	trace.addAssignmentOperation(tag, target, source, resultType);
}

void LazyTraceContext::traceReturnOperation(Type resultType, const TypedValueRef& ref) {
	if (paused_) {
		return;
	}
	if (isFollowing()) {
		follow(RETURN);
		return;
	}
	auto tag = recordSnapshot();
	state->executionTrace.addReturn(tag, resultType, ref);
}

TypedValueRef& LazyTraceContext::traceBinaryOp(Op op, Type resultType, const TypedValueRef& left,
                                               const TypedValueRef& right) {
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {left, right});
	});
}

TypedValueRef& LazyTraceContext::traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) {
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {input});
	});
}

TypedValueRef& LazyTraceContext::traceTernaryOp(Op op, Type resultType, const TypedValueRef& first,
                                                const TypedValueRef& second, const TypedValueRef& third) {
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {first, second, third});
	});
}

bool LazyTraceContext::traceBool(const TypedValueRef& value, const double probability) {
	if (currentPaused()) {
		// In passive mode, return false to guarantee loop termination.
		return false;
	}

	bool result;
	bool shouldTerminate = false;
	auto& symCtx = currentSymbolicExecutionContext();

	if (symCtx.getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW) {
		auto recordResult = symCtx.followNoThrow();
		result = recordResult.branchDirection;
		shouldTerminate = recordResult.shouldTerminate;
	} else {
		// record
		auto tag = recordSnapshot();
		if (state->executionTrace.checkTag(tag)) {
			state->executionTrace.addCmpOperation(tag, value, probability);
			auto recordResult = symCtx.recordNoThrow(tag);
			result = recordResult.branchDirection;
			shouldTerminate = recordResult.shouldTerminate;
		} else {
			// Control flow merge/loop detected. Enter passive mode.
			setCurrentPaused(true);
			return false;
		}
	}

	if (shouldTerminate) {
		// The symbolic execution signals termination (SecondVisit).
		// Enter passive mode instead of throwing.
		setCurrentPaused(true);
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
	auto traceIteration = 0;

	// Symbolic execution loop: explore all execution paths
	// No try/catch needed - the traced function always returns normally
	while (symbolicExecutionContext.shouldContinue()) {
		traceIteration = traceIteration + 1;
		log::trace("Completing Trace Iteration {}", traceIteration);
		log::trace("{}", *executionTrace);

		// Prepare for next iteration
		symbolicExecutionContext.next();
		executionTrace->resetExecution();
		tc->resume(); // Reset persistent state (staticVars, aliveVars, paused_)

		// Execute the traced function - it always returns normally
		traceFunction();

		// After each iteration, the static variable stack must be empty.
		// Since the function completes normally, all destructors fire in order.
		assert(completingTraceContext.staticVars.empty() && "static variable stack not empty after tracing iteration");
	}

	// Clean up: reset state pointer. activeTracer is cleared by ActiveTracerGuard.
	tc->state.reset();

	log::debug("Completing Tracing Terminated with {} iterations", traceIteration);
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
	regionRecorders.clear();
	regionMemos.clear();
	setActiveTracer(this);
	// Ensure the thread-local active tracer is cleared even if an exception
	// escapes the per-function loop below.
	ActiveTracerGuard activeTracerGuard;

	bool isFirstFunction = true;
	while (!functionsToTrace.empty()) {
		auto currentFunction = functionsToTrace.front();
		functionsToTrace.pop_front();
		if (traceModule->hasFunction(currentFunction.getName())) {
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
		auto wrapperFunc = currentFunction.getFunction();

		auto rootAddress = __builtin_return_address(0);
		auto tr = tracing::TagRecorder((tracing::TagAddress) rootAddress, arena);
		SymbolicExecutionContext symbolicExecutionContext;
		state.emplace(tr, executionTrace, symbolicExecutionContext, options);
		auto traceIteration = 0;

		while (symbolicExecutionContext.shouldContinue()) {
			traceIteration = traceIteration + 1;
			log::trace("Lazy Trace Iteration {}", traceIteration);
			log::trace("{}", executionTrace);
			symbolicExecutionContext.next();
			executionTrace.resetExecution();
			resume();
			wrapperFunc();
			assert(staticVars.empty() && "static variable stack not empty after tracing iteration");
		}

		state.reset();
		log::debug("Lazy Tracing Terminated with {} iterations", traceIteration);
		log::trace("Final trace: {}", executionTrace);
		verifyNoEmptyBlocks(executionTrace);
	}

	// activeTracer is cleared by ActiveTracerGuard.
	return traceModule;
}

void LazyTraceContext::allocateValRef(ValueRef ref) {
	if (inActiveRegion()) {
		// Unconditional, unlike the root branch below: the old RegionTraceContext
		// never gated its own allocateValRef/freeValRef on any paused flag,
		// because aliveVars must stay accurate to real C++ scope lifetimes
		// (which keep happening regardless of tracing being paused) for
		// traceRegionEnd's escape/memoization decision to be correct.
		topFrame().aliveVars.increment(ref);
		return;
	}
	if (paused_) {
		return;
	}
	aliveVars.increment(ref);
}

void LazyTraceContext::freeValRef(ValueRef ref) {
	if (inActiveRegion()) {
		topFrame().aliveVars.decrement(ref);
		return;
	}
	if (paused_) {
		return;
	}
	aliveVars.decrement(ref);
}

void LazyTraceContext::pushStaticVal(void* valPtr, size_t size) {
	// Always maintain the static variable stack, even in passive mode.
	// Static variables may have been pushed before entering passive mode,
	// and their destructors will call popStaticVal after.
	auto env = currentEnv();
	env.staticVars.emplace_back(valPtr, size);
	bool shouldLog = inActiveRegion() || !paused_;
	if (shouldLog && log::options::getLogStaticVars()) {
		log::info("pushStaticVal: [{}]", formatStaticVarsOf(env.staticVars));
	}
}

void LazyTraceContext::popStaticVal() {
	// Always maintain the static variable stack, even in passive mode.
	auto env = currentEnv();
	bool shouldLog = inActiveRegion() || !paused_;
	if (shouldLog && log::options::getLogStaticVars()) {
		log::info("popStaticVal: [{}] (popping last)", formatStaticVarsOf(env.staticVars));
	}
	env.staticVars.pop_back();
}

TraceContextBase* LazyTraceContext::getRootContext() {
	return this;
}

bool LazyTraceContext::traceRegionBegin(TagAddress callSite) {
	tracedAnyRegion_ = true;
	bool recording;
	uint64_t P;
	if (!inActiveRegion()) {
		// Top-level entry: memoized replay only applies here (see
		// docs/region.md) -- a region nested inside another region is never
		// memo-checked (matches the pre-flattening design, where a nested
		// region() call reused the *same* RegionTraceContext instance and its
		// traceRegionBegin had no memo lookup of its own at all).
		P = currentStateHash();
		if (auto siteIter = regionMemos.find(callSite); siteIter != regionMemos.end()) {
			if (auto memoIter = siteIter->second.find(P); memoIter != siteIter->second.end()) {
				// Memo hit: skip the region body. In FOLLOW mode jump the shared
				// cursor onto the recorded continuation before resuming following.
				// In RECORD mode nothing is emitted; the next post-region operation
				// merges via the existing control-flow-merge machinery.
				if (isFollowing()) {
					auto& continuation = memoIter->second;
					state->executionTrace.currentBlockIndex = continuation.continuationBlockIndex;
					state->executionTrace.currentOperationIndex = continuation.continuationOperationIndex;
				}
				return false;
			}
		}
		recording = !isFollowing();
	} else {
		// Nested region(): P and recording are inherited unchanged from the
		// enclosing frame, not recomputed -- see RegionFrame::recording's
		// comment for why (this exactly mirrors the pre-flattening design,
		// where a nested call reused the same RegionTraceContext instance and
		// so never touched its own P_/recording_ either).
		auto& outer = topFrame();
		P = outer.P;
		recording = outer.recording;
	}

	const bool nested = inActiveRegion();
	// A nested region shares the enclosing region's tag recorder as well as its
	// P: the exception-based substrate serves a whole nesting chain from one
	// context, so tag identity there does not change on the way in, and the
	// enclosing region's replay of a later pass has to walk the same tags it
	// recorded on the first one.
	TagRecorder* recorder = nullptr;
	if (nested) {
		recorder = topFrame().recorder;
	} else {
		auto recorderNode = regionRecorders.try_emplace(callSite, callSite, state->executionTrace.getArena());
		recorder = &recorderNode.first->second;
	}

	if (activeRegionDepth_ == regionFramePool_.size()) {
		regionFramePool_.emplace_back();
	}
	auto& frame = regionFramePool_[activeRegionDepth_];
	frame.reinitialize(recorder, callSite, P, recording);
	// Only a top-level region opens an exploration loop. A nested one defers to
	// the enclosing region's: giving it a loop of its own meant that, on the
	// enclosing region's second pass, the nested body started a fresh recording
	// while the shared trace cursor was mid-replay, desynchronizing follow().
	frame.explores = recording && !nested;
	++activeRegionDepth_;

	if (frame.explores) {
		state->executionTrace.createRegionEntryBlock();
		frame.entryBlockIndex = state->executionTrace.getCurrentBlockIndex();
		// Mirrors the top-level `while (shouldContinue()) { next(); ...; fn(); }`
		// shape: next() must run once before the first pass so iterations != 0
		// by the time traceRegionContinue() checks shouldContinue() below
		// (iterations == 0 would otherwise always report "more to explore",
		// regardless of whether this pass actually recorded a branch).
		frame.localCtx.next();
	}
	return true;
}

bool LazyTraceContext::traceRegionContinue() {
	auto& f = topFrame();
	if (!f.explores) {
		// Not this frame's loop to drive: either a replay, or a nested region
		// whose exploration is the enclosing region's business.
		return false;
	}
	auto& trace = state->executionTrace;
	if (f.localCtx.shouldContinue()) {
		// This pass leaves an unresolved decision behind (either the very
		// first pass just recorded a fresh branch, or an earlier pass did and
		// this one explored its other side but hit yet another fresh branch).
		// Converge this pass's tail into a lazily-created exit block -- lazy so
		// a branch-free region never allocates one -- then rewind for the next
		// pass exactly like ExecutionTrace::resetExecution() does at the
		// top level, scoped to this region's entry instead of block 0.
		// Only remember this pass's open tail; do not wire it anywhere yet.
		// Creating an exit block here and jumping into it eagerly is what
		// corrupted regions whose body continues past its branch: the
		// operations that follow are reached again on the next pass, so
		// ExecutionTrace::checkTag's control-flow-merge machinery joins the
		// arms into a merge block of its own and repoints these tails at it --
		// leaving the region's exit block dead and empty, while its own stray
		// jump appended a second terminator to the merge block.
		if (!blockAlreadyTerminated(trace.getCurrentBlock())) {
			f.pendingTails.push_back(trace.getCurrentBlockIndex());
		}
		f.localCtx.next();
		trace.setCurrentBlock(f.entryBlockIndex);
		// Deliberately not reset: values created by this pass that are still
		// alive belong to an enclosing C++ scope and will still be released
		// there, so their counts have to survive into the next pass. Zeroing
		// them here made that eventual release underflow the count.
		f.paused = false;
		return true;
	}
	// Decide convergence once, now that every pass has run and the
	// control-flow-merge machinery has had its say. Only tails still lacking a
	// terminator need joining; any it already repointed at its own merge block
	// are finished, and that merge block is then the real continuation.
	std::vector<uint32_t> openTails;
	for (auto tail : f.pendingTails) {
		if (!blockAlreadyTerminated(trace.getBlock(tail))) {
			openTails.push_back(tail);
		}
	}
	if (!openTails.empty()) {
		// Arms still ending mid-air: the body stopped at its branch, so nothing
		// after it forced a merge. Join them, and the current tail with them,
		// into one continuation for the enclosing function to carry on from.
		const auto exitBlockIndex = trace.createBlock();
		f.exitBlockIndex = exitBlockIndex;
		f.hasExitBlock = true;
		for (auto tail : openTails) {
			trace.appendJump(&trace.getBlock(tail), &trace.getBlock(exitBlockIndex));
		}
		if (!blockAlreadyTerminated(trace.getCurrentBlock())) {
			trace.appendJump(&trace.getCurrentBlock(), &trace.getBlock(exitBlockIndex));
		}
		trace.setCurrentBlock(exitBlockIndex);
	}
	// else: every earlier tail was absorbed by a merge block, so the cursor is
	// already on the real continuation. Creating an exit block here would only
	// leave an unreachable empty one behind.
	// else: either a single pass with no branch ever recorded (leave the
	// cursor exactly where fn() left it, matching zero-overhead behavior for
	// the common branch-free region), or the last pass's tail already lands
	// in a CMP/JMP-terminated block with no exit block to converge into
	// anyway (only possible if every pass ended pre-terminated, i.e.
	// hasExitBlock is false here too).
	return false;
}

void LazyTraceContext::traceRegionEnd() {
	auto& f = topFrame();
	const bool recording = f.recording;
	const bool hasEscapes = f.aliveVars.size() > 0;
	const auto callSite = f.callSite;
	const auto P = f.P;

	// Pop before transferring escapes below, so that allocateValRef() routes
	// them into whatever scope the region is returning *into* -- the enclosing
	// region frame when nested, this context's own root state otherwise. `f`
	// stays valid across the pop: frames are pooled, so this only lowers the
	// depth counter and leaves the (distinct) pool entry untouched.
	--activeRegionDepth_;

	if (hasEscapes) {
		// A value created inside the region that is still alive at region end
		// escapes into the enclosing scope, so its liveness bookkeeping has to
		// move there too. Delegating to allocateValRef() rather than touching
		// aliveVars directly is what keeps that target correct for a *nested*
		// region: freeValRef() will later be routed by the very same rule when
		// the value dies, and crediting the true root here instead (as this did
		// before) left the enclosing frame to be decremented for a ref it never
		// incremented -- an assert in debug builds, and a uint32_t underflow to
		// ~4.29e9 in release, which then made this loop iterate ~4.29e9 times.
		//
		// Deliberately not gated on `recording`, unlike the memoization below:
		// this mirrors real C++ object lifetimes, which happen whether or not
		// this engagement recorded anything. A FOLLOW replay still constructs
		// the escaping value (allocateValRef is likewise ungated inside a
		// region) and still destroys it afterwards, so skipping the hand-off
		// here left the enclosing scope decrementing a ref it never incremented.
		f.aliveVars.forEachAliveRef([this](ValueRef ref, uint32_t count) {
			for (uint32_t i = 0; i < count; ++i) {
				allocateValRef(ref);
			}
		});
	} else if (recording && !inActiveRegion()) {
		// Memoize only a top-level region: only a top-level traceRegionBegin
		// ever performs a memo lookup, so an entry filed here by a nested
		// region could never be read back by another nested entry -- only by a
		// later *top-level* entry at the same call site (the same region() in a
		// helper reached both directly and from inside another region), which
		// would match it against a P computed fresh at that site while this key
		// carries the P inherited from the enclosing chain.
		regionMemos[callSite][P] =
		    RegionExec {state->executionTrace.currentOperationIndex, state->executionTrace.currentBlockIndex};
	}
}

Snapshot LazyTraceContext::recordSnapshot() {
	if (inActiveRegion()) {
		auto& f = topFrame();
		return {f.recorder->createTag(), currentStateHash() ^ f.P};
	}
	return {state->tagRecorder.createTag(), currentStateHash()};
}

uint64_t LazyTraceContext::currentStateHash() const {
	if (inActiveRegion()) {
		auto& f = topFrame();
		return hashStaticVector(f.staticVars) ^ f.aliveVars.hash();
	}
	return TraceContextBase::currentStateHash();
}

} // namespace nautilus::tracing
