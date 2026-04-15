

#include <nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp>
#include <string>

namespace nautilus::compiler::ir {
MulOperation::MulOperation(common::Arena& arena, OperationIdentifier identifier, Operation* leftInput,
                           Operation* rightInput)
    : BinaryOperation(arena, OperationType::MulOp, identifier, leftInput->getStamp(), leftInput, rightInput) {
}

bool MulOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::MulOp;
}

} // namespace nautilus::compiler::ir
