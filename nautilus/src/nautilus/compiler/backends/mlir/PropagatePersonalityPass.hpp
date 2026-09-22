#pragma once

#include <memory>
#include <mlir/Pass/Pass.h>

namespace nautilus::compiler::mlir {

// MLIR pass that restores the `personality` attribute on every `func.func`
// that contains an `llvm.landingpad`.
//
// MLIRLoweringProvider marks a function that needs exception handling by
// setting a `personality` *discardable* attribute on its `func.func`, which
// `convert-func-to-llvm` later forwards onto the resulting `llvm.func`. The
// generic MLIR inliner knows nothing about that convention: when it inlines a
// callee that owns a landing pad into a caller that has none of its own -- a
// nested Nautilus function reached through a plain call because the caller has
// no live destructors to clean up -- the `llvm.invoke` / `llvm.landingpad`
// ops move into the caller but the attribute stays behind on the callee. The
// LLVM dialect verifier then rejects the module with "llvm.landingpad needs to
// be in a function with a personality" (#481).
//
// Must run after the inliner and before `convert-func-to-llvm`.
std::unique_ptr<::mlir::Pass> createPropagatePersonalityPass();

} // namespace nautilus::compiler::mlir
