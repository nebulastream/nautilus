
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"

namespace nautilus::compiler::ir {

LoadOperation::LoadOperation(const OperationIdentifier& identifier, Operation* address, Type type)
    : Operation(OperationType::LoadOp, identifier, type, {address}) {
}

const Operation* LoadOperation::getAddress() {
	return inputs[0];
}

std::string LoadOperation::toString() {
	return identifier.toString() + " = load(" + getAddress()->getIdentifier().toString() + ")";
}

} // namespace nautilus::compiler::ir
