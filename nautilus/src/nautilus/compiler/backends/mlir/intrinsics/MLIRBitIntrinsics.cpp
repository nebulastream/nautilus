
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBitIntrinsics.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include <bit>
#include <cstdint>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>

// Declare extern "C" wrapper functions from bit.cpp
extern "C" {
uint8_t countl_zero_u8_impl(uint8_t x);
uint16_t countl_zero_u16_impl(uint16_t x);
uint32_t countl_zero_u32_impl(uint32_t x);
uint64_t countl_zero_u64_impl(uint64_t x);

uint8_t countr_zero_u8_impl(uint8_t x);
uint16_t countr_zero_u16_impl(uint16_t x);
uint32_t countr_zero_u32_impl(uint32_t x);
uint64_t countr_zero_u64_impl(uint64_t x);

uint8_t popcount_u8_impl(uint8_t x);
uint16_t popcount_u16_impl(uint16_t x);
uint32_t popcount_u32_impl(uint32_t x);
uint64_t popcount_u64_impl(uint64_t x);

uint8_t rotl_u8_impl(uint8_t x, uint8_t s);
uint16_t rotl_u16_impl(uint16_t x, uint16_t s);
uint32_t rotl_u32_impl(uint32_t x, uint32_t s);
uint64_t rotl_u64_impl(uint64_t x, uint64_t s);

uint8_t rotr_u8_impl(uint8_t x, uint8_t s);
uint16_t rotr_u16_impl(uint16_t x, uint16_t s);
uint32_t rotr_u32_impl(uint32_t x, uint32_t s);
uint64_t rotr_u64_impl(uint64_t x, uint64_t s);

uint16_t byteswap_u16_impl(uint16_t x);
uint32_t byteswap_u32_impl(uint32_t x);
uint64_t byteswap_u64_impl(uint64_t x);
}

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
                                  const compiler::ir::ProxyCallOperation* call,
                                  MLIRLoweringProvider::ValueFrame& frame) {
	auto inputArg = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	auto result = builder->create<MLIROp>(builder->getUnknownLoc(), inputArg.getType(), inputArg);
	frame.setValue(call->getIdentifier(), result);
	return true;
}

/// Generic helper for binary bit operations
template <typename MLIROp>
bool replaceWithBinaryBitIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                   const compiler::ir::ProxyCallOperation* call,
                                   MLIRLoweringProvider::ValueFrame& frame) {
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
	auto result = builder->create<::mlir::LLVM::CountLeadingZerosOp>(builder->getUnknownLoc(), inputArg.getType(),
	                                                                 inputArg, isZeroUndefAttr);
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
	auto result = builder->create<::mlir::LLVM::CountTrailingZerosOp>(builder->getUnknownLoc(), inputArg.getType(),
	                                                                  inputArg, isZeroUndefAttr);
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
	manager.addIntrinsic(reinterpret_cast<void*>(countl_zero_u8_impl), replaceWithCtlzIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(countl_zero_u16_impl), replaceWithCtlzIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(countl_zero_u32_impl), replaceWithCtlzIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(countl_zero_u64_impl), replaceWithCtlzIntrinsic);

	// ========================================================================
	// Count trailing zeros (cttz)
	// ========================================================================
	manager.addIntrinsic(reinterpret_cast<void*>(countr_zero_u8_impl), replaceWithCttzIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(countr_zero_u16_impl), replaceWithCttzIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(countr_zero_u32_impl), replaceWithCttzIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(countr_zero_u64_impl), replaceWithCttzIntrinsic);

	// ========================================================================
	// Population count (ctpop)
	// ========================================================================
	manager.addIntrinsic(reinterpret_cast<void*>(popcount_u8_impl),
	                     replaceWithUnaryBitIntrinsic<::mlir::LLVM::CtPopOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(popcount_u16_impl),
	                     replaceWithUnaryBitIntrinsic<::mlir::LLVM::CtPopOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(popcount_u32_impl),
	                     replaceWithUnaryBitIntrinsic<::mlir::LLVM::CtPopOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(popcount_u64_impl),
	                     replaceWithUnaryBitIntrinsic<::mlir::LLVM::CtPopOp>);

	// ========================================================================
	// Rotate left (rotl) - using funnel shift left
	// ========================================================================
	manager.addIntrinsic(reinterpret_cast<void*>(rotl_u8_impl), replaceWithRotlIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(rotl_u16_impl), replaceWithRotlIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(rotl_u32_impl), replaceWithRotlIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(rotl_u64_impl), replaceWithRotlIntrinsic);

	// ========================================================================
	// Rotate right (rotr) - using funnel shift right
	// ========================================================================
	manager.addIntrinsic(reinterpret_cast<void*>(rotr_u8_impl), replaceWithRotrIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(rotr_u16_impl), replaceWithRotrIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(rotr_u32_impl), replaceWithRotrIntrinsic);
	manager.addIntrinsic(reinterpret_cast<void*>(rotr_u64_impl), replaceWithRotrIntrinsic);

	// ========================================================================
	// Byte swap (bswap)
	// ========================================================================
	manager.addIntrinsic(reinterpret_cast<void*>(byteswap_u16_impl),
	                     replaceWithUnaryBitIntrinsic<::mlir::LLVM::ByteSwapOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(byteswap_u32_impl),
	                     replaceWithUnaryBitIntrinsic<::mlir::LLVM::ByteSwapOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(byteswap_u64_impl),
	                     replaceWithUnaryBitIntrinsic<::mlir::LLVM::ByteSwapOp>);

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
