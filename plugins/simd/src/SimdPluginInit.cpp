// This translation unit is only added to the build when ENABLE_MLIR_BACKEND is ON
// (see plugins/simd/CMakeLists.txt), so the registration runs unconditionally.
#include "MLIRVectorIntrinsics.hpp"

namespace {
// Static initializer to register MLIR intrinsic plugins when the simd plugin library is loaded.
struct SimdIntrinsicRegistrar {
	SimdIntrinsicRegistrar() {
		nautilus::compiler::mlir::RegisterMLIRVectorIntrinsicPlugin();
	}
};
static SimdIntrinsicRegistrar registrar_;
} // namespace
