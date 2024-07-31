
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include <cstdint>
#include <string>

namespace nautilus::compiler::ir {
ReturnOperation::ReturnOperation() : Operation(Operation::OperationType::ReturnOp, Type::v) {
}

ReturnOperation::ReturnOperation(Operation* returnValue) : Operation(Operation::OperationType::ReturnOp, returnValue->getStamp(), {returnValue}) {
}

std::string ReturnOperation::toString() {
	if (hasReturnValue()) {
		return "return (" + getReturnValue()->getIdentifier().toString() + ")";
	} else {
		return "return";
	}
}

Operation* ReturnOperation::getReturnValue() {
	return inputs[0];
}

void ReturnOperation::setReturnValue(Operation* newReturnValue) {
	inputs[0] = newReturnValue;
}

bool ReturnOperation::hasReturnValue() {
	return stamp != Type::v;
}

} // namespace nautilus::compiler::ir
