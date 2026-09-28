#pragma once

#include <cstddef>
#include <string>

namespace nautilus::tracing {

class TraceModule;
class SourceLocationResolver;

/// Groups the copies recorded while tracing @p module (TraceContext::traceCopy, with `dump.copySites` set) by the
/// C++ code that made them, most frequent first, for the `copy_sites` dump.
///
/// Each copy is attributed to its call chain from the innermost frame outside the tracer (usually a val<T> copy
/// constructor in a nautilus header) up to the first frame outside nautilus, i.e. the embedder's code that caused
/// it; a second table groups by that frame alone. Frames resolve to source locations only when the binaries carry
/// debug information (and ENABLE_STACKTRACE is on); otherwise they show the enclosing function or the address.
/// At most @p limit rows are printed per table.
std::string formatCopySites(const TraceModule& module, SourceLocationResolver& resolver, size_t limit = 50);

} // namespace nautilus::tracing
