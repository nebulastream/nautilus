#pragma once

namespace nautilus::compiler::mlir {

/// Registers the MLIR popcount intrinsic plugin: intercepts the
/// nautilus_popcount_* function pointers (see popcount_impl.hpp) and replaces
/// the call with LLVM's `llvm.ctpop` intrinsic during MLIR lowering.
void RegisterMLIRPopcountIntrinsicPlugin();

} // namespace nautilus::compiler::mlir
