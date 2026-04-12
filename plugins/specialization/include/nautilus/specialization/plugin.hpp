#pragma once
// Force-link / auto-registration header for the nautilus-specialization MLIR intrinsic plugin.
//
// Problem: *PluginInit.cpp TUs live in a static archive (nautilus-specialization).
// The linker only pulls object files from an archive to satisfy unresolved external
// symbol references. Because the anonymous-namespace registrar in
// SpecializationPluginInit.cpp has no external linkage, the linker strips that TU
// and the static initializer that registers MLIR assume intrinsics never runs.
//
// Solution: this header forward-declares the (externally-linked) registration
// function and calls it from an anonymous-namespace static initializer.
// A static with a non-trivial constructor that calls external functions cannot
// be eliminated by the optimizer (the compiler cannot prove the call is
// side-effect-free), so the linker is forced to include the definition TU from
// the archive, registering the intrinsics before main().
//
// Usage: #include <nautilus/specialization/plugin.hpp> in any TU that links
// against nautilus-specialization as a static archive and needs MLIR assume
// intrinsics.

#include "nautilus/config.hpp"

#ifdef ENABLE_MLIR_BACKEND
namespace nautilus::compiler::mlir {
// Defined in plugins/specialization/src/MLIRAssumeIntrinsics.cpp
void RegisterMLIRAssumeIntrinsicPlugin();
} // namespace nautilus::compiler::mlir

namespace {
// Each TU that includes this header gets its own instance (anonymous namespace
// = internal linkage). The constructor calls an external function, so the
// compiler cannot eliminate it — initialization is guaranteed before main().
struct NautilusSpecializationMlirPluginRegistrar {
	NautilusSpecializationMlirPluginRegistrar() {
		nautilus::compiler::mlir::RegisterMLIRAssumeIntrinsicPlugin();
	}
};
[[maybe_unused]] static NautilusSpecializationMlirPluginRegistrar nautilus_specialization_mlir_plugin_registrar_;
} // namespace
#endif
