
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstdint>
#include <string>

namespace nautilus::compiler::ir {

ConstIntOperation::ConstIntOperation(OperationIdentifier identifier, int64_t constantValue, Type stamp)
    : Operation(OperationType::ConstIntOp, identifier, stamp), constantValue(constantValue) {
}

int64_t ConstIntOperation::getValue() const {
	return constantValue;
}

bool ConstIntOperation::classof(const Operation*) {
	return false;
}

} // namespace nautilus::compiler::ir
