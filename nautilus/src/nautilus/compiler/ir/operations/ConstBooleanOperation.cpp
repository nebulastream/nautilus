
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <string>

namespace nautilus::compiler::ir {

ConstBooleanOperation::ConstBooleanOperation(OperationIdentifier identifier, bool constantValue) : Operation(OperationType::ConstBooleanOp, identifier, Type::b), constantValue(constantValue) {
}

bool ConstBooleanOperation::getValue() {
	return constantValue;
}

bool ConstBooleanOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::ConstBooleanOp;
}

std::string ConstBooleanOperation::toString() {
	return identifier.toString() + " = " + std::to_string(constantValue);
}

} // namespace nautilus::compiler::ir
