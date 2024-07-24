

#include <nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp>

namespace nautilus::compiler::ir {

BinaryCompOperation::BinaryCompOperation(OperationIdentifier identifier, Operation* leftInput, Operation* rightInput,
                                         Type type)
    : BinaryOperation(OperationType::BinaryComp, identifier, leftInput->getStamp(), leftInput, rightInput), type(type) {
}

std::string BinaryCompOperation::toString() {

	std::string opSymbol;
	switch (type) {
	case BAND:
		opSymbol = " & ";
		break;
	case BOR:
		opSymbol = " | ";
		break;
	case XOR:
		opSymbol = " ^ ";
		break;
	}

	return getIdentifier().toString() + " = " + getLeftInput()->getIdentifier().toString() + opSymbol +
	       getRightInput()->getIdentifier().toString();
}

BinaryCompOperation::Type BinaryCompOperation::getType() const {
	return type;
}

bool BinaryCompOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::BinaryComp;
}
} // namespace nautilus::compiler::ir
