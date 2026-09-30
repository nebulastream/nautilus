
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <string>

namespace nautilus::compiler::ir {

ConstFloatOperation::ConstFloatOperation(common::Arena& /*arena*/, OperationIdentifier identifier, double constantValue,
                                         Type stamp, ConstantOrigin origin)
    : Operation(OperationType::ConstFloatOp, identifier, stamp), constantValue(constantValue), constantOrigin(origin) {
}

double ConstFloatOperation::getValue() const {
	return constantValue;
}

ConstantOrigin ConstFloatOperation::getConstantOrigin() const {
	return constantOrigin;
}

bool ConstFloatOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::ConstFloatOp;
}

} // namespace nautilus::compiler::ir
