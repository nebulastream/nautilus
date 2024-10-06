
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"

namespace nautilus::compiler::ir {

LoadOperation::LoadOperation(const OperationIdentifier& identifier, Operation* address, Type type)
    : Operation(OperationType::LoadOp, identifier, type, {address}) {
}

const Operation* LoadOperation::getAddress() const {
	return inputs[0];
}

} // namespace nautilus::compiler::ir
