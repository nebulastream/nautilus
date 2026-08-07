#pragma once

#include <mlir/IR/BuiltinOps.h>
#include <string>

namespace nautilus::compiler::ir {
class IRGraph;
}

namespace nautilus::compiler::mlir {

inline constexpr auto CLEANUP_PAD_ATTR = "nautilus.cleanup_pad";
inline constexpr auto ALLOCA_INDEX_ATTR = "nautilus.alloca_index";

std::string cleanupDestructorSymbol(void* functionPtr);

/// Replaces marked LLVM calls with invokes and materializes shared cleanup
/// landing pads. The module must already be fully converted to LLVM dialect.
::mlir::LogicalResult materializeCppExceptionHandling(::mlir::ModuleOp module, const ir::IRGraph& ir);

} // namespace nautilus::compiler::mlir
