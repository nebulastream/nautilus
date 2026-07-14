#pragma once

#include <chrono>
#include <cstddef>

namespace nautilus::profile {

struct SamplerConfig {
	// Sampling period. Defaults to 1ms (1 kHz) per the design doc.
	std::chrono::microseconds interval {std::chrono::microseconds(1000)};
	// Maximum captured frames per sample.
	size_t max_depth = 64;
	// Per-thread SPSC ring buffer capacity. Samples are dropped if the ring
	// is full and a sampler-overruns counter is surfaced in the final trace.
	size_t ring_slots = 4096;
};

// Starts an in-process SIGPROF-based sampler. Multiple calls reconfigure the
// running sampler. Threads that want to be sampled must call
// registerThisThread() after startup; otherwise they are ignored.
void startSampler(SamplerConfig cfg = {});

// Stops the sampler and drains any buffered samples into the Recorder as
// Event::Kind::Sample entries so flushTrace() emits them alongside regions.
// Returns the number of samples drained.
size_t stopSampler();

// Marks the current thread as a sampling target.
void registerThisThread();

// Removes the current thread from the sampling set.
void unregisterThisThread();

} // namespace nautilus::profile
