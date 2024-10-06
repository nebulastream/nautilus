

#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"

namespace nautilus::compiler::ir {

ShiftOperation::ShiftOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput,
                               ShiftType type)
    : BinaryOperation(OperationType::ShiftOp, identifier, leftInput->getStamp(), leftInput, rightInput), type(type) {
}

ShiftOperation::ShiftType ShiftOperation::getType() const {
	return type;
}

bool ShiftOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::ShiftOp;
}
} // namespace nautilus::compiler::ir
