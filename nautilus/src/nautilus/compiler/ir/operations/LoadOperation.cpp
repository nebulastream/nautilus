
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"

namespace nautilus::compiler::ir {

LoadOperation::LoadOperation(const OperationIdentifier& identifier, Operation* address, Type type)
    : Operation(OperationType::LoadOp, identifier, type, {address}), hints_() {
}

LoadOperation::LoadOperation(const OperationIdentifier& identifier, Operation* address, Type type, LoadHints hints)
    : Operation(OperationType::LoadOp, identifier, type, {address}), hints_(hints) {
}

const Operation* LoadOperation::getAddress() const {
	return inputs[0];
}

const LoadHints& LoadOperation::getLoadHints() const {
	return hints_;
}

} // namespace nautilus::compiler::ir
