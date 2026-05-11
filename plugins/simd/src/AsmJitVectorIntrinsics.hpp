#pragma once

namespace nautilus::compiler::asmjit {

/// Registers the AsmJit vector intrinsic plugin for SIMD operations.
/// This plugin intercepts vector_*_impl function pointers and replaces them
/// with native 128-bit SIMD instructions (XMM on x86-64, NEON on AArch64)
/// during AsmJit lowering. Only 128-bit configurations
/// (f32x4, f64x2, i32x4, i64x2) are registered; wider widths fall back to
/// the scalar `vector_*_impl` C++ implementations.
void RegisterAsmJitVectorIntrinsicPlugin();

} // namespace nautilus::compiler::asmjit
