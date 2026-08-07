#pragma once

// Forward-declared so public profile headers do not pull in LLVM.
namespace llvm {
class JITEventListener;
} // namespace llvm

namespace nautilus::profile {

// Returns a process-singleton JITEventListener that, when attached to an
// engine via engine::Options::addMLIRJitEventListener, receives an
// ObjectLoaded notification for every module the MLIR backend JIT-compiles.
// The listener walks the emitted object file's symbol table and calls
// registerJitCode for every function, with authoritative address and size
// taken from the object file itself (via llvm::object::computeSymbolSizes).
//
// Also idempotently registers the MLIR intrinsic plugin that inlines
// llvm.readcyclecounter() at region-event call sites so the JIT-emitted
// path bypasses the default clock_gettime route — about a 3-5x speedup on
// the profiler's hot path.
//
// The singleton outlives the process; there is nothing to free. The caller
// is free to reuse the same pointer across every Engine and NautilusModule.
llvm::JITEventListener* mlirJitEventListener();

// Registers the MLIR intrinsic plugin with the global registry. Called
// automatically from mlirJitEventListener(); exposed separately so users
// who only want the inlined-counter optimization (and not the symbol
// listener) can opt in explicitly.
void registerMLIRIntrinsics();

} // namespace nautilus::profile
