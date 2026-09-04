#pragma once

#include <cstdint>
#include <source_location>
#include <string>

namespace nautilus {

/// The source position a region() call site was written at.
///
/// Stores the pointers std::source_location hands out rather than the location object
/// itself: those point at string literals with static storage duration, so a position
/// stays readable for as long as the trace that recorded it, and this struct stays
/// trivially copyable and independent of how the standard library models a location.
struct SourceLocation {
	/// Source file of the call site; nullptr if the position is unknown.
	const char* file = nullptr;
	/// Enclosing function of the call site; nullptr if unknown.
	const char* function = nullptr;
	uint32_t line = 0;
	uint32_t column = 0;

	static SourceLocation from(const std::source_location& location) {
		return SourceLocation {location.file_name(), location.function_name(), static_cast<uint32_t>(location.line()),
		                       static_cast<uint32_t>(location.column())};
	}

	bool isKnown() const {
		return file != nullptr;
	}

	/// "file:line:column", or "<unknown location>" for a default-constructed position.
	std::string toString() const {
		if (!isKnown()) {
			return "<unknown location>";
		}
		return std::string(file) + ":" + std::to_string(line) + ":" + std::to_string(column);
	}
};

/// The attributes of one region() call site: where it was written and, optionally, what
/// the user called it. Both are fixed properties of the call site, so they never take
/// part in tag or snapshot identity -- they describe a region, they do not distinguish
/// one engagement of it from another.
struct RegionAttributes {
	/// User-defined region name, or nullptr for an unnamed region. A string literal or
	/// any other string that outlives the trace; it is stored, not copied.
	const char* name = nullptr;

	/// Where the region() call site is written.
	SourceLocation location {};

	bool hasName() const {
		return name != nullptr && name[0] != '\0';
	}

	/// A one-line description for diagnostics and dumps: `"accumulate" at src/Query.cpp:42:2`
	/// for a named region, `at src/Query.cpp:42:2` for an unnamed one. Callers supply the
	/// word "region" themselves, so the description reads as part of their sentence.
	std::string toString() const {
		std::string description;
		if (hasName()) {
			description += "\"" + std::string(name) + "\" ";
		}
		description += "at " + location.toString();
		return description;
	}
};

} // namespace nautilus
