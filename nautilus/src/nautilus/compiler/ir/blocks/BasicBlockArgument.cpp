
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"

namespace nautilus::compiler::ir {
BasicBlockArgument::BasicBlockArgument(const OperationIdentifier identifier, Type stamp)
    : Operation(OperationType::BasicBlockArgument, identifier, stamp) {
}

bool BasicBlockArgument::classof(const Operation* op) {
	return op->getOperationType() == OperationType::BasicBlockArgument;
}
} // namespace nautilus::compiler::ir
