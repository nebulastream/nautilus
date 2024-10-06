
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"

namespace nautilus::compiler::ir {

OrOperation::OrOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput)
    : BinaryOperation(OperationType::OrOp, identifier, Type::b, leftInput, rightInput) {
}

bool OrOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::AddOp;
}
} // namespace nautilus::compiler::ir
