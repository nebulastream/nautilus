#include "MLIROverflowIntrinsics.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <nautilus/builtin/overflow/overflow.hpp>

namespace nautilus::compiler::mlir {
namespace {
using Call = compiler::ir::CallOperation;
using Frame = MLIRLoweringProvider::ValueFrame;

::mlir::Value argument(const Call* call, Frame& frame, unsigned index) {
	return frame.getValue(call->getInputArguments()[index]->getIdentifier());
}

// `{result, overflow} = op(lhs, rhs)`; stores the result through `out`, the call yields the flag.
template <typename Op>
bool emit(std::unique_ptr<::mlir::OpBuilder>& b, const Call* call, Frame& frame, ::mlir::Value lhs, ::mlir::Value rhs,
          ::mlir::Value out, unsigned alignment) {
	auto loc = b->getUnknownLoc();
	auto structType = ::mlir::LLVM::LLVMStructType::getLiteral(b->getContext(), {lhs.getType(), b->getI1Type()});
	auto pair = Op::create(*b, loc, structType, lhs, rhs);
	auto result = ::mlir::LLVM::ExtractValueOp::create(*b, loc, pair, 0);
	auto overflow = ::mlir::LLVM::ExtractValueOp::create(*b, loc, pair, 1);
	::mlir::LLVM::StoreOp::create(*b, loc, result, out, alignment);
	frame.setValue(call->getIdentifier(), overflow);
	return true;
}

// Scalar operands: (a, b, out*).
template <typename Op>
bool scalar(std::unique_ptr<::mlir::OpBuilder>& b, const Call* call, Frame& frame) {
	auto lhs = argument(call, frame, 0);
	auto rhs = argument(call, frame, 1);
	unsigned alignment = lhs.getType().getIntOrFloatBitWidth() / 8;
	return emit<Op>(b, call, frame, lhs, rhs, argument(call, frame, 2), alignment);
}

// 128-bit operands in memory: (a*, b*, out*), any alignment.
template <typename Op>
bool wide(std::unique_ptr<::mlir::OpBuilder>& b, const Call* call, Frame& frame) {
	auto loc = b->getUnknownLoc();
	auto i128 = b->getIntegerType(128);
	auto lhs = ::mlir::LLVM::LoadOp::create(*b, loc, i128, argument(call, frame, 0), 1);
	auto rhs = ::mlir::LLVM::LoadOp::create(*b, loc, i128, argument(call, frame, 1), 1);
	return emit<Op>(b, call, frame, lhs, rhs, argument(call, frame, 2), 1);
}

class Plugin final : public MLIRIntrinsicPlugin {
public:
	void registerIntrinsics(MLIRIntrinsicManager& m) override {
		using namespace ::mlir::LLVM;
#define ADD(FN, HANDLER) m.addIntrinsic(reinterpret_cast<void*>(::nautilus::builtin::detail::FN), HANDLER)
		ADD(add_overflow_i32_impl, scalar<SAddWithOverflowOp>);
		ADD(sub_overflow_i32_impl, scalar<SSubWithOverflowOp>);
		ADD(mul_overflow_i32_impl, scalar<SMulWithOverflowOp>);
		ADD(add_overflow_i64_impl, scalar<SAddWithOverflowOp>);
		ADD(sub_overflow_i64_impl, scalar<SSubWithOverflowOp>);
		ADD(mul_overflow_i64_impl, scalar<SMulWithOverflowOp>);
		ADD(add_overflow_i128_impl, wide<SAddWithOverflowOp>);
		ADD(sub_overflow_i128_impl, wide<SSubWithOverflowOp>);
		ADD(mul_overflow_i128_impl, wide<SMulWithOverflowOp>);
#undef ADD
	}
};
} // namespace

void RegisterMLIROverflowIntrinsicPlugin() {
	MLIRIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<Plugin>());
}
} // namespace nautilus::compiler::mlir
