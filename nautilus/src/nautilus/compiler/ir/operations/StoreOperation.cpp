
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

StoreOperation::StoreOperation(common::Arena& arena, Operation* value, Operation* address)
    : Operation(arena, OperationType::StoreOp, OperationIdentifier {0}, Type::v, {value, address}) {
}

Operation* StoreOperation::getValue() const {
	return inputs[0];
}

Operation* StoreOperation::getAddress() const {
	return inputs[1];
}

void StoreOperation::setValue(Operation* newValue) {
	inputs[0] = newValue;
}

void StoreOperation::setAddress(Operation* newAddress) {
	inputs[1] = newAddress;
}

bool StoreOperation::classof(const Operation* op) {
	return op->getOperationType() == OperationType::StoreOp;
}

} // namespace nautilus::compiler::ir
