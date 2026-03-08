
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstddef>

namespace nautilus::compiler::ir {

AllocaOperation::AllocaOperation(size_t allocationSize)
    : Operation(OperationType::AllocaOp, Type::ptr, {}), allocationSize(allocationSize) {
}

size_t AllocaOperation::getSize() const {
	return allocationSize;
}

} // namespace nautilus::compiler::ir
