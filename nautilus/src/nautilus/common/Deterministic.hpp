#pragma once

#include <cstdlib>
#include <string_view>

namespace nautilus::common {

/// `NAUTILUS_DETERMINISTIC=1`: the compiler takes nothing from the clock or the entropy source into what it computes
/// or how long it takes, so that two runs of the same binary execute the same instructions (the callgrind gate,
/// tools/latency/callgrind-gate.py; docs/options.md). Compilation unit ids and the process token become a function
/// of a per-process counter. For measurement only: the unit ids of two processes then collide, and the process token
/// no longer tells processes apart. Read once.
inline bool deterministicMode() {
	static const bool enabled = [] {
		const char* value = std::getenv("NAUTILUS_DETERMINISTIC");
		return value != nullptr && *value != '\0' && std::string_view(value) != "0";
	}();
	return enabled;
}

} // namespace nautilus::common
