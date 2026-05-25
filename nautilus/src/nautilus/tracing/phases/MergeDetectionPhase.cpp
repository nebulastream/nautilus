
#include "MergeDetectionPhase.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include <algorithm>

namespace nautilus::tracing {

void MergeDetectionPhase::apply(ExecutionTrace& trace, std::vector<MergePoint>& mergePoints) {
	std::sort(mergePoints.begin(), mergePoints.end(),
	          [](const MergePoint& a, const MergePoint& b) { return a.secondBlockIndex > b.secondBlockIndex; });

	for (auto& mp : mergePoints) {
		auto& secondBlock = trace.getBlock(mp.secondBlockIndex);

		auto& returnRefs = trace.returnRefs;
		std::erase_if(returnRefs, [&](const operation_identifier& ref) {
			return ref.blockIndex == mp.secondBlockIndex && ref.operationIndex >= mp.secondOpCount;
		});

		if (mp.secondOpCount < secondBlock.operations.size()) {
			secondBlock.operations.erase(secondBlock.operations.begin() + mp.secondOpCount,
			                             secondBlock.operations.end());
		}

		trace.setCurrentBlock(mp.secondBlockIndex);
		trace.processControlFlowMerge(mp.first);
	}
}

} // namespace nautilus::tracing
