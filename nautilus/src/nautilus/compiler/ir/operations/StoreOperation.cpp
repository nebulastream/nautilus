
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

StoreOperation::StoreOperation(Operation* value, Operation* address) : Operation(OperationType::StoreOp, Type::v, {value, address}) {
}

Operation* StoreOperation::getValue() {
	return inputs[0];
}

Operation* StoreOperation::getAddress() {
	return inputs[1];
}

void StoreOperation::setValue(Operation* newValue) {
	inputs[0] = newValue;
}

void StoreOperation::setAddress(Operation* newAddress) {
	inputs[1] = newAddress;
}

std::string StoreOperation::toString() {
	return "store(" + getValue()->getIdentifier().toString() + ", " + getAddress()->getIdentifier().toString() + ")";
}
} // namespace nautilus::compiler::ir
