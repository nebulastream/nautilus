
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include <utility>

namespace nautilus::compiler::ir {
ProxyCallOperation::ProxyCallOperation(OperationIdentifier identifier, const std::vector<Operation*>& inputArguments,
                                       Type resultType)
    : Operation(Operation::OperationType::ProxyCallOp, identifier, resultType, inputArguments) {
}

ProxyCallOperation::ProxyCallOperation(std::string functionSymbol, void* functionPtr, OperationIdentifier identifier,
                                       std::vector<Operation*> inputArguments, Type resultType)
    : Operation(Operation::OperationType::ProxyCallOp, identifier, resultType, std::move(inputArguments)),
      mangedFunctionSymbol(std::move(functionSymbol)), functionPtr(functionPtr) {
}

const std::vector<Operation*>& ProxyCallOperation::getInputArguments() const {
	return inputs;
}

void ProxyCallOperation::setInputArguments(std::vector<Operation*>& newInputArguments) {
	this->inputs = newInputArguments;
}

std::string ProxyCallOperation::toString() {
	std::string baseString = "";
	if (!identifier.toString().empty()) {
		baseString = identifier.toString() + " = ";
	}
	baseString = baseString + getFunctionSymbol() + "(";
	if (!inputs.empty()) {
		// baseString += inputArguments[0].lock()->getIdentifier().toString();
		// for (int i = 1; i < (int) inputArguments.size(); ++i) {
		//     baseString += ", " + inputArguments.at(i).lock()->getIdentifier().toString();

		baseString += inputs[0]->getIdentifier().toString();
		// baseString += inputArguments[0].lock()->getIdentifier();
		for (int i = 1; i < (int) inputs.size(); ++i) {
			baseString += ", " + inputs.at(i)->getIdentifier().toString();
			// baseString += ", " + inputArguments.at(i).lock()->getIdentifier();
		}
	}
	return baseString + ")";
}

std::string ProxyCallOperation::getFunctionSymbol() {
	return mangedFunctionSymbol;
}

void* ProxyCallOperation::getFunctionPtr() {
	return functionPtr;
}

} // namespace nautilus::compiler::ir
