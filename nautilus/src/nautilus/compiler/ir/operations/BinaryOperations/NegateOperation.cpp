
#include "nautilus/compiler/ir/operations/BinaryOperations/NegateOperation.hpp"

namespace nautilus::compiler::ir {

NegateOperation::NegateOperation(OperationIdentifier identifier, Operation* input)
    : Operation(OperationType::NegateOp, identifier, input->getStamp(), {input}) {
}

std::string NegateOperation::toString() {
	return identifier.toString() + "= ~" + getInput()->getIdentifier().toString();
}

bool NegateOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::NegateOp;
}

Operation* NegateOperation::getInput() {
	return inputs[0];
}

void NegateOperation::setInput(Operation* newInput) {
	this->inputs[0] = newInput;
}
} // namespace nautilus::compiler::ir
