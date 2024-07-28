

#include <nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp>
#include <string>

namespace nautilus::compiler::ir {
ModOperation::ModOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput) : BinaryOperation(OperationType::ModOp, identifier, leftInput->getStamp(), leftInput, rightInput) {
}

std::string ModOperation::toString() {
	return getIdentifier().toString() + " = " + getLeftInput()->getIdentifier().toString() + " % " + getRightInput()->getIdentifier().toString();
}

bool ModOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::DivOp;
}

} // namespace nautilus::compiler::ir
