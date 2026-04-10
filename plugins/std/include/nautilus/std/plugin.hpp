#pragma once
// Force-link / auto-registration header for the nautilus-std MLIR intrinsic plugin.
//
// Problem: *PluginInit.cpp TUs live in a static archive (nautilus-std). The
// linker only pulls object files from an archive to satisfy unresolved external
// symbol references. Because the anonymous-namespace registrar in
// StdPluginInit.cpp has no external linkage, the linker strips that TU and the
// static initializer that registers MLIR intrinsics never runs.
//
// Solution: this header forward-declares the (externally-linked) registration
// functions and calls them from an anonymous-namespace static initializer.
// A static with a non-trivial constructor that calls external functions cannot
// be eliminated by the optimizer (the compiler cannot prove the calls are
// side-effect-free), so the linker is forced to include the definition TUs from
// the archive, registering the intrinsics before main().
//
// Usage: #include <nautilus/std/plugin.hpp> in any TU that links against
// nautilus-std as a static archive and needs math/bit/atomic MLIR intrinsics.

#include "nautilus/config.hpp"

#ifdef ENABLE_MLIR_BACKEND
namespace nautilus::compiler::mlir {
// Defined in plugins/std/src/MLIRMathIntrinsics.cpp
void RegisterMLIRMathIntrinsicPlugin();
// Defined in plugins/std/src/MLIRBitIntrinsics.cpp
void RegisterMLIRBitIntrinsicPlugin();
// Defined in plugins/std/src/MLIRAtomicIntrinsics.cpp
void RegisterMLIRAtomicIntrinsicPlugin();
} // namespace nautilus::compiler::mlir

namespace {
// Each TU that includes this header gets its own instance (anonymous namespace
// = internal linkage). The constructor calls external functions, so the
// compiler cannot eliminate it — initialization is guaranteed before main().
struct NautilusStdMlirPluginRegistrar {
	NautilusStdMlirPluginRegistrar() {
		nautilus::compiler::mlir::RegisterMLIRMathIntrinsicPlugin();
		nautilus::compiler::mlir::RegisterMLIRBitIntrinsicPlugin();
		nautilus::compiler::mlir::RegisterMLIRAtomicIntrinsicPlugin();
	}
};
[[maybe_unused]] static NautilusStdMlirPluginRegistrar nautilus_std_mlir_plugin_registrar_;
} // namespace
#endif
