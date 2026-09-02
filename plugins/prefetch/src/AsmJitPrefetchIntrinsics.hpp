#pragma once

namespace nautilus::compiler::asmjit {

/// Registers the AsmJit prefetch intrinsic plugin: intercepts the
/// nautilus_prefetch_*_impl function pointers (see prefetch_impl.hpp) and
/// replaces the call with a native prefetch instruction -- x86-64
/// PREFETCHT0/T1/T2/NTA/W/WT1, or AArch64 PRFM -- during AsmJit lowering.
void RegisterAsmJitPrefetchIntrinsicPlugin();

} // namespace nautilus::compiler::asmjit
