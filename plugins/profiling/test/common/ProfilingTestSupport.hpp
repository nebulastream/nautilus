#pragma once

#include "nautilus/profiling/availability.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstdint>

namespace nautilus::profiling::test {

/// Skips the calling test when hardware counters cannot be read here.
///
/// This is a skip and not a failure on purpose. Counters are a privileged
/// resource: containers, a hardened `perf_event_paranoid`, VMs without a
/// virtualized PMU and every non-Linux host all deny them, and most CI runners
/// are at least one of those. A test suite that failed there would be a test of
/// the runner, not of this plugin. The skip message carries the probe's reason
/// so a genuine regression is still distinguishable from an environment that
/// never allowed counting in the first place.
#define NAUTILUS_PROFILING_SKIP_IF_UNAVAILABLE()                                                                       \
	do {                                                                                                               \
		const auto& nautilusProfilingSupport = ::nautilus::profiling::support();                                       \
		if (!nautilusProfilingSupport) {                                                                               \
			SKIP("hardware counters unavailable: " << nautilusProfilingSupport.reason);                                \
		}                                                                                                              \
	} while (false)

/// A workload with a known, non-trivial instruction count that the optimizer
/// cannot fold away.
///
/// `volatile` on the accumulator is what keeps it honest: without it the whole
/// loop constant-folds at -O3 and the counters legitimately come back near
/// zero, which would look exactly like a broken counter.
inline uint64_t busyLoop(uint64_t iterations) {
	volatile uint64_t accumulator = 0;
	for (uint64_t i = 0; i < iterations; ++i) {
		accumulator = accumulator + i;
	}
	return accumulator;
}

} // namespace nautilus::profiling::test
