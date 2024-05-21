#include "nautilus/tracing/Block.hpp"
#include "nautilus/common/Types.hpp"
#include "nautilus/common/traceing.hpp"

namespace nautilus::tracing {

Block::Block(uint16_t blockId) : blockId(blockId), type(Type::Default) {};

operation_identifier Block::addOperation(nautilus::tracing::TraceOperation&& operation) {
	uint16_t operationIndex = operations.size();
	operations.emplace_back(operation);
	return {blockId, operationIndex};
}

void Block::addArgument(TypedValueRef ref) {
	// only add ref to arguments if it not already exists
	if (std::find(arguments.begin(), arguments.end(), ref) == arguments.end()) {
		arguments.emplace_back(ref);
	}
}

std::ostream& operator<<(std::ostream& os, const Block& block) {

	os << "(";
	for (size_t i = 0; i < block.arguments.size(); i++) {
		if (i != 0) {
			os << ",";
		}
		os << "$" << block.arguments[i].ref << ":" << toString(block.arguments[i].type);
	}
	os << ")";
	if (block.type == Block::Type::ControlFlowMerge) {
		os << " ControlFlowMerge";
	}
	os << "\n";
	for (const auto& operation : block.operations) {
		if (operation.op != FREE) {
			os << "\t" << operation << "\n";
		}
	}
	return os;
}

} // namespace nautilus::tracing
