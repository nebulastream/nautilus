#pragma once

namespace nautilus::compiler::mlir {

/// Registers the MLIR prefetch intrinsic plugin: intercepts the
/// nautilus_prefetch_*_impl function pointers (see prefetch_impl.hpp) and
/// replaces the call with LLVM's `llvm.prefetch` intrinsic during MLIR
/// lowering, so the pipeline emits a native prefetch instruction instead of a
/// real function call.
void RegisterMLIRPrefetchIntrinsicPlugin();

} // namespace nautilus::compiler::mlir
