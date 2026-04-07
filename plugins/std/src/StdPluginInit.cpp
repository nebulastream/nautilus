
#ifdef ENABLE_MLIR_BACKEND
#include "MLIRAtomicIntrinsics.hpp"
#include "MLIRBitIntrinsics.hpp"
#include "MLIRMathIntrinsics.hpp"

namespace {
// Static initializer to register MLIR intrinsic plugins when the std plugin library is loaded.
struct StdIntrinsicRegistrar {
	StdIntrinsicRegistrar() {
		nautilus::compiler::mlir::RegisterMLIRMathIntrinsicPlugin();
		nautilus::compiler::mlir::RegisterMLIRBitIntrinsicPlugin();
		nautilus::compiler::mlir::RegisterMLIRAtomicIntrinsicPlugin();
	}
};
static StdIntrinsicRegistrar registrar_;
} // namespace
#endif
