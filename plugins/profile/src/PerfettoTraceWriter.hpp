#pragma once

#include "nautilus/profile/Recorder.hpp"
#include <string>
#include <vector>

namespace nautilus::profile::detail {

// Serializes the given drained Event vector into the Perfetto native trace
// protobuf format and writes it to `path`. Returns true on success.
//
// The encoding is hand-rolled protobuf wire-format (no Perfetto SDK or
// protoc dependency). It targets the subset of perfetto.protos that the
// Perfetto UI needs to render:
//
//   - One ProcessDescriptor track for the running process.
//   - One ThreadDescriptor child track per (tid, module) pair.
//   - TrackEvent slices (TYPE_SLICE_BEGIN / TYPE_SLICE_END) for regions.
//   - TrackEvent counters (TYPE_COUNTER) for traceCounter() values.
//   - PerfSample packets with an interned Frame / Callstack / Mapping table
//     for sampler stacks. The Perfetto UI renders these as a proper
//     time-aligned flamegraph that filters under any selected region.
//
// All timestamps are converted from raw cycle-counter ticks to nanoseconds
// using the session's calibrated ticks_per_us, so the trace can be loaded
// against BUILTIN_CLOCK_MONOTONIC.
bool writePerfettoTrace(const std::string& path, const std::vector<Event>& events);

} // namespace nautilus::profile::detail
