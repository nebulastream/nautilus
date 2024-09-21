
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"

namespace nautilus::compiler::ir {
BasicBlockArgument::BasicBlockArgument(const OperationIdentifier identifier, Type stamp) : Operation(OperationType::BasicBlockArgument, identifier, stamp) {
}
} // namespace nautilus::compiler::ir
