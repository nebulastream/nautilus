#pragma once

#include <cstdint>
#include <string>

namespace nautilus::profile {

// Opens a named "module" — a logical grouping of regions that surfaces as a
// Perfetto track when the trace is flushed. Nesting and overlapping modules
// are allowed; regions emitted between openModule/closeModule are associated
// with the innermost open module on the current thread.
//
// The name must have static storage duration (string literal).
void openModule(const char* name);
void closeModule(const char* name);

// Writes every event recorded since the last flush/clear to `path` in Chrome
// Trace Event JSON format. The file loads natively in https://ui.perfetto.dev
// and in chrome://tracing.
//
// Returns true on success, false on I/O error. Clears the internal buffer
// regardless of success.
bool flushTrace(const std::string& path);

// Writes every recorded event to `path` in Perfetto's native trace protobuf
// format (https://perfetto.dev/docs/reference/trace-packet-proto). Compared
// to flushTrace, this gives the Perfetto UI a properly-typed CPU sampling
// track with an aggregated flamegraph that filters under any selected
// region — instead of one orphan instant-event marker per sample.
//
// File extension by convention is `.perfetto-trace` (or `.pftrace`); the
// writer doesn't enforce one. Returns true on success, false on I/O error.
// Clears the internal buffer regardless of success.
bool flushPerfettoTrace(const std::string& path);

} // namespace nautilus::profile
