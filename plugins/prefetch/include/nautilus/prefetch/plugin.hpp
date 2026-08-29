#pragma once
// Force-link / auto-registration header for the nautilus-prefetch MLIR/AsmJit
// intrinsic plugins.
//
// Problem: *PluginInit.cpp TUs live in a static archive (nautilus-prefetch). The
// linker only pulls object files from an archive to satisfy unresolved external
// symbol references. Because the anonymous-namespace registrar in
// PrefetchPluginInit.cpp has no external linkage, the linker strips that TU and
// the static initializer that registers the prefetch intrinsics never runs.
//
// Solution: this header forward-declares the (externally-linked) registration
// functions and calls them from an anonymous-namespace static initializer.
// A static with a non-trivial constructor that calls external functions cannot
// be eliminated by the optimizer (the compiler cannot prove the call is
// side-effect-free), so the linker is forced to include the definition TU from
// the archive, registering the intrinsics before main().
//
// Usage: #include <nautilus/prefetch/plugin.hpp> in any TU that links against
// nautilus-prefetch as a static archive and needs the native MLIR/AsmJit
// prefetch lowering.

#include "nautilus/config.hpp"

#ifdef ENABLE_MLIR_BACKEND
namespace nautilus::compiler::mlir {
// Defined in plugins/prefetch/src/MLIRPrefetchIntrinsics.cpp
void RegisterMLIRPrefetchIntrinsicPlugin();
} // namespace nautilus::compiler::mlir

namespace {
// Each TU that includes this header gets its own instance (anonymous namespace
// = internal linkage). The constructor calls an external function, so the
// compiler cannot eliminate it -- initialization is guaranteed before main().
struct NautilusPrefetchMlirPluginRegistrar {
	NautilusPrefetchMlirPluginRegistrar() {
		nautilus::compiler::mlir::RegisterMLIRPrefetchIntrinsicPlugin();
	}
};
[[maybe_unused]] static NautilusPrefetchMlirPluginRegistrar nautilus_prefetch_mlir_plugin_registrar_;
} // namespace
#endif

#ifdef ENABLE_ASMJIT_BACKEND
namespace nautilus::compiler::asmjit {
// Defined in plugins/prefetch/src/AsmJitPrefetchIntrinsics.cpp
void RegisterAsmJitPrefetchIntrinsicPlugin();
} // namespace nautilus::compiler::asmjit

namespace {
struct NautilusPrefetchAsmJitPluginRegistrar {
	NautilusPrefetchAsmJitPluginRegistrar() {
		nautilus::compiler::asmjit::RegisterAsmJitPrefetchIntrinsicPlugin();
	}
};
[[maybe_unused]] static NautilusPrefetchAsmJitPluginRegistrar nautilus_prefetch_asmjit_plugin_registrar_;
} // namespace
#endif
