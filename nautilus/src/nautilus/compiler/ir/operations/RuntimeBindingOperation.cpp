#include "nautilus/compiler/ir/operations/RuntimeBindingOperation.hpp"

namespace nautilus::compiler::ir {

RuntimeBindingOperation::RuntimeBindingOperation(common::Arena&, OperationIdentifier identifier,
                                                 const runtime_binding::Entry& binding)
    : Operation(OperationType::RuntimeBindingOp, identifier, Type::ptr), binding(binding) {
}

const runtime_binding::Entry& RuntimeBindingOperation::getBinding() const {
	return binding;
}

bool RuntimeBindingOperation::classof(const Operation* op) {
	return op->getOperationType() == OperationType::RuntimeBindingOp;
}

} // namespace nautilus::compiler::ir
