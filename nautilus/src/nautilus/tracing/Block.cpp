#include "nautilus/tracing/Block.hpp"
#include "nautilus/tracing/TraceOperation.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/tracing/Types.hpp"

namespace nautilus::tracing {

Block::Block(uint16_t blockId) : blockId(blockId), type(Type::Default) {
}

operation_identifier Block::addOperation(nautilus::tracing::TraceOperation&& operation) {
	uint32_t operationIndex = operations.size();
	operations.emplace_back(std::move(operation));
	return {blockId, operationIndex};
}

void Block::addArgument(TypedValueRef ref) {
	// Only add ref to arguments if it does not already exist. O(1) via argumentSet.
	if (argumentSet.emplace(ref.ref).second) {
		arguments.emplace_back(ref);
	}
}

} // namespace nautilus::tracing
