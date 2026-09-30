
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <string>

namespace nautilus::compiler::ir {

ConstBooleanOperation::ConstBooleanOperation(common::Arena& /*arena*/, OperationIdentifier identifier,
                                             bool constantValue, ConstantOrigin origin)
    : Operation(OperationType::ConstBooleanOp, identifier, Type::b), constantValue(constantValue),
      constantOrigin(origin) {
}

bool ConstBooleanOperation::getValue() const {
	return constantValue;
}

ConstantOrigin ConstBooleanOperation::getConstantOrigin() const {
	return constantOrigin;
}

bool ConstBooleanOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::ConstBooleanOp;
}

} // namespace nautilus::compiler::ir
