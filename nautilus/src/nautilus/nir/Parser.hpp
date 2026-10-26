#pragma once

#include "nautilus/nir/Diagnostics.hpp"
#include "nautilus/nir/Module.hpp"
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace nautilus::nir {

/// A parse error carries a NIR-P code and the position in the text (13-nir-spec.md §6.6, §9.4).
struct ParseError {
	DiagCode code;
	uint32_t line;
	uint32_t column;
	std::string message;

	/// "NIR-P002 at 3:14: message"
	std::string str() const;
};

struct ParseOptions {
	/// Verify the module after parsing (§9.4); the diagnostics are returned in ParseResult::verifierDiagnostics.
	bool verify = true;
};

struct ParseResult {
	std::optional<Module> module;
	std::optional<ParseError> error;
	std::vector<Diagnostic> verifierDiagnostics;

	/// Parsed, and (if requested) verified without diagnostics.
	bool ok() const {
		return module && !error && verifierDiagnostics.empty();
	}
};

/// Parses the text form of §9. The parser stops at the first error.
ParseResult parse(std::string_view text, const ParseOptions& options = {});

} // namespace nautilus::nir
