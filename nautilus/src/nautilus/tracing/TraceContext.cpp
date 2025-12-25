
#include "TraceContext.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/logging.hpp"
#include "symbolic_execution/SymbolicExecutionContext.hpp"
#include "symbolic_execution/TraceTerminationException.hpp"
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

// Thread-local TraceContext object (not a pointer)
// This is allocated in thread-local storage - zero heap allocation overhead
static thread_local TraceContext traceContext;

TraceState::TraceState(TagRecorder& tr, ExecutionTrace& et, SymbolicExecutionContext& sec, const engine::Options& opts)
    : tagRecorder(tr), executionTrace(et), symbolicExecutionContext(sec), options(opts) {
	// TraceState only holds references - the actual objects are stack-allocated in trace()
}

TraceContext* TraceContext::get() {
	return traceContext.state ? &traceContext : nullptr;
}

TraceContext* TraceContext::initialize(TagRecorder& tagRecorder, ExecutionTrace& executionTrace,
                                       SymbolicExecutionContext& symbolicExecutionContext,
                                       const engine::Options& options) {
	traceContext.state = std::make_unique<TraceState>(tagRecorder, executionTrace, symbolicExecutionContext, options);
	return &traceContext;
}

void TraceContext::resume() {
	// Clear dynamic containers
	staticVars.clear();

	// Reset aliveVars to initial state (all counts to 0, hash to 0)
	aliveVars.reset();

	// Note: state (with executionTrace and symbolicExecutionContext) is NOT reset here
	// as it needs to persist across trace iterations
}

TypedValueRef& TraceContext::registerFunctionArgument(Type type, size_t index) {
	return state->executionTrace.setArgument(type, index);
}

void TraceContext::traceValueDestruction(nautilus::tracing::TypedValueRef) {
	// currently yed not implemented
}

bool TraceContext::isFollowing() {
	return state->symbolicExecutionContext.getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW;
}

TypedValueRef& TraceContext::follow([[maybe_unused]] Op op) {
	auto& currentOperation = state->executionTrace.getCurrentOperation();
	state->executionTrace.nextOperation();
	assert(currentOperation.op == op);
	return currentOperation.resultRef;
}

TypedValueRef& TraceContext::traceConstValue(Type type, const ConstantLiteral& constValue) {
	log::debug("Trace Constant");
	auto op = Op::CONST;
	if (isFollowing()) {
		return follow(op);
	}
	auto tag = recordSnapshot();
	auto globalTabIter = state->executionTrace.globalTagMap.find(tag);
	if (globalTabIter != state->executionTrace.globalTagMap.end()) {
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
TypedValueRef& TraceContext::traceOperation(Op op, OnCreation&& onCreation) {
	if (isFollowing()) {
		return follow(op);
	} else {
		auto tag = recordSnapshot();
		if (state->executionTrace.checkTag(tag)) {
			return onCreation(tag);
		} else {
			// TODO find a way to handle this more graceful.
			throw TraceTerminationException();
		}
	}
}

TypedValueRef& TraceContext::traceCopy(const TypedValueRef& ref) {
	log::debug("Trace Copy");
	return traceOperation(ASSIGN, [&](Snapshot& tag) -> TypedValueRef& {
		auto resultRef = state->executionTrace.getNextValueRef();
		return state->executionTrace.addAssignmentOperation(tag, {resultRef, ref.type}, ref, ref.type);
	});
}

TypedValueRef& TraceContext::traceCall(void* fptn, Type resultType,
                                       const std::vector<tracing::TypedValueRef>& arguments,
                                       const FunctionAttributes fnAttrs) {
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

void TraceContext::traceAssignment(const TypedValueRef& targetRef, const TypedValueRef& sourceRef, Type resultType) {
	traceOperation(ASSIGN, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addAssignmentOperation(tag, targetRef, sourceRef, resultType);
	});
}

void TraceContext::traceReturnOperation(Type resultType, const TypedValueRef& ref) {
	if (isFollowing()) {
		follow(RETURN);
	} else {
		auto tag = recordSnapshot();
		state->executionTrace.addReturn(tag, resultType, ref);
	}
}

TypedValueRef& TraceContext::traceOperation(Op op, Type resultType, std::vector<InputVariant> inputs) {
	return traceOperation(op, [&, inputs = std::move(inputs)](Snapshot& tag) mutable -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, std::move(inputs));
	});
}

bool TraceContext::traceCmp(const TypedValueRef& targetRef, const double probability) {
	bool result;
	if (state->symbolicExecutionContext.getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW) {
		// eval execution path one step
		// we repeat the operation
		result = state->symbolicExecutionContext.follow();
	} else {
		// record
		auto tag = recordSnapshot();
		if (state->executionTrace.checkTag(tag)) {
			state->executionTrace.addCmpOperation(tag, targetRef, probability);
			result = state->symbolicExecutionContext.record(tag);
		} else {
			// this is actually the same tag -> throw up
			throw TraceTerminationException();
		}
	}

	auto& currentOperation = state->executionTrace.getCurrentOperation();
	assert(currentOperation.op == CMP);

	uint16_t nextBlock;
	if (result) {
		nextBlock = tracing::get<BlockRef>(currentOperation.input[1]).block;
	} else {
		nextBlock = tracing::get<BlockRef>(currentOperation.input[2]).block;
	}
	state->executionTrace.setCurrentBlock(nextBlock);
	return result;
}

std::unique_ptr<ExecutionTrace> TraceContext::trace(std::function<void()>& traceFunction,
                                                    const engine::Options& options) {
	log::debug("Initialize Tracing");
	auto rootAddress = __builtin_return_address(0);
	auto tr = tracing::TagRecorder((tracing::TagAddress) rootAddress);

	// Allocate ExecutionTrace and SymbolicExecutionContext on the stack
	// This is the key optimization: no heap allocations for these large objects
	ExecutionTrace executionTrace;
	SymbolicExecutionContext symbolicExecutionContext;

	// Initialize TraceContext with references to our stack objects
	auto tc = initialize(tr, executionTrace, symbolicExecutionContext, options);
	auto traceIteration = 0;

	// Symbolic execution loop: explore all execution paths
	while (symbolicExecutionContext.shouldContinue()) {
		try {
			traceIteration = traceIteration + 1;
			log::trace("Trace Iteration {}", traceIteration);
			log::trace("{}", executionTrace);

			// Prepare for next iteration
			symbolicExecutionContext.next();
			executionTrace.resetExecution();
			TraceContext::get()->resume(); // Reset persistent state (staticVars, aliveVars)

			// Execute the traced function
			traceFunction();
		} catch (const TraceTerminationException& ex) {
			// Normal termination when we hit a known control flow merge or loop
		}
	}

	// Clean up: reset state pointer (TraceContext is no longer initialized)
	tc->state.reset();

	log::debug("Tracing Terminated with {} iterations", traceIteration);
	log::trace("Final trace: {}", executionTrace);

	// Move stack-allocated executionTrace into a unique_ptr for return
	// The caller gets ownership of the trace
	return std::make_unique<ExecutionTrace>(std::move(executionTrace));
}

std::vector<StaticVarHolder>& TraceContext::getStaticVars() {
	return staticVars;
}

void TraceContext::allocateValRef(ValueRef ref) {
	aliveVars.increment(ref);
}
void TraceContext::freeValRef(ValueRef ref) {
	aliveVars.decrement(ref);
}

std::string TraceContext::getMangledName(void* fnptr) {
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

std::string TraceContext::getFunctionName(void* fnptr, const std::string& mangledName) {
	// Check if function name normalization is enabled
	bool normalizeFunctionNames = state->options.getOptionOrDefault("engine.normalizeFunctionNames", false);

	if (normalizeFunctionNames) {
		// Return normalized name (runtimeFunc0, runtimeFunc1, etc.)
		// Check if we already have a normalized name for this function
		auto it = state->normalizedFunctionNameCache.find(fnptr);
		if (it != state->normalizedFunctionNameCache.end()) {
			return "runtimeFunc" + std::to_string(it->second);
		}

		// Assign a new normalized function index
		uint32_t index = state->nextNormalizedFunctionIndex++;
		state->normalizedFunctionNameCache[fnptr] = index;
		return "runtimeFunc" + std::to_string(index);
	}

	// Check if demangling is enabled
	bool demangleFunctionNames = state->options.getOptionOrDefault("engine.demangleFunctionNames", true);

	if (!demangleFunctionNames) {
		// Return the mangled name as-is
		return mangledName;
	}

	// Try to demangle the function name for human-readable output
	int status;
	char* demangled = __cxxabiv1::__cxa_demangle(mangledName.c_str(), nullptr, nullptr, &status);
	if (status == 0 && demangled != nullptr) {
		// Demangling succeeded
		std::string result(demangled);
		std::free(demangled);
		return result;
	}

	// Demangling failed, return the mangled name
	return mangledName;
}

constexpr size_t fnv_prime = 0x100000001b3;
constexpr size_t offset_basis = 0xcbf29ce484222325;

uint64_t hashStaticVector(const std::vector<StaticVarHolder>& data) {
	size_t hash = offset_basis;
	for (auto& value : data) {
		hash ^= *value.ptr;
		hash *= fnv_prime;
	}
	return hash;
}

Snapshot TraceContext::recordSnapshot() {
	return {state->tagRecorder.createTag(), hashStaticVector(staticVars) ^ aliveVars.hash()};
}

} // namespace nautilus::tracing
