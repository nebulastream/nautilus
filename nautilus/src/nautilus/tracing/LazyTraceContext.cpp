#include "LazyTraceContext.hpp"
#include "TraceOperation.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
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
	completingTraceContext.state =
	    std::make_unique<TraceState>(tagRecorder, executionTrace, symbolicExecutionContext, options);
	completingTraceContext.paused_ = false;
	setActiveTracer(&completingTraceContext);
	return &completingTraceContext;
}

void LazyTraceContext::resume() {
	staticVars.clear();
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

TypedValueRef& LazyTraceContext::follow([[maybe_unused]] Op op) {
	auto& currentOperation = state->executionTrace.getCurrentOperation();
	state->executionTrace.nextOperation();
	assert(currentOperation.op == op);
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

TypedValueRef& LazyTraceContext::traceAlloca(size_t allocSize) {
	auto op = Op::ALLOCA;
	auto resultType = Type::ptr;
	return traceOperation(op, [&, allocSize](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {AllocSize {allocSize}});
	});
}

TypedValueRef& LazyTraceContext::traceCopy(const TypedValueRef& ref) {
	if (paused_) {
		return dummyRef_;
	}
	log::debug("Trace Copy");
	return traceOperation(ASSIGN, [&](Snapshot& tag) -> TypedValueRef& {
		auto resultRef = state->executionTrace.getNextValueRef();
		return state->executionTrace.addAssignmentOperation(tag, {resultRef, ref.type}, ref, ref.type);
	});
}

TypedValueRef& LazyTraceContext::traceCall(void* fptn, Type resultType,
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

TypedValueRef& LazyTraceContext::traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
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
		functionsToTrace.push_back(compiler::CompilableFunction(functionName, fwrapper));
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

TypedValueRef& LazyTraceContext::traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
                                                          std::function<void()> fwrapper) {
	if (paused_) {
		return dummyRef_;
	}
	auto functionName = definition->name();
	if (registeredFunctions.insert(functionName).second) {
		functionsToTrace.push_back(compiler::CompilableFunction(functionName, std::move(fwrapper)));
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

void LazyTraceContext::traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) {
	if (paused_) {
		return;
	}
	traceOperation(ASSIGN, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addAssignmentOperation(tag, target, source, resultType);
	});
}

void LazyTraceContext::traceReturnOperation(Type resultType, const TypedValueRef& ref) {
	if (paused_) {
		return;
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

std::unique_ptr<ExecutionTrace> LazyTraceContext::trace(std::function<void()>& traceFunction,
                                                        const engine::Options& options, Arena& arena) {
	log::debug("Initialize Completing Tracing");
	auto rootAddress = __builtin_return_address(0);
	auto tr = tracing::TagRecorder((tracing::TagAddress) rootAddress);

	// The ExecutionTrace borrows the caller-provided arena for all
	// allocations; the arena must outlive the returned trace.
	auto executionTrace = std::make_unique<ExecutionTrace>(arena);
	SymbolicExecutionContext symbolicExecutionContext;

	// Initialize LazyTraceContext with references to our objects
	auto tc = initialize(tr, *executionTrace, symbolicExecutionContext, options);
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

	// Clean up: deregister active tracer and reset state pointer
	setActiveTracer(nullptr);
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
	setActiveTracer(this);

	while (!functionsToTrace.empty()) {
		auto currentFunction = functionsToTrace.front();
		functionsToTrace.pop_front();
		if (traceModule->hasFunction(currentFunction.getName())) {
			log::debug("Function '{}' already traced, skipping.", currentFunction.getName());
			continue;
		}

		auto& executionTrace = traceModule->addNewFunction(currentFunction.getName(), arena);
		auto wrapperFunc = currentFunction.getFunction();

		auto rootAddress = __builtin_return_address(0);
		auto tr = tracing::TagRecorder((tracing::TagAddress) rootAddress);
		SymbolicExecutionContext symbolicExecutionContext;
		state = std::make_unique<TraceState>(tr, executionTrace, symbolicExecutionContext, options);
		// Same hook as ExceptionBasedTraceContext — subclasses (the PE
		// plugin) override to sweep any per-trace state they own before
		// the next inner function's body runs.
		this->beforeInnerFunction();
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
	}

	setActiveTracer(nullptr);
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

const void* LazyTraceContext::currentTag() {
	return state->tagRecorder.createTag();
}

} // namespace nautilus::tracing
