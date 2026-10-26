#pragma once

#include <cstdint>

namespace nautilus::tracing {

/// What a tracing session counted (docs/plans/nautilus-0.4/10-observability.md §3, 01-tracing-frontend.md §4): the
/// keys of #561 plus the split of the re-run time and the copies by kind. Collected by the tracer; the compilation
/// pipeline reports them as `tracing.*`.
struct TracingStatistics {
	/// Passes of the traced bodies over all scopes (a function or a region is one scope).
	uint64_t iterations = 0;
	/// Operations a re-run stepped over in follow mode.
	uint64_t replayedOperations = 0;
	/// Frame-walk snapshots taken in record mode.
	uint64_t snapshots = 0;
	/// Operations of the finished traces.
	uint64_t operations = 0;
	/// Traced calls that only returned a dummy because the pass was already finished (a control-flow merge).
	uint64_t pausedOperations = 0;
	/// Copies recorded, by kind: `val` copy construction, assignment, and the copies the library makes on purpose
	/// (`ptr + 0`).
	uint64_t copiesConstruct = 0;
	uint64_t copiesAssign = 0;
	uint64_t copiesExplicit = 0;
	/// Scopes traced and the most passes any one needed.
	uint64_t scopes = 0;
	uint64_t maxScopeIterations = 0;
	/// Wall time of the first pass of a function and of every later pass, and the re-run time split by what the pass
	/// was doing: stepping through the recorded path, recording the new direction, or running out the paused tail.
	/// Region scopes are timed within the pass of their enclosing function.
	double firstRunMs = 0;
	double rerunMs = 0;
	double rerunFollowMs = 0;
	double rerunRecordMs = 0;
	double rerunPausedMs = 0;

	[[nodiscard]] uint64_t copies() const {
		return copiesConstruct + copiesExplicit;
	}
};

} // namespace nautilus::tracing
