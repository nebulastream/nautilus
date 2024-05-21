
#include "nautilus/compiler/ir/operations/LogicalOperations/NegateOperation.hpp"

namespace nautilus::compiler::ir {

NegateOperation::NegateOperation(OperationIdentifier identifier, Operation* input)
    : Operation(OperationType::NegateOp, identifier, Type::b, {input}) {
}

std::string NegateOperation::toString() {
	return identifier.toString() + "= not " + getInput()->getIdentifier().toString();
}

bool NegateOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::AddOp;
}

Operation* NegateOperation::getInput() {
	return inputs[0];
}

void NegateOperation::setInput(Operation* newInput) {
	this->inputs[0] = newInput;
}
} // namespace nautilus::compiler::ir
