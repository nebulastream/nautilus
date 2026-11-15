#pragma once

#include "nautilus/options.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace nautilus::tracing {

/// A problem the tracer found in the user's code (docs/diagnostics.md): a stable code, a title, the source location,
/// what is wrong and what to do about it (docs/plans/nautilus-0.4/15-agent-experience.md §4.1).
struct TracingDiagnostic {
	std::string code;    // NAUT-T001
	std::string title;   // "constant loop"
	std::string message; // what is wrong
	std::string hint;    // what to do
	std::string file;
	uint32_t line = 0;
	std::string function;

	/// "file:line", or "<unknown location>".
	[[nodiscard]] std::string location() const;
	/// `NAUT-T001 constant loop at Query.cpp:42: the loop condition does not depend on a val; use a val loop variable`
	[[nodiscard]] std::string toText() const;
	/// One JSON object on one line.
	[[nodiscard]] std::string toJson() const;
};

enum class DiagnosticsMode : uint8_t { Off, Text, Json };

/// `engine.diagnostics` of @p options.
DiagnosticsMode diagnosticsMode(const engine::Options& options);

/// Writes @p diagnostic to standard error in the requested form; nothing for `Off`.
void reportDiagnostic(const TracingDiagnostic& diagnostic, DiagnosticsMode mode);

} // namespace nautilus::tracing
