
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRMathIntrinsics.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include <cmath>
#include <stdexcept>
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/Math/IR/Math.h>
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
                               const compiler::ir::ProxyCallOperation* call, MLIRLoweringProvider::ValueFrame& frame) {
        auto inputArg = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
        auto result = builder->create<MLIROp>(builder->getUnknownLoc(), inputArg);
        frame.setValue(call->getIdentifier(), result);
        return true;
}

/// Generic helper for binary operations (2 arguments)
template <typename MLIROp>
bool replaceWithBinaryIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                const compiler::ir::ProxyCallOperation* call, MLIRLoweringProvider::ValueFrame& frame) {
        auto arg1 = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
        auto arg2 = frame.getValue(call->getInputArguments().at(1)->getIdentifier());
        auto result = builder->create<MLIROp>(builder->getUnknownLoc(), arg1, arg2);
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
        auto result = builder->create<MLIROp>(builder->getUnknownLoc(), arg1, arg2, arg3);
        frame.setValue(call->getIdentifier(), result);
        return true;
}

/// Helper mapping Nautilus Type -> MLIR type for intrinsic results
::mlir::Type getResultType(::mlir::OpBuilder& builder, Type type) {
        switch (type) {
        case Type::v:
                return builder.getNoneType();
        case Type::b:
                return builder.getI1Type();
        case Type::i8:
                return builder.getI8Type();
        case Type::i16:
                return builder.getI16Type();
        case Type::i32:
                return builder.getI32Type();
        case Type::i64:
                return builder.getI64Type();
        case Type::ui8:
                return builder.getIntegerType(8, /*isSigned=*/false);
        case Type::ui16:
                return builder.getIntegerType(16, /*isSigned=*/false);
        case Type::ui32:
                return builder.getIntegerType(32, /*isSigned=*/false);
        case Type::ui64:
                return builder.getIntegerType(64, /*isSigned=*/false);
        case Type::f32:
                return builder.getF32Type();
        case Type::f64:
                return builder.getF64Type();
        case Type::ptr:
                return ::mlir::LLVM::LLVMPointerType::get(builder.getContext());
        }
        throw std::runtime_error("Unsupported intrinsic result type");
}

::mlir::Value castRoundedValue(std::unique_ptr<::mlir::OpBuilder>& builder, ::mlir::Value roundedValue,
                             const ir::ProxyCallOperation* call) {
        auto targetType = getResultType(*builder, call->getStamp());
        if (!targetType.isa<::mlir::IntegerType>()) {
                return roundedValue;
        }

        auto integerType = targetType.cast<::mlir::IntegerType>();
        if (integerType.isUnsigned()) {
                return builder->create<::mlir::arith::FPToUIOp>(builder->getUnknownLoc(), targetType, roundedValue);
        }

        return builder->create<::mlir::arith::FPToSIOp>(builder->getUnknownLoc(), targetType, roundedValue);
}

template <typename RoundOp>
bool replaceWithIntegerRoundIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                                     const compiler::ir::ProxyCallOperation* call,
                                     MLIRLoweringProvider::ValueFrame& frame) {
        auto inputArg = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
        auto rounded = builder->create<RoundOp>(builder->getUnknownLoc(), inputArg);
        auto result = castRoundedValue(builder, rounded, call);
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
