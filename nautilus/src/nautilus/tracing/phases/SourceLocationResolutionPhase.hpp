
#pragma once

#include "nautilus/tracing/ExecutionTrace.hpp"

namespace nautilus::tracing {

/**
 * @brief Post-SSA phase that resolves source locations for each trace operation.
 *
 * Iterates all TraceOperations and resolves their Snapshot::tag to a
 * SourceLocationChain using backward-cpp debug info. The chain represents
 * the full call hierarchy from innermost to outermost user frame.
 */
class SourceLocationResolutionPhase {
public:
	static void apply(ExecutionTrace& trace);
};

} // namespace nautilus::tracing
