
#include "nautilus/compiler/ir/operations/CastOperation.hpp"

namespace nautilus::compiler::ir {

CastOperation::CastOperation(OperationIdentifier identifier, Operation* input, Type targetStamp)
    : Operation(OperationType::CastOp, identifier, targetStamp, {input}) {
}

Operation* CastOperation::getInput() const {
	return inputs[0];
}

void CastOperation::setInput(Operation* newInput) {
	this->inputs[0] = newInput;
}

} // namespace nautilus::compiler::ir
