
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstdint>
#include <utility>

namespace nautilus::compiler::ir {

AllocaOperation::AllocaOperation(common::Arena& /*arena*/, OperationIdentifier id, uint32_t allocaIndex,
                                 std::optional<CleanupEffect> cleanupEffect)
    : Operation(OperationType::AllocaOp, id, Type::ptr), allocaIndex(allocaIndex),
      cleanupEffect(std::move(cleanupEffect)) {
}

uint32_t AllocaOperation::getIndex() const {
	return allocaIndex;
}

const std::optional<CleanupEffect>& AllocaOperation::getCleanupEffect() const {
	return cleanupEffect;
}

bool AllocaOperation::classof(const Operation* op) {
	return op->getOperationType() == OperationType::AllocaOp;
}

} // namespace nautilus::compiler::ir
