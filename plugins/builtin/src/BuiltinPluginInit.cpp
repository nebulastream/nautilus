// Static initializers that register the builtin plugin's backend intrinsics
// (MLIR and/or AsmJit) when the builtin plugin library is loaded.
//
// Each per-backend TU is compiled in conditionally by plugins/builtin/CMakeLists.txt
// based on which backends are enabled, so the matching #ifdef blocks below
// match the build set exactly.

#include "nautilus/config.hpp"

#ifdef ENABLE_MLIR_BACKEND
#include "int128/MLIRInt128Intrinsics.hpp"
#include "overflow/MLIROverflowIntrinsics.hpp"
#include "prefetch/MLIRPrefetchIntrinsics.hpp"
#endif

#ifdef ENABLE_ASMJIT_BACKEND
#include "prefetch/AsmJitPrefetchIntrinsics.hpp"
#endif

namespace {
struct BuiltinIntrinsicRegistrar {
	BuiltinIntrinsicRegistrar() {
#ifdef ENABLE_MLIR_BACKEND
		nautilus::compiler::mlir::RegisterMLIRPrefetchIntrinsicPlugin();
		nautilus::compiler::mlir::RegisterMLIRInt128IntrinsicPlugin();
		nautilus::compiler::mlir::RegisterMLIROverflowIntrinsicPlugin();
#endif
#ifdef ENABLE_ASMJIT_BACKEND
		nautilus::compiler::asmjit::RegisterAsmJitPrefetchIntrinsicPlugin();
#endif
	}
};
static BuiltinIntrinsicRegistrar registrar_;
} // namespace
