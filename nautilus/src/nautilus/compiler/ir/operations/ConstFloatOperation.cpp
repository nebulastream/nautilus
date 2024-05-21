
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <string>

namespace nautilus::compiler::ir {

ConstFloatOperation::ConstFloatOperation(OperationIdentifier identifier, double constantValue, Type stamp)
    : Operation(OperationType::ConstFloatOp, identifier, stamp), constantValue(constantValue) {
}

double ConstFloatOperation::getValue() {
	return constantValue;
}

bool ConstFloatOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::ConstIntOp;
}

std::string ConstFloatOperation::toString() {
	return identifier.toString() + " = " + std::to_string(constantValue);
}

} // namespace nautilus::compiler::ir
