
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"

namespace nautilus::compiler::ir {

AndOperation::AndOperation(common::Arena& arena, OperationIdentifier identifier, Operation* leftInput,
                           Operation* rightInput)
    : BinaryOperation(arena, OperationType::AndOp, identifier, Type::b, leftInput, rightInput) {
}

bool AndOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::AndOp;
}

} // namespace nautilus::compiler::ir
