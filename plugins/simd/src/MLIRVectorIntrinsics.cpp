
#include "MLIRVectorIntrinsics.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "vector_impl.hpp"
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>
#include <mlir/Dialect/Math/IR/Math.h>
#include <mlir/IR/Types.h>

namespace nautilus::compiler::mlir {

// ============================================================================
// MLIR Vector Intrinsic Plugin
//
// Intercepts vector_*_impl function pointers and replaces them with native
// MLIR LLVM dialect vector operations during MLIR lowering. This enables
// LLVM's backend to generate native SIMD instructions (SSE, AVX, NEON, etc.).
//
// Strategy: We use LLVM dialect fixed-width vector types (e.g. vector<4xf32>)
// and LLVM dialect load/store for memory access. The data is passed through
// the tracing system as opaque pointers (ptr). In the intrinsic handler, we:
//   1. Load from ptr → LLVM vector type
//   2. Perform vector operation
//   3. Alloca + store result
//   4. Return pointer to alloca
// ============================================================================

class MLIRVectorIntrinsicPlugin : public MLIRIntrinsicPlugin {
public:
	void registerIntrinsics(MLIRIntrinsicManager& manager) override;
	~MLIRVectorIntrinsicPlugin() override = default;
};

// ============================================================================
// Helper: get MLIR fixed-width vector type
// ============================================================================

static ::mlir::VectorType getVecType(::mlir::Type elemTy, int64_t lanes) {
	return ::mlir::VectorType::get({lanes}, elemTy);
}

// ============================================================================
// Helper: load a vector from a pointer argument
// ============================================================================

static ::mlir::Value loadVecFromPtr(std::unique_ptr<::mlir::OpBuilder>& builder, ::mlir::Value ptr,
                                    ::mlir::VectorType vecTy) {
	return builder->create<::mlir::LLVM::LoadOp>(builder->getUnknownLoc(), vecTy, ptr);
}

// ============================================================================
// Helper: alloca + store a vector result, return pointer
// ============================================================================

static ::mlir::Value storeVecToAlloca(std::unique_ptr<::mlir::OpBuilder>& builder, ::mlir::Value vec,
                                      ::mlir::VectorType vecTy) {
	auto ptrTy = ::mlir::LLVM::LLVMPointerType::get(builder->getContext());
	auto i64Ty = builder->getI64Type();
	auto elemTy = vecTy.getElementType();
	auto numElements = vecTy.getNumElements();
	auto byteSize = numElements * (elemTy.getIntOrFloatBitWidth() / 8);
	auto sizeVal = builder->create<::mlir::LLVM::ConstantOp>(builder->getUnknownLoc(), i64Ty,
	                                                         builder->getI64IntegerAttr(byteSize));
	auto alloca =
	    builder->create<::mlir::LLVM::AllocaOp>(builder->getUnknownLoc(), ptrTy, builder->getI8Type(), sizeVal, 0u);
	builder->create<::mlir::LLVM::StoreOp>(builder->getUnknownLoc(), vec, alloca);
	return alloca;
}

// ============================================================================
// Intrinsic handler templates
// ============================================================================

/// Binary vector operation: load two vectors, apply op, store result
template <typename MLIROp, typename ElemT, int64_t N>
bool vectorBinaryIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                           MLIRLoweringProvider::ValueFrame& frame) {
	auto ptrA = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto ptrB = frame.getValue(call->getInputArguments()[1]->getIdentifier());

	::mlir::Type elemTy;
	if constexpr (std::is_same_v<ElemT, float>) {
		elemTy = builder->getF32Type();
	} else if constexpr (std::is_same_v<ElemT, double>) {
		elemTy = builder->getF64Type();
	} else if constexpr (std::is_same_v<ElemT, int32_t>) {
		elemTy = builder->getI32Type();
	} else {
		elemTy = builder->getI64Type();
	}
	auto vecTy = getVecType(elemTy, N);

	auto a = loadVecFromPtr(builder, ptrA, vecTy);
	auto b = loadVecFromPtr(builder, ptrB, vecTy);
	auto result = builder->create<MLIROp>(builder->getUnknownLoc(), a, b);
	auto resultPtr = storeVecToAlloca(builder, result, vecTy);
	frame.setValue(call->getIdentifier(), resultPtr);
	return true;
}

/// Unary vector operation
template <typename MLIROp, typename ElemT, int64_t N>
bool vectorUnaryIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                          MLIRLoweringProvider::ValueFrame& frame) {
	auto ptrA = frame.getValue(call->getInputArguments()[0]->getIdentifier());

	::mlir::Type elemTy;
	if constexpr (std::is_same_v<ElemT, float>) {
		elemTy = builder->getF32Type();
	} else if constexpr (std::is_same_v<ElemT, double>) {
		elemTy = builder->getF64Type();
	} else if constexpr (std::is_same_v<ElemT, int32_t>) {
		elemTy = builder->getI32Type();
	} else {
		elemTy = builder->getI64Type();
	}
	auto vecTy = getVecType(elemTy, N);

	auto a = loadVecFromPtr(builder, ptrA, vecTy);
	auto result = builder->create<MLIROp>(builder->getUnknownLoc(), a);
	auto resultPtr = storeVecToAlloca(builder, result, vecTy);
	frame.setValue(call->getIdentifier(), resultPtr);
	return true;
}

/// Ternary vector operation (fma, blend)
template <typename MLIROp, typename ElemT, int64_t N>
bool vectorTernaryIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                            MLIRLoweringProvider::ValueFrame& frame) {
	auto ptrA = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto ptrB = frame.getValue(call->getInputArguments()[1]->getIdentifier());
	auto ptrC = frame.getValue(call->getInputArguments()[2]->getIdentifier());

	::mlir::Type elemTy;
	if constexpr (std::is_same_v<ElemT, float>) {
		elemTy = builder->getF32Type();
	} else if constexpr (std::is_same_v<ElemT, double>) {
		elemTy = builder->getF64Type();
	} else if constexpr (std::is_same_v<ElemT, int32_t>) {
		elemTy = builder->getI32Type();
	} else {
		elemTy = builder->getI64Type();
	}
	auto vecTy = getVecType(elemTy, N);

	auto a = loadVecFromPtr(builder, ptrA, vecTy);
	auto b = loadVecFromPtr(builder, ptrB, vecTy);
	auto c = loadVecFromPtr(builder, ptrC, vecTy);
	auto result = builder->create<MLIROp>(builder->getUnknownLoc(), a, b, c);
	auto resultPtr = storeVecToAlloca(builder, result, vecTy);
	frame.setValue(call->getIdentifier(), resultPtr);
	return true;
}

/// Load: read from raw T* pointer into vector, return vector pointer
template <typename ElemT, int64_t N>
bool vectorLoadIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                         MLIRLoweringProvider::ValueFrame& frame) {
	auto srcPtr = frame.getValue(call->getInputArguments()[0]->getIdentifier());

	::mlir::Type elemTy;
	if constexpr (std::is_same_v<ElemT, float>) {
		elemTy = builder->getF32Type();
	} else if constexpr (std::is_same_v<ElemT, double>) {
		elemTy = builder->getF64Type();
	} else if constexpr (std::is_same_v<ElemT, int32_t>) {
		elemTy = builder->getI32Type();
	} else {
		elemTy = builder->getI64Type();
	}
	auto vecTy = getVecType(elemTy, N);

	// Load vector directly from the source pointer (treat as vector pointer).
	// Use element alignment (not natural vector alignment) so LLVM emits an
	// unaligned vector load (e.g. vmovdqu instead of vmovdqa on x86). User
	// data from std::vector::data() or new[] is typically only aligned to
	// alignof(T), not to the full SIMD register width.
	auto vec = builder->create<::mlir::LLVM::LoadOp>(builder->getUnknownLoc(), vecTy, srcPtr, sizeof(ElemT));
	auto resultPtr = storeVecToAlloca(builder, vec, vecTy);
	frame.setValue(call->getIdentifier(), resultPtr);
	return true;
}

/// Store: write vector data to raw T* pointer
template <typename ElemT, int64_t N>
bool vectorStoreIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                          MLIRLoweringProvider::ValueFrame& frame) {
	auto destPtr = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto vecPtr = frame.getValue(call->getInputArguments()[1]->getIdentifier());

	::mlir::Type elemTy;
	if constexpr (std::is_same_v<ElemT, float>) {
		elemTy = builder->getF32Type();
	} else if constexpr (std::is_same_v<ElemT, double>) {
		elemTy = builder->getF64Type();
	} else if constexpr (std::is_same_v<ElemT, int32_t>) {
		elemTy = builder->getI32Type();
	} else {
		elemTy = builder->getI64Type();
	}
	auto vecTy = getVecType(elemTy, N);

	auto vec = loadVecFromPtr(builder, vecPtr, vecTy);
	// Use element alignment for the store to user memory (same rationale as
	// the load intrinsic above — user buffers are not guaranteed to be
	// aligned to the full vector width).
	builder->create<::mlir::LLVM::StoreOp>(builder->getUnknownLoc(), vec, destPtr, sizeof(ElemT));
	// Store returns void, but we still need to set a value for the frame.
	// The store_impl returns void, so the ProxyCallOp result may not be used.
	// We pass destPtr as the output so the frame has something valid.
	frame.setValue(call->getIdentifier(), destPtr);
	return true;
}

/// Negate for float: 0 - x
template <typename ElemT, int64_t N>
bool vectorNegFloatIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                             MLIRLoweringProvider::ValueFrame& frame) {
	auto ptrA = frame.getValue(call->getInputArguments()[0]->getIdentifier());

	::mlir::Type elemTy;
	if constexpr (std::is_same_v<ElemT, float>) {
		elemTy = builder->getF32Type();
	} else {
		elemTy = builder->getF64Type();
	}
	auto vecTy = getVecType(elemTy, N);

	auto a = loadVecFromPtr(builder, ptrA, vecTy);
	auto result = builder->create<::mlir::LLVM::FNegOp>(builder->getUnknownLoc(), a);
	auto resultPtr = storeVecToAlloca(builder, result, vecTy);
	frame.setValue(call->getIdentifier(), resultPtr);
	return true;
}

/// Negate for int: 0 - x
template <typename ElemT, int64_t N>
bool vectorNegIntIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                           MLIRLoweringProvider::ValueFrame& frame) {
	auto ptrA = frame.getValue(call->getInputArguments()[0]->getIdentifier());

	::mlir::Type elemTy;
	if constexpr (std::is_same_v<ElemT, int32_t>) {
		elemTy = builder->getI32Type();
	} else {
		elemTy = builder->getI64Type();
	}
	auto vecTy = getVecType(elemTy, N);

	auto a = loadVecFromPtr(builder, ptrA, vecTy);
	// zero - a
	auto zero =
	    builder->create<::mlir::arith::ConstantOp>(builder->getUnknownLoc(), vecTy, builder->getZeroAttr(vecTy));
	auto result = builder->create<::mlir::LLVM::SubOp>(builder->getUnknownLoc(), zero, a);
	auto resultPtr = storeVecToAlloca(builder, result, vecTy);
	frame.setValue(call->getIdentifier(), resultPtr);
	return true;
}

/// Reduce add for float types: llvm.vector.reduce.fadd
template <typename ElemT, int64_t N>
bool vectorReduceAddFloatIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                   const compiler::ir::ProxyCallOperation* call,
                                   MLIRLoweringProvider::ValueFrame& frame) {
	auto ptrA = frame.getValue(call->getInputArguments()[0]->getIdentifier());

	::mlir::Type elemTy;
	if constexpr (std::is_same_v<ElemT, float>) {
		elemTy = builder->getF32Type();
	} else {
		elemTy = builder->getF64Type();
	}
	auto vecTy = getVecType(elemTy, N);

	auto a = loadVecFromPtr(builder, ptrA, vecTy);
	// Start accumulator at 0.0
	auto zero = builder->create<::mlir::arith::ConstantOp>(builder->getUnknownLoc(), elemTy,
	                                                       builder->getFloatAttr(elemTy, 0.0));
	auto result = builder->create<::mlir::LLVM::vector_reduce_fadd>(builder->getUnknownLoc(), elemTy, zero, a,
	                                                                ::mlir::LLVM::FastmathFlags::reassoc);
	frame.setValue(call->getIdentifier(), result);
	return true;
}

/// Reduce add for int types: llvm.vector.reduce.add
template <typename ElemT, int64_t N>
bool vectorReduceAddIntIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                 const compiler::ir::ProxyCallOperation* call,
                                 MLIRLoweringProvider::ValueFrame& frame) {
	auto ptrA = frame.getValue(call->getInputArguments()[0]->getIdentifier());

	::mlir::Type elemTy;
	if constexpr (std::is_same_v<ElemT, int32_t>) {
		elemTy = builder->getI32Type();
	} else {
		elemTy = builder->getI64Type();
	}
	auto vecTy = getVecType(elemTy, N);

	auto a = loadVecFromPtr(builder, ptrA, vecTy);
	auto result = builder->create<::mlir::LLVM::vector_reduce_add>(builder->getUnknownLoc(), elemTy, a);
	frame.setValue(call->getIdentifier(), result);
	return true;
}

/// Reduce min/max for float types
template <typename ReduceOp, typename ElemT, int64_t N>
bool vectorReduceFloatIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                const compiler::ir::ProxyCallOperation* call, MLIRLoweringProvider::ValueFrame& frame) {
	auto ptrA = frame.getValue(call->getInputArguments()[0]->getIdentifier());

	::mlir::Type elemTy;
	if constexpr (std::is_same_v<ElemT, float>) {
		elemTy = builder->getF32Type();
	} else {
		elemTy = builder->getF64Type();
	}
	auto vecTy = getVecType(elemTy, N);

	auto a = loadVecFromPtr(builder, ptrA, vecTy);
	auto result = builder->create<ReduceOp>(builder->getUnknownLoc(), elemTy, a, ::mlir::LLVM::FastmathFlags::nnan);
	frame.setValue(call->getIdentifier(), result);
	return true;
}

/// Reduce min/max for int types
template <typename ReduceOp, typename ElemT, int64_t N>
bool vectorReduceIntIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                              MLIRLoweringProvider::ValueFrame& frame) {
	auto ptrA = frame.getValue(call->getInputArguments()[0]->getIdentifier());

	::mlir::Type elemTy;
	if constexpr (std::is_same_v<ElemT, int32_t>) {
		elemTy = builder->getI32Type();
	} else {
		elemTy = builder->getI64Type();
	}
	auto vecTy = getVecType(elemTy, N);

	auto a = loadVecFromPtr(builder, ptrA, vecTy);
	auto result = builder->create<ReduceOp>(builder->getUnknownLoc(), elemTy, a);
	frame.setValue(call->getIdentifier(), result);
	return true;
}

/// Float comparison: FCmpOp → vector<NxI1> → sext to all-ones/zeros → bitcast to float
template <::mlir::LLVM::FCmpPredicate Pred, typename ElemT, int64_t N>
bool vectorFCmpIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                         MLIRLoweringProvider::ValueFrame& frame) {
	auto ptrA = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto ptrB = frame.getValue(call->getInputArguments()[1]->getIdentifier());

	::mlir::Type elemTy;
	if constexpr (std::is_same_v<ElemT, float>) {
		elemTy = builder->getF32Type();
	} else {
		elemTy = builder->getF64Type();
	}
	auto vecTy = getVecType(elemTy, N);

	auto a = loadVecFromPtr(builder, ptrA, vecTy);
	auto b = loadVecFromPtr(builder, ptrB, vecTy);

	// Compare → vector<NxI1>
	auto cmp = builder->create<::mlir::LLVM::FCmpOp>(builder->getUnknownLoc(), Pred, a, b);

	// Sign-extend i1 → iK (produces all-ones for true, all-zeros for false)
	auto intElemTy = builder->getIntegerType(elemTy.getIntOrFloatBitWidth());
	auto intVecTy = getVecType(intElemTy, N);
	auto extended = builder->create<::mlir::LLVM::SExtOp>(builder->getUnknownLoc(), intVecTy, cmp);

	// Bitcast int → float to match the expected return type
	auto result = builder->create<::mlir::LLVM::BitcastOp>(builder->getUnknownLoc(), vecTy, extended);

	auto resultPtr = storeVecToAlloca(builder, result, vecTy);
	frame.setValue(call->getIdentifier(), resultPtr);
	return true;
}

/// Integer comparison: ICmpOp → vector<NxI1> → sext to all-ones/zeros
template <::mlir::LLVM::ICmpPredicate Pred, typename ElemT, int64_t N>
bool vectorICmpIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                         MLIRLoweringProvider::ValueFrame& frame) {
	auto ptrA = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto ptrB = frame.getValue(call->getInputArguments()[1]->getIdentifier());

	::mlir::Type elemTy;
	if constexpr (std::is_same_v<ElemT, int32_t>) {
		elemTy = builder->getI32Type();
	} else {
		elemTy = builder->getI64Type();
	}
	auto vecTy = getVecType(elemTy, N);

	auto a = loadVecFromPtr(builder, ptrA, vecTy);
	auto b = loadVecFromPtr(builder, ptrB, vecTy);

	// Compare → vector<NxI1>
	auto cmp = builder->create<::mlir::LLVM::ICmpOp>(builder->getUnknownLoc(), Pred, a, b);

	// Sign-extend i1 → iK (produces all-ones for true, all-zeros for false)
	auto extended = builder->create<::mlir::LLVM::SExtOp>(builder->getUnknownLoc(), vecTy, cmp);

	auto resultPtr = storeVecToAlloca(builder, extended, vecTy);
	frame.setValue(call->getIdentifier(), resultPtr);
	return true;
}

/// Float bitwise: bitcast to int, apply op, bitcast back
template <typename MLIROp, typename ElemT, int64_t N>
bool vectorBitwiseFloatIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                 const compiler::ir::ProxyCallOperation* call,
                                 MLIRLoweringProvider::ValueFrame& frame) {
	auto ptrA = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto ptrB = frame.getValue(call->getInputArguments()[1]->getIdentifier());

	::mlir::Type elemTy;
	if constexpr (std::is_same_v<ElemT, float>) {
		elemTy = builder->getF32Type();
	} else {
		elemTy = builder->getF64Type();
	}
	auto vecTy = getVecType(elemTy, N);
	auto intElemTy = builder->getIntegerType(elemTy.getIntOrFloatBitWidth());
	auto intVecTy = getVecType(intElemTy, N);

	auto a = loadVecFromPtr(builder, ptrA, vecTy);
	auto b = loadVecFromPtr(builder, ptrB, vecTy);

	// Bitcast float → int
	auto ai = builder->create<::mlir::LLVM::BitcastOp>(builder->getUnknownLoc(), intVecTy, a);
	auto bi = builder->create<::mlir::LLVM::BitcastOp>(builder->getUnknownLoc(), intVecTy, b);

	// Apply bitwise op on integers
	auto ri = builder->create<MLIROp>(builder->getUnknownLoc(), ai, bi);

	// Bitcast int → float
	auto result = builder->create<::mlir::LLVM::BitcastOp>(builder->getUnknownLoc(), vecTy, ri);

	auto resultPtr = storeVecToAlloca(builder, result, vecTy);
	frame.setValue(call->getIdentifier(), resultPtr);
	return true;
}

/// Blend: mask != 0 → i1 vector → select(mask, a, b)
template <typename ElemT, int64_t N>
bool vectorBlendIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                          MLIRLoweringProvider::ValueFrame& frame) {
	auto ptrMask = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto ptrA = frame.getValue(call->getInputArguments()[1]->getIdentifier());
	auto ptrB = frame.getValue(call->getInputArguments()[2]->getIdentifier());

	::mlir::Type elemTy;
	if constexpr (std::is_same_v<ElemT, float>) {
		elemTy = builder->getF32Type();
	} else if constexpr (std::is_same_v<ElemT, double>) {
		elemTy = builder->getF64Type();
	} else if constexpr (std::is_same_v<ElemT, int32_t>) {
		elemTy = builder->getI32Type();
	} else {
		elemTy = builder->getI64Type();
	}
	auto vecTy = getVecType(elemTy, N);

	auto mask = loadVecFromPtr(builder, ptrMask, vecTy);
	auto a = loadVecFromPtr(builder, ptrA, vecTy);
	auto b = loadVecFromPtr(builder, ptrB, vecTy);

	// Convert mask to i1 vector: mask != 0
	::mlir::Value maskI1;
	if constexpr (std::is_floating_point_v<ElemT>) {
		// Float: bitcast to int first, then compare != 0
		auto intElemTy = builder->getIntegerType(elemTy.getIntOrFloatBitWidth());
		auto intVecTy = getVecType(intElemTy, N);
		auto maskInt = builder->create<::mlir::LLVM::BitcastOp>(builder->getUnknownLoc(), intVecTy, mask);
		auto zero = builder->create<::mlir::arith::ConstantOp>(builder->getUnknownLoc(), intVecTy,
		                                                       builder->getZeroAttr(intVecTy));
		maskI1 = builder->create<::mlir::LLVM::ICmpOp>(builder->getUnknownLoc(), ::mlir::LLVM::ICmpPredicate::ne,
		                                               maskInt, zero);
	} else {
		// Int: compare directly != 0
		auto zero =
		    builder->create<::mlir::arith::ConstantOp>(builder->getUnknownLoc(), vecTy, builder->getZeroAttr(vecTy));
		maskI1 = builder->create<::mlir::LLVM::ICmpOp>(builder->getUnknownLoc(), ::mlir::LLVM::ICmpPredicate::ne, mask,
		                                               zero);
	}

	// select: where mask is set → a, else → b
	auto result = builder->create<::mlir::LLVM::SelectOp>(builder->getUnknownLoc(), maskI1, a, b);

	auto resultPtr = storeVecToAlloca(builder, result, vecTy);
	frame.setValue(call->getIdentifier(), resultPtr);
	return true;
}

// ============================================================================
// Helper: get MLIR element type from C++ type
// ============================================================================

template <typename ElemT>
static ::mlir::Type getElemType(::mlir::OpBuilder& builder) {
	if constexpr (std::is_same_v<ElemT, float>) {
		return builder.getF32Type();
	} else if constexpr (std::is_same_v<ElemT, double>) {
		return builder.getF64Type();
	} else if constexpr (std::is_same_v<ElemT, int32_t>) {
		return builder.getI32Type();
	} else {
		return builder.getI64Type();
	}
}

// ============================================================================
// Broadcast: splat a scalar to all vector lanes
// ============================================================================

template <typename ElemT, int64_t N>
bool vectorBroadcastIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                              MLIRLoweringProvider::ValueFrame& frame) {
	auto scalar = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto loc = builder->getUnknownLoc();

	auto elemTy = getElemType<ElemT>(*builder);
	auto vecTy = getVecType(elemTy, N);

	// Insert scalar at index 0 into undef vector, then shuffle-splat
	auto undef = builder->create<::mlir::LLVM::UndefOp>(loc, vecTy);
	auto zero = builder->create<::mlir::LLVM::ConstantOp>(loc, builder->getI32Type(), builder->getI32IntegerAttr(0));
	auto inserted = builder->create<::mlir::LLVM::InsertElementOp>(loc, undef, scalar, zero);

	// Shuffle with all-zero mask to broadcast lane 0 to all lanes
	llvm::SmallVector<int32_t> mask(N, 0);
	auto result = builder->create<::mlir::LLVM::ShuffleVectorOp>(loc, inserted, undef, mask);

	auto resultPtr = storeVecToAlloca(builder, result, vecTy);
	frame.setValue(call->getIdentifier(), resultPtr);
	return true;
}

// ============================================================================
// Gather: indexed load from non-contiguous memory
// ============================================================================

template <typename ElemT, int64_t N>
bool vectorGatherIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                           MLIRLoweringProvider::ValueFrame& frame) {
	auto basePtr = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto idxPtr = frame.getValue(call->getInputArguments()[1]->getIdentifier());
	auto loc = builder->getUnknownLoc();

	auto elemTy = getElemType<ElemT>(*builder);
	auto vecTy = getVecType(elemTy, N);
	auto i32Ty = builder->getI32Type();
	auto idxVecTy = getVecType(i32Ty, N);
	auto ptrTy = ::mlir::LLVM::LLVMPointerType::get(builder->getContext());

	// Load all indices as a vector<Nxi32>
	auto indices = builder->create<::mlir::LLVM::LoadOp>(loc, idxVecTy, idxPtr);

	// Build result element by element: extract index, GEP, load, insert
	::mlir::Value result = builder->create<::mlir::LLVM::UndefOp>(loc, vecTy);
	for (int64_t i = 0; i < N; i++) {
		::mlir::Value iConst = builder->create<::mlir::LLVM::ConstantOp>(loc, i32Ty, builder->getI32IntegerAttr(i));
		::mlir::Value idx = builder->create<::mlir::LLVM::ExtractElementOp>(loc, indices, iConst);
		auto elemPtr = builder->create<::mlir::LLVM::GEPOp>(loc, ptrTy, elemTy, basePtr,
		                                                    llvm::ArrayRef<::mlir::LLVM::GEPArg> {idx});
		::mlir::Value elem = builder->create<::mlir::LLVM::LoadOp>(loc, elemTy, elemPtr);
		result = builder->create<::mlir::LLVM::InsertElementOp>(loc, result, elem, iConst);
	}

	auto resultPtr = storeVecToAlloca(builder, result, vecTy);
	frame.setValue(call->getIdentifier(), resultPtr);
	return true;
}

// ============================================================================
// Scatter: indexed store to non-contiguous memory
// ============================================================================

template <typename ElemT, int64_t N>
bool vectorScatterIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                            MLIRLoweringProvider::ValueFrame& frame) {
	auto basePtr = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto idxPtr = frame.getValue(call->getInputArguments()[1]->getIdentifier());
	auto dataPtr = frame.getValue(call->getInputArguments()[2]->getIdentifier());
	auto loc = builder->getUnknownLoc();

	auto elemTy = getElemType<ElemT>(*builder);
	auto vecTy = getVecType(elemTy, N);
	auto i32Ty = builder->getI32Type();
	auto idxVecTy = getVecType(i32Ty, N);
	auto ptrTy = ::mlir::LLVM::LLVMPointerType::get(builder->getContext());

	// Load indices and data
	auto indices = builder->create<::mlir::LLVM::LoadOp>(loc, idxVecTy, idxPtr);
	auto data = loadVecFromPtr(builder, dataPtr, vecTy);

	// Store element by element: extract index + element, GEP, store
	for (int64_t i = 0; i < N; i++) {
		::mlir::Value iConst = builder->create<::mlir::LLVM::ConstantOp>(loc, i32Ty, builder->getI32IntegerAttr(i));
		::mlir::Value idx = builder->create<::mlir::LLVM::ExtractElementOp>(loc, indices, iConst);
		::mlir::Value elem = builder->create<::mlir::LLVM::ExtractElementOp>(loc, data, iConst);
		auto elemPtr = builder->create<::mlir::LLVM::GEPOp>(loc, ptrTy, elemTy, basePtr,
		                                                    llvm::ArrayRef<::mlir::LLVM::GEPArg> {idx});
		builder->create<::mlir::LLVM::StoreOp>(loc, elem, elemPtr);
	}

	frame.setValue(call->getIdentifier(), basePtr);
	return true;
}

// ============================================================================
// Extract: get a single lane by runtime index
// ============================================================================

template <typename ElemT, int64_t N>
bool vectorExtractIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                            MLIRLoweringProvider::ValueFrame& frame) {
	auto vecPtr = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto idx = frame.getValue(call->getInputArguments()[1]->getIdentifier());
	auto loc = builder->getUnknownLoc();

	auto elemTy = getElemType<ElemT>(*builder);
	auto vecTy = getVecType(elemTy, N);

	auto vec = loadVecFromPtr(builder, vecPtr, vecTy);
	auto result = builder->create<::mlir::LLVM::ExtractElementOp>(loc, vec, idx);

	frame.setValue(call->getIdentifier(), result);
	return true;
}

// ============================================================================
// Insert: replace a single lane by runtime index, return new vector
// ============================================================================

template <typename ElemT, int64_t N>
bool vectorInsertIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                           MLIRLoweringProvider::ValueFrame& frame) {
	auto vecPtr = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto value = frame.getValue(call->getInputArguments()[1]->getIdentifier());
	auto idx = frame.getValue(call->getInputArguments()[2]->getIdentifier());
	auto loc = builder->getUnknownLoc();

	auto elemTy = getElemType<ElemT>(*builder);
	auto vecTy = getVecType(elemTy, N);

	auto vec = loadVecFromPtr(builder, vecPtr, vecTy);
	auto result = builder->create<::mlir::LLVM::InsertElementOp>(loc, vec, value, idx);

	auto resultPtr = storeVecToAlloca(builder, result, vecTy);
	frame.setValue(call->getIdentifier(), resultPtr);
	return true;
}

// ============================================================================
// Registration macros
// ============================================================================

// clang-format off

// Register binary ops (add, sub, mul, div) for a given type/width
#define REGISTER_VECTOR_BINARY_FLOAT(manager, T, N, SUFFIX)                                                          \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_add_##SUFFIX##_impl),                                       \
	                     vectorBinaryIntrinsic<::mlir::LLVM::FAddOp, T, N>);                                         \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_sub_##SUFFIX##_impl),                                       \
	                     vectorBinaryIntrinsic<::mlir::LLVM::FSubOp, T, N>);                                         \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_mul_##SUFFIX##_impl),                                       \
	                     vectorBinaryIntrinsic<::mlir::LLVM::FMulOp, T, N>);                                         \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_div_##SUFFIX##_impl),                                       \
	                     vectorBinaryIntrinsic<::mlir::LLVM::FDivOp, T, N>);

#define REGISTER_VECTOR_BINARY_INT(manager, T, N, SUFFIX)                                                            \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_add_##SUFFIX##_impl),                                       \
	                     vectorBinaryIntrinsic<::mlir::LLVM::AddOp, T, N>);                                          \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_sub_##SUFFIX##_impl),                                       \
	                     vectorBinaryIntrinsic<::mlir::LLVM::SubOp, T, N>);                                          \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_mul_##SUFFIX##_impl),                                       \
	                     vectorBinaryIntrinsic<::mlir::LLVM::MulOp, T, N>);                                          \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_div_##SUFFIX##_impl),                                       \
	                     vectorBinaryIntrinsic<::mlir::LLVM::SDivOp, T, N>);

// Register load/store for a given type/width
#define REGISTER_VECTOR_LOAD_STORE(manager, T, N, SUFFIX)                                                            \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_load_##SUFFIX##_impl),                                      \
	                     (vectorLoadIntrinsic<T, N>));                                                                \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_store_##SUFFIX##_impl),                                     \
	                     (vectorStoreIntrinsic<T, N>));

// Register abs for float types (math.absf works on vectors)
#define REGISTER_VECTOR_ABS_FLOAT(manager, T, N, SUFFIX)                                                             \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_abs_##SUFFIX##_impl),                                       \
	                     vectorUnaryIntrinsic<::mlir::math::AbsFOp, T, N>);

// Register abs for int types
#define REGISTER_VECTOR_ABS_INT(manager, T, N, SUFFIX)                                                               \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_abs_##SUFFIX##_impl),                                       \
	                     vectorUnaryIntrinsic<::mlir::math::AbsIOp, T, N>);

// Register neg
#define REGISTER_VECTOR_NEG_FLOAT(manager, T, N, SUFFIX)                                                             \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_neg_##SUFFIX##_impl),                                       \
	                     (vectorNegFloatIntrinsic<T, N>));

#define REGISTER_VECTOR_NEG_INT(manager, T, N, SUFFIX)                                                               \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_neg_##SUFFIX##_impl),                                       \
	                     (vectorNegIntIntrinsic<T, N>));

// Register min/max for float types
#define REGISTER_VECTOR_MINMAX_FLOAT(manager, T, N, SUFFIX)                                                          \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_min_##SUFFIX##_impl),                                       \
	                     vectorBinaryIntrinsic<::mlir::arith::MinNumFOp, T, N>);                                     \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_max_##SUFFIX##_impl),                                       \
	                     vectorBinaryIntrinsic<::mlir::arith::MaxNumFOp, T, N>);

// Register min/max for int types
#define REGISTER_VECTOR_MINMAX_INT(manager, T, N, SUFFIX)                                                            \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_min_##SUFFIX##_impl),                                       \
	                     vectorBinaryIntrinsic<::mlir::arith::MinSIOp, T, N>);                                       \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_max_##SUFFIX##_impl),                                       \
	                     vectorBinaryIntrinsic<::mlir::arith::MaxSIOp, T, N>);

// Register fma for float types
#define REGISTER_VECTOR_FMA(manager, T, N, SUFFIX)                                                                   \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_fma_##SUFFIX##_impl),                                       \
	                     vectorTernaryIntrinsic<::mlir::LLVM::FMAOp, T, N>);

// Register reduce for float types
#define REGISTER_VECTOR_REDUCE_FLOAT(manager, T, N, SUFFIX)                                                          \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_reduce_add_##SUFFIX##_impl),                                \
	                     (vectorReduceAddFloatIntrinsic<T, N>));                                                      \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_reduce_min_##SUFFIX##_impl),                                \
	                     (vectorReduceFloatIntrinsic<::mlir::LLVM::vector_reduce_fmin, T, N>));                       \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_reduce_max_##SUFFIX##_impl),                                \
	                     (vectorReduceFloatIntrinsic<::mlir::LLVM::vector_reduce_fmax, T, N>));

// Register reduce for int types
#define REGISTER_VECTOR_REDUCE_INT(manager, T, N, SUFFIX)                                                            \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_reduce_add_##SUFFIX##_impl),                                \
	                     (vectorReduceAddIntIntrinsic<T, N>));                                                        \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_reduce_min_##SUFFIX##_impl),                                \
	                     (vectorReduceIntIntrinsic<::mlir::LLVM::vector_reduce_smin, T, N>));                         \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_reduce_max_##SUFFIX##_impl),                                \
	                     (vectorReduceIntIntrinsic<::mlir::LLVM::vector_reduce_smax, T, N>));

// Register float comparisons (eq, ne, lt, le, gt, ge)
#define REGISTER_VECTOR_CMP_FLOAT(manager, T, N, SUFFIX)                                                             \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_eq_##SUFFIX##_impl),                                        \
	                     (vectorFCmpIntrinsic<::mlir::LLVM::FCmpPredicate::oeq, T, N>));                             \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_ne_##SUFFIX##_impl),                                        \
	                     (vectorFCmpIntrinsic<::mlir::LLVM::FCmpPredicate::one, T, N>));                             \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_lt_##SUFFIX##_impl),                                        \
	                     (vectorFCmpIntrinsic<::mlir::LLVM::FCmpPredicate::olt, T, N>));                             \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_le_##SUFFIX##_impl),                                        \
	                     (vectorFCmpIntrinsic<::mlir::LLVM::FCmpPredicate::ole, T, N>));                             \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_gt_##SUFFIX##_impl),                                        \
	                     (vectorFCmpIntrinsic<::mlir::LLVM::FCmpPredicate::ogt, T, N>));                             \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_ge_##SUFFIX##_impl),                                        \
	                     (vectorFCmpIntrinsic<::mlir::LLVM::FCmpPredicate::oge, T, N>));

// Register int comparisons (eq, ne, lt, le, gt, ge)
#define REGISTER_VECTOR_CMP_INT(manager, T, N, SUFFIX)                                                               \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_eq_##SUFFIX##_impl),                                        \
	                     (vectorICmpIntrinsic<::mlir::LLVM::ICmpPredicate::eq, T, N>));                              \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_ne_##SUFFIX##_impl),                                        \
	                     (vectorICmpIntrinsic<::mlir::LLVM::ICmpPredicate::ne, T, N>));                              \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_lt_##SUFFIX##_impl),                                        \
	                     (vectorICmpIntrinsic<::mlir::LLVM::ICmpPredicate::slt, T, N>));                             \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_le_##SUFFIX##_impl),                                        \
	                     (vectorICmpIntrinsic<::mlir::LLVM::ICmpPredicate::sle, T, N>));                             \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_gt_##SUFFIX##_impl),                                        \
	                     (vectorICmpIntrinsic<::mlir::LLVM::ICmpPredicate::sgt, T, N>));                             \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_ge_##SUFFIX##_impl),                                        \
	                     (vectorICmpIntrinsic<::mlir::LLVM::ICmpPredicate::sge, T, N>));

// Register float bitwise (bitcast to int, apply op, bitcast back)
#define REGISTER_VECTOR_BITWISE_FLOAT(manager, T, N, SUFFIX)                                                         \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_and_##SUFFIX##_impl),                                       \
	                     (vectorBitwiseFloatIntrinsic<::mlir::LLVM::AndOp, T, N>));                                  \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_or_##SUFFIX##_impl),                                        \
	                     (vectorBitwiseFloatIntrinsic<::mlir::LLVM::OrOp, T, N>));                                   \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_xor_##SUFFIX##_impl),                                       \
	                     (vectorBitwiseFloatIntrinsic<::mlir::LLVM::XOrOp, T, N>));

// Register int bitwise (direct LLVM ops)
#define REGISTER_VECTOR_BITWISE_INT(manager, T, N, SUFFIX)                                                           \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_and_##SUFFIX##_impl),                                       \
	                     vectorBinaryIntrinsic<::mlir::LLVM::AndOp, T, N>);                                          \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_or_##SUFFIX##_impl),                                        \
	                     vectorBinaryIntrinsic<::mlir::LLVM::OrOp, T, N>);                                           \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_xor_##SUFFIX##_impl),                                       \
	                     vectorBinaryIntrinsic<::mlir::LLVM::XOrOp, T, N>);

// Register blend (mask → i1 → select)
#define REGISTER_VECTOR_BLEND(manager, T, N, SUFFIX)                                                                 \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_blend_##SUFFIX##_impl),                                     \
	                     (vectorBlendIntrinsic<T, N>));

// Register broadcast (scalar → splat vector)
#define REGISTER_VECTOR_BROADCAST(manager, T, N, SUFFIX)                                                             \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_broadcast_##SUFFIX##_impl),                                 \
	                     (vectorBroadcastIntrinsic<T, N>));

// Register gather (indexed load)
#define REGISTER_VECTOR_GATHER(manager, T, N, SUFFIX)                                                                \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_gather_##SUFFIX##_impl),                                    \
	                     (vectorGatherIntrinsic<T, N>));

// Register scatter (indexed store)
#define REGISTER_VECTOR_SCATTER(manager, T, N, SUFFIX)                                                               \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_scatter_##SUFFIX##_impl),                                   \
	                     (vectorScatterIntrinsic<T, N>));

// Register extract (lane access)
#define REGISTER_VECTOR_EXTRACT(manager, T, N, SUFFIX)                                                               \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_extract_##SUFFIX##_impl),                                   \
	                     (vectorExtractIntrinsic<T, N>));

// Register insert (lane replace)
#define REGISTER_VECTOR_INSERT(manager, T, N, SUFFIX)                                                                \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_insert_##SUFFIX##_impl),                                    \
	                     (vectorInsertIntrinsic<T, N>));

// Register shift operations (integer only)
#define REGISTER_VECTOR_SHIFT_INT(manager, T, N, SUFFIX)                                                             \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_shl_##SUFFIX##_impl),                                       \
	                     vectorBinaryIntrinsic<::mlir::LLVM::ShlOp, T, N>);                                          \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_shr_##SUFFIX##_impl),                                       \
	                     vectorBinaryIntrinsic<::mlir::LLVM::AShrOp, T, N>);

// Full registration for float type/width
#define REGISTER_VECTOR_FLOAT_ALL(manager, T, N, SUFFIX)                                                             \
	REGISTER_VECTOR_LOAD_STORE(manager, T, N, SUFFIX)                                                                \
	REGISTER_VECTOR_BINARY_FLOAT(manager, T, N, SUFFIX)                                                              \
	REGISTER_VECTOR_ABS_FLOAT(manager, T, N, SUFFIX)                                                                 \
	REGISTER_VECTOR_NEG_FLOAT(manager, T, N, SUFFIX)                                                                 \
	REGISTER_VECTOR_MINMAX_FLOAT(manager, T, N, SUFFIX)                                                              \
	REGISTER_VECTOR_FMA(manager, T, N, SUFFIX)                                                                       \
	REGISTER_VECTOR_REDUCE_FLOAT(manager, T, N, SUFFIX)                                                              \
	REGISTER_VECTOR_CMP_FLOAT(manager, T, N, SUFFIX)                                                                 \
	REGISTER_VECTOR_BITWISE_FLOAT(manager, T, N, SUFFIX)                                                             \
	REGISTER_VECTOR_BLEND(manager, T, N, SUFFIX)                                                                     \
	REGISTER_VECTOR_BROADCAST(manager, T, N, SUFFIX)                                                                 \
	REGISTER_VECTOR_GATHER(manager, T, N, SUFFIX)                                                                    \
	REGISTER_VECTOR_SCATTER(manager, T, N, SUFFIX)                                                                   \
	REGISTER_VECTOR_EXTRACT(manager, T, N, SUFFIX)                                                                   \
	REGISTER_VECTOR_INSERT(manager, T, N, SUFFIX)

// Full registration for int type/width
#define REGISTER_VECTOR_INT_ALL(manager, T, N, SUFFIX)                                                               \
	REGISTER_VECTOR_LOAD_STORE(manager, T, N, SUFFIX)                                                                \
	REGISTER_VECTOR_BINARY_INT(manager, T, N, SUFFIX)                                                                \
	REGISTER_VECTOR_ABS_INT(manager, T, N, SUFFIX)                                                                   \
	REGISTER_VECTOR_NEG_INT(manager, T, N, SUFFIX)                                                                   \
	REGISTER_VECTOR_MINMAX_INT(manager, T, N, SUFFIX)                                                                \
	REGISTER_VECTOR_REDUCE_INT(manager, T, N, SUFFIX)                                                                \
	REGISTER_VECTOR_CMP_INT(manager, T, N, SUFFIX)                                                                   \
	REGISTER_VECTOR_BITWISE_INT(manager, T, N, SUFFIX)                                                               \
	REGISTER_VECTOR_BLEND(manager, T, N, SUFFIX)                                                                     \
	REGISTER_VECTOR_BROADCAST(manager, T, N, SUFFIX)                                                                 \
	REGISTER_VECTOR_GATHER(manager, T, N, SUFFIX)                                                                    \
	REGISTER_VECTOR_SCATTER(manager, T, N, SUFFIX)                                                                   \
	REGISTER_VECTOR_EXTRACT(manager, T, N, SUFFIX)                                                                   \
	REGISTER_VECTOR_INSERT(manager, T, N, SUFFIX)                                                                    \
	REGISTER_VECTOR_SHIFT_INT(manager, T, N, SUFFIX)

// clang-format on

// ============================================================================
// Plugin Registration
// ============================================================================

void MLIRVectorIntrinsicPlugin::registerIntrinsics(MLIRIntrinsicManager& manager) {
	// Float: 128-bit (4 lanes), 256-bit (8 lanes), 512-bit (16 lanes)
	REGISTER_VECTOR_FLOAT_ALL(manager, float, 4, f32x4)
	REGISTER_VECTOR_FLOAT_ALL(manager, float, 8, f32x8)
	REGISTER_VECTOR_FLOAT_ALL(manager, float, 16, f32x16)

	// Double: 128-bit (2 lanes), 256-bit (4 lanes), 512-bit (8 lanes)
	REGISTER_VECTOR_FLOAT_ALL(manager, double, 2, f64x2)
	REGISTER_VECTOR_FLOAT_ALL(manager, double, 4, f64x4)
	REGISTER_VECTOR_FLOAT_ALL(manager, double, 8, f64x8)

	// int32_t: 128-bit (4 lanes), 256-bit (8 lanes), 512-bit (16 lanes)
	REGISTER_VECTOR_INT_ALL(manager, int32_t, 4, i32x4)
	REGISTER_VECTOR_INT_ALL(manager, int32_t, 8, i32x8)
	REGISTER_VECTOR_INT_ALL(manager, int32_t, 16, i32x16)

	// int64_t: 128-bit (2 lanes), 256-bit (4 lanes), 512-bit (8 lanes)
	REGISTER_VECTOR_INT_ALL(manager, int64_t, 2, i64x2)
	REGISTER_VECTOR_INT_ALL(manager, int64_t, 4, i64x4)
	REGISTER_VECTOR_INT_ALL(manager, int64_t, 8, i64x8)
}

void RegisterMLIRVectorIntrinsicPlugin() {
	MLIRIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<MLIRVectorIntrinsicPlugin>());
}

} // namespace nautilus::compiler::mlir
