#pragma once

#include "nautilus/compiler/backends/nir/NirBackend.hpp"
#include <string>

namespace nautilus::compiler::cpp {

/// The C++ source of an SSA NIR module (docs/plans/nautilus-0.4/05-backends.md, M4.3).
///
/// Every function becomes one C function. Values of integer and pointer types are held as unsigned integers of their
/// width (NIR is signless), so arithmetic wraps without undefined behavior, and the signed view is a cast where an
/// operation needs it. Checked `div`/`rem`, masked shifts and saturating float to integer conversions follow D10 and
/// D11 (06-semantics.md §4); memory is accessed with `memcpy`. Calls that may throw are made inside a `try` that stores
/// the exception in the host's exception frame, as the lowering of the 0.3 IR does.
std::string lowerNirToCpp(const nir::Module& module, const nirbackend::HostEnvironment& host);

} // namespace nautilus::compiler::cpp
