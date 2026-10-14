#pragma once

namespace nautilus::compiler::asmjit {

/// Registers the AsmJit popcount intrinsic plugin: intercepts the
/// nautilus_popcount_* function pointers (see popcount_impl.hpp) and replaces
/// the call with a native population count -- x86-64 POPCNT, or AArch64
/// CNT + ADDV -- during AsmJit lowering.
void RegisterAsmJitPopcountIntrinsicPlugin();

} // namespace nautilus::compiler::asmjit
