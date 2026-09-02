
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include <utility>

namespace nautilus::compiler::ir {
CallOperation::CallOperation(common::Arena& arena, OperationIdentifier identifier,
                             std::span<Operation* const> inputArguments, Type resultType, FunctionId calleeId)
    : Operation(arena, Operation::OperationType::CallOp, identifier, resultType, inputArguments), calleeId(calleeId) {
}

CallOperation::CallOperation(common::Arena& arena, const std::string& functionSymbol, const std::string& functionName,
                             void* functionPtr, OperationIdentifier identifier,
                             std::span<Operation* const> inputArguments, Type resultType,
                             const FunctionAttributes fnAttrs, FunctionId calleeId, std::vector<Destructor> destructors,
                             bool exceptionHandling, void* captureFunc, bool isNautilusCall)
    : Operation(arena, Operation::OperationType::CallOp, identifier, resultType, inputArguments),
      mangedFunctionSymbol(functionSymbol), functionName(functionName), functionPtr(functionPtr),
      captureFunc(captureFunc), fnAttrs(fnAttrs), destructors(std::move(destructors)),
      exceptionHandling(exceptionHandling), isNautilusCall(isNautilusCall), calleeId(calleeId) {
}

std::span<Operation* const> CallOperation::getInputArguments() const {
	return getInputs();
}

void CallOperation::setInputArguments(common::Arena& arena, std::span<Operation* const> newInputArguments) {
	// Re-allocate a new buffer of the requested size from the arena and
	// rebind the inputs span. The previous buffer is left in the arena
	// (it is reclaimed in bulk on the next reset).
	this->inputs = allocateInputs(arena, newInputArguments);
}

const std::string& CallOperation::getFunctionName() const {
	return functionName;
}

const std::string& CallOperation::getFunctionSymbol() const {
	return mangedFunctionSymbol;
}

void* CallOperation::getFunctionPtr() {
	return functionPtr;
}

void* CallOperation::getCaptureFunc() const {
	return captureFunc;
}

const FunctionAttributes& CallOperation::getFunctionAttributes() const {
	return fnAttrs;
}

const std::vector<CallOperation::Destructor>& CallOperation::getDestructors() const {
	return destructors;
}

bool CallOperation::requiresExceptionHandling() const {
	return exceptionHandling;
}

bool CallOperation::isNautilusFunctionCall() const {
	return isNautilusCall;
}

void CallOperation::markNoThrow() {
	exceptionHandling = false;
	fnAttrs.noUnwind = true;
	destructors.clear();
	captureFunc = nullptr;
}

bool CallOperation::classof(const Operation* op) {
	return op->getOperationType() == OperationType::CallOp;
}

} // namespace nautilus::compiler::ir
