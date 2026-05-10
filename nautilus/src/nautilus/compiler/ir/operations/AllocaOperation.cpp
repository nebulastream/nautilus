
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstdint>

namespace nautilus::compiler::ir {

AllocaOperation::AllocaOperation(common::Arena& /*arena*/, OperationIdentifier id, uint32_t allocaIndex)
    : Operation(OperationType::AllocaOp, id, Type::ptr), allocaIndex(allocaIndex) {
}

uint32_t AllocaOperation::getIndex() const {
	return allocaIndex;
}

bool AllocaOperation::classof(const Operation* op) {
	return op->getOperationType() == OperationType::AllocaOp;
}

} // namespace nautilus::compiler::ir
