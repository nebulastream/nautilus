
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstdint>
#include <string>

namespace nautilus::compiler::ir {

ConstIntOperation::ConstIntOperation(common::Arena& /*arena*/, OperationIdentifier identifier, int64_t constantValue,
                                     Type stamp, ConstantOrigin origin)
    : Operation(OperationType::ConstIntOp, identifier, stamp), constantValue(constantValue), constantOrigin(origin) {
}

int64_t ConstIntOperation::getValue() const {
	return constantValue;
}

ConstantOrigin ConstIntOperation::getConstantOrigin() const {
	return constantOrigin;
}

bool ConstIntOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::ConstIntOp;
}

} // namespace nautilus::compiler::ir
