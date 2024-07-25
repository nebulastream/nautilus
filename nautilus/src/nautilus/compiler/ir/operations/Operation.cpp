
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/NegateOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include <string>

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

std::ostream& operator<<(std::ostream& os, const OperationIdentifier& identifier) {
	os << identifier.id;
	return os;
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

Operation* BinaryOperation::getLeftInput() {
	return inputs[0];
}

Operation* BinaryOperation::getRightInput() {
	return inputs[1];
}

void BinaryOperation::setLeftInput(Operation* newLeftInput) {
	inputs[0] = newLeftInput;
}

void BinaryOperation::setRightInput(Operation* newRightInput) {
	inputs[1] = newRightInput;
}

} // namespace nautilus::compiler::ir
