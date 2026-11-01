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
		manager.addIntrinsic(reinterpret_cast<void*>(&nautlis_assume_stub), [](NirIntrinsicCall& call) -> bool {
			[[maybe_unused]] auto& builder = call.builder;
			::mlir::LLVM::AssumeOp::create(*builder, builder->getUnknownLoc(), call.args[0]);
			return true;
		});
		manager.addIntrinsic(
		    reinterpret_cast<void*>(&nautilus_assume_constant_stub), [](NirIntrinsicCall& call) -> bool {
			    [[maybe_unused]] auto& builder = call.builder;
			    auto value = call.args[0];
			    auto expected = call.args[1];
			    auto cmp = ::mlir::arith::CmpIOp::create(*builder, builder->getUnknownLoc(),
			                                             ::mlir::arith::CmpIPredicate::eq, value, expected);
			    ::mlir::LLVM::AssumeOp::create(*builder, builder->getUnknownLoc(), cmp);
			    return true;
		    });
		manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_assume_range_stub), [](NirIntrinsicCall& call) -> bool {
			[[maybe_unused]] auto& builder = call.builder;
			auto value = call.args[0];
			auto lo = call.args[1];
			auto hi = call.args[2];
			auto geLo = ::mlir::arith::CmpIOp::create(*builder, builder->getUnknownLoc(),
			                                          ::mlir::arith::CmpIPredicate::sge, value, lo);
			auto leHi = ::mlir::arith::CmpIOp::create(*builder, builder->getUnknownLoc(),
			                                          ::mlir::arith::CmpIPredicate::sle, value, hi);
			::mlir::LLVM::AssumeOp::create(*builder, builder->getUnknownLoc(), geLo);
			::mlir::LLVM::AssumeOp::create(*builder, builder->getUnknownLoc(), leHi);
			return true;
		});
		manager.addIntrinsic(
		    reinterpret_cast<void*>(&nautilus_assume_nonzero_stub), [](NirIntrinsicCall& call) -> bool {
			    [[maybe_unused]] auto& builder = call.builder;
			    auto value = call.args[0];
			    auto zero = ::mlir::arith::ConstantOp::create(*builder, builder->getUnknownLoc(), value.getType(),
			                                                  builder->getIntegerAttr(value.getType(), 0));
			    auto cmp = ::mlir::arith::CmpIOp::create(*builder, builder->getUnknownLoc(),
			                                             ::mlir::arith::CmpIPredicate::ne, value, zero);
			    ::mlir::LLVM::AssumeOp::create(*builder, builder->getUnknownLoc(), cmp);
			    return true;
		    });
		manager.addIntrinsic(
		    reinterpret_cast<void*>(&nautilus_assume_aligned_stub), [](NirIntrinsicCall& call) -> bool {
			    auto& builder = call.builder;
			    auto constOp =
			        ::mlir::arith::ConstantOp::create(*builder, builder->getUnknownLoc(), builder->getI1Type(),
			                                          builder->getIntegerAttr(builder->getI1Type(), true));
			    auto ptr = call.args[0];
			    auto align = call.args[1];
			    ::mlir::LLVM::AssumeOp::create(*builder, builder->getUnknownLoc(), constOp,
			                                   ::mlir::LLVM::AssumeAlignTag {}, ptr, align);
			    return true;
		    });
		manager.addIntrinsic(
		    reinterpret_cast<void*>(&nautilus_assume_separate_storage_stub), [](NirIntrinsicCall& call) -> bool {
			    auto& builder = call.builder;
			    auto constOp =
			        ::mlir::arith::ConstantOp::create(*builder, builder->getUnknownLoc(), builder->getI1Type(),
			                                          builder->getIntegerAttr(builder->getI1Type(), true));
			    auto ptrA = call.args[0];
			    auto ptrB = call.args[1];
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
