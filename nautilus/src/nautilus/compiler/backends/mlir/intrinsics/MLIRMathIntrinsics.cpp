
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRMathIntrinsics.hpp"
#include <cmath>
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>

namespace nautilus::compiler::mlir {

// Compiler intrinsic for math operations
class MLIRMathIntrinsicPlugin : public MLIRIntrinsicPlugin {
public:
	void registerIntrinsics(MLIRIntrinsicManager& manager) override;
	~MLIRMathIntrinsicPlugin() override = default;
};

// ============================================================================
// Helper Templates for Intrinsic Registration
// ============================================================================

/// Generic helper for unary operations (1 argument)
template <typename MLIROp>
bool replaceWithUnaryIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                const compiler::ir::ProxyCallOperation* call,
                                MLIRLoweringProvider::ValueFrame& frame) {
	auto inputArg = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	auto result = builder->create<MLIROp>(builder->getUnknownLoc(), inputArg.getType(), inputArg);
	frame.setValue(call->getIdentifier(), result);
	return true;
}

/// Generic helper for binary operations (2 arguments)
template <typename MLIROp>
bool replaceWithBinaryIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                 const compiler::ir::ProxyCallOperation* call,
                                 MLIRLoweringProvider::ValueFrame& frame) {
	auto arg1 = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	auto arg2 = frame.getValue(call->getInputArguments().at(1)->getIdentifier());
	auto result = builder->create<MLIROp>(builder->getUnknownLoc(), arg1.getType(), arg1, arg2);
	frame.setValue(call->getIdentifier(), result);
	return true;
}

/// Generic helper for ternary operations (3 arguments)
template <typename MLIROp>
bool replaceWithTernaryIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                  const compiler::ir::ProxyCallOperation* call,
                                  MLIRLoweringProvider::ValueFrame& frame) {
	auto arg1 = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
	auto arg2 = frame.getValue(call->getInputArguments().at(1)->getIdentifier());
	auto arg3 = frame.getValue(call->getInputArguments().at(2)->getIdentifier());
	auto result = builder->create<MLIROp>(builder->getUnknownLoc(), arg1.getType(), arg1, arg2, arg3);
	frame.setValue(call->getIdentifier(), result);
	return true;
}

/// Helper to register both float and double versions of a unary intrinsic
template <typename MLIROp>
void registerUnaryFloatIntrinsic(MLIRIntrinsicManager& manager, float (*floatFunc)(float),
                                  double (*doubleFunc)(double)) {
	manager.addIntrinsic(reinterpret_cast<void*>(floatFunc), replaceWithUnaryIntrinsic<MLIROp>);
	manager.addIntrinsic(reinterpret_cast<void*>(doubleFunc), replaceWithUnaryIntrinsic<MLIROp>);
}

/// Helper to register both float and double versions of a binary intrinsic
template <typename MLIROp>
void registerBinaryFloatIntrinsic(MLIRIntrinsicManager& manager, float (*floatFunc)(float, float),
                                   double (*doubleFunc)(double, double)) {
	manager.addIntrinsic(reinterpret_cast<void*>(floatFunc), replaceWithBinaryIntrinsic<MLIROp>);
	manager.addIntrinsic(reinterpret_cast<void*>(doubleFunc), replaceWithBinaryIntrinsic<MLIROp>);
}

/// Helper to register both float and double versions of a ternary intrinsic
template <typename MLIROp>
void registerTernaryFloatIntrinsic(MLIRIntrinsicManager& manager, float (*floatFunc)(float, float, float),
                                    double (*doubleFunc)(double, double, double)) {
	manager.addIntrinsic(reinterpret_cast<void*>(floatFunc), replaceWithTernaryIntrinsic<MLIROp>);
	manager.addIntrinsic(reinterpret_cast<void*>(doubleFunc), replaceWithTernaryIntrinsic<MLIROp>);
}

// ============================================================================
// Intrinsic Registration
// ============================================================================

void MLIRMathIntrinsicPlugin::registerIntrinsics(MLIRIntrinsicManager& manager) {
	// Integer abs
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<int32_t (*)(int32_t)>(&std::abs)),
	                     replaceWithUnaryIntrinsic<::mlir::LLVM::AbsOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<int64_t (*)(int64_t)>(&std::abs)),
	                     replaceWithUnaryIntrinsic<::mlir::LLVM::AbsOp>);

	// Floating-point abs
	registerUnaryFloatIntrinsic<::mlir::LLVM::FAbsOp>(manager, &std::fabs, &std::fabs);

	// Trigonometric functions
	registerUnaryFloatIntrinsic<::mlir::LLVM::SinOp>(manager, &std::sin, &std::sin);
	registerUnaryFloatIntrinsic<::mlir::LLVM::CosOp>(manager, &std::cos, &std::cos);
	registerUnaryFloatIntrinsic<::mlir::LLVM::TanOp>(manager, &std::tan, &std::tan);

	// Inverse trigonometric functions
	registerUnaryFloatIntrinsic<::mlir::LLVM::ASinOp>(manager, &std::asin, &std::asin);
	registerUnaryFloatIntrinsic<::mlir::LLVM::ACosOp>(manager, &std::acos, &std::acos);
	registerUnaryFloatIntrinsic<::mlir::LLVM::ATanOp>(manager, &std::atan, &std::atan);
	registerBinaryFloatIntrinsic<::mlir::LLVM::ATan2Op>(manager, &std::atan2, &std::atan2);

	// Hyperbolic functions
	registerUnaryFloatIntrinsic<::mlir::LLVM::SinhOp>(manager, &std::sinh, &std::sinh);
	registerUnaryFloatIntrinsic<::mlir::LLVM::CoshOp>(manager, &std::cosh, &std::cosh);
	registerUnaryFloatIntrinsic<::mlir::LLVM::TanhOp>(manager, &std::tanh, &std::tanh);

	// Exponential and logarithmic functions
	registerUnaryFloatIntrinsic<::mlir::LLVM::ExpOp>(manager, &std::exp, &std::exp);
	registerUnaryFloatIntrinsic<::mlir::LLVM::Exp2Op>(manager, &std::exp2, &std::exp2);
	registerUnaryFloatIntrinsic<::mlir::LLVM::LogOp>(manager, &std::log, &std::log);
	registerUnaryFloatIntrinsic<::mlir::LLVM::Log2Op>(manager, &std::log2, &std::log2);
	registerUnaryFloatIntrinsic<::mlir::LLVM::Log10Op>(manager, &std::log10, &std::log10);

	// Power and root functions
	registerUnaryFloatIntrinsic<::mlir::LLVM::SqrtOp>(manager, &std::sqrt, &std::sqrt);
	registerBinaryFloatIntrinsic<::mlir::LLVM::PowOp>(manager, &std::pow, &std::pow);

	// Note: PowIOp takes (float/double, int32) - could be added if powi is exposed in API
	// Note: Exp10Op exists in LLVM but may not be in standard C++ library on all platforms
	// Note: cbrt, expm1, log1p exist in C++ std but don't have direct LLVM intrinsics

	// Rounding functions
	registerUnaryFloatIntrinsic<::mlir::LLVM::FCeilOp>(manager, &std::ceil, &std::ceil);
	registerUnaryFloatIntrinsic<::mlir::LLVM::FFloorOp>(manager, &std::floor, &std::floor);
	registerUnaryFloatIntrinsic<::mlir::LLVM::FTruncOp>(manager, &std::trunc, &std::trunc);
	registerUnaryFloatIntrinsic<::mlir::LLVM::RoundOp>(manager, &std::round, &std::round);
	registerUnaryFloatIntrinsic<::mlir::LLVM::RintOp>(manager, &std::rint, &std::rint);
	registerUnaryFloatIntrinsic<::mlir::LLVM::NearbyintOp>(manager, &std::nearbyint, &std::nearbyint);

	// Rounding functions that return integer types
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<long (*)(float)>(&std::lround)),
	                     replaceWithUnaryIntrinsic<::mlir::LLVM::LroundOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<long (*)(double)>(&std::lround)),
	                     replaceWithUnaryIntrinsic<::mlir::LLVM::LroundOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<long long (*)(float)>(&std::llround)),
	                     replaceWithUnaryIntrinsic<::mlir::LLVM::LlroundOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<long long (*)(double)>(&std::llround)),
	                     replaceWithUnaryIntrinsic<::mlir::LLVM::LlroundOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<long (*)(float)>(&std::lrint)),
	                     replaceWithUnaryIntrinsic<::mlir::LLVM::LrintOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<long (*)(double)>(&std::lrint)),
	                     replaceWithUnaryIntrinsic<::mlir::LLVM::LrintOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<long long (*)(float)>(&std::llrint)),
	                     replaceWithUnaryIntrinsic<::mlir::LLVM::LlrintOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<long long (*)(double)>(&std::llrint)),
	                     replaceWithUnaryIntrinsic<::mlir::LLVM::LlrintOp>);

	// Floating-point manipulation
	registerBinaryFloatIntrinsic<::mlir::LLVM::CopySignOp>(manager, &std::copysign, &std::copysign);
	registerTernaryFloatIntrinsic<::mlir::LLVM::FMAOp>(manager, &std::fma, &std::fma);

	// Note: FMulAddOp (llvm.intr.fmuladd) exists but is different from FMA:
	// FMulAdd may be split into separate multiply and add if beneficial,
	// while FMA must be executed as a single operation (no intermediate rounding)

	// Modulo operations (using FRemOp for fmod)
	registerBinaryFloatIntrinsic<::mlir::LLVM::FRemOp>(manager, &std::fmod, &std::fmod);

	// Min/Max operations
	registerBinaryFloatIntrinsic<::mlir::LLVM::MinNumOp>(manager, &std::fmin, &std::fmin);
	registerBinaryFloatIntrinsic<::mlir::LLVM::MaxNumOp>(manager, &std::fmax, &std::fmax);
}

void RegisterMLIRMathIntrinsicPlugin() {
	MLIRIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<MLIRMathIntrinsicPlugin>());
}

} // namespace nautilus
