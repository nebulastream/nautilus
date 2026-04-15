
#include "nautilus/compiler/ir/operations/BinaryOperations/NegateOperation.hpp"

namespace nautilus::compiler::ir {

NegateOperation::NegateOperation(common::Arena& arena, OperationIdentifier identifier, Operation* input)
    : Operation(arena, OperationType::NegateOp, identifier, input->getStamp(), {input}) {
}

bool NegateOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::NegateOp;
}

Operation* NegateOperation::getInput() const {
	return inputs[0];
}

void NegateOperation::setInput(Operation* newInput) {
	this->inputs[0] = newInput;
}
} // namespace nautilus::compiler::ir
