#pragma once
// Force-link / auto-registration header for the nautilus-builtin MLIR/AsmJit
// intrinsic plugins.
//
// Problem: *PluginInit.cpp TUs live in a static archive (nautilus-builtin). The
// linker only pulls object files from an archive to satisfy unresolved external
// symbol references. Because the anonymous-namespace registrar in
// BuiltinPluginInit.cpp has no external linkage, the linker strips that TU and
// the static initializer that registers the builtin intrinsics never runs.
//
// Solution: this header forward-declares the (externally-linked) registration
// functions and calls them from an anonymous-namespace static initializer.
// A static with a non-trivial constructor that calls external functions cannot
// be eliminated by the optimizer (the compiler cannot prove the call is
// side-effect-free), so the linker is forced to include the definition TU from
// the archive, registering the intrinsics before main().
//
// Usage: #include <nautilus/builtin/plugin.hpp> in any TU that links against
// nautilus-builtin as a static archive and needs the native MLIR/AsmJit
// builtin lowering (e.g. prefetch, int128, popcount).

#include "nautilus/config.hpp"

#ifdef ENABLE_MLIR_BACKEND
namespace nautilus::compiler::mlir {
// Defined in plugins/builtin/src/prefetch/MLIRPrefetchIntrinsics.cpp
void RegisterMLIRPrefetchIntrinsicPlugin();
// Defined in plugins/builtin/src/int128/MLIRInt128Intrinsics.cpp
void RegisterMLIRInt128IntrinsicPlugin();
// Defined in plugins/builtin/src/popcount/MLIRPopcountIntrinsics.cpp
void RegisterMLIRPopcountIntrinsicPlugin();
} // namespace nautilus::compiler::mlir

namespace {
// Each TU that includes this header gets its own instance (anonymous namespace
// = internal linkage). The constructor calls an external function, so the
// compiler cannot eliminate it -- initialization is guaranteed before main().
struct NautilusBuiltinMlirPluginRegistrar {
	NautilusBuiltinMlirPluginRegistrar() {
		nautilus::compiler::mlir::RegisterMLIRPrefetchIntrinsicPlugin();
		nautilus::compiler::mlir::RegisterMLIRInt128IntrinsicPlugin();
		nautilus::compiler::mlir::RegisterMLIRPopcountIntrinsicPlugin();
	}
};
[[maybe_unused]] static NautilusBuiltinMlirPluginRegistrar nautilus_builtin_mlir_plugin_registrar_;
} // namespace
#endif

#ifdef ENABLE_ASMJIT_BACKEND
namespace nautilus::compiler::asmjit {
// Defined in plugins/builtin/src/prefetch/AsmJitPrefetchIntrinsics.cpp
void RegisterAsmJitPrefetchIntrinsicPlugin();
// Defined in plugins/builtin/src/popcount/AsmJitPopcountIntrinsics.cpp
void RegisterAsmJitPopcountIntrinsicPlugin();
} // namespace nautilus::compiler::asmjit

namespace {
struct NautilusBuiltinAsmJitPluginRegistrar {
	NautilusBuiltinAsmJitPluginRegistrar() {
		nautilus::compiler::asmjit::RegisterAsmJitPrefetchIntrinsicPlugin();
		nautilus::compiler::asmjit::RegisterAsmJitPopcountIntrinsicPlugin();
	}
};
[[maybe_unused]] static NautilusBuiltinAsmJitPluginRegistrar nautilus_builtin_asmjit_plugin_registrar_;
} // namespace
#endif
