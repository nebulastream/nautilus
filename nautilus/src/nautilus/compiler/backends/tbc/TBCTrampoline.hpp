#pragma once

#include "nautilus/compiler/backends/tbc/TBCCode.hpp"
#include <cstddef>
#include <cstdint>

namespace nautilus::compiler::tbc {

/// Maximum arity supported for escaping internal function pointers.
constexpr size_t kTrampolineMaxArity = 8;

/// Acquire a real, natively callable C function pointer for
/// `program.functions[functionIndex]`.
///
/// Internal functions have no machine code, but their address can escape to
/// native code (FunctionAddressOf passed through invoke()). bc solves this
/// with dyncallback, which allocates runtime-executable memory and is
/// therefore forbidden on iOS. Instead, tbc binds one of a fixed pool of
/// pre-compiled template trampolines to the function: the trampoline forwards
/// its (integer-class) arguments into the interpreter.
///
/// Only integer-class signatures (bool/int/pointer arguments and return, or
/// void return) up to kTrampolineMaxArity arguments are supported — the
/// trampolines pass everything as 64-bit integer-class values, which is
/// ABI-compatible because every VM consumer reads its exact declared width.
/// Float signatures throw NotImplementedException.
///
/// Throws RuntimeException when the pool is exhausted.
void* acquireTrampoline(const TBCProgram* program, uint32_t functionIndex);

/// Release every trampoline slot bound to `program` (called from
/// ~TBCProgram).
void releaseTrampolines(const TBCProgram* program);

} // namespace nautilus::compiler::tbc
