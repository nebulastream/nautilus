
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBitIntrinsics.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include <bit>
#include <cstdint>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>

namespace nautilus::compiler::mlir {

// Compiler intrinsic for bit manipulation operations
class MLIRBitIntrinsicPlugin : public MLIRIntrinsicPlugin {
public:
	void registerIntrinsics(MLIRIntrinsicManager& manager) override;
	~MLIRBitIntrinsicPlugin() override = default;
};

// ============================================================================
// Helper Templates for Intrinsic Registration
// ============================================================================

/// Generic helper for unary bit operations
template <typename MLIROp>
bool replaceWithUnaryBitIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                  const compiler::ir::ProxyCallOperation* call, MLIRLoweringProvider::ValueFrame& frame) {
	auto inputArg = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	auto result = builder->create<MLIROp>(builder->getUnknownLoc(), inputArg.getType(), inputArg);
	frame.setValue(call->getIdentifier(), result);
	return true;
}

/// Generic helper for binary bit operations
template <typename MLIROp>
bool replaceWithBinaryBitIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                   const compiler::ir::ProxyCallOperation* call, MLIRLoweringProvider::ValueFrame& frame) {
	auto arg1 = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	auto arg2 = frame.getValue(call->getInputArguments().at(1)->getIdentifier());
	auto result = builder->create<MLIROp>(builder->getUnknownLoc(), arg1.getType(), arg1, arg2);
	frame.setValue(call->getIdentifier(), result);
	return true;
}

/// Helper for CTLZ intrinsic (count leading zeros)
/// LLVM CTLZ takes a second boolean argument indicating if zero is undefined
bool replaceWithCtlzIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                               MLIRLoweringProvider::ValueFrame& frame) {
	auto inputArg = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	// Second argument: is_zero_undef = false (zero is defined)
	auto isZeroUndefAttr = builder->getBoolAttr(false);
	auto result = builder->create<::mlir::LLVM::CountLeadingZerosOp>(builder->getUnknownLoc(), inputArg.getType(), inputArg,
	                                                                  isZeroUndefAttr);
	frame.setValue(call->getIdentifier(), result);
	return true;
}

/// Helper for CTTZ intrinsic (count trailing zeros)
/// LLVM CTTZ takes a second boolean argument indicating if zero is undefined
bool replaceWithCttzIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                               MLIRLoweringProvider::ValueFrame& frame) {
	auto inputArg = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	// Second argument: is_zero_undef = false (zero is defined)
	auto isZeroUndefAttr = builder->getBoolAttr(false);
	auto result = builder->create<::mlir::LLVM::CountTrailingZerosOp>(builder->getUnknownLoc(), inputArg.getType(), inputArg,
	                                                                   isZeroUndefAttr);
	frame.setValue(call->getIdentifier(), result);
	return true;
}

/// Helper for rotate left using funnel shift
/// rotl(x, s) = fshl(x, x, s)
bool replaceWithRotlIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                               MLIRLoweringProvider::ValueFrame& frame) {
	auto x = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	auto s = frame.getValue(call->getInputArguments().at(1)->getIdentifier());
	// fshl(x, x, s) performs a rotate left
	auto result = builder->create<::mlir::LLVM::FshlOp>(builder->getUnknownLoc(), x.getType(), x, x, s);
	frame.setValue(call->getIdentifier(), result);
	return true;
}

/// Helper for rotate right using funnel shift
/// rotr(x, s) = fshr(x, x, s)
bool replaceWithRotrIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                               MLIRLoweringProvider::ValueFrame& frame) {
	auto x = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	auto s = frame.getValue(call->getInputArguments().at(1)->getIdentifier());
	// fshr(x, x, s) performs a rotate right
	auto result = builder->create<::mlir::LLVM::FshrOp>(builder->getUnknownLoc(), x.getType(), x, x, s);
	frame.setValue(call->getIdentifier(), result);
	return true;
}

// ============================================================================
// Intrinsic Registration
// ============================================================================

void MLIRBitIntrinsicPlugin::registerIntrinsics(MLIRIntrinsicManager& manager) {
	// ========================================================================
	// Count leading zeros (ctlz)
	// ========================================================================
	manager.addIntrinsic(reinterpret_cast<void*>(&std::countl_zero<uint8_t>), replaceWithCtlzIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&std::countl_zero<uint16_t>), replaceWithCtlzIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&std::countl_zero<uint32_t>), replaceWithCtlzIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&std::countl_zero<uint64_t>), replaceWithCtlzIntrinsic);

	// ========================================================================
	// Count trailing zeros (cttz)
	// ========================================================================
	manager.addIntrinsic(reinterpret_cast<void*>(&std::countr_zero<uint8_t>), replaceWithCttzIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&std::countr_zero<uint16_t>), replaceWithCttzIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&std::countr_zero<uint32_t>), replaceWithCttzIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&std::countr_zero<uint64_t>), replaceWithCttzIntrinsic);

	// ========================================================================
	// Population count (ctpop)
	// ========================================================================
	manager.addIntrinsic(reinterpret_cast<void*>(&std::popcount<uint8_t>),
	                     replaceWithUnaryBitIntrinsic<::mlir::LLVM::CtPopOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(&std::popcount<uint16_t>),
	                     replaceWithUnaryBitIntrinsic<::mlir::LLVM::CtPopOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(&std::popcount<uint32_t>),
	                     replaceWithUnaryBitIntrinsic<::mlir::LLVM::CtPopOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(&std::popcount<uint64_t>),
	                     replaceWithUnaryBitIntrinsic<::mlir::LLVM::CtPopOp>);

	// ========================================================================
	// Rotate left (rotl) - using funnel shift left
	// ========================================================================
	manager.addIntrinsic(reinterpret_cast<void*>(&std::rotl<uint8_t>), replaceWithRotlIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&std::rotl<uint16_t>), replaceWithRotlIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&std::rotl<uint32_t>), replaceWithRotlIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&std::rotl<uint64_t>), replaceWithRotlIntrinsic);

	// ========================================================================
	// Rotate right (rotr) - using funnel shift right
	// ========================================================================
	manager.addIntrinsic(reinterpret_cast<void*>(&std::rotr<uint8_t>), replaceWithRotrIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&std::rotr<uint16_t>), replaceWithRotrIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&std::rotr<uint32_t>), replaceWithRotrIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(&std::rotr<uint64_t>), replaceWithRotrIntrinsic);

	// ========================================================================
	// Byte swap (bswap)
	// ========================================================================
#if __cpp_lib_byteswap >= 202110L
	manager.addIntrinsic(reinterpret_cast<void*>(&std::byteswap<uint16_t>),
	                     replaceWithUnaryBitIntrinsic<::mlir::LLVM::ByteSwapOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(&std::byteswap<uint32_t>),
	                     replaceWithUnaryBitIntrinsic<::mlir::LLVM::ByteSwapOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(&std::byteswap<uint64_t>),
	                     replaceWithUnaryBitIntrinsic<::mlir::LLVM::ByteSwapOp>);
#endif

	// Note: The following bit operations don't have direct LLVM intrinsics:
	// - countl_one, countr_one: Can be implemented as countl_zero(~x), countr_zero(~x)
	// - has_single_bit: Can be implemented as popcount(x) == 1
	// - bit_width: Can be implemented as bitwidth - countl_zero(x)
	// - bit_ceil, bit_floor: Complex operations without direct intrinsics
	// These will use the runtime library functions instead
}

void RegisterMLIRBitIntrinsicPlugin() {
	MLIRIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<MLIRBitIntrinsicPlugin>());
}

} // namespace nautilus::compiler::mlir
