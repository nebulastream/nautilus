
#include "TraceContext.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/logging.hpp"
#include "symbolic_execution/SymbolicExecutionContext.hpp"
#include <cassert>
#include <csetjmp>
#include <fmt/format.h>

namespace fmt {
template <>
struct formatter<nautilus::tracing::ExecutionTrace> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::ExecutionTrace& trace, format_context& ctx) -> format_context::iterator;
};
} // namespace fmt

namespace nautilus::tracing {

static thread_local TraceContext* traceContext;

TraceContext* TraceContext::get() {
	return traceContext;
}

TraceContext* TraceContext::initialize(TagRecorder& tagRecorder) {
	traceContext = new TraceContext(tagRecorder);
	traceContext->dynamicVars.reserve(128);
	return traceContext;
}

void TraceContext::resume() {
	staticVars.clear();
	dynamicVars.clear();
	traceContext->dynamicVars.reserve(128);
}

void TraceContext::terminate() {
	delete traceContext;
	traceContext = nullptr;
}

TraceContext::TraceContext(TagRecorder& tagRecorder)
    : tagRecorder(tagRecorder), executionTrace(std::make_unique<ExecutionTrace>()),
      symbolicExecutionContext(std::make_unique<SymbolicExecutionContext>()) {
}

TypedValueRef& TraceContext::registerFunctionArgument(Type type, size_t index) {
	return executionTrace->setArgument(type, index);
}

void TraceContext::traceValueDestruction(nautilus::tracing::TypedValueRef) {
	// currently yed not implemented
}

bool TraceContext::isFollowing() {
	return symbolicExecutionContext->getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW;
}

TypedValueRef& TraceContext::follow([[maybe_unused]] Op op) {
	auto& currentOperation = executionTrace->getCurrentOperation();
	executionTrace->nextOperation();
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
	auto globalTabIter = executionTrace->globalTagMap.find(tag);
	if (globalTabIter != executionTrace->globalTagMap.end()) {
		auto& ref = globalTabIter->second;
		auto& originalRef = executionTrace->getBlocks()[ref.blockIndex].operations[ref.operationIndex];
		auto resultRef = executionTrace->addOperationWithResult(tag, op, type, {constValue});
		executionTrace->addAssignmentOperation(tag, originalRef.resultRef, resultRef, resultRef.type);
		return originalRef.resultRef;
	} else {
		return executionTrace->addOperationWithResult(tag, op, type, {constValue});
	}
}

template <typename OnCreation>
TypedValueRef& TraceContext::traceOperation(Op op, OnCreation&& onCreation) {
	if (isFollowing()) {
		return follow(op);
	} else {
		auto tag = recordSnapshot();
		if (executionTrace->checkTag(tag)) {
			return onCreation(tag);
		} else {
			// Terminate trace iteration via longjmp (faster than exception)
			std::longjmp(traceTerminationJmpBuf, 1);
		}
	}
}

TypedValueRef& TraceContext::traceCopy(const TypedValueRef& ref) {
	log::debug("Trace Copy");
	return traceOperation(ASSIGN, [&](Snapshot& tag) -> TypedValueRef& {
		auto resultRef = executionTrace->getNextValueRef();
		return executionTrace->addAssignmentOperation(tag, {resultRef, ref.type}, ref, ref.type);
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
		return executionTrace->addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

void TraceContext::traceAssignment(const TypedValueRef& targetRef, const TypedValueRef& sourceRef, Type resultType) {
	traceOperation(ASSIGN, [&](Snapshot& tag) -> TypedValueRef& {
		return executionTrace->addAssignmentOperation(tag, targetRef, sourceRef, resultType);
	});
}

void TraceContext::traceReturnOperation(Type resultType, const TypedValueRef& ref) {
	if (isFollowing()) {
		follow(RETURN);
	} else {
		auto tag = recordSnapshot();
		executionTrace->addReturn(tag, resultType, ref);
	}
}

TypedValueRef& TraceContext::traceOperation(Op op, Type resultType, std::initializer_list<InputVariant> inputs) {
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return executionTrace->addOperationWithResult(tag, op, resultType, inputs);
	});
}

bool TraceContext::traceCmp(const TypedValueRef& targetRef) {
	bool result;
	if (symbolicExecutionContext->getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW) {
		// eval execution path one step
		// we repeat the operation
		result = symbolicExecutionContext->follow();
	} else {
		// record
		auto tag = recordSnapshot();
		if (executionTrace->checkTag(tag)) {
			executionTrace->addCmpOperation(tag, targetRef);
			result = symbolicExecutionContext->record(tag);
		} else {
			// Terminate trace iteration via longjmp (faster than exception)
			std::longjmp(traceTerminationJmpBuf, 1);
		}
	}

	auto& currentOperation = executionTrace->getCurrentOperation();
	assert(currentOperation.op == CMP);

	uint16_t nextBlock;
	if (result) {
		nextBlock = std::get<BlockRef>(currentOperation.input[1]).block;
	} else {
		nextBlock = std::get<BlockRef>(currentOperation.input[2]).block;
	}
	executionTrace->setCurrentBlock(nextBlock);
	return result;
}

std::unique_ptr<ExecutionTrace> TraceContext::trace(std::function<void()>& traceFunction) {
	log::debug("Initialize Tracing");
	auto rootAddress = __builtin_return_address(0);
	auto tr = tracing::TagRecorder((tracing::TagAddress) rootAddress);
	auto tc = initialize(tr);
	volatile int traceIteration = 0;
	while (tc->symbolicExecutionContext->shouldContinue()) {
		// Set up non-local jump target for trace termination
		if (setjmp(tc->traceTerminationJmpBuf) == 0) {
			// Normal execution path
			traceIteration = traceIteration + 1;
			log::trace("Trace Iteration {}", traceIteration);
			log::trace("{}", *tc->executionTrace);
			tc->symbolicExecutionContext->next();
			tc->executionTrace->resetExecution();
			TraceContext::get()->resume();
			traceFunction();
		}
		// If we reach here via longjmp, the iteration terminated early
		// Simply continue to next iteration
	}
	auto trace = std::move(tc->executionTrace);
	terminate();
	log::debug("Tracing Terminated with {} iterations", traceIteration);
	log::trace("Final trace: {}", *trace);
	return trace;
}

std::vector<StaticVarHolder>& TraceContext::getStaticVars() {
	return staticVars;
}

void TraceContext::allocateValRef(ValueRef ref) {
	while (dynamicVars.size() <= ref) {
		dynamicVars.emplace_back(0);
	}
	dynamicVars.at(ref)++;
}
void TraceContext::freeValRef(ValueRef ref) {
	auto& refCounter = dynamicVars.at(ref);
	// the ref counter should always be greater than zero.
	assert(refCounter > 0);
	refCounter--;
	while (!dynamicVars.empty() && dynamicVars.back() == 0) {
		dynamicVars.pop_back();
	}
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

uint64_t hashDynamicVector(const DynamicValueMap& data) {
	size_t hash = offset_basis;
	for (auto value : data) {
		hash ^= value;
		hash *= fnv_prime;
	}
	return hash;
}

Snapshot TraceContext::recordSnapshot() {
	return {tagRecorder.createTag(), hashStaticVector(staticVars) ^ hashDynamicVector(dynamicVars)};
}

} // namespace nautilus::tracing
