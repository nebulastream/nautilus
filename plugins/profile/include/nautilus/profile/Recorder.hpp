#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace nautilus::profile {

// Test-oriented observation API for the stub runtime. Step 3 keeps the
// recorder around as a lightweight debug/assert hook that shadows the
// Chrome-trace writer — every event emitted to the trace file is also
// appended here and remains visible to C++ tests.
struct Event {
	enum class Kind {
		Begin,
		End,
		CounterI64,
		Sample,
	};

	Kind kind;
	std::string name;
	int64_t value;                  // only meaningful for CounterI64
	uint64_t timestamp_us;          // monotonic microseconds since recorder init
	uint64_t tid;                   // OS thread id
	std::string module;             // innermost open module on the emitting thread
	std::vector<std::string> stack; // resolved frames, Sample events only
};

// Returns and clears the buffer of events recorded since the last call.
// Thread-safe.
std::vector<Event> takeRecordedEvents();

// Drops any events recorded so far and resets the monotonic clock origin.
void clearRecordedEvents();

} // namespace nautilus::profile
