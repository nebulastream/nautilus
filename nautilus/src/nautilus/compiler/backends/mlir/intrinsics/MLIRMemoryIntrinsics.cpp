
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRMemoryIntrinsics.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include <cstring>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>

namespace nautilus::compiler::mlir {

// Compiler intrinsic for memory operations
class MLIRMemoryIntrinsicPlugin : public MLIRIntrinsicPlugin {
public:
	void registerIntrinsics(MLIRIntrinsicManager& manager) override;
	~MLIRMemoryIntrinsicPlugin() override = default;
};

// ============================================================================
// Helper Functions for Memory Intrinsic Registration
// ============================================================================

/// Helper for memcpy intrinsic (3 arguments: dest, src, count)
bool replaceWithMemcpyIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                const compiler::ir::ProxyCallOperation* call, MLIRLoweringProvider::ValueFrame& frame) {
	auto dest = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	auto src = frame.getValue(call->getInputArguments().at(1)->getIdentifier());
	auto count = frame.getValue(call->getInputArguments().at(2)->getIdentifier());

	// Create memcpy intrinsic: llvm.memcpy(dest, src, len, isVolatile)
	auto isVolatile = builder->create<::mlir::LLVM::ConstantOp>(builder->getUnknownLoc(), builder->getI1Type(),
	                                                            builder->getBoolAttr(false));

	builder->create<::mlir::LLVM::MemcpyOp>(builder->getUnknownLoc(), dest, src, count, isVolatile);

	// memcpy returns the destination pointer
	frame.setValue(call->getIdentifier(), dest);
	return true;
}

/// Helper for memmove intrinsic (3 arguments: dest, src, count)
bool replaceWithMemmoveIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                 const compiler::ir::ProxyCallOperation* call,
                                 MLIRLoweringProvider::ValueFrame& frame) {
	auto dest = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	auto src = frame.getValue(call->getInputArguments().at(1)->getIdentifier());
	auto count = frame.getValue(call->getInputArguments().at(2)->getIdentifier());

	// Create memmove intrinsic: llvm.memmove(dest, src, len, isVolatile)
	auto isVolatile = builder->create<::mlir::LLVM::ConstantOp>(builder->getUnknownLoc(), builder->getI1Type(),
	                                                            builder->getBoolAttr(false));

	builder->create<::mlir::LLVM::MemmoveOp>(builder->getUnknownLoc(), dest, src, count, isVolatile);

	// memmove returns the destination pointer
	frame.setValue(call->getIdentifier(), dest);
	return true;
}

/// Helper for memset intrinsic (3 arguments: dest, value, count)
bool replaceWithMemsetIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                const compiler::ir::ProxyCallOperation* call, MLIRLoweringProvider::ValueFrame& frame) {
	auto dest = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	auto value = frame.getValue(call->getInputArguments().at(1)->getIdentifier());
	auto count = frame.getValue(call->getInputArguments().at(2)->getIdentifier());

	// LLVM memset intrinsic requires the value to be i8, so truncate from i32 to i8
	auto i8Type = builder->getIntegerType(8);
	auto truncatedValue = builder->create<::mlir::LLVM::TruncOp>(builder->getUnknownLoc(), i8Type, value);

	// Create memset intrinsic: llvm.memset(dest, val, len, isVolatile)
	auto isVolatile = builder->create<::mlir::LLVM::ConstantOp>(builder->getUnknownLoc(), builder->getI1Type(),
	                                                            builder->getBoolAttr(false));

	builder->create<::mlir::LLVM::MemsetOp>(builder->getUnknownLoc(), dest, truncatedValue, count, isVolatile);

	// memset returns the destination pointer
	frame.setValue(call->getIdentifier(), dest);
	return true;
}

// ============================================================================
// Intrinsic Registration
// ============================================================================

void MLIRMemoryIntrinsicPlugin::registerIntrinsics(MLIRIntrinsicManager& manager) {
	// Register memcpy
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<void* (*) (void*, const void*, size_t)>(&std::memcpy)),
	                     replaceWithMemcpyIntrinsic);

	// Register memmove
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<void* (*) (void*, const void*, size_t)>(&std::memmove)),
	                     replaceWithMemmoveIntrinsic);

	// Register memset
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<void* (*) (void*, int, size_t)>(&std::memset)),
	                     replaceWithMemsetIntrinsic);
}

void RegisterMLIRMemoryIntrinsicPlugin() {
	MLIRIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<MLIRMemoryIntrinsicPlugin>());
}

} // namespace nautilus::compiler::mlir
