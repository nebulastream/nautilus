
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace nautilus::tracing {

/**
 * @brief Represents a resolved source location (file, line, column, function name).
 */
struct SourceLocation {
	std::string file;
	uint32_t line = 0;
	uint32_t column = 0;
	std::string function;

	bool isValid() const {
		return line != 0 && !file.empty();
	}
};

/// A chain of source locations representing the call hierarchy.
/// Index 0 = innermost (where the operation is), last = outermost caller.
using SourceLocationChain = std::vector<SourceLocation>;

} // namespace nautilus::tracing
