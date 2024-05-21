

#include <nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp>

namespace nautilus::compiler::ir {

BinaryCompOperation::BinaryCompOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput,
                                         Type type)
    : BinaryOperation(OperationType::AddOp, identifier, leftInput->getStamp(), leftInput, rightInput), type(type) {
}

std::string BinaryCompOperation::toString() {
	if (type == BOR) {
		return getIdentifier().toString() + " = " + getLeftInput()->getIdentifier().toString() + " | " +
		       getRightInput()->getIdentifier().toString();
	} else {
		return getIdentifier().toString() + " = " + getLeftInput()->getIdentifier().toString() + " & " +
		       getRightInput()->getIdentifier().toString();
	}
}

bool BinaryCompOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::AddOp;
}
} // namespace nautilus::compiler::ir
