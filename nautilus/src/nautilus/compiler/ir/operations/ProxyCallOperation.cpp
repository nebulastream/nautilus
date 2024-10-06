
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include <utility>

namespace nautilus::compiler::ir {
ProxyCallOperation::ProxyCallOperation(OperationIdentifier identifier, const std::vector<Operation*>& inputArguments,
                                       Type resultType)
    : Operation(Operation::OperationType::ProxyCallOp, identifier, resultType, inputArguments) {
}

ProxyCallOperation::ProxyCallOperation(const std::string& functionSymbol, const std::string& functionName,
                                       void* functionPtr, OperationIdentifier identifier,
                                       std::vector<Operation*> inputArguments, Type resultType)
    : Operation(Operation::OperationType::ProxyCallOp, identifier, resultType, std::move(inputArguments)),
      mangedFunctionSymbol(functionSymbol), functionName(functionName), functionPtr(functionPtr) {
}

const std::vector<Operation*>& ProxyCallOperation::getInputArguments() const {
	return inputs;
}

void ProxyCallOperation::setInputArguments(std::vector<Operation*>& newInputArguments) {
	this->inputs = newInputArguments;
}

const std::string& ProxyCallOperation::getFunctionName() const {
	return functionName;
}

const std::string& ProxyCallOperation::getFunctionSymbol() const {
	return mangedFunctionSymbol;
}

void* ProxyCallOperation::getFunctionPtr() {
	return functionPtr;
}

} // namespace nautilus::compiler::ir
