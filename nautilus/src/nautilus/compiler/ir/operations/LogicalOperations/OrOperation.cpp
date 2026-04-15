
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"

namespace nautilus::compiler::ir {

OrOperation::OrOperation(common::Arena& arena, OperationIdentifier identifier, Operation* leftInput,
                         Operation* rightInput)
    : BinaryOperation(arena, OperationType::OrOp, identifier, Type::b, leftInput, rightInput) {
}

bool OrOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::OrOp;
}
} // namespace nautilus::compiler::ir
