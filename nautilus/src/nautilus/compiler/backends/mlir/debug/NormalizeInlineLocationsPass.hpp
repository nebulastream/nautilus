#pragma once

#include <memory>
#include <mlir/Pass/Pass.h>

namespace nautilus::compiler::mlir {

// Post-conversion MLIR pass that makes every inlined-call location tree in the
// module safe for `DIScopeForLLVMFuncOpPass`, at any inlining depth.
//
// The MLIR inliner records an inlined operation's origin by wrapping its
// location in `CallSiteLoc(<the op's own location>, <the call site>)`.
// `DIScopeForLLVMFuncOpPass` turns that into DWARF scopes via `getNestedLoc()`,
// whose very first statement is
//
//     auto calleeFileName = extractFileLoc(calleeLoc).getFilename();
//
// with no check that a file location was found. `extractFileLoc()` returns a
// null attribute for a location carrying no source position -- an `UnknownLoc`,
// which the inliner's own constant materialization and CSE routinely produce --
// and calling `getFilename()` on that dereferences null and segfaults the
// compile. `getNestedLoc()` then recurses for a callee that is itself a
// `CallSiteLoc` and repeats the same unchecked dereference at every level, so
// the guard has to hold however deeply calls nest.
//
// Note that the crash needs only *one* such frame, not deep nesting: the
// dereference happens before any recursion. The issue that prompted this
// (#467) reported it as needing two or more hops of Nautilus-to-Nautilus
// calls, and that is a real empirical trigger, but not the mechanism. What a
// second hop changes is that a function which has itself had a callee inlined
// into it gets constants re-materialized without a location, and is then
// inlined in turn -- producing `CallSiteLoc(UnknownLoc, <call site>)`. It is
// shape-dependent rather than depth-dependent: measured over the Nautilus
// fixtures, 2- and 3-hop chains produce such frames while 1-, 4- and 50-hop
// chains do not, and the callee-spine depth reaching this pass is 1 in every
// one of them.
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
