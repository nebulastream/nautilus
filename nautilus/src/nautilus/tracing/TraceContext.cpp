
#include "TraceContext.hpp"
#include "nautilus/logging.hpp"
#include "symbolic_execution/SymbolicExecutionContext.hpp"
#include "symbolic_execution/TraceTerminationException.hpp"
#include <cassert>

namespace nautilus::tracing {

static thread_local TraceContext* traceContext;

TraceContext* TraceContext::get() {
	return traceContext;
}

TraceContext* TraceContext::initialize(TagRecorder& tagRecorder) {
	traceContext = new TraceContext(tagRecorder);
	return traceContext;
}

void TraceContext::terminate() {
	delete traceContext;
	traceContext = nullptr;
}

TraceContext::TraceContext(TagRecorder& tagRecorder) : tagRecorder(tagRecorder), executionTrace(std::make_unique<ExecutionTrace>()), symbolicExecutionContext(std::make_unique<SymbolicExecutionContext>()) {
}

value_ref TraceContext::registerFunctionArgument(Type type, size_t index) {
	return executionTrace->setArgument(type, index);
}

void TraceContext::traceValueDestruction(nautilus::tracing::value_ref) {
	/*if (symbolicExecutionContext->getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW) {
	    auto currentOperation = executionTrace->getCurrentOperation();
	    executionTrace->nextOperation();
	    assert(currentOperation.op == FREE);
	    return;
	}*/
	// auto op = Op::FREE;
	// auto valueType = Type::v;
	// auto tag = tagRecorder.createTag();
	// executionTrace->addOperation(tag, varRefMap, op, valueType, 0, target);
	return;
}

value_ref TraceContext::traceLoad(value_ref src, Type resultType) {
	if (isFollowing()) {
		auto currentOperation = executionTrace->getCurrentOperation();
		executionTrace->nextOperation();
		assert(currentOperation.op == LOAD);
		return currentOperation.resultRef;
	}
	auto input = InputVariant(src);
	auto op = Op::LOAD;
	auto tag = recordSnapshot();
	if (executionTrace->checkTag(tag)) {
		auto resultRef = executionTrace->addOperationWithResult(tag, op, resultType, std::vector<InputVariant> {input});
		return resultRef;
	}
	throw TraceTerminationException();
};

void TraceContext::traceStore(value_ref target, value_ref src, Type valueType) {
	if (isFollowing()) {
		auto currentOperation = executionTrace->getCurrentOperation();
		executionTrace->nextOperation();
		assert(currentOperation.op == STORE);
		return;
	}
	auto op = Op::STORE;
	auto tag = recordSnapshot();
	if (executionTrace->checkTag(tag)) {
		executionTrace->addOperation(tag, op, valueType, target, src);
		return;
	}
	throw TraceTerminationException();
}

bool TraceContext::isFollowing() {
	return symbolicExecutionContext->getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW;
}

value_ref TraceContext::traceConstValue(Type type, std::any constValue) {
	log::debug("Trace Constant");
	if (isFollowing()) {
		auto currentOperation = executionTrace->getCurrentOperation();
		executionTrace->nextOperation();
		assert(currentOperation.op == CONST);
		return currentOperation.resultRef;
	}
	auto input = InputVariant(constValue);
	auto op = Op::CONST;
	auto tag = recordSnapshot();
	auto globalTabIter = executionTrace->globalTagMap.find(tag);
	if (globalTabIter != executionTrace->globalTagMap.end()) {
		auto& ref = globalTabIter->second;
		auto originalRef = executionTrace->getBlocks()[ref.blockIndex].operations[ref.operationIndex];
		auto resultRef = executionTrace->addOperationWithResult(tag, op, type, std::vector<InputVariant> {input});
		executionTrace->addAssignmentOperation(tag, originalRef.resultRef, resultRef, resultRef.type);
		return originalRef.resultRef;
	} else {
		auto resultRef = executionTrace->addOperationWithResult(tag, op, type, std::vector<InputVariant> {input});
		return resultRef;
	}
}

Tag* TraceContext::getTag() {
	return tagRecorder.createTag();
}

value_ref TraceContext::traceCopy(nautilus::tracing::value_ref ref) {
	log::debug("Trace Copy");
	if (symbolicExecutionContext->getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW) {
		auto currentOperation = executionTrace->getCurrentOperation();
		executionTrace->nextOperation();
		assert(currentOperation.op == ASSIGN);
		return currentOperation.resultRef;
	}
	auto tag = recordSnapshot();
	if (executionTrace->checkTag(tag)) {
		auto resultRef = executionTrace->getNextValueRef();
		executionTrace->addAssignmentOperation(tag, {resultRef, ref.type}, ref, ref.type);
		return {resultRef, ref.type};
	}
	throw TraceTerminationException();
}

value_ref TraceContext::traceCall(const std::string& functionName, void* fptn, Type resultType, std::vector<tracing::value_ref> arguments) {
	if (symbolicExecutionContext->getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW) {
		auto currentOperation = executionTrace->getCurrentOperation();
		executionTrace->nextOperation();
		assert(currentOperation.op == CALL);
		//   executionTrace->variableBitset[currentOperation.resultRef] = true;
		return currentOperation.resultRef;
	}

	auto tag = recordSnapshot();
	if (executionTrace->checkTag(tag)) {
		auto functionArguments = InputVariant(FunctionCall {
		    .functionName = functionName,
		    .ptr = fptn,
		    .arguments = arguments,
		});
		auto op = Op::CALL;
		auto resultRef = executionTrace->addOperationWithResult(tag, op, resultType, std::vector<InputVariant> {functionArguments});
		//  executionTrace->variableBitset[resultRef] = true;
		return resultRef;
	}
	throw TraceTerminationException();
}

void TraceContext::traceAssignment(value_ref targetRef, value_ref sourceRef, Type resultType) {
	if (symbolicExecutionContext->getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW) {
		auto currentOperation = executionTrace->getCurrentOperation();
		executionTrace->nextOperation();
		assert(currentOperation.op == ASSIGN);
		return;
	}
	auto tag = recordSnapshot();

	if (executionTrace->checkTag(tag)) {
		executionTrace->addAssignmentOperation(tag, targetRef, sourceRef, resultType);
		return;
	}
	throw TraceTerminationException();
}

value_ref TraceContext::traceCast(value_ref state, Type resultType) {
	// std::cout <<executionTrace->variableBitset << std::endl;
	if (isFollowing()) {
		auto currentOperation = executionTrace->getCurrentOperation();
		executionTrace->nextOperation();
		assert(currentOperation.op == CAST);
		//   executionTrace->variableBitset[currentOperation.resultRef] = true;
		return currentOperation.resultRef;
	}
	// TODO is expected? check if we repeat a known trace or if this is a new operation.
	// we are in a know operation if the operation at the current block[currentOperationCounter] is equal to the
	// received operation.
	auto tag = recordSnapshot();
	if (executionTrace->checkTag(tag)) {
		auto leftIV = InputVariant(state);
		auto op = Op::CAST;
		auto resultRef = executionTrace->addOperationWithResult(tag, op, resultType, std::vector<InputVariant> {leftIV});
		//  executionTrace->variableBitset[resultRef] = true;
		return resultRef;
	}
	throw TraceTerminationException();
}

void TraceContext::traceReturnOperation(Type type, value_ref ref) {
	if (symbolicExecutionContext->getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW) {
		auto currentOperation = executionTrace->getCurrentOperation();
		executionTrace->nextOperation();
		assert(currentOperation.op == RETURN);
		return;
	}
	auto tag = recordSnapshot();
	executionTrace->addReturn(tag, type, ref);
	return;
}

value_ref TraceContext::traceUnaryOperation(nautilus::tracing::Op op, Type resultType, nautilus::tracing::value_ref& inputRef) {
	if (isFollowing()) {
		auto currentOperation = executionTrace->getCurrentOperation();
		executionTrace->nextOperation();
		assert(currentOperation.op == op);
		return currentOperation.resultRef;
	}

	auto tag = recordSnapshot();
	if (executionTrace->checkTag(tag)) {
		auto inputVariant = InputVariant(inputRef);
		auto resultRef = executionTrace->addOperationWithResult(tag, op, resultType, std::vector<InputVariant> {inputVariant});
		return resultRef;
	}
	throw TraceTerminationException();
}

value_ref TraceContext::traceBinaryOperation(Op op, Type resultType, value_ref& leftRef, value_ref& rightRef) {
	// std::cout <<executionTrace->variableBitset << std::endl;
	if (isFollowing()) {
		auto currentOperation = executionTrace->getCurrentOperation();
		executionTrace->nextOperation();
		assert(currentOperation.op == op);
		//   executionTrace->variableBitset[currentOperation.resultRef] = true;
		return currentOperation.resultRef;
	}

	auto tag = recordSnapshot();
	if (executionTrace->checkTag(tag)) {
		auto leftIV = InputVariant(leftRef);
		auto rightIV = InputVariant(rightRef);
		auto resultRef = executionTrace->addOperationWithResult(tag, op, resultType, std::vector<InputVariant> {leftIV, rightIV});
		return resultRef;
	}
	throw TraceTerminationException();
}

bool TraceContext::traceCmp(value_ref targetRef) {
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
			// this is actually the same tag -> throw up
			throw TraceTerminationException();
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
	auto traceIteration = 0;
	while (tc->symbolicExecutionContext->shouldContinue()) {
		try {
			traceIteration = traceIteration + 1;
			log::trace("Trace Iteration {}", traceIteration);
			log::trace("{}", tc->executionTrace->toString());

			tc->symbolicExecutionContext->next();
			tc->executionTrace->resetExecution();
			TraceContext::get()->resume();
			traceFunction();
		} catch (const TraceTerminationException& ex) {
		}
	}
	auto trace = std::move(tc->executionTrace);
	terminate();
	log::debug("Tracing Terminated with {} iterations", traceIteration);
	log::trace("Final trace: {}", trace->toString());
	return trace;
}

std::vector<StaticVarHolder>& TraceContext::getStaticVars() {
	return staticVars;
}

Snapshot TraceContext::recordSnapshot() {
	return {tagRecorder.createTag(), hashStaticVector(staticVars)};
}

} // namespace nautilus::tracing
