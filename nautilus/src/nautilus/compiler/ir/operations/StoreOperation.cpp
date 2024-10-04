
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"

namespace nautilus::compiler::ir {

StoreOperation::StoreOperation(Operation* value, Operation* address) : Operation(OperationType::StoreOp, Type::v, {value, address}) {
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

} // namespace nautilus::compiler::ir
