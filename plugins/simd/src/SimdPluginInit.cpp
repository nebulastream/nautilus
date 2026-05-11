// Static initializers that register the SIMD plugin's backend intrinsics
// (MLIR and/or AsmJit) when the simd plugin library is loaded.
//
// Each per-backend TU is compiled in conditionally by plugins/simd/CMakeLists.txt
// based on which backends are enabled, so the matching #ifdef blocks below
// match the build set exactly.

#include "nautilus/config.hpp"

#ifdef ENABLE_MLIR_BACKEND
#include "MLIRVectorIntrinsics.hpp"
#endif

#ifdef ENABLE_ASMJIT_BACKEND
#include "AsmJitVectorIntrinsics.hpp"
#endif

namespace {
struct SimdIntrinsicRegistrar {
	SimdIntrinsicRegistrar() {
#ifdef ENABLE_MLIR_BACKEND
		nautilus::compiler::mlir::RegisterMLIRVectorIntrinsicPlugin();
#endif
#ifdef ENABLE_ASMJIT_BACKEND
		nautilus::compiler::asmjit::RegisterAsmJitVectorIntrinsicPlugin();
#endif
	}
};
static SimdIntrinsicRegistrar registrar_;
} // namespace
