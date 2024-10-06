
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstdint>
#include <string>

namespace nautilus::compiler::ir {

ConstPtrOperation::ConstPtrOperation(OperationIdentifier identifier, void* constantValue)
    : Operation(OperationType::ConstPtrOp, identifier, Type::ptr), constantValue(constantValue) {
}

void* ConstPtrOperation::getValue() const {
	return constantValue;
}

bool ConstPtrOperation::classof(const Operation*) {
	return false;
}

} // namespace nautilus::compiler::ir
