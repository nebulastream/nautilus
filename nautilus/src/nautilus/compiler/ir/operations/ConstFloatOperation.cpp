
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <string>

namespace nautilus::compiler::ir {

ConstFloatOperation::ConstFloatOperation(OperationIdentifier identifier, double constantValue, Type stamp)
    : Operation(OperationType::ConstFloatOp, identifier, stamp), constantValue(constantValue) {
}

double ConstFloatOperation::getValue() const {
	return constantValue;
}

bool ConstFloatOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::ConstFloatOp;
}

} // namespace nautilus::compiler::ir
