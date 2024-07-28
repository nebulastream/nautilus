
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"

namespace nautilus::compiler::ir {
BasicBlockArgument::BasicBlockArgument(const OperationIdentifier identifier, Type stamp) : Operation(OperationType::BasicBlockArgument, identifier, stamp) {
}

std::ostream& operator<<(std::ostream& os, const BasicBlockArgument& argument) {
	os << argument.identifier;
	return os;
}

std::string BasicBlockArgument::toString() {
	return identifier.toString();
}
} // namespace nautilus::compiler::ir
