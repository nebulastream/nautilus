#pragma once

namespace nautilus::compiler::mlir {

/// Registers the MLIR overflow intrinsic plugin: replaces the {add,sub,mul}_overflow_*_impl
/// runtime helpers (see overflow.hpp) with `llvm.s{add,sub,mul}.with.overflow`.
void RegisterMLIROverflowIntrinsicPlugin();

} // namespace nautilus::compiler::mlir
