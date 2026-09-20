#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace nautilus::profiling::detail {

/// Whether @p symbol is a nautilus region-qualified name, as opposed to a
/// demangled C++ symbol that merely contains `::`.
///
/// A nautilus qualified name is nothing but `::`-joined identifiers. A real C++
/// signature almost always carries a `(` for its argument list or a `<` for a
/// template by the time a demangler is done with it, and either one means the
/// `::` in it are namespaces, which were never frames.
[[nodiscard]] inline bool isQualifiedRegionName(std::string_view symbol) {
	if (symbol.find("::") == std::string_view::npos) {
		return false;
	}
	return symbol.find('(') == std::string_view::npos && symbol.find('<') == std::string_view::npos;
}

/// `a::b::c` -> {`a`, `a::b`, `a::b::c`}, so a region shows up inside its
/// parent rather than beside it. Each level keeps its fully qualified name,
/// which is how a graph labels a region the way the report names it.
///
/// A name that is not region-qualified comes back as one frame, unchanged.
[[nodiscard]] inline std::vector<std::string> expandRegionFrames(const std::string& symbol) {
	if (!isQualifiedRegionName(symbol)) {
		return {symbol};
	}
	std::vector<std::string> frames;
	size_t start = 0;
	while (start <= symbol.size()) {
		const size_t separator = symbol.find("::", start);
		const size_t levelEnd = separator == std::string::npos ? symbol.size() : separator;
		frames.push_back(symbol.substr(0, levelEnd));
		if (separator == std::string::npos) {
			break;
		}
		start = separator + 2;
	}
	return frames;
}

} // namespace nautilus::profiling::detail
