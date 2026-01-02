#include "nautilus/tracing/Block.hpp"
#include "nautilus/tracing/TraceOperation.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/tracing/Types.hpp"
#include <algorithm>
#include <cassert>

namespace nautilus::tracing {

Block::Block(uint16_t blockId) : blockId(blockId), type(Type::Default) {
}

operation_identifier Block::addOperation(nautilus::tracing::TraceOperation&& operation) {
	uint16_t operationIndex = operations.size();
	operations.emplace_back(operation);
	auto inserted =
	    localValueRefPositions.try_emplace(operation.resultRef.ref, static_cast<int32_t>(operationIndex)).second;

	assert(operation.op == ASSIGN || operation.op == JMP || inserted && "Duplicate value ref");
	return {blockId, operationIndex};
}

void Block::addArgument(TypedValueRef ref) {
	localValueRefPositions.insert_or_assign(ref.ref, -1);
	// only add ref to arguments if it not already exists
	if (std::find(arguments.begin(), arguments.end(), ref) == arguments.end()) {
		arguments.emplace_back(ref);
	}
}

} // namespace nautilus::tracing
