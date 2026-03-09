
#include "SourceLocationResolutionPhase.hpp"
#include "nautilus/tracing/tag/SourceLocationResolver.hpp"

namespace nautilus::tracing {

void SourceLocationResolutionPhase::apply(ExecutionTrace& trace) {
	SourceLocationResolver resolver;
	for (auto& block : trace.blocks) {
		for (auto& op : block.operations) {
			Tag* tag = op.tag.getTag();
			if (tag != nullptr) {
				op.sourceLocations = resolver.resolve(tag);
			}
		}
	}
}

} // namespace nautilus::tracing
