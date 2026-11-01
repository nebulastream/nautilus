
#include "MLIRBitIntrinsics.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include <bit>
#include <cstdint>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>

// Declare extern "C" wrapper functions from bit.cpp
extern "C" {
uint8_t countl_zero_u8_impl(uint8_t) noexcept;
uint16_t countl_zero_u16_impl(uint16_t) noexcept;
uint32_t countl_zero_u32_impl(uint32_t) noexcept;
uint64_t countl_zero_u64_impl(uint64_t) noexcept;

uint8_t countr_zero_u8_impl(uint8_t) noexcept;
uint16_t countr_zero_u16_impl(uint16_t) noexcept;
uint32_t countr_zero_u32_impl(uint32_t) noexcept;
uint64_t countr_zero_u64_impl(uint64_t) noexcept;

uint8_t popcount_u8_impl(uint8_t) noexcept;
uint16_t popcount_u16_impl(uint16_t) noexcept;
uint32_t popcount_u32_impl(uint32_t) noexcept;
uint64_t popcount_u64_impl(uint64_t) noexcept;

uint8_t rotl_u8_impl(uint8_t, uint8_t) noexcept;
uint16_t rotl_u16_impl(uint16_t, uint16_t) noexcept;
uint32_t rotl_u32_impl(uint32_t, uint32_t) noexcept;
uint64_t rotl_u64_impl(uint64_t, uint64_t) noexcept;

uint8_t rotr_u8_impl(uint8_t, uint8_t) noexcept;
uint16_t rotr_u16_impl(uint16_t, uint16_t) noexcept;
uint32_t rotr_u32_impl(uint32_t, uint32_t) noexcept;
uint64_t rotr_u64_impl(uint64_t, uint64_t) noexcept;

uint16_t byteswap_u16_impl(uint16_t) noexcept;
uint32_t byteswap_u32_impl(uint32_t) noexcept;
uint64_t byteswap_u64_impl(uint64_t) noexcept;
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
bool replaceWithUnaryBitIntrinsic(NirIntrinsicCall& call) {
	[[maybe_unused]] auto& builder = call.builder;
	auto inputArg = call.args[0];
	auto result = MLIROp::create(*builder, builder->getUnknownLoc(), inputArg.getType(), inputArg);
	call.result = result;
	return true;
}

/// Generic helper for binary bit operations
template <typename MLIROp>
bool replaceWithBinaryBitIntrinsic(NirIntrinsicCall& call) {
	[[maybe_unused]] auto& builder = call.builder;
	auto arg1 = call.args[0];
	auto arg2 = call.args[1];
	auto result = MLIROp::create(*builder, builder->getUnknownLoc(), arg1.getType(), arg1, arg2);
	call.result = result;
	return true;
}

/// Helper for CTLZ intrinsic (count leading zeros)
/// LLVM CTLZ takes a second boolean argument indicating if zero is undefined
bool replaceWithCtlzIntrinsic(NirIntrinsicCall& call) {
	[[maybe_unused]] auto& builder = call.builder;
	auto inputArg = call.args[0];
	// Second argument: is_zero_undef = false (zero is defined)
	auto isZeroUndefAttr = builder->getBoolAttr(false);
	auto result = ::mlir::LLVM::CountLeadingZerosOp::create(*builder, builder->getUnknownLoc(), inputArg.getType(),
	                                                        inputArg, isZeroUndefAttr);
	call.result = result;
	return true;
}

/// Helper for CTTZ intrinsic (count trailing zeros)
/// LLVM CTTZ takes a second boolean argument indicating if zero is undefined
bool replaceWithCttzIntrinsic(NirIntrinsicCall& call) {
	[[maybe_unused]] auto& builder = call.builder;
	auto inputArg = call.args[0];
	// Second argument: is_zero_undef = false (zero is defined)
	auto isZeroUndefAttr = builder->getBoolAttr(false);
	auto result = ::mlir::LLVM::CountTrailingZerosOp::create(*builder, builder->getUnknownLoc(), inputArg.getType(),
	                                                         inputArg, isZeroUndefAttr);
	call.result = result;
	return true;
}

/// Helper for rotate left using funnel shift
/// rotl(x, s) = fshl(x, x, s)
bool replaceWithRotlIntrinsic(NirIntrinsicCall& call) {
	[[maybe_unused]] auto& builder = call.builder;
	auto x = call.args[0];
	auto s = call.args[1];
	// fshl(x, x, s) performs a rotate left
	auto result = ::mlir::LLVM::FshlOp::create(*builder, builder->getUnknownLoc(), x.getType(), x, x, s);
	call.result = result;
	return true;
}

/// Helper for rotate right using funnel shift
/// rotr(x, s) = fshr(x, x, s)
bool replaceWithRotrIntrinsic(NirIntrinsicCall& call) {
	[[maybe_unused]] auto& builder = call.builder;
	auto x = call.args[0];
	auto s = call.args[1];
	// fshr(x, x, s) performs a rotate right
	auto result = ::mlir::LLVM::FshrOp::create(*builder, builder->getUnknownLoc(), x.getType(), x, x, s);
	call.result = result;
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
