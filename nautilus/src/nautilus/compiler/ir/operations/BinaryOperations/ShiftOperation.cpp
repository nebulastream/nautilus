

#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"

namespace nautilus::compiler::ir {

ShiftOperation::ShiftOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput,
                               ShiftType type)
    : BinaryOperation(OperationType::AddOp, identifier, leftInput->getStamp(), leftInput, rightInput), type(type) {
}

std::string ShiftOperation::toString() {
	if (type == LS) {
		return getIdentifier().toString() + " = " + getLeftInput()->getIdentifier().toString() + " << " +
		       getRightInput()->getIdentifier().toString();
	} else {
		return getIdentifier().toString() + " = " + getLeftInput()->getIdentifier().toString() + " >> " +
		       getRightInput()->getIdentifier().toString();
	}
}

bool ShiftOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::AddOp;
}
} // namespace nautilus::compiler::ir
