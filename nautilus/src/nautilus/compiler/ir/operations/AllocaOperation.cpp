#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstdint>

namespace nautilus::compiler::ir {

AllocaOperation::AllocaOperation(OperationIdentifier id, uint32_t allocaIndex)
    : Operation(OperationType::AllocaOp, id, Type::ptr), allocaIndex(allocaIndex) {
}

uint32_t AllocaOperation::getIndex() const {
	return allocaIndex;
}

} // namespace nautilus::compiler::ir
