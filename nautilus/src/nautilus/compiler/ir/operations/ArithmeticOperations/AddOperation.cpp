
#include <nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp>

namespace nautilus::compiler::ir {

AddOperation::AddOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput)
    : BinaryOperation(OperationType::AddOp, identifier, leftInput->getStamp(), leftInput, rightInput) {
}

bool AddOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::AddOp;
}

} // namespace nautilus::compiler::ir
