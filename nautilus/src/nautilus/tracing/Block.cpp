#include "nautilus/tracing/Block.hpp"
#include "nautilus/tracing/TraceOperation.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/tracing/Types.hpp"
#include <algorithm>

namespace nautilus::tracing {

Block::Block(uint32_t blockId) : blockId(blockId), type(Type::Default) {
	// Most blocks contain only a handful of operations; reserve a small
	// initial capacity to avoid reallocating the operations pointer vector
	// in the common case.
	operations.reserve(4);
}

operation_identifier Block::addOperation(TraceOperation* operation) {
	uint32_t operationIndex = operations.size();
	operations.push_back(operation);
	return {blockId, operationIndex};
}

void Block::addArgument(TypedValueRef ref) {
	// only add ref to arguments if it not already exists
	if (std::find(arguments.begin(), arguments.end(), ref) == arguments.end()) {
		arguments.emplace_back(ref);
	}
}

} // namespace nautilus::tracing
