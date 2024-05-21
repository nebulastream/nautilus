
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"

namespace nautilus::compiler::ir {

AndOperation::AndOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput)
    : BinaryOperation(OperationType::AndOp, identifier, Type::b, leftInput, rightInput) {
}

std::string AndOperation::toString() {
	return getIdentifier().toString() + " = " + getLeftInput()->getIdentifier().toString() + " and " +
	       getRightInput()->getIdentifier().toString();
}

bool AndOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::AddOp;
}

} // namespace nautilus::compiler::ir
