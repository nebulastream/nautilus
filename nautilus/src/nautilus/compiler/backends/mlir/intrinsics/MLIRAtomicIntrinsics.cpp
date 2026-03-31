
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRAtomicIntrinsics.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>

// Forward-declare the runtime functions so we can take their addresses.
// These are defined in nautilus/src/nautilus/api/std/atomic.cpp as static functions,
// but the intrinsic system matches by function pointer address at registration time.
// We re-declare them here with extern "C"-style linkage matching the actual definitions.
// Since the actual functions are static in atomic.cpp, we use a different approach:
// we include the header and reference the invoke targets indirectly.
// Instead, we define non-static accessor functions in atomic.cpp and declare them here.

namespace nautilus::compiler::mlir {

// ============================================================================
// Atomic Load Intrinsic Replacement
// ============================================================================

template <int BitWidth>
bool replaceWithAtomicLoadIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                    const compiler::ir::ProxyCallOperation* call,
                                    MLIRLoweringProvider::ValueFrame& frame) {
	auto address = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	auto resultType = builder->getIntegerType(BitWidth);
	auto loc = builder->getUnknownLoc();
	// Alignment must be at least the natural width for atomic access
	constexpr unsigned alignment = BitWidth / 8;
	auto loadOp = builder->create<::mlir::LLVM::LoadOp>(loc, resultType, address,
	                                                     /*alignment=*/alignment, /*isVolatile=*/false,
	                                                     /*isNonTemporal=*/false, /*invariant=*/false,
	                                                     /*invariantGroup=*/false,
	                                                     ::mlir::LLVM::AtomicOrdering::seq_cst);
	frame.setValue(call->getIdentifier(), loadOp);
	return true;
}

// ============================================================================
// Atomic Store Intrinsic Replacement
// ============================================================================

template <int BitWidth>
bool replaceWithAtomicStoreIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                     const compiler::ir::ProxyCallOperation* call,
                                     MLIRLoweringProvider::ValueFrame& frame) {
	auto address = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	auto value = frame.getValue(call->getInputArguments().at(1)->getIdentifier());
	auto loc = builder->getUnknownLoc();
	// Alignment must be at least the natural width for atomic access
	constexpr unsigned alignment = BitWidth / 8;
	builder->create<::mlir::LLVM::StoreOp>(loc, value, address,
	                                        /*alignment=*/alignment, /*isVolatile=*/false,
	                                        /*isNonTemporal=*/false, /*isInvariantGroup=*/false,
	                                        ::mlir::LLVM::AtomicOrdering::seq_cst);
	return true;
}

// ============================================================================
// Atomic RMW (fetch_add, fetch_sub, exchange) Intrinsic Replacement
// ============================================================================

template <::mlir::LLVM::AtomicBinOp BinOp>
bool replaceWithAtomicRMWIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                   const compiler::ir::ProxyCallOperation* call,
                                   MLIRLoweringProvider::ValueFrame& frame) {
	auto address = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	auto value = frame.getValue(call->getInputArguments().at(1)->getIdentifier());
	auto rmwOp = builder->create<::mlir::LLVM::AtomicRMWOp>(builder->getUnknownLoc(), BinOp, address, value,
	                                                         ::mlir::LLVM::AtomicOrdering::seq_cst);
	frame.setValue(call->getIdentifier(), rmwOp);
	return true;
}

// ============================================================================
// Atomic Compare-and-Exchange Intrinsic Replacement
// ============================================================================

bool replaceWithAtomicCmpXchgIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                       const compiler::ir::ProxyCallOperation* call,
                                       MLIRLoweringProvider::ValueFrame& frame) {
	auto ptrArg = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	auto expectedPtr = frame.getValue(call->getInputArguments().at(1)->getIdentifier());
	auto desired = frame.getValue(call->getInputArguments().at(2)->getIdentifier());
	auto loc = builder->getUnknownLoc();

	// Load the expected value from the expected pointer
	auto expectedVal = builder->create<::mlir::LLVM::LoadOp>(loc, desired.getType(), expectedPtr);

	// Perform atomic cmpxchg: returns { T, i1 }
	auto cmpxchgOp = builder->create<::mlir::LLVM::AtomicCmpXchgOp>(loc, ptrArg, expectedVal, desired,
	                                                                  ::mlir::LLVM::AtomicOrdering::seq_cst,
	                                                                  ::mlir::LLVM::AtomicOrdering::seq_cst);

	// Extract the success flag (index 1)
	auto successFlag = builder->create<::mlir::LLVM::ExtractValueOp>(loc, cmpxchgOp, 1);

	// Extract the old value (index 0) and store it back to the expected pointer on failure.
	// CAS semantics: on failure, *expected is updated with the actual value found.
	auto oldVal = builder->create<::mlir::LLVM::ExtractValueOp>(loc, cmpxchgOp, 0);
	builder->create<::mlir::LLVM::StoreOp>(loc, oldVal, expectedPtr);

	frame.setValue(call->getIdentifier(), successFlag);
	return true;
}

// ============================================================================
// Intrinsic Plugin
// ============================================================================

class MLIRAtomicIntrinsicPlugin : public MLIRIntrinsicPlugin {
public:
	void registerIntrinsics(MLIRIntrinsicManager& manager) override;
	~MLIRAtomicIntrinsicPlugin() override = default;
};

} // namespace nautilus::compiler::mlir

// We need the actual function pointer addresses from the runtime.
// The runtime functions are defined as static in atomic.cpp, so we provide
// non-static accessor functions to get their addresses.
namespace nautilus {
// Declared in atomic.cpp — returns function pointers for intrinsic registration
void* getAtomicLoadI32Ptr();
void* getAtomicLoadI64Ptr();
void* getAtomicLoadU32Ptr();
void* getAtomicLoadU64Ptr();
void* getAtomicStoreI32Ptr();
void* getAtomicStoreI64Ptr();
void* getAtomicStoreU32Ptr();
void* getAtomicStoreU64Ptr();
void* getAtomicExchangeI32Ptr();
void* getAtomicExchangeI64Ptr();
void* getAtomicExchangeU32Ptr();
void* getAtomicExchangeU64Ptr();
void* getAtomicCasI32Ptr();
void* getAtomicCasI64Ptr();
void* getAtomicCasU32Ptr();
void* getAtomicCasU64Ptr();
void* getAtomicFetchAddI32Ptr();
void* getAtomicFetchAddI64Ptr();
void* getAtomicFetchAddU32Ptr();
void* getAtomicFetchAddU64Ptr();
void* getAtomicFetchSubI32Ptr();
void* getAtomicFetchSubI64Ptr();
void* getAtomicFetchSubU32Ptr();
void* getAtomicFetchSubU64Ptr();
} // namespace nautilus

namespace nautilus::compiler::mlir {

void MLIRAtomicIntrinsicPlugin::registerIntrinsics(MLIRIntrinsicManager& manager) {
	// Atomic Load
	manager.addIntrinsic(nautilus::getAtomicLoadI32Ptr(), replaceWithAtomicLoadIntrinsic<32>);
	manager.addIntrinsic(nautilus::getAtomicLoadI64Ptr(), replaceWithAtomicLoadIntrinsic<64>);
	manager.addIntrinsic(nautilus::getAtomicLoadU32Ptr(), replaceWithAtomicLoadIntrinsic<32>);
	manager.addIntrinsic(nautilus::getAtomicLoadU64Ptr(), replaceWithAtomicLoadIntrinsic<64>);

	// Atomic Store
	manager.addIntrinsic(nautilus::getAtomicStoreI32Ptr(), replaceWithAtomicStoreIntrinsic<32>);
	manager.addIntrinsic(nautilus::getAtomicStoreI64Ptr(), replaceWithAtomicStoreIntrinsic<64>);
	manager.addIntrinsic(nautilus::getAtomicStoreU32Ptr(), replaceWithAtomicStoreIntrinsic<32>);
	manager.addIntrinsic(nautilus::getAtomicStoreU64Ptr(), replaceWithAtomicStoreIntrinsic<64>);

	// Atomic Exchange (uses AtomicRMW with xchg)
	manager.addIntrinsic(nautilus::getAtomicExchangeI32Ptr(),
	                     replaceWithAtomicRMWIntrinsic<::mlir::LLVM::AtomicBinOp::xchg>);
	manager.addIntrinsic(nautilus::getAtomicExchangeI64Ptr(),
	                     replaceWithAtomicRMWIntrinsic<::mlir::LLVM::AtomicBinOp::xchg>);
	manager.addIntrinsic(nautilus::getAtomicExchangeU32Ptr(),
	                     replaceWithAtomicRMWIntrinsic<::mlir::LLVM::AtomicBinOp::xchg>);
	manager.addIntrinsic(nautilus::getAtomicExchangeU64Ptr(),
	                     replaceWithAtomicRMWIntrinsic<::mlir::LLVM::AtomicBinOp::xchg>);

	// Atomic CAS
	manager.addIntrinsic(nautilus::getAtomicCasI32Ptr(), replaceWithAtomicCmpXchgIntrinsic);
	manager.addIntrinsic(nautilus::getAtomicCasI64Ptr(), replaceWithAtomicCmpXchgIntrinsic);
	manager.addIntrinsic(nautilus::getAtomicCasU32Ptr(), replaceWithAtomicCmpXchgIntrinsic);
	manager.addIntrinsic(nautilus::getAtomicCasU64Ptr(), replaceWithAtomicCmpXchgIntrinsic);

	// Atomic Fetch-Add (uses AtomicRMW with add)
	manager.addIntrinsic(nautilus::getAtomicFetchAddI32Ptr(),
	                     replaceWithAtomicRMWIntrinsic<::mlir::LLVM::AtomicBinOp::add>);
	manager.addIntrinsic(nautilus::getAtomicFetchAddI64Ptr(),
	                     replaceWithAtomicRMWIntrinsic<::mlir::LLVM::AtomicBinOp::add>);
	manager.addIntrinsic(nautilus::getAtomicFetchAddU32Ptr(),
	                     replaceWithAtomicRMWIntrinsic<::mlir::LLVM::AtomicBinOp::add>);
	manager.addIntrinsic(nautilus::getAtomicFetchAddU64Ptr(),
	                     replaceWithAtomicRMWIntrinsic<::mlir::LLVM::AtomicBinOp::add>);

	// Atomic Fetch-Sub (uses AtomicRMW with sub)
	manager.addIntrinsic(nautilus::getAtomicFetchSubI32Ptr(),
	                     replaceWithAtomicRMWIntrinsic<::mlir::LLVM::AtomicBinOp::sub>);
	manager.addIntrinsic(nautilus::getAtomicFetchSubI64Ptr(),
	                     replaceWithAtomicRMWIntrinsic<::mlir::LLVM::AtomicBinOp::sub>);
	manager.addIntrinsic(nautilus::getAtomicFetchSubU32Ptr(),
	                     replaceWithAtomicRMWIntrinsic<::mlir::LLVM::AtomicBinOp::sub>);
	manager.addIntrinsic(nautilus::getAtomicFetchSubU64Ptr(),
	                     replaceWithAtomicRMWIntrinsic<::mlir::LLVM::AtomicBinOp::sub>);
}

void RegisterMLIRAtomicIntrinsicPlugin() {
	MLIRIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<MLIRAtomicIntrinsicPlugin>());
}

} // namespace nautilus::compiler::mlir
