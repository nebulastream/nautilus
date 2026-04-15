
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include <cstdint>
#include <string>

namespace nautilus::compiler::ir {
ReturnOperation::ReturnOperation(common::Arena& /*arena*/) : Operation(Operation::OperationType::ReturnOp, Type::v) {
}

ReturnOperation::ReturnOperation(common::Arena& arena, Operation* returnValue)
    : Operation(arena, Operation::OperationType::ReturnOp, OperationIdentifier {0}, returnValue->getStamp(),
                {returnValue}) {
}

Operation* ReturnOperation::getReturnValue() const {
	return inputs[0];
}

void ReturnOperation::setReturnValue(Operation* newReturnValue) {
	inputs[0] = newReturnValue;
}

bool ReturnOperation::hasReturnValue() const {
	return stamp != Type::v;
}

bool ReturnOperation::classof(const Operation* op) {
	return op->getOperationType() == OperationType::ReturnOp;
}

} // namespace nautilus::compiler::ir
