
#ifdef ENABLE_MLIR_BACKEND
#include "MLIRBitIntrinsics.hpp"
#include "MLIRMathIntrinsics.hpp"

namespace {
// Static initializer to register MLIR intrinsic plugins when the std plugin library is loaded.
struct StdIntrinsicRegistrar {
	StdIntrinsicRegistrar() {
		nautilus::compiler::mlir::RegisterMLIRMathIntrinsicPlugin();
		nautilus::compiler::mlir::RegisterMLIRBitIntrinsicPlugin();
	}
};
static StdIntrinsicRegistrar registrar_;
} // namespace
#endif
