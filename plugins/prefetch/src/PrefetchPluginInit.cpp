// Static initializers that register the prefetch plugin's backend intrinsics
// (MLIR and/or AsmJit) when the prefetch plugin library is loaded.
//
// Each per-backend TU is compiled in conditionally by plugins/prefetch/CMakeLists.txt
// based on which backends are enabled, so the matching #ifdef blocks below
// match the build set exactly.

#include "nautilus/config.hpp"

#ifdef ENABLE_MLIR_BACKEND
#include "MLIRPrefetchIntrinsics.hpp"
#endif

#ifdef ENABLE_ASMJIT_BACKEND
#include "AsmJitPrefetchIntrinsics.hpp"
#endif

namespace {
struct PrefetchIntrinsicRegistrar {
	PrefetchIntrinsicRegistrar() {
#ifdef ENABLE_MLIR_BACKEND
		nautilus::compiler::mlir::RegisterMLIRPrefetchIntrinsicPlugin();
#endif
#ifdef ENABLE_ASMJIT_BACKEND
		nautilus::compiler::asmjit::RegisterAsmJitPrefetchIntrinsicPlugin();
#endif
	}
};
static PrefetchIntrinsicRegistrar registrar_;
} // namespace
