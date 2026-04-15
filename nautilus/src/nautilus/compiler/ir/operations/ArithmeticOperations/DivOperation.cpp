
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include <string>

namespace nautilus::compiler::ir {
DivOperation::DivOperation(common::Arena& arena, OperationIdentifier identifier, Operation* leftInput,
                           Operation* rightInput)
    : BinaryOperation(arena, OperationType::DivOp, identifier, leftInput->getStamp(), leftInput, rightInput) {
}

bool DivOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::DivOp;
}

} // namespace nautilus::compiler::ir
