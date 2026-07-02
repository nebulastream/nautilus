#pragma once

#include "nautilus/compiler/backends/tbc/TBCCode.hpp"
#include <cstddef>
#include <cstdint>

namespace nautilus::compiler::tbc {

/// Best dispatch mode the current compiler can execute (tail-call on Clang,
/// computed goto on GCC, switch elsewhere).
DispatchMode bestAvailableDispatchMode();

/// Clamp a requested mode to what the build supports.
DispatchMode clampDispatchMode(DispatchMode requested);

/// Execute `program.functions[functionIndex]` with the given 64-bit argument
/// slots (already normalized per the function's argument types) and return the
/// raw 64-bit result slot (0 for void). Runs on a thread-local contiguous VM
/// stack; safe for reentrant use (external calls that call back into TBC code).
/// Throws RuntimeException on VM stack overflow.
uint64_t invoke(const TBCProgram& program, uint32_t functionIndex, const uint64_t* args, size_t argCount);

} // namespace nautilus::compiler::tbc
