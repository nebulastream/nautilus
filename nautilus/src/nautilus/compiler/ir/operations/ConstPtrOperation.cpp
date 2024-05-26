
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstdint>
#include <string>

namespace nautilus::compiler::ir {

ConstPtrOperation::ConstPtrOperation(OperationIdentifier identifier, void* constantValue)
    : Operation(OperationType::ConstPtrOp, identifier, Type::ptr), constantValue(constantValue) {
}

void* ConstPtrOperation::getValue() {
	return constantValue;
}

bool ConstPtrOperation::classof(const Operation*) {
	return false;
}

std::string ConstPtrOperation::toString() {
	return identifier.toString() + " = ptr" ;
}

} // namespace nautilus::compiler::ir
