#pragma once

#include <memory>
#include <mlir/Pass/Pass.h>

namespace nautilus::compiler::mlir {

// Post-conversion MLIR pass that emits `llvm.intr.dbg.value` intrinsics
// for each SSA value that carries a Nautilus `$N` NameLoc.
//
// Must run after `convert-func-to-llvm` (so everything is in the LLVM
// dialect and `LLVMFuncOp`s exist) and after `DIScopeForLLVMFuncOpPass`
// (so each LLVMFuncOp already has a distinct DISubprogram attached via
// its fused location).  For every operation whose location contains a
// NameLoc of the form `$N` and whose result is an LLVM-typed SSA value,
// the pass inserts a `llvm.intr.dbg.value` after the op that tags the
// value as the variable named `$N` in GDB/LLDB.  When the same `$N`
// appears on multiple emitted ops (some Nautilus ops lower to several
// MLIR ops), only the last op in program order gets the intrinsic — it
// is the one whose result is bound to the Nautilus identifier.
//
// Uses the `DW_OP_LLVM_arg 0, DW_OP_stack_value` expression so the
// debugger interprets the intrinsic operand as the variable's live
// value (not its address), which works without spilling to memory.
std::unique_ptr<::mlir::Pass> createEmitDbgValuePass();

} // namespace nautilus::compiler::mlir
