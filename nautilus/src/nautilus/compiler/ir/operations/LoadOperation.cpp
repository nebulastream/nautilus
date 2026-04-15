
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"

namespace nautilus::compiler::ir {

LoadOperation::LoadOperation(common::Arena& arena, OperationIdentifier identifier, Operation* address, Type type)
    : Operation(arena, OperationType::LoadOp, identifier, type, {address}) {
}

const Operation* LoadOperation::getAddress() const {
	return inputs[0];
}

bool LoadOperation::classof(const Operation* op) {
	return op->getOperationType() == OperationType::LoadOp;
}

} // namespace nautilus::compiler::ir
