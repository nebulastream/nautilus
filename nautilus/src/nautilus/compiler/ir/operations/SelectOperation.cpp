#include "nautilus/compiler/ir/operations/SelectOperation.hpp"

namespace nautilus::compiler::ir {

SelectOperation::SelectOperation(OperationIdentifier identifier, Operation* condition, Operation* trueValue,
                                 Operation* falseValue, Type resultStamp)
    : Operation(OperationType::SelectOp, identifier, resultStamp, {condition, trueValue, falseValue}) {
}

Operation* SelectOperation::getCondition() const {
	return inputs[0];
}

Operation* SelectOperation::getTrueValue() const {
	return inputs[1];
}

Operation* SelectOperation::getFalseValue() const {
	return inputs[2];
}

void SelectOperation::setCondition(Operation* newCondition) {
	inputs[0] = newCondition;
}

void SelectOperation::setTrueValue(Operation* newTrueValue) {
	inputs[1] = newTrueValue;
}

void SelectOperation::setFalseValue(Operation* newFalseValue) {
	inputs[2] = newFalseValue;
}

} // namespace nautilus::compiler::ir
