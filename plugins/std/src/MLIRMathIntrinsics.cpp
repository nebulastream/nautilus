
#include "MLIRMathIntrinsics.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include <cmath>
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>
#include <mlir/Dialect/Math/IR/Math.h>
#include <mlir/IR/Types.h>
#include <mlir/Support/LLVM.h>
#include <stdexcept>

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
bool replaceWithUnaryIntrinsic(NirIntrinsicCall& call) {
	[[maybe_unused]] auto& builder = call.builder;
	auto inputArg = call.args[0];
	auto result = MLIROp::create(*builder, builder->getUnknownLoc(), inputArg);
	call.result = result;
	return true;
}

/// Generic helper for binary operations (2 arguments)
template <typename MLIROp>
bool replaceWithBinaryIntrinsic(NirIntrinsicCall& call) {
	[[maybe_unused]] auto& builder = call.builder;
	auto arg1 = call.args[0];
	auto arg2 = call.args[1];
	auto result = MLIROp::create(*builder, builder->getUnknownLoc(), arg1, arg2);
	call.result = result;
	return true;
}

/// Generic helper for ternary operations (3 arguments)
template <typename MLIROp>
bool replaceWithTernaryIntrinsic(NirIntrinsicCall& call) {
	[[maybe_unused]] auto& builder = call.builder;
	auto arg1 = call.args[0];
	auto arg2 = call.args[1];
	auto arg3 = call.args[2];
	auto result = MLIROp::create(*builder, builder->getUnknownLoc(), arg1, arg2, arg3);
	call.result = result;
	return true;
}

/// The rounding functions that return an integer (`lround`, `llround`, `lrint`, `llrint`) return a signed one.
::mlir::Value castRoundedValue(std::unique_ptr<::mlir::OpBuilder>& builder, ::mlir::Value roundedValue,
                               ::mlir::Type targetType) {
	if (!targetType || !::mlir::isa<::mlir::IntegerType>(targetType)) {
		return roundedValue;
	}
	return ::mlir::arith::FPToSIOp::create(*builder, builder->getUnknownLoc(), targetType, roundedValue);
}

template <typename RoundOp>
bool replaceWithIntegerRoundIntrinsic(NirIntrinsicCall& call) {
	[[maybe_unused]] auto& builder = call.builder;
	auto inputArg = call.args[0];
	auto rounded = RoundOp::create(*builder, builder->getUnknownLoc(), inputArg);
	auto result = castRoundedValue(builder, rounded, call.resultType);
	call.result = result;
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
	                     replaceWithUnaryIntrinsic<::mlir::math::AbsIOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<int64_t (*)(int64_t)>(&std::abs)),
	                     replaceWithUnaryIntrinsic<::mlir::math::AbsIOp>);

	// Floating-point abs
	registerUnaryFloatIntrinsic<::mlir::math::AbsFOp>(manager, &std::fabs, &std::fabs);

	// Trigonometric functions
	registerUnaryFloatIntrinsic<::mlir::math::SinOp>(manager, &std::sin, &std::sin);
	registerUnaryFloatIntrinsic<::mlir::math::CosOp>(manager, &std::cos, &std::cos);
	registerUnaryFloatIntrinsic<::mlir::math::TanOp>(manager, &std::tan, &std::tan);

	// Inverse trigonometric functions
	registerUnaryFloatIntrinsic<::mlir::math::AsinOp>(manager, &std::asin, &std::asin);
	registerUnaryFloatIntrinsic<::mlir::math::AcosOp>(manager, &std::acos, &std::acos);
	registerUnaryFloatIntrinsic<::mlir::math::AtanOp>(manager, &std::atan, &std::atan);
	registerBinaryFloatIntrinsic<::mlir::math::Atan2Op>(manager, &std::atan2, &std::atan2);

	// Hyperbolic functions
	registerUnaryFloatIntrinsic<::mlir::math::SinhOp>(manager, &std::sinh, &std::sinh);
	registerUnaryFloatIntrinsic<::mlir::math::CoshOp>(manager, &std::cosh, &std::cosh);
	registerUnaryFloatIntrinsic<::mlir::math::TanhOp>(manager, &std::tanh, &std::tanh);

	// Exponential and logarithmic functions
	registerUnaryFloatIntrinsic<::mlir::math::ExpOp>(manager, &std::exp, &std::exp);
	registerUnaryFloatIntrinsic<::mlir::math::Exp2Op>(manager, &std::exp2, &std::exp2);
	registerUnaryFloatIntrinsic<::mlir::math::LogOp>(manager, &std::log, &std::log);
	registerUnaryFloatIntrinsic<::mlir::math::Log2Op>(manager, &std::log2, &std::log2);
	registerUnaryFloatIntrinsic<::mlir::math::Log10Op>(manager, &std::log10, &std::log10);

	// Power and root functions
	registerUnaryFloatIntrinsic<::mlir::math::SqrtOp>(manager, &std::sqrt, &std::sqrt);
	registerBinaryFloatIntrinsic<::mlir::math::PowFOp>(manager, &std::pow, &std::pow);

	// Note: PowIOp takes (float/double, int32) - could be added if powi is exposed in API
	// Note: Exp10Op exists in LLVM but may not be in standard C++ library on all platforms
	// Note: cbrt, expm1, log1p exist in C++ std but don't have direct LLVM intrinsics

	// Rounding functions
	registerUnaryFloatIntrinsic<::mlir::math::CeilOp>(manager, &std::ceil, &std::ceil);
	registerUnaryFloatIntrinsic<::mlir::math::FloorOp>(manager, &std::floor, &std::floor);
	registerUnaryFloatIntrinsic<::mlir::math::TruncOp>(manager, &std::trunc, &std::trunc);
	registerUnaryFloatIntrinsic<::mlir::math::RoundOp>(manager, &std::round, &std::round);
	registerUnaryFloatIntrinsic<::mlir::math::RoundEvenOp>(manager, &std::rint, &std::rint);
	registerUnaryFloatIntrinsic<::mlir::math::RoundEvenOp>(manager, &std::nearbyint, &std::nearbyint);

	// Rounding functions that return integer types
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<long (*)(float)>(&std::lround)),
	                     replaceWithIntegerRoundIntrinsic<::mlir::math::RoundOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<long (*)(double)>(&std::lround)),
	                     replaceWithIntegerRoundIntrinsic<::mlir::math::RoundOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<long long (*)(float)>(&std::llround)),
	                     replaceWithIntegerRoundIntrinsic<::mlir::math::RoundOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<long long (*)(double)>(&std::llround)),
	                     replaceWithIntegerRoundIntrinsic<::mlir::math::RoundOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<long (*)(float)>(&std::lrint)),
	                     replaceWithIntegerRoundIntrinsic<::mlir::math::RoundEvenOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<long (*)(double)>(&std::lrint)),
	                     replaceWithIntegerRoundIntrinsic<::mlir::math::RoundEvenOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<long long (*)(float)>(&std::llrint)),
	                     replaceWithIntegerRoundIntrinsic<::mlir::math::RoundEvenOp>);
	manager.addIntrinsic(reinterpret_cast<void*>(static_cast<long long (*)(double)>(&std::llrint)),
	                     replaceWithIntegerRoundIntrinsic<::mlir::math::RoundEvenOp>);

	// Floating-point manipulation
	registerBinaryFloatIntrinsic<::mlir::math::CopySignOp>(manager, &std::copysign, &std::copysign);
	registerTernaryFloatIntrinsic<::mlir::math::FmaOp>(manager, &std::fma, &std::fma);

	// Note: FMulAddOp (llvm.intr.fmuladd) exists but is different from FMA:
	// FMulAdd may be split into separate multiply and add if beneficial,
	// while FMA must be executed as a single operation (no intermediate rounding)

	// Modulo operations (using FRemOp for fmod)
	registerBinaryFloatIntrinsic<::mlir::arith::RemFOp>(manager, &std::fmod, &std::fmod);

	// Min/Max operations
	registerBinaryFloatIntrinsic<::mlir::arith::MinNumFOp>(manager, &std::fmin, &std::fmin);
	registerBinaryFloatIntrinsic<::mlir::arith::MaxNumFOp>(manager, &std::fmax, &std::fmax);
}

void RegisterMLIRMathIntrinsicPlugin() {
	MLIRIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<MLIRMathIntrinsicPlugin>());
}

} // namespace nautilus::compiler::mlir
