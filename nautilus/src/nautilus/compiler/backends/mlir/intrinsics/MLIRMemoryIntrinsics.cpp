
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRMemoryIntrinsics.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include <cstring>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>
#include <mlir/IR/BuiltinOps.h>

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
bool replaceWithMemcpyIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::CallOperation* call,
                                MLIRLoweringProvider::ValueFrame& frame) {
	auto dest = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto src = frame.getValue(call->getInputArguments()[1]->getIdentifier());
	auto count = frame.getValue(call->getInputArguments()[2]->getIdentifier());

	// Create memcpy intrinsic: llvm.memcpy(dest, src, len, isVolatile)
	auto isVolatile = ::mlir::LLVM::ConstantOp::create(*builder, builder->getUnknownLoc(), builder->getI1Type(),
	                                                   builder->getBoolAttr(false));

	::mlir::LLVM::MemcpyOp::create(*builder, builder->getUnknownLoc(), dest, src, count, isVolatile);

	// memcpy returns the destination pointer
	frame.setValue(call->getIdentifier(), dest);
	return true;
}

/// Helper for memmove intrinsic (3 arguments: dest, src, count)
bool replaceWithMemmoveIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::CallOperation* call,
                                 MLIRLoweringProvider::ValueFrame& frame) {
	auto dest = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto src = frame.getValue(call->getInputArguments()[1]->getIdentifier());
	auto count = frame.getValue(call->getInputArguments()[2]->getIdentifier());

	// Create memmove intrinsic: llvm.memmove(dest, src, len, isVolatile)
	auto isVolatile = ::mlir::LLVM::ConstantOp::create(*builder, builder->getUnknownLoc(), builder->getI1Type(),
	                                                   builder->getBoolAttr(false));

	::mlir::LLVM::MemmoveOp::create(*builder, builder->getUnknownLoc(), dest, src, count, isVolatile);

	// memmove returns the destination pointer
	frame.setValue(call->getIdentifier(), dest);
	return true;
}

/// Helper for memset intrinsic (3 arguments: dest, value, count)
bool replaceWithMemsetIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::CallOperation* call,
                                MLIRLoweringProvider::ValueFrame& frame) {
	auto dest = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto value = frame.getValue(call->getInputArguments()[1]->getIdentifier());
	auto count = frame.getValue(call->getInputArguments()[2]->getIdentifier());

	// LLVM memset intrinsic requires the value to be i8, so truncate from i32 to i8
	auto i8Type = builder->getIntegerType(8);
	auto truncatedValue = ::mlir::LLVM::TruncOp::create(*builder, builder->getUnknownLoc(), i8Type, value);

	// Create memset intrinsic: llvm.memset(dest, val, len, isVolatile)
	auto isVolatile = ::mlir::LLVM::ConstantOp::create(*builder, builder->getUnknownLoc(), builder->getI1Type(),
	                                                   builder->getBoolAttr(false));

	::mlir::LLVM::MemsetOp::create(*builder, builder->getUnknownLoc(), dest, truncatedValue, count, isVolatile);

	// memset returns the destination pointer
	frame.setValue(call->getIdentifier(), dest);
	return true;
}

/// Helper for memcmp (3 arguments: s1, s2, count): a call to the C library's `memcmp` by name.
/// There is no LLVM memcmp intrinsic, but LLVM knows the library function: it expands a compare
/// of a constant length, or an equality test against zero (via bcmp), into loads and compares.
/// A call through the runtime address (the default lowering) would hide the function from it.
/// The JIT resolves the symbol from the process.
bool replaceWithMemcmpCall(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::CallOperation* call,
                           MLIRLoweringProvider::ValueFrame& frame) {
	auto loc = builder->getUnknownLoc();
	auto* block = builder->getInsertionBlock();
	auto module = block->getParentOp()->getParentOfType<::mlir::ModuleOp>();
	auto ptrType = ::mlir::LLVM::LLVMPointerType::get(builder->getContext());
	auto i32 = builder->getI32Type();
	auto i64 = builder->getI64Type();
	auto fnType = ::mlir::LLVM::LLVMFunctionType::get(i32, {ptrType, ptrType, i64});
	if (!module.lookupSymbol<::mlir::LLVM::LLVMFuncOp>("memcmp")) {
		::mlir::OpBuilder::InsertionGuard guard(*builder);
		builder->setInsertionPointToStart(module.getBody());
		::mlir::LLVM::LLVMFuncOp::create(*builder, loc, "memcmp", fnType, ::mlir::LLVM::Linkage::External);
	}
	auto s1 = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto s2 = frame.getValue(call->getInputArguments()[1]->getIdentifier());
	auto count = frame.getValue(call->getInputArguments()[2]->getIdentifier());
	auto result = ::mlir::LLVM::CallOp::create(*builder, loc, fnType, "memcmp", ::mlir::ValueRange {s1, s2, count});
	frame.setValue(call->getIdentifier(), result.getResult());
	return true;
}

// ============================================================================
// Intrinsic Registration
// ============================================================================

void MLIRMemoryIntrinsicPlugin::registerIntrinsics(MLIRIntrinsicManager& manager) {
	// Register memcpy
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<void* (*) (void*, const void*, size_t)>(&std::memcpy)),
	                     replaceWithMemcpyIntrinsic, "memcpy");

	// Register memmove
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<void* (*) (void*, const void*, size_t)>(&std::memmove)),
	                     replaceWithMemmoveIntrinsic, "memmove");

	// Register memset
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<void* (*) (void*, int, size_t)>(&std::memset)),
	                     replaceWithMemsetIntrinsic, "memset");

	// Register memcmp (a named library call LLVM can expand)
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<int (*)(const void*, const void*, size_t)>(&std::memcmp)),
	                     replaceWithMemcmpCall, "memcmp");
}

void RegisterMLIRMemoryIntrinsicPlugin() {
	MLIRIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<MLIRMemoryIntrinsicPlugin>());
}

} // namespace nautilus::compiler::mlir
