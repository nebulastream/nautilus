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
// The singleton outlives the process; there is nothing to free. The caller
// is free to reuse the same pointer across every Engine and NautilusModule.
llvm::JITEventListener* mlirJitEventListener();

} // namespace nautilus::profile
