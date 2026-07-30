#include "MLIRInt128Intrinsics.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <nautilus/int128.hpp>

namespace nautilus::compiler::mlir {
namespace {
using Call = compiler::ir::ProxyCallOperation;
using Frame = MLIRLoweringProvider::ValueFrame;

::mlir::Value load(std::unique_ptr<::mlir::OpBuilder>& b, const Call* call, Frame& frame, unsigned index) {
	auto ptr = frame.getValue(call->getInputArguments()[index]->getIdentifier());
	return ::mlir::LLVM::LoadOp::create(*b, b->getUnknownLoc(), b->getIntegerType(128), ptr);
}

void save(std::unique_ptr<::mlir::OpBuilder>& b, const Call* call, Frame& frame, ::mlir::Value value) {
	auto one = ::mlir::LLVM::ConstantOp::create(*b, b->getUnknownLoc(), b->getI64Type(), b->getI64IntegerAttr(1));
	auto ptr = ::mlir::LLVM::AllocaOp::create(
	    *b, b->getUnknownLoc(), ::mlir::LLVM::LLVMPointerType::get(b->getContext()), b->getIntegerType(128), one, 16);
	::mlir::LLVM::StoreOp::create(*b, b->getUnknownLoc(), value, ptr);
	frame.setValue(call->getIdentifier(), ptr);
}

template <typename Op>
bool binary(std::unique_ptr<::mlir::OpBuilder>& b, const Call* call, Frame& frame) {
	save(b, call, frame, Op::create(*b, b->getUnknownLoc(), load(b, call, frame, 0), load(b, call, frame, 1)));
	return true;
}

bool negate(std::unique_ptr<::mlir::OpBuilder>& b, const Call* call, Frame& frame) {
	auto i128 = b->getIntegerType(128);
	auto zero = ::mlir::LLVM::ConstantOp::create(*b, b->getUnknownLoc(), i128, b->getIntegerAttr(i128, 0));
	save(b, call, frame, ::mlir::LLVM::SubOp::create(*b, b->getUnknownLoc(), zero, load(b, call, frame, 0)));
	return true;
}

bool bitNot(std::unique_ptr<::mlir::OpBuilder>& b, const Call* call, Frame& frame) {
	auto i128 = b->getIntegerType(128);
	auto ones = ::mlir::LLVM::ConstantOp::create(*b, b->getUnknownLoc(), i128, b->getIntegerAttr(i128, -1));
	save(b, call, frame, ::mlir::LLVM::XOrOp::create(*b, b->getUnknownLoc(), load(b, call, frame, 0), ones));
	return true;
}

bool make(std::unique_ptr<::mlir::OpBuilder>& b, const Call* call, Frame& frame) {
	auto i128 = b->getIntegerType(128);
	auto low = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto high = frame.getValue(call->getInputArguments()[1]->getIdentifier());
	auto lo = ::mlir::LLVM::ZExtOp::create(*b, b->getUnknownLoc(), i128, low);
	::mlir::Value hi = ::mlir::LLVM::SExtOp::create(*b, b->getUnknownLoc(), i128, high);
	auto shift = ::mlir::LLVM::ConstantOp::create(*b, b->getUnknownLoc(), i128, b->getIntegerAttr(i128, 64));
	hi = ::mlir::LLVM::ShlOp::create(*b, b->getUnknownLoc(), hi, shift);
	save(b, call, frame, ::mlir::LLVM::OrOp::create(*b, b->getUnknownLoc(), lo, hi));
	return true;
}

template <typename Op>
bool shift(std::unique_ptr<::mlir::OpBuilder>& b, const Call* call, Frame& frame) {
	auto amount = frame.getValue(call->getInputArguments()[1]->getIdentifier());
	auto mask = ::mlir::LLVM::ConstantOp::create(*b, b->getUnknownLoc(), b->getI32Type(), b->getI32IntegerAttr(127));
	amount = ::mlir::LLVM::AndOp::create(*b, b->getUnknownLoc(), amount, mask);
	amount = ::mlir::LLVM::ZExtOp::create(*b, b->getUnknownLoc(), b->getIntegerType(128), amount);
	save(b, call, frame, Op::create(*b, b->getUnknownLoc(), load(b, call, frame, 0), amount));
	return true;
}

bool compare(std::unique_ptr<::mlir::OpBuilder>& b, const Call* call, Frame& frame,
             ::mlir::LLVM::ICmpPredicate predicate) {
	auto result = ::mlir::LLVM::ICmpOp::create(*b, b->getUnknownLoc(), predicate, load(b, call, frame, 0),
	                                           load(b, call, frame, 1));
	frame.setValue(call->getIdentifier(), result);
	return true;
}
bool eq(std::unique_ptr<::mlir::OpBuilder>& b, const Call* c, Frame& f) {
	return compare(b, c, f, ::mlir::LLVM::ICmpPredicate::eq);
}
bool lt(std::unique_ptr<::mlir::OpBuilder>& b, const Call* c, Frame& f) {
	return compare(b, c, f, ::mlir::LLVM::ICmpPredicate::slt);
}

bool low(std::unique_ptr<::mlir::OpBuilder>& b, const Call* call, Frame& frame) {
	auto result = ::mlir::LLVM::TruncOp::create(*b, b->getUnknownLoc(), b->getI64Type(), load(b, call, frame, 0));
	frame.setValue(call->getIdentifier(), result);
	return true;
}
bool high(std::unique_ptr<::mlir::OpBuilder>& b, const Call* call, Frame& frame) {
	auto i128 = b->getIntegerType(128);
	auto amount = ::mlir::LLVM::ConstantOp::create(*b, b->getUnknownLoc(), i128, b->getIntegerAttr(i128, 64));
	auto shifted = ::mlir::LLVM::AShrOp::create(*b, b->getUnknownLoc(), load(b, call, frame, 0), amount);
	frame.setValue(call->getIdentifier(),
	               ::mlir::LLVM::TruncOp::create(*b, b->getUnknownLoc(), b->getI64Type(), shifted));
	return true;
}

bool memoryLoad(std::unique_ptr<::mlir::OpBuilder>& b, const Call* call, Frame& frame) {
	auto ptr = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto value = ::mlir::LLVM::LoadOp::create(*b, b->getUnknownLoc(), b->getIntegerType(128), ptr, 1);
	save(b, call, frame, value);
	return true;
}

bool memoryStore(std::unique_ptr<::mlir::OpBuilder>& b, const Call* call, Frame& frame) {
	auto ptr = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	::mlir::LLVM::StoreOp::create(*b, b->getUnknownLoc(), load(b, call, frame, 1), ptr, 1);
	return true;
}

class Plugin final : public MLIRIntrinsicPlugin {
public:
	void registerIntrinsics(MLIRIntrinsicManager& m) override {
#define ADD(FN, HANDLER) m.addIntrinsic(reinterpret_cast<void*>(::nautilus::detail::FN), HANDLER)
		ADD(int128_make_impl, make);
		ADD(int128_add_impl, binary<::mlir::LLVM::AddOp>);
		ADD(int128_sub_impl, binary<::mlir::LLVM::SubOp>);
		ADD(int128_mul_impl, binary<::mlir::LLVM::MulOp>);
		ADD(int128_div_impl, binary<::mlir::LLVM::SDivOp>);
		ADD(int128_rem_impl, binary<::mlir::LLVM::SRemOp>);
		ADD(int128_and_impl, binary<::mlir::LLVM::AndOp>);
		ADD(int128_or_impl, binary<::mlir::LLVM::OrOp>);
		ADD(int128_xor_impl, binary<::mlir::LLVM::XOrOp>);
		ADD(int128_neg_impl, negate);
		ADD(int128_not_impl, bitNot);
		ADD(int128_shl_impl, shift<::mlir::LLVM::ShlOp>);
		ADD(int128_shr_impl, shift<::mlir::LLVM::AShrOp>);
		ADD(int128_eq_impl, eq);
		ADD(int128_lt_impl, lt);
		ADD(int128_low_impl, low);
		ADD(int128_high_impl, high);
		ADD(int128_load_impl, memoryLoad);
		ADD(int128_store_impl, memoryStore);
#undef ADD
	}
};
} // namespace

void RegisterMLIRInt128IntrinsicPlugin() {
	MLIRIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<Plugin>());
}
} // namespace nautilus::compiler::mlir
