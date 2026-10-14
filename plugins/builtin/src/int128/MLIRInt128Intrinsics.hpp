#pragma once

namespace nautilus::compiler::mlir {

/// Registers the MLIR int128 intrinsic plugin: intercepts the int128_*_impl
/// runtime helpers (see int128.hpp) and replaces each call with the native
/// LLVM `i128` operation during MLIR lowering.
void RegisterMLIRInt128IntrinsicPlugin();

} // namespace nautilus::compiler::mlir
