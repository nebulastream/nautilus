

#include <nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp>

namespace nautilus::compiler::ir {

BinaryCompOperation::BinaryCompOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput, Type type)
    : BinaryOperation(OperationType::BinaryComp, identifier, leftInput->getStamp(), leftInput, rightInput), type(type) {
}

BinaryCompOperation::Type BinaryCompOperation::getType() const {
	return type;
}

bool BinaryCompOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::BinaryComp;
}
} // namespace nautilus::compiler::ir
