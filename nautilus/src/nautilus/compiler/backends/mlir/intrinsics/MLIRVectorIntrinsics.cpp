
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRVectorIntrinsics.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>
#include <mlir/Dialect/Math/IR/Math.h>
#include <mlir/IR/Types.h>
#include <nautilus/api/vector/vector_impl.hpp>

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
	auto ptrA = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	auto ptrB = frame.getValue(call->getInputArguments().at(1)->getIdentifier());

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
	auto ptrA = frame.getValue(call->getInputArguments().at(0)->getIdentifier());

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
	auto ptrA = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	auto ptrB = frame.getValue(call->getInputArguments().at(1)->getIdentifier());
	auto ptrC = frame.getValue(call->getInputArguments().at(2)->getIdentifier());

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
	auto srcPtr = frame.getValue(call->getInputArguments().at(0)->getIdentifier());

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

	// Load vector directly from the source pointer (treat as vector pointer)
	auto vec = builder->create<::mlir::LLVM::LoadOp>(builder->getUnknownLoc(), vecTy, srcPtr);
	auto resultPtr = storeVecToAlloca(builder, vec, vecTy);
	frame.setValue(call->getIdentifier(), resultPtr);
	return true;
}

/// Store: write vector data to raw T* pointer
template <typename ElemT, int64_t N>
bool vectorStoreIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
                          MLIRLoweringProvider::ValueFrame& frame) {
	auto destPtr = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	auto vecPtr = frame.getValue(call->getInputArguments().at(1)->getIdentifier());

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
	builder->create<::mlir::LLVM::StoreOp>(builder->getUnknownLoc(), vec, destPtr);
	// Store returns void, but we still need to set a value for the frame.
	// The store_impl returns void, so the ProxyCallOp result may not be used.
	// We pass destPtr as the output so the frame has something valid.
	frame.setValue(call->getIdentifier(), destPtr);
	return true;
}

/// Negate for float: 0 - x
template <typename ElemT, int64_t N>
bool vectorNegFloatIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                             const compiler::ir::ProxyCallOperation* call, MLIRLoweringProvider::ValueFrame& frame) {
	auto ptrA = frame.getValue(call->getInputArguments().at(0)->getIdentifier());

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
	auto ptrA = frame.getValue(call->getInputArguments().at(0)->getIdentifier());

	::mlir::Type elemTy;
	if constexpr (std::is_same_v<ElemT, int32_t>) {
		elemTy = builder->getI32Type();
	} else {
		elemTy = builder->getI64Type();
	}
	auto vecTy = getVecType(elemTy, N);

	auto a = loadVecFromPtr(builder, ptrA, vecTy);
	// zero - a
	auto zero = builder->create<::mlir::arith::ConstantOp>(
	    builder->getUnknownLoc(), vecTy, builder->getZeroAttr(vecTy));
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
	auto ptrA = frame.getValue(call->getInputArguments().at(0)->getIdentifier());

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
	auto ptrA = frame.getValue(call->getInputArguments().at(0)->getIdentifier());

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
	                     (vectorReduceAddFloatIntrinsic<T, N>));

// Register reduce for int types
#define REGISTER_VECTOR_REDUCE_INT(manager, T, N, SUFFIX)                                                            \
	manager.addIntrinsic(reinterpret_cast<void*>(&vector_reduce_add_##SUFFIX##_impl),                                \
	                     (vectorReduceAddIntIntrinsic<T, N>));

// Full registration for float type/width
#define REGISTER_VECTOR_FLOAT_ALL(manager, T, N, SUFFIX)                                                             \
	REGISTER_VECTOR_LOAD_STORE(manager, T, N, SUFFIX)                                                                \
	REGISTER_VECTOR_BINARY_FLOAT(manager, T, N, SUFFIX)                                                              \
	REGISTER_VECTOR_ABS_FLOAT(manager, T, N, SUFFIX)                                                                 \
	REGISTER_VECTOR_NEG_FLOAT(manager, T, N, SUFFIX)                                                                 \
	REGISTER_VECTOR_MINMAX_FLOAT(manager, T, N, SUFFIX)                                                              \
	REGISTER_VECTOR_FMA(manager, T, N, SUFFIX)                                                                       \
	REGISTER_VECTOR_REDUCE_FLOAT(manager, T, N, SUFFIX)

// Full registration for int type/width
#define REGISTER_VECTOR_INT_ALL(manager, T, N, SUFFIX)                                                               \
	REGISTER_VECTOR_LOAD_STORE(manager, T, N, SUFFIX)                                                                \
	REGISTER_VECTOR_BINARY_INT(manager, T, N, SUFFIX)                                                                \
	REGISTER_VECTOR_ABS_INT(manager, T, N, SUFFIX)                                                                   \
	REGISTER_VECTOR_NEG_INT(manager, T, N, SUFFIX)                                                                   \
	REGISTER_VECTOR_MINMAX_INT(manager, T, N, SUFFIX)                                                                \
	REGISTER_VECTOR_REDUCE_INT(manager, T, N, SUFFIX)

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
