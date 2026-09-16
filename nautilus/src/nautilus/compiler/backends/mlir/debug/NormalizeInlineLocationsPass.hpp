#pragma once

#include <memory>
#include <mlir/Pass/Pass.h>

namespace nautilus::compiler::mlir {

// Post-conversion MLIR pass that makes every inlined-call location tree in the
// module safe for `DIScopeForLLVMFuncOpPass`, at any inlining depth.
//
// The MLIR inliner records an inlined operation's origin by wrapping its
// location in `CallSiteLoc(<the op's own location>, <the call site>)`. Inlining
// a chain of calls nests those wrappers, one level per hop, so a program whose
// entry function calls a helper that calls another helper -- the ordinary shape
// of any non-trivial Nautilus program -- reaches
// `DIScopeForLLVMFuncOpPass` with `CallSiteLoc`s nested two or more deep.
//
// That pass walks the nesting (`getNestedLoc()`) and, at *every* level, calls
// `extractFileLoc(callee).getFilename()` without checking whether a file
// location was found. Any level whose callee carries no source position at all
// -- an `UnknownLoc`, which the inliner's own constant materialization and CSE
// routinely produce -- therefore dereferences a null attribute and segfaults
// the compile. A single hop happens to survive because the pass never recurses
// for it; two or more do not.
//
// Nautilus cannot patch the upstream pass, so this one removes the input that
// trips it: a call frame that carries no source position describes nothing a
// line table could ever show, so it is dropped and the operation is attributed
// to the innermost enclosing frame that does have one. Frames that do carry a
// position are preserved exactly, however deeply they nest, which is what keeps
// a profile's inline attribution intact.
//
// Must run after the LLVM-dialect conversion and immediately before
// `DIScopeForLLVMFuncOpPass`.
std::unique_ptr<::mlir::Pass> createNormalizeInlineLocationsPass();

} // namespace nautilus::compiler::mlir
