
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {
Operation::Operation(OperationType opType, const OperationIdentifier& identifier, Type stamp,
                     const std::vector<Operation*>& inputs)
    : opType(opType), identifier(identifier), stamp(stamp), inputs(inputs) {
}

Operation::Operation(OperationType opType, Type stamp, const std::vector<Operation*>& inputs)
    : opType(opType), identifier(0), stamp(stamp), inputs(inputs) {
}

Operation::~Operation() noexcept = default;

Operation::OperationType Operation::getOperationType() const {
	return opType;
}

const OperationIdentifier& Operation::getIdentifier() const {
	return identifier;
}

const Type& Operation::getStamp() const {
	return stamp;
}

OperationIdentifier::OperationIdentifier(uint32_t ir) : id(ir) {
}

uint32_t OperationIdentifier::getId() const {
	return id;
}

bool OperationIdentifier::operator==(const OperationIdentifier& rhs) const {
	return id == rhs.id;
}

bool OperationIdentifier::operator!=(const OperationIdentifier& rhs) const {
	return !(rhs == *this);
}

std::string OperationIdentifier::toString() const {
	return "$" + std::to_string(id);
}

bool OperationIdentifier::operator<(const OperationIdentifier& rhs) const {
	if (id < rhs.id)
		return true;
	return false;
}

bool OperationIdentifier::operator>(const OperationIdentifier& rhs) const {
	return rhs < *this;
}

bool OperationIdentifier::operator<=(const OperationIdentifier& rhs) const {
	return !(rhs < *this);
}

bool OperationIdentifier::operator>=(const OperationIdentifier& rhs) const {
	return !(*this < rhs);
}

bool Operation::isConstOperation() const {
	return opType == OperationType::ConstBooleanOp || opType == OperationType::ConstFloatOp ||
	       opType == OperationType::ConstIntOp;
}

BinaryOperation::BinaryOperation(OperationType opType, const OperationIdentifier& identifier, Type type,
                                 Operation* left, Operation* right)
    : Operation(opType, identifier, type, {left, right}) {
}

Operation* BinaryOperation::getLeftInput() const {
	return inputs[0];
}

Operation* BinaryOperation::getRightInput() const {
	return inputs[1];
}

void BinaryOperation::setLeftInput(Operation* newLeftInput) {
	inputs[0] = newLeftInput;
}

void BinaryOperation::setRightInput(Operation* newRightInput) {
	inputs[1] = newRightInput;
}

} // namespace nautilus::compiler::ir
