
#pragma once

#include "nautilus/tracing/tag/SourceLocationResolver.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace nautilus::debug {

/**
 * @brief Resolves user-declared variable names from the host binary's DWARF.
 *
 * Sits on top of @c tracing::SourceLocationResolver: that resolver
 * symbolises return addresses captured by the trace's @c TagRecorder into
 * @c (file, function, line, column) frames; this resolver takes one of
 * those frames (typically the innermost user frame after Nautilus paths
 * have been filtered out) and asks the binary's DWARF for the
 * @c DW_TAG_variable / @c DW_TAG_formal_parameter declared at that
 * location, returning the @c DW_AT_name attribute.
 *
 * The result is the user's source-level variable name (`sum`, `factor`,
 * `i`, ...) — strictly more information than the trace alone can
 * recover, since trace tags only know which function the trace
 * operation was inside, not which named local it was being assigned to.
 *
 * Preconditions: the host binary must be compiled with @c -g (DWARF
 * debug info present). Without debug info, every lookup returns the
 * empty string and the rest of Nautilus degrades cleanly.
 *
 * Thread-safety: @c resolveVariableName is internally serialised; the
 * cache built on first call is read-mostly afterwards.
 *
 * @note On platforms other than Linux, or when the LLVM DWARF
 * libraries are not linked (non-MLIR builds), @c getInstance returns
 * @c nullptr. Callers must handle that case — it's the same shape as
 * @c SourceLocationResolver's degraded behaviour when @c ENABLE_STACKTRACE
 * is off.
 */
class DwarfVariableResolver {
public:
	/// Process-wide singleton, lazily initialised on first call. Returns
	/// @c nullptr when the resolver isn't compiled in or the host
	/// platform isn't supported (currently: anything that isn't Linux).
	static DwarfVariableResolver* getInstance();

	/// Looks up the user-declared name at the given source coordinates.
	/// Returns the empty string if no matching DIE is found, the binary
	/// has no DWARF, or column-disambiguation can't pick a unique name.
	std::optional<std::string> resolveVariableName(std::string_view file, uint32_t line, uint32_t column);

	/// Convenience overload that takes a frame produced by
	/// @c SourceLocationResolver. The frame's file/line/column are
	/// forwarded to the three-argument form above.
	std::optional<std::string> resolveVariableName(const tracing::SourceFrame& frame) {
		return resolveVariableName(frame.file, frame.line, frame.column);
	}

	DwarfVariableResolver(const DwarfVariableResolver&) = delete;
	DwarfVariableResolver& operator=(const DwarfVariableResolver&) = delete;

protected:
	DwarfVariableResolver() = default;
	virtual ~DwarfVariableResolver() = default;
};

} // namespace nautilus::debug
