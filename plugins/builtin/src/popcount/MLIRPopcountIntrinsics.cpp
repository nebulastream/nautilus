#include "MLIRPopcountIntrinsics.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "popcount_impl.hpp"
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>

namespace nautilus::compiler::mlir {

// ============================================================================
// MLIR Popcount Intrinsic Plugin
//
// Intercepts the nautilus_popcount_* function pointers and replaces the call
// with LLVM's `llvm.ctpop` intrinsic (mlir::LLVM::CtPopOp). LLVM integers are
// signless, so the signed and unsigned variants of a width share one lowering:
// `llvm.ctpop.iN` counts the bits of the N-bit pattern and returns an iN.
// ============================================================================

class MLIRPopcountIntrinsicPlugin : public MLIRIntrinsicPlugin {
public:
	void registerIntrinsics(MLIRIntrinsicManager& manager) override;
	~MLIRPopcountIntrinsicPlugin() override = default;
};

namespace {

bool ctpopIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::CallOperation* call,
                    MLIRLoweringProvider::ValueFrame& frame) {
	auto input = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto result = ::mlir::LLVM::CtPopOp::create(*builder, builder->getUnknownLoc(), input.getType(), input);
	frame.setValue(call->getIdentifier(), result);
	return true;
}

} // namespace

void MLIRPopcountIntrinsicPlugin::registerIntrinsics(MLIRIntrinsicManager& manager) {
	using namespace nautilus::builtin::detail;
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_popcount_i8), ctpopIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_popcount_i16), ctpopIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_popcount_i32), ctpopIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_popcount_i64), ctpopIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_popcount_u8), ctpopIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_popcount_u16), ctpopIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_popcount_u32), ctpopIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_popcount_u64), ctpopIntrinsic);
}

void RegisterMLIRPopcountIntrinsicPlugin() {
	MLIRIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<MLIRPopcountIntrinsicPlugin>());
}

} // namespace nautilus::compiler::mlir
