#pragma once
// Force-link / auto-registration header for the nautilus-simd MLIR intrinsic plugin.
//
// Problem: *PluginInit.cpp TUs live in a static archive (nautilus-simd). The
// linker only pulls object files from an archive to satisfy unresolved external
// symbol references. Because the anonymous-namespace registrar in
// SimdPluginInit.cpp has no external linkage, the linker strips that TU and
// the static initializer that registers MLIR vector intrinsics never runs.
//
// Solution: this header forward-declares the (externally-linked) registration
// function and calls it from an anonymous-namespace static initializer.
// A static with a non-trivial constructor that calls external functions cannot
// be eliminated by the optimizer (the compiler cannot prove the call is
// side-effect-free), so the linker is forced to include the definition TU from
// the archive, registering the intrinsics before main().
//
// Usage: #include <nautilus/simd/plugin.hpp> in any TU that links against
// nautilus-simd as a static archive and needs MLIR vector intrinsics.

#include "nautilus/config.hpp"

#ifdef ENABLE_MLIR_BACKEND
namespace nautilus::compiler::mlir {
// Defined in plugins/simd/src/MLIRVectorIntrinsics.cpp
void RegisterMLIRVectorIntrinsicPlugin();
} // namespace nautilus::compiler::mlir

namespace {
// Each TU that includes this header gets its own instance (anonymous namespace
// = internal linkage). The constructor calls an external function, so the
// compiler cannot eliminate it — initialization is guaranteed before main().
struct NautilusSimdMlirPluginRegistrar {
	NautilusSimdMlirPluginRegistrar() {
		nautilus::compiler::mlir::RegisterMLIRVectorIntrinsicPlugin();
	}
};
[[maybe_unused]] static NautilusSimdMlirPluginRegistrar nautilus_simd_mlir_plugin_registrar_;
} // namespace
#endif
