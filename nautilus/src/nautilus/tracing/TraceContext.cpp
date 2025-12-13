
#include "TraceContext.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/logging.hpp"
#include "symbolic_execution/SymbolicExecutionContext.hpp"
#include "symbolic_execution/TraceTerminationException.hpp"
#include <cassert>
#include <fmt/format.h>

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

TraceState::TraceState(TagRecorder& tr, ExecutionTrace& et, SymbolicExecutionContext& sec)
    : tagRecorder(tr), executionTrace(et), symbolicExecutionContext(sec) {
	// TraceState only holds references - the actual objects are stack-allocated in trace()
}

TraceContext* TraceContext::get() {
	return traceContext.state ? &traceContext : nullptr;
}

TraceContext* TraceContext::initialize(TagRecorder& tagRecorder, ExecutionTrace& executionTrace,
                                       SymbolicExecutionContext& symbolicExecutionContext) {
	traceContext.state = std::make_unique<TraceState>(tagRecorder, executionTrace, symbolicExecutionContext);
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

TypedValueRef& TraceContext::traceCall(const std::string& functionName, const std::string& mangledName, void* fptn,
                                       Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
                                       const FunctionAttributes fnAttrs) {
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

TypedValueRef& TraceContext::traceOperation(Op op, Type resultType, std::initializer_list<InputVariant> inputs) {
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, inputs);
	});
}

bool TraceContext::traceCmp(const TypedValueRef& targetRef) {
	bool result;
	if (state->symbolicExecutionContext.getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW) {
		// eval execution path one step
		// we repeat the operation
		result = state->symbolicExecutionContext.follow();
	} else {
		// record
		auto tag = recordSnapshot();
		if (state->executionTrace.checkTag(tag)) {
			state->executionTrace.addCmpOperation(tag, targetRef);
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
		nextBlock = std::get<BlockRef>(currentOperation.input[1]).block;
	} else {
		nextBlock = std::get<BlockRef>(currentOperation.input[2]).block;
	}
	state->executionTrace.setCurrentBlock(nextBlock);
	return result;
}

std::unique_ptr<ExecutionTrace> TraceContext::trace(std::function<void()>& traceFunction) {
	log::debug("Initialize Tracing");
	auto rootAddress = __builtin_return_address(0);
	auto tr = tracing::TagRecorder((tracing::TagAddress) rootAddress);

	// Allocate ExecutionTrace and SymbolicExecutionContext on the stack
	// This is the key optimization: no heap allocations for these large objects
	ExecutionTrace executionTrace;
	SymbolicExecutionContext symbolicExecutionContext;

	// Initialize TraceContext with references to our stack objects
	auto tc = initialize(tr, executionTrace, symbolicExecutionContext);
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
