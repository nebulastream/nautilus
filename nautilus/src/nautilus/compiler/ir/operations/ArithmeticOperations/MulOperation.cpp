

#include <nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp>
#include <string>

namespace nautilus::compiler::ir {
MulOperation::MulOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput)
    : BinaryOperation(OperationType::MulOp, identifier, leftInput->getStamp(), leftInput, rightInput) {
}

std::string MulOperation::toString() {
	return getIdentifier().toString() + " = " + getLeftInput()->getIdentifier().toString() + " * " +
	       getRightInput()->getIdentifier().toString();
}

bool MulOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::MulOp;
}

} // namespace nautilus::compiler::ir
