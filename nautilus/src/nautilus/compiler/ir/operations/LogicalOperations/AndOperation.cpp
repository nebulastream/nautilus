
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"

namespace nautilus::compiler::ir {

AndOperation::AndOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput) : BinaryOperation(OperationType::AndOp, identifier, Type::b, leftInput, rightInput) {
}

bool AndOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::AddOp;
}

} // namespace nautilus::compiler::ir
