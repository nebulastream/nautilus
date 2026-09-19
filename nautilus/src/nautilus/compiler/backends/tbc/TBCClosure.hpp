#pragma once

#include "nautilus/compiler/backends/tbc/TBCCode.hpp"

namespace nautilus::compiler::tbc {

/// Build one native entry point per function in @p program, filling its
/// `closureBindings` and `closures`.
///
/// Interpreted functions have no machine code of their own, but their address
/// escapes in two ways: as the module's entry point
/// (TBCExecutable::getInvocableFunctionPtr) and as a `FunctionAddressOf` value
/// handed to native code. Both need a genuine C function pointer, which the
/// shared NativeClosure component provides — via libffi static trampolines when
/// NAUTILUS_FFI_CLOSURES is set (no runtime-executable memory, so iOS-safe), and
/// via a dyncall callback otherwise.
///
/// Must run after every function's signature is known and before lowering, so
/// FunctionAddressOf can resolve any module function regardless of lowering
/// order. Every argument and return type is supported, at any arity.
void createClosures(TBCProgram& program);

} // namespace nautilus::compiler::tbc
