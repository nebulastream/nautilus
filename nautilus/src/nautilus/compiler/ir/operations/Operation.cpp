
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/operations/OperationProperties.hpp"

namespace nautilus::compiler::ir {

std::string OperationIdentifier::toString() const {
	return "$" + std::to_string(id);
}

bool Operation::isConstOperation() const {
	// Note: historically only the int/float/bool const ops were considered
	// "const" — ConstPtrOp was not. The property table reports ConstPtrOp as
	// Constant too, so gate that out here to preserve behavior.
	return opType != OperationType::ConstPtrOp && isConstantOp(opType);
}

BinaryOperation::BinaryOperation(common::Arena& arena, OperationType opType, OperationIdentifier identifier, Type type,
                                 Operation* left, Operation* right)
    : Operation(arena, opType, identifier, type, {left, right}) {
}

bool BinaryOperation::classof(const Operation* op) {
	switch (op->getOperationType()) {
	case OperationType::AddOp:
	case OperationType::SubOp:
	case OperationType::MulOp:
	case OperationType::DivOp:
	case OperationType::ModOp:
	case OperationType::AndOp:
	case OperationType::OrOp:
	case OperationType::ShiftOp:
	case OperationType::CompareOp:
	case OperationType::BinaryComp:
		return true;
	default:
		return false;
	}
}

Operation* BinaryOperation::getLeftInput() const {
	return inputs[0];
}

Operation* BinaryOperation::getRightInput() const {
	return inputs[1];
}

void BinaryOperation::setLeftInput(Operation* newLeftInput) {
	inputs[0] = newLeftInput;
}

void BinaryOperation::setRightInput(Operation* newRightInput) {
	inputs[1] = newRightInput;
}

} // namespace nautilus::compiler::ir
