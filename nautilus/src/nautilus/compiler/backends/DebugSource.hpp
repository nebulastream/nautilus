#pragma once

#include <string>

namespace nautilus::compiler {

/// The file that debug information and profilers point at (D14, 05 §7): the text form of the module, written next to
/// the code so that a debugger's `list` or `perf annotate` can show the line an address came from. Shared by the
/// backends that publish lines (`mlir` as DWARF, `asmjit` as a line table).

/// A unique absolute path `nautilus_debug_<pid>_<n>.<extension>`. Under the temp directory, or under the working
/// directory for a profiling-only compile (`perf`, `perf.sample` without `debug`): there the file has to outlive the
/// compile and be found next to `perf.data` when `perf inject` or `perf annotate` run, minutes or hours later, and
/// $TMPDIR is routinely cleaned between record and report.
std::string newDebugSourcePath(bool inWorkingDirectory, const std::string& extension);

/// A path under the temp directory, for when the preferred directory turns out not to be writable.
std::string debugSourceFallbackPath(const std::string& extension);

/// Writes `text` to `path`. If that fails (a read-only working directory) it is written to a fallback path and a
/// message goes to stderr; returns the path that was written, which is what the locations must name.
std::string writeDebugSource(const std::string& path, const std::string& text);

} // namespace nautilus::compiler
