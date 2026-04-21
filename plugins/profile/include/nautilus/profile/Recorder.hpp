#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace nautilus::profile {

// Test-oriented observation API for the stub runtime that ships in step 1.
// Step 3 replaces the in-memory recorder with a Perfetto session and the
// recorder is kept only as a lightweight debug/assert helper.
struct Event {
	enum class Kind {
		Begin,
		End,
		CounterI64,
	};

	Kind kind;
	std::string name;
	int64_t value; // only meaningful for CounterI64
};

// Returns and clears the buffer of events recorded since the last call.
// Thread-safe.
std::vector<Event> takeRecordedEvents();

// Drops any events recorded so far.
void clearRecordedEvents();

} // namespace nautilus::profile
