
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstdint>
#include <string>

namespace nautilus::compiler::ir {

ConstPtrOperation::ConstPtrOperation(common::Arena& /*arena*/, OperationIdentifier identifier, void* constantValue)
    : Operation(OperationType::ConstPtrOp, identifier, Type::ptr), constantValue(constantValue) {
}

void* ConstPtrOperation::getValue() const {
	return constantValue;
}

bool ConstPtrOperation::classof(const Operation* Op) {
	return Op->getOperationType() == OperationType::ConstPtrOp;
}

} // namespace nautilus::compiler::ir
