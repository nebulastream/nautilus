#include "nautilus/compiler/backends/mlir/intrinsics/MLIRAssumeIntrinsics.hpp"
#include "nautilus/api/profile/assume_stubs.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/profile/assume.hpp"
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>
namespace nautilus::compiler::mlir {
// Compiler intrinsic for assume
class NautilusAssumeIntrinsicPlugin : public MLIRIntrinsicPlugin {
public:
	void registerIntrinsics(MLIRIntrinsicManager& manager) override {
		manager.addIntrinsic(
		    reinterpret_cast<void*>(&nautlis_assume_stub),
		    [](std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
		       [[maybe_unused]] MLIRLoweringProvider::ValueFrame& frame) -> bool {
			    builder->create<::mlir::LLVM::AssumeOp>(
			        builder->getUnknownLoc(), frame.getValue(call->getInputArguments().at(0)->getIdentifier()));
			    return true;
		    });
		manager.addIntrinsic(
		    reinterpret_cast<void*>(&nautilus_assume_aligned_stub),
		    [](std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
		       [[maybe_unused]] compiler::mlir::MLIRLoweringProvider::ValueFrame& frame) -> bool {
			    auto constOp =
			        builder->create<::mlir::arith::ConstantOp>(builder->getUnknownLoc(), builder->getI1Type(),
			                                                   builder->getIntegerAttr(builder->getI1Type(), true));
			    auto ptr = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
			    auto align = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
			    builder->create<::mlir::LLVM::AssumeOp>(builder->getUnknownLoc(), constOp,
			                                            ::mlir::LLVM::AssumeAlignTag {}, ptr, align);
			    return true;
		    });
	}
	~NautilusAssumeIntrinsicPlugin() override = default;
};

void RegisterMLIRAssumeIntrinsicPlugin() {
	MLIRIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<NautilusAssumeIntrinsicPlugin>());
}
} // namespace nautilus::compiler::mlir
