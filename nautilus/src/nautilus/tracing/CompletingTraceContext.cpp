
#include "CompletingTraceContext.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "symbolic_execution/SymbolicExecutionContext.hpp"
#include <cassert>
#include <cxxabi.h>
#include <dlfcn.h>
#include <fmt/format.h>
#include <sstream>

namespace fmt {
template <>
struct formatter<nautilus::tracing::ExecutionTrace> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::ExecutionTrace& trace, format_context& ctx) -> format_context::iterator;
};
} // namespace fmt

namespace nautilus::tracing {

// Thread-local CompletingTraceContext object (not a pointer)
static thread_local CompletingTraceContext completingTraceContext;

CompletingTraceContext* CompletingTraceContext::initialize(TagRecorder& tagRecorder, ExecutionTrace& executionTrace,
                                                           SymbolicExecutionContext& symbolicExecutionContext,
                                                           const engine::Options& options) {
	completingTraceContext.state =
	    std::make_unique<TraceState>(tagRecorder, executionTrace, symbolicExecutionContext, options);
	completingTraceContext.terminated_ = false;
	setActiveTracer(&completingTraceContext);
	return &completingTraceContext;
}

void CompletingTraceContext::resume() {
	staticVars.clear();
	aliveVars.reset();
	terminated_ = false;
}

TypedValueRef& CompletingTraceContext::registerFunctionArgument(Type type, size_t index) {
	if (terminated_) {
		return dummyRef_;
	}
	return state->executionTrace.setArgument(type, index);
}

bool CompletingTraceContext::isFollowing() {
	return state->symbolicExecutionContext.getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW;
}

TypedValueRef& CompletingTraceContext::follow([[maybe_unused]] Op op) {
	auto& currentOperation = state->executionTrace.getCurrentOperation();
	state->executionTrace.nextOperation();
	assert(currentOperation.op == op);
	return currentOperation.resultRef;
}

TypedValueRef& CompletingTraceContext::traceConstant(Type type, const ConstantLiteral& constValue) {
	if (terminated_) {
		return dummyRef_;
	}
	log::debug("Trace Constant");
	auto op = Op::CONST;
	if (isFollowing()) {
		return follow(op);
	}
	auto tag = recordSnapshot();
	auto globalTabIter = state->executionTrace.tagMap.find(tag);
	if (globalTabIter != state->executionTrace.tagMap.end()) {
		auto& ref = globalTabIter->second;
		auto& originalRef = state->executionTrace.getBlocks()[ref.blockIndex].operations[ref.operationIndex];
		auto resultRef = state->executionTrace.addOperationWithResult(tag, op, type, {constValue});
		state->executionTrace.addAssignmentOperation(tag, originalRef.resultRef, resultRef, resultRef.type);
		return originalRef.resultRef;
	} else {
		return state->executionTrace.addOperationWithResult(tag, op, type, {constValue});
	}
}

template <typename OnCreation>
TypedValueRef& CompletingTraceContext::traceOperation(Op op, OnCreation&& onCreation) {
	if (terminated_) {
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
			terminated_ = true;
			return dummyRef_;
		}
	}
}

TypedValueRef& CompletingTraceContext::traceCopy(const TypedValueRef& ref) {
	if (terminated_) {
		return dummyRef_;
	}
	log::debug("Trace Copy");
	return traceOperation(ASSIGN, [&](Snapshot& tag) -> TypedValueRef& {
		auto resultRef = state->executionTrace.getNextValueRef();
		return state->executionTrace.addAssignmentOperation(tag, {resultRef, ref.type}, ref, ref.type);
	});
}

TypedValueRef& CompletingTraceContext::traceCall(void* fptn, Type resultType,
                                                 const std::vector<tracing::TypedValueRef>& arguments,
                                                 FunctionAttributes fnAttrs) {
	if (terminated_) {
		return dummyRef_;
	}
	auto mangledName = getMangledName(fptn);
	auto functionName = getFunctionName(fptn, mangledName);
	auto op = Op::CALL;
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto functionArguments = FunctionCall {.functionName = functionName,
		                                       .mangledName = mangledName,
		                                       .ptr = fptn,
		                                       .arguments = arguments,
		                                       .fnAttrs = fnAttrs};
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

void CompletingTraceContext::traceAssignment(const TypedValueRef& target, const TypedValueRef& source,
                                             Type resultType) {
	if (terminated_) {
		return;
	}
	traceOperation(ASSIGN, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addAssignmentOperation(tag, target, source, resultType);
	});
}

void CompletingTraceContext::traceReturnOperation(Type resultType, const TypedValueRef& ref) {
	if (terminated_) {
		return;
	}
	if (isFollowing()) {
		follow(RETURN);
	} else {
		auto tag = recordSnapshot();
		state->executionTrace.addReturn(tag, resultType, ref);
	}
}

TypedValueRef& CompletingTraceContext::traceBinaryOp(Op op, Type resultType, const TypedValueRef& left,
                                                     const TypedValueRef& right) {
	if (terminated_) {
		return dummyRef_;
	}
	return traceOperation(
	    op, [&, inputs = std::vector<InputVariant> {left, right}](Snapshot& tag) mutable -> TypedValueRef& {
		    return state->executionTrace.addOperationWithResult(tag, op, resultType, std::move(inputs));
	    });
}

TypedValueRef& CompletingTraceContext::traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) {
	if (terminated_) {
		return dummyRef_;
	}
	return traceOperation(op, [&, inputs = std::vector<InputVariant> {input}](Snapshot& tag) mutable -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, std::move(inputs));
	});
}

TypedValueRef& CompletingTraceContext::traceTernaryOp(Op op, Type resultType, const TypedValueRef& first,
                                                      const TypedValueRef& second, const TypedValueRef& third) {
	if (terminated_) {
		return dummyRef_;
	}
	return traceOperation(
	    op, [&, inputs = std::vector<InputVariant> {first, second, third}](Snapshot& tag) mutable -> TypedValueRef& {
		    return state->executionTrace.addOperationWithResult(tag, op, resultType, std::move(inputs));
	    });
}

bool CompletingTraceContext::traceBool(const TypedValueRef& value, const double probability) {
	if (terminated_) {
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
			terminated_ = true;
			return false;
		}
	}

	if (shouldTerminate) {
		// The symbolic execution signals termination (SecondVisit).
		// Enter passive mode instead of throwing.
		terminated_ = true;
		return false;
	}

	auto& currentOperation = state->executionTrace.getCurrentOperation();
	assert(currentOperation.op == CMP);

	uint16_t nextBlock;
	if (result) {
		nextBlock = std::get<BlockRef>(currentOperation.input[1]).block;
	} else {
		nextBlock = std::get<BlockRef>(currentOperation.input[2]).block;
	}
	state->executionTrace.setCurrentBlock(nextBlock);
	return result;
}

std::unique_ptr<ExecutionTrace> CompletingTraceContext::trace(std::function<void()>& traceFunction,
                                                              const engine::Options& options) {
	log::debug("Initialize Completing Tracing");
	auto rootAddress = __builtin_return_address(0);
	auto tr = tracing::TagRecorder((tracing::TagAddress) rootAddress);

	// Allocate ExecutionTrace and SymbolicExecutionContext on the stack
	ExecutionTrace executionTrace;
	SymbolicExecutionContext symbolicExecutionContext;

	// Initialize CompletingTraceContext with references to our stack objects
	auto tc = initialize(tr, executionTrace, symbolicExecutionContext, options);
	auto traceIteration = 0;

	// Symbolic execution loop: explore all execution paths
	// No try/catch needed - the traced function always returns normally
	while (symbolicExecutionContext.shouldContinue()) {
		traceIteration = traceIteration + 1;
		log::trace("Completing Trace Iteration {}", traceIteration);
		log::trace("{}", executionTrace);

		// Prepare for next iteration
		symbolicExecutionContext.next();
		executionTrace.resetExecution();
		tc->resume(); // Reset persistent state (staticVars, aliveVars, terminated_)

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
	log::trace("Final trace: {}", executionTrace);

	return std::make_unique<ExecutionTrace>(std::move(executionTrace));
}

void CompletingTraceContext::allocateValRef(ValueRef ref) {
	if (terminated_) {
		return;
	}
	aliveVars.increment(ref);
}

void CompletingTraceContext::freeValRef(ValueRef ref) {
	if (terminated_) {
		return;
	}
	aliveVars.decrement(ref);
}

void CompletingTraceContext::pushStaticVal(void* valPtr) {
	// Always maintain the static variable stack, even in passive mode.
	// Static variables may have been pushed before entering passive mode,
	// and their destructors will call popStaticVal after.
	staticVars.emplace_back((size_t*) valPtr);
	if (!terminated_ && log::options::getLogStaticVars()) {
		log::info("pushStaticVal: [{}]", formatStaticVars());
	}
}

void CompletingTraceContext::popStaticVal() {
	// Always maintain the static variable stack, even in passive mode.
	if (!terminated_ && log::options::getLogStaticVars()) {
		log::info("popStaticVal: [{}] (popping last)", formatStaticVars());
	}
	staticVars.pop_back();
}

std::string CompletingTraceContext::formatStaticVars() const {
	std::string result;
	for (size_t i = 0; i < staticVars.size(); i++) {
		if (i > 0) {
			result += ", ";
		}
		result += std::to_string(getStaticVarValue(staticVars[i]));
	}
	return result;
}

std::string CompletingTraceContext::getMangledName(void* fnptr) {
	if (const auto it = mangledNameCache.find(fnptr); it != mangledNameCache.end()) {
		return it->second;
	}

	Dl_info info;
	dladdr(reinterpret_cast<void*>(fnptr), &info);
	if (info.dli_sname != nullptr) {
		mangledNameCache[fnptr] = info.dli_sname;
		return info.dli_sname;
	}
	std::stringstream ss;
	ss << fnptr;
	std::string ptrStr = ss.str();
	mangledNameCache[fnptr] = ptrStr;
	return ptrStr;
}

std::string CompletingTraceContext::getFunctionName(void* fnptr, const std::string& mangledName) {
	bool normalizeFunctionNames = state->options.getOptionOrDefault("engine.normalizeFunctionNames", false);

	if (normalizeFunctionNames) {
		auto it = state->normalizedFunctionNameCache.find(fnptr);
		if (it != state->normalizedFunctionNameCache.end()) {
			return "runtimeFunc" + std::to_string(it->second);
		}
		uint32_t index = state->nextNormalizedFunctionIndex++;
		state->normalizedFunctionNameCache[fnptr] = index;
		return "runtimeFunc" + std::to_string(index);
	}

	bool demangleFunctionNames = state->options.getOptionOrDefault("engine.demangleFunctionNames", true);

	if (!demangleFunctionNames) {
		return mangledName;
	}

	int status;
	char* demangled = __cxxabiv1::__cxa_demangle(mangledName.c_str(), nullptr, nullptr, &status);
	if (status == 0 && demangled != nullptr) {
		std::string result(demangled);
		std::free(demangled);
		return result;
	}
	return mangledName;
}

Snapshot CompletingTraceContext::recordSnapshot() {
	return {state->tagRecorder.createTag(), hashStaticVector(staticVars) ^ aliveVars.hash()};
}

} // namespace nautilus::tracing
