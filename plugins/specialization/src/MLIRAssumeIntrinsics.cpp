#include "MLIRAssumeIntrinsics.hpp"
#include "assume_stubs.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/specialization/assume.hpp"
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
			    ::mlir::LLVM::AssumeOp::create(*builder, builder->getUnknownLoc(),
			                                   frame.getValue(call->getInputArguments()[0]->getIdentifier()));
			    return true;
		    });
		manager.addIntrinsic(
		    reinterpret_cast<void*>(&nautilus_assume_constant_stub),
		    [](std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
		       MLIRLoweringProvider::ValueFrame& frame) -> bool {
			    auto value = frame.getValue(call->getInputArguments()[0]->getIdentifier());
			    auto expected = frame.getValue(call->getInputArguments()[1]->getIdentifier());
			    auto cmp = ::mlir::arith::CmpIOp::create(*builder, builder->getUnknownLoc(),
			                                             ::mlir::arith::CmpIPredicate::eq, value, expected);
			    ::mlir::LLVM::AssumeOp::create(*builder, builder->getUnknownLoc(), cmp);
			    return true;
		    });
		manager.addIntrinsic(
		    reinterpret_cast<void*>(&nautilus_assume_range_stub),
		    [](std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
		       MLIRLoweringProvider::ValueFrame& frame) -> bool {
			    auto value = frame.getValue(call->getInputArguments()[0]->getIdentifier());
			    auto lo = frame.getValue(call->getInputArguments()[1]->getIdentifier());
			    auto hi = frame.getValue(call->getInputArguments()[2]->getIdentifier());
			    auto geLo = ::mlir::arith::CmpIOp::create(*builder, builder->getUnknownLoc(),
			                                              ::mlir::arith::CmpIPredicate::sge, value, lo);
			    auto leHi = ::mlir::arith::CmpIOp::create(*builder, builder->getUnknownLoc(),
			                                              ::mlir::arith::CmpIPredicate::sle, value, hi);
			    ::mlir::LLVM::AssumeOp::create(*builder, builder->getUnknownLoc(), geLo);
			    ::mlir::LLVM::AssumeOp::create(*builder, builder->getUnknownLoc(), leHi);
			    return true;
		    });
		manager.addIntrinsic(
		    reinterpret_cast<void*>(&nautilus_assume_nonzero_stub),
		    [](std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
		       MLIRLoweringProvider::ValueFrame& frame) -> bool {
			    auto value = frame.getValue(call->getInputArguments()[0]->getIdentifier());
			    auto zero = ::mlir::arith::ConstantOp::create(*builder, builder->getUnknownLoc(), value.getType(),
			                                                  builder->getIntegerAttr(value.getType(), 0));
			    auto cmp = ::mlir::arith::CmpIOp::create(*builder, builder->getUnknownLoc(),
			                                             ::mlir::arith::CmpIPredicate::ne, value, zero);
			    ::mlir::LLVM::AssumeOp::create(*builder, builder->getUnknownLoc(), cmp);
			    return true;
		    });
		manager.addIntrinsic(
		    reinterpret_cast<void*>(&nautilus_assume_aligned_stub),
		    [](std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
		       [[maybe_unused]] compiler::mlir::MLIRLoweringProvider::ValueFrame& frame) -> bool {
			    auto constOp =
			        ::mlir::arith::ConstantOp::create(*builder, builder->getUnknownLoc(), builder->getI1Type(),
			                                          builder->getIntegerAttr(builder->getI1Type(), true));
			    auto ptr = frame.getValue(call->getInputArguments()[0]->getIdentifier());
			    auto align = frame.getValue(call->getInputArguments()[1]->getIdentifier());
			    ::mlir::LLVM::AssumeOp::create(*builder, builder->getUnknownLoc(), constOp,
			                                   ::mlir::LLVM::AssumeAlignTag {}, ptr, align);
			    return true;
		    });
		manager.addIntrinsic(
		    reinterpret_cast<void*>(&nautilus_assume_separate_storage_stub),
		    [](std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
		       [[maybe_unused]] compiler::mlir::MLIRLoweringProvider::ValueFrame& frame) -> bool {
			    auto constOp =
			        ::mlir::arith::ConstantOp::create(*builder, builder->getUnknownLoc(), builder->getI1Type(),
			                                          builder->getIntegerAttr(builder->getI1Type(), true));
			    auto ptrA = frame.getValue(call->getInputArguments()[0]->getIdentifier());
			    auto ptrB = frame.getValue(call->getInputArguments()[1]->getIdentifier());
			    ::mlir::LLVM::AssumeOp::create(*builder, builder->getUnknownLoc(), constOp,
			                                   ::mlir::LLVM::AssumeSeparateStorageTag {}, ptrA, ptrB);
			    return true;
		    });
	}
	~NautilusAssumeIntrinsicPlugin() override = default;
};

void RegisterMLIRAssumeIntrinsicPlugin() {
	MLIRIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<NautilusAssumeIntrinsicPlugin>());
}
} // namespace nautilus::compiler::mlir
