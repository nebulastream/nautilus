
#pragma once

#include "nautilus/tracing/Block.hpp"
#include <vector>

namespace nautilus::tracing {

class ExecutionTrace;

struct MergePoint {
	operation_identifier first;
	uint32_t secondBlockIndex;
	uint32_t secondOpCount;
};

class MergeDetectionPhase {
public:
	void apply(ExecutionTrace& trace, std::vector<MergePoint>& mergePoints);
};

} // namespace nautilus::tracing
