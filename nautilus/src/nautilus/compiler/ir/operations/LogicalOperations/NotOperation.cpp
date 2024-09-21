
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"

namespace nautilus::compiler::ir {

NotOperation::NotOperation(OperationIdentifier identifier, Operation* input) : Operation(OperationType::NotOp, identifier, Type::b, {input}) {
}

bool NotOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::NotOp;
}

Operation* NotOperation::getInput() const {
	return inputs[0];
}

void NotOperation::setInput(Operation* newInput) {
	this->inputs[0] = newInput;
}
} // namespace nautilus::compiler::ir
