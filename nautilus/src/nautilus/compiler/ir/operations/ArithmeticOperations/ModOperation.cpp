

#include <nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp>
#include <string>

namespace nautilus::compiler::ir {
ModOperation::ModOperation(common::Arena& arena, OperationIdentifier identifier, Operation* leftInput,
                           Operation* rightInput)
    : BinaryOperation(arena, OperationType::ModOp, identifier, leftInput->getStamp(), leftInput, rightInput) {
}

bool ModOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::ModOp;
}

} // namespace nautilus::compiler::ir
