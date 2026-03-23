#pragma once

namespace nautilus::compiler::mlir {

/// Registers the MLIR vector intrinsic plugin for SIMD operations.
/// This plugin intercepts vector_*_impl function pointers and replaces them
/// with native MLIR vector dialect operations during MLIR lowering.
void RegisterMLIRVectorIntrinsicPlugin();

} // namespace nautilus::compiler::mlir
