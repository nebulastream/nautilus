// This translation unit is only added to the build when ENABLE_MLIR_BACKEND is ON
// (see plugins/specialization/CMakeLists.txt), so the registration runs unconditionally.
#include "MLIRAssumeIntrinsics.hpp"

namespace {
// Static initializer to register MLIR intrinsic plugins when the specialization plugin library is loaded.
struct SpecializationIntrinsicRegistrar {
	SpecializationIntrinsicRegistrar() {
		nautilus::compiler::mlir::RegisterMLIRAssumeIntrinsicPlugin();
	}
};
static SpecializationIntrinsicRegistrar registrar_;
} // namespace
