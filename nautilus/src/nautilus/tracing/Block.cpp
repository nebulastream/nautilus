#include "nautilus/tracing/Block.hpp"
#include "nautilus/tracing/TraceOperation.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/tracing/Types.hpp"

namespace nautilus::tracing {

Block::Block(uint16_t blockId) : blockId(blockId), type(Type::Default) {
}

operation_identifier Block::addOperation(uint32_t operationIndex) {
	uint16_t localOperationIndex = operations.size();
	operations.emplace_back(operationIndex);
	return {blockId, localOperationIndex};
}

void Block::addArgument(TypedValueRef ref) {
	// only add ref to arguments if it not already exists
	if (std::find(arguments.begin(), arguments.end(), ref) == arguments.end()) {
		arguments.emplace_back(ref);
	}
}

} // namespace nautilus::tracing
