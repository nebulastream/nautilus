#pragma once

#include <cstdint>

namespace nautilus::tracing {

/// Optional callback for external callers (e.g. Python bindings) to provide
/// additional position information that disambiguates call sites which would
/// otherwise produce identical stack-based tags.
///
/// When non-null, the returned value is XOR'd into the snapshot hash so that
/// two operations at different source positions (but routed through the same
/// C++ dispatcher) receive distinct snapshots.
inline thread_local uint64_t (*externalPositionHintFn)() = nullptr;

} // namespace nautilus::tracing
