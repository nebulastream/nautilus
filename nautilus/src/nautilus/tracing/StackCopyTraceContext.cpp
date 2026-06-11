
#include "StackCopyTraceContext.hpp"
#include "TraceOperation.hpp"
#include "fiber/SanitizerFiberSupport.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "symbolic_execution/SymbolicExecutionContext.hpp"
#include <cassert>
#include <cstring>
#include <fmt/format.h>
#include <utility>

namespace fmt {
template <>
struct formatter<nautilus::tracing::ExecutionTrace> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::ExecutionTrace& trace, format_context& ctx) -> format_context::iterator;
};
} // namespace fmt

namespace nautilus::tracing {

// Thread-local StackCopyTraceContext object (not a pointer)
static thread_local StackCopyTraceContext stackCopyTraceContext;

StackCopyTraceContext::~StackCopyTraceContext() = default;

void StackCopyTraceContext::resume() {
	staticVars.clear();
	aliveVars.reset();
	paused_ = false;
	resetPruningState();
}

TypedValueRef& StackCopyTraceContext::registerFunctionArgument(Type type, size_t index) {
	if (paused_) {
		return dummyRef_;
	}
	return state->executionTrace.setArgument(type, index);
}

TypedValueRef& StackCopyTraceContext::traceConstant(Type type, const ConstantLiteral& constValue) {
	if (paused_) {
		return dummyRef_;
	}
	log::debug("Trace Constant");
	auto op = Op::CONST;
	auto tag = recordSnapshot();
	auto globalTabIter = state->executionTrace.globalTagMap.find(tag);
	if (globalTabIter != state->executionTrace.globalTagMap.end()) {
		auto& ref = globalTabIter->second;
		auto* originalRef = state->executionTrace.getBlocks()[ref.blockIndex]->operations[ref.operationIndex];
		auto resultRef = state->executionTrace.addOperationWithResult(tag, op, type, {constValue});
		state->executionTrace.addAssignmentOperation(tag, originalRef->resultRef, resultRef, resultRef.type);
		trackConstant(originalRef->resultRef, type, constValue);
		return originalRef->resultRef;
	} else {
		auto& resultRef = state->executionTrace.addOperationWithResult(tag, op, type, {constValue});
		trackConstant(resultRef, type, constValue);
		return resultRef;
	}
}

template <typename OnCreation>
TypedValueRef& StackCopyTraceContext::traceOperation([[maybe_unused]] Op op, OnCreation&& onCreation) {
	if (paused_) {
		return dummyRef_;
	}
	auto tag = recordSnapshot();
	if (state->executionTrace.checkTag(tag)) {
		return onCreation(tag);
	}
	// Control-flow merge or loop detected: finish this path passively.
	paused_ = true;
	return dummyRef_;
}

TypedValueRef& StackCopyTraceContext::traceAlloca(size_t size, size_t align) {
	auto op = Op::ALLOCA;
	auto resultType = Type::ptr;
	return traceOperation(op, [&, size, align](Snapshot& tag) -> TypedValueRef& {
		auto index = state->executionTrace.addAllocaSpec(size, align);
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {index});
	});
}

TypedValueRef& StackCopyTraceContext::traceCopy(const TypedValueRef& ref) {
	if (paused_) {
		return dummyRef_;
	}
	log::debug("Trace Copy");
	auto& resultRef = traceOperation(ASSIGN, [&](Snapshot& tag) -> TypedValueRef& {
		auto newRef = state->executionTrace.getNextValueRef();
		return state->executionTrace.addAssignmentOperation(tag, {newRef, ref.type}, ref, ref.type);
	});
	if (!paused_) {
		trackAssignment(resultRef, ref, ref.type);
	}
	return resultRef;
}

TypedValueRef& StackCopyTraceContext::traceCall(void* fptn, Type resultType,
                                                const std::vector<tracing::TypedValueRef>& arguments,
                                                FunctionAttributes fnAttrs) {
	if (paused_) {
		return dummyRef_;
	}
	auto mangledName = getMangledName(fptn);
	auto functionName = getFunctionName(fptn, mangledName);
	auto op = Op::CALL;
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* functionArguments =
		    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
		                                                                        .mangledName = mangledName,
		                                                                        .ptr = fptn,
		                                                                        .arguments = arguments,
		                                                                        .fnAttrs = fnAttrs});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

TypedValueRef& StackCopyTraceContext::traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
                                                        const std::vector<tracing::TypedValueRef>& arguments,
                                                        FunctionAttributes fnAttrs) {
	if (paused_) {
		return dummyRef_;
	}
	auto op = Op::INDIRECT_CALL;
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* indirectCall = state->executionTrace.getArena().create<IndirectFunctionCall>(
		    IndirectFunctionCall {.fnPtr = fnPtrRef, .arguments = arguments, .fnAttrs = fnAttrs});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {indirectCall});
	});
}

TypedValueRef& StackCopyTraceContext::traceNautilusCall(const NautilusFunctionDefinition* definition,
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
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* functionArguments =
		    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
		                                                                        .mangledName = functionName,
		                                                                        .ptr = (void*) definition,
		                                                                        .arguments = arguments,
		                                                                        .fnAttrs = fnAttrs});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

TypedValueRef& StackCopyTraceContext::traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
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
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* functionArguments =
		    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
		                                                                        .mangledName = functionName,
		                                                                        .ptr = (void*) definition,
		                                                                        .arguments = {},
		                                                                        .fnAttrs = {}});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

void StackCopyTraceContext::traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) {
	if (paused_) {
		return;
	}
	trackAssignment(target, source, resultType);
	traceOperation(ASSIGN, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addAssignmentOperation(tag, target, source, resultType);
	});
}

void StackCopyTraceContext::traceReturnOperation(Type resultType, const TypedValueRef& ref) {
	if (paused_) {
		return;
	}
	auto tag = recordSnapshot();
	state->executionTrace.addReturn(tag, resultType, ref);
}

TypedValueRef& StackCopyTraceContext::traceBinaryOp(Op op, Type resultType, const TypedValueRef& left,
                                                    const TypedValueRef& right) {
	if (paused_) {
		return dummyRef_;
	}
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {left, right});
	});
}

TypedValueRef& StackCopyTraceContext::traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) {
	if (paused_) {
		return dummyRef_;
	}
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {input});
	});
}

TypedValueRef& StackCopyTraceContext::traceTernaryOp(Op op, Type resultType, const TypedValueRef& first,
                                                     const TypedValueRef& second, const TypedValueRef& third) {
	if (paused_) {
		return dummyRef_;
	}
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {first, second, third});
	});
}

bool StackCopyTraceContext::traceBool(const TypedValueRef& valueRef, const double probability) {
	if (paused_) {
		// In passive mode, return false to guarantee loop termination.
		return false;
	}

#ifndef NAUTILUS_FIBER_SUPPORTED
	(void) probability;
	throw RuntimeException("stackCopyTracing is not supported on this architecture");
#else
	auto tag = recordSnapshot();
	if (bool constantResult; findConstantBranch(valueRef, &constantResult)) {
		guardPrunedTag(tag);
		return constantResult;
	}
	if (!state->executionTrace.checkTag(tag)) {
		// Control flow merge/loop detected. Finish the path passively.
		paused_ = true;
		return false;
	}
	state->executionTrace.addCmpOperation(tag, valueRef, probability);

	auto& snap = pendingSnapshots_.emplace_back();
	snap.cmpTag = tag;
	snap.staticVarsCopy = staticVars;
	snap.aliveVarsCopy = aliveVars;
	if (detail::nautilus_captureContext(&snap.context) == 0) {
		// First pass: persist the false-branch continuation and explore true.
		captureStackInto(pendingSnapshots_.back());
		auto& currentOperation = state->executionTrace.getCurrentOperation();
		assert(currentOperation.op == CMP);
		state->executionTrace.setCurrentBlock(std::get<BlockRef*>(currentOperation.input[1])->block);
		return true;
	}
	// Resumed from the snapshot: the host scheduler restored the stack bytes and the
	// tracer state and re-positioned the trace at this CMP. Explore the false branch.
	// Do not touch pendingSnapshots_ here - the snapshot we resumed from is gone.
	detail::sanitizerFinishSwitchFiber(fiberFakeStack_);
	auto& currentOperation = state->executionTrace.getCurrentOperation();
	assert(currentOperation.op == CMP);
	state->executionTrace.setCurrentBlock(std::get<BlockRef*>(currentOperation.input[2])->block);
	return false;
#endif
}

void StackCopyTraceContext::captureStackInto(PathSnapshot& snap) {
	auto* stackPointer = static_cast<std::byte*>(detail::capturedStackPointer(snap.context));
	// Include the ABI red zone below the captured stack pointer; the bytes are dead
	// on restore but copying them keeps the region computation trivially safe.
	constexpr ptrdiff_t redZone = 128;
	auto* base = stackPointer - redZone;
	if (base < stack_->bottom()) {
		base = stack_->bottom();
	}
	snap.stackBase = base;
	snap.stackCopy.assign(base, static_cast<std::byte*>(static_cast<void*>(stack_->top())));
}

void StackCopyTraceContext::runTraced() {
	// The return address of this call terminates every tag walk: it sits in a frame
	// at the side-stack base that is below(=older than) every snapshot's stack
	// pointer and therefore byte-identical across all resumes.
	auto rootAddress = (TagAddress) __builtin_return_address(0);
	// The TagRecorder (and its heap-allocated tag trie) must live outside the side
	// stack: stack restores would otherwise rewind the trie's container state.
	tagRecorder_ = std::make_unique<TagRecorder>(rootAddress);
	state = std::make_unique<TraceState>(*tagRecorder_, *currentTrace_, *symbolicExecutionContext_, *currentOptions_);
	(*currentWrapper_)();
}

void StackCopyTraceContext::fiberEntry(void* arg) {
	auto* self = static_cast<StackCopyTraceContext*>(arg);
	// First switch onto the side stack; record the host stack bounds for the way back.
	detail::sanitizerFinishSwitchFiber(self->fiberFakeStack_, &self->hostStackBottom_, &self->hostStackSize_);
	try {
		self->runTraced();
	} catch (...) {
		// Never unwind through the assembly stack switch; hand the exception to the
		// host scheduler instead.
		self->pendingException_ = std::current_exception();
	}
	self->switchToHost();
}

void StackCopyTraceContext::switchToHost() {
	detail::sanitizerStartSwitchFiber(&fiberFakeStack_, hostStackBottom_, hostStackSize_);
	detail::nautilus_resumeContext(&hostContext_);
}

void StackCopyTraceContext::traceFunction(std::function<void()>& wrapper) {
#ifndef NAUTILUS_FIBER_SUPPORTED
	(void) wrapper;
	throw RuntimeException("stackCopyTracing is not supported on this architecture");
#else
	currentWrapper_ = &wrapper;
	pendingSnapshots_.clear();
	pendingException_ = nullptr;
	pathCount_ = 0;

	detail::sanitizerStartSwitchFiber(&hostFakeStack_, stack_->bottom(), stack_->usableSize());
	detail::nautilus_enterOnStack(&hostContext_, stack_->top(), &StackCopyTraceContext::fiberEntry, this);
	// Control lands here after EVERY completed path: the fiber resumes hostContext_,
	// which was captured by the enterOnStack call above (setjmp-like multi-return).
	// All loop state therefore lives in members, never in locals that span switches.
	detail::sanitizerFinishSwitchFiber(hostFakeStack_);
	while (pendingException_ == nullptr && !pendingSnapshots_.empty()) {
		if (++pathCount_ > SymbolicExecutionContext::MAX_ITERATIONS) {
			pendingSnapshots_.clear();
			throw RuntimeException("Tracing got lost and reached the max number of iterations.");
		}
		assert(staticVars.empty() && "static variable stack not empty after tracing path");
		auto& snap = pendingSnapshots_.front();
		auto& trace = state->executionTrace;

		// Equivalent of ExecutionTrace::resetExecution() between the iterations of the
		// re-executing tracers (minus the position reset, done via the CMP tag below).
		trace.globalTagMap.merge(trace.localTagMap);
		trace.localTagMap.clear();

		// Re-position at the CMP. Look it up by tag: control-flow merges of previously
		// explored paths may have moved the operation, and the tag maps track that.
		auto cmpPosition = trace.globalTagMap.find(snap.cmpTag);
		if (cmpPosition == trace.globalTagMap.end()) {
			pendingSnapshots_.clear();
			throw RuntimeException("Pending branch snapshot lost its CMP operation");
		}
		trace.currentBlockIndex = cmpPosition->second.blockIndex;
		trace.currentOperationIndex = cmpPosition->second.operationIndex;

		// FOLLOW-replay parity: the re-executing tracers re-run the function prologue
		// once per path, which bumps lastValueRef once per function argument
		// (ExecutionTrace::setArgument). Emulate that bump so value-ref numbering in
		// the recorded suffixes stays byte-identical to the reference traces.
		trace.lastValueRef += static_cast<ValueRef>(trace.getArguments().size());

		staticVars = std::move(snap.staticVarsCopy);
		aliveVars = std::move(snap.aliveVarsCopy);
		paused_ = false;
		prunedTags.clear();

		// Restore the stack bytes (we run on the host stack, the target region is the
		// side stack - no overlap) and jump back into traceBool's resumed branch.
		detail::sanitizerUnpoisonStack(snap.stackBase, static_cast<size_t>(stack_->top() - snap.stackBase));
		std::memcpy(snap.stackBase, snap.stackCopy.data(), snap.stackCopy.size());
		detail::CapturedContext resumeTarget = snap.context;
		pendingSnapshots_.pop_front();
		detail::sanitizerStartSwitchFiber(&hostFakeStack_, stack_->bottom(), stack_->usableSize());
		detail::nautilus_resumeContext(&resumeTarget);
		// nautilus_resumeContext never returns; the next path termination re-enters
		// this scheduler right after the enterOnStack call above.
	}
	if (pendingException_ != nullptr) {
		pendingSnapshots_.clear();
		auto exception = std::exchange(pendingException_, nullptr);
		std::rethrow_exception(exception);
	}
#endif
}

void StackCopyTraceContext::traceCurrentFunction(std::function<void()>& wrapper) {
	symbolicExecutionContext_ = std::make_unique<SymbolicExecutionContext>();
	resume();
	traceFunction(wrapper);
	state.reset();
	tagRecorder_.reset();
}

std::unique_ptr<ExecutionTrace> StackCopyTraceContext::trace(std::function<void()>& traceFunction,
                                                             const engine::Options& options, Arena& arena) {
	log::debug("Initialize StackCopy Tracing");
	auto& tc = stackCopyTraceContext;
	auto executionTrace = std::make_unique<ExecutionTrace>(arena);
	setActiveTracer(&tc);
	// Ensure the thread-local active tracer is cleared even if an exception escapes.
	ActiveTracerGuard activeTracerGuard;

	auto stackSize = static_cast<size_t>(options.getOptionOrDefault<int>("engine.traceStackSize", 8 * 1024 * 1024));
	if (tc.stack_ == nullptr || tc.requestedStackSize_ != stackSize) {
		tc.stack_ = std::make_unique<detail::FiberStack>(stackSize);
		tc.requestedStackSize_ = stackSize;
	}
	tc.currentOptions_ = &options;
	tc.currentTrace_ = executionTrace.get();
	tc.traceCurrentFunction(traceFunction);

	log::debug("StackCopy Tracing Terminated");
	log::trace("Final trace: {}", *executionTrace);
	return executionTrace;
}

std::unique_ptr<TraceModule> StackCopyTraceContext::Trace(std::list<compiler::CompilableFunction>& functions,
                                                          const engine::Options& options, Arena& arena) {
	return stackCopyTraceContext.startTrace(functions, options, arena);
}

std::unique_ptr<TraceModule> StackCopyTraceContext::startTrace(std::list<compiler::CompilableFunction>& functions,
                                                               const engine::Options& options, Arena& arena) {
	log::debug("Initialize StackCopy Tracing");
	auto traceModule = std::make_unique<TraceModule>();
	functionsToTrace = functions;
	registeredFunctions.clear();
	setActiveTracer(this);
	// Ensure the thread-local active tracer is cleared even if an exception escapes
	// the per-function loop below.
	ActiveTracerGuard activeTracerGuard;

	auto stackSize = static_cast<size_t>(options.getOptionOrDefault<int>("engine.traceStackSize", 8 * 1024 * 1024));
	if (stack_ == nullptr || requestedStackSize_ != stackSize) {
		stack_ = std::make_unique<detail::FiberStack>(stackSize);
		requestedStackSize_ = stackSize;
	}
	currentOptions_ = &options;

	bool isFirstFunction = true;
	while (!functionsToTrace.empty()) {
		auto currentFunction = functionsToTrace.front();
		functionsToTrace.pop_front();
		if (traceModule->hasFunction(currentFunction.getName())) {
			log::debug("Function '{}' already traced, skipping.", currentFunction.getName());
			continue;
		}

		auto& executionTrace = traceModule->addNewFunction(currentFunction.getName(), arena);
		// Tag the first popped function as the entry point (see the re-executing
		// tracers for the host-vs-device classification rationale).
		auto attributes = currentFunction.getAttributes();
		if (isFirstFunction) {
			attributes["entry"] = "true";
			isFirstFunction = false;
		}
		traceModule->setFunctionAttributes(currentFunction.getName(), attributes);
		auto wrapperFunc = currentFunction.getFunction();

		currentTrace_ = &executionTrace;
		traceCurrentFunction(wrapperFunc);

		log::debug("StackCopy Tracing of '{}' terminated", currentFunction.getName());
		log::trace("Final trace: {}", executionTrace);
	}

	// activeTracer is cleared by ActiveTracerGuard.
	return traceModule;
}

void StackCopyTraceContext::allocateValRef(ValueRef ref) {
	if (paused_) {
		return;
	}
	aliveVars.increment(ref);
}

void StackCopyTraceContext::freeValRef(ValueRef ref) {
	if (paused_) {
		return;
	}
	aliveVars.decrement(ref);
}

void StackCopyTraceContext::pushStaticVal(void* valPtr, size_t size) {
	// Always maintain the static variable stack, even in passive mode: pushes from
	// before entering passive mode are popped by destructors afterwards.
	staticVars.emplace_back(valPtr, size);
}

void StackCopyTraceContext::popStaticVal() {
	staticVars.pop_back();
}

Snapshot StackCopyTraceContext::recordSnapshot() {
	return {state->tagRecorder.createTag(), hashStaticVector(staticVars) ^ aliveVars.hash()};
}

} // namespace nautilus::tracing
