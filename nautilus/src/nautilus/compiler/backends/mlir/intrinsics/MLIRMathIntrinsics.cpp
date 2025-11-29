
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRMathIntrinsics.hpp"
#include <cmath>
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>

namespace nautilus::compiler::mlir {

// Compiler intrinsic for assume
class MLIRMathIntrinsicPlugin : public MLIRIntrinsicPlugin {
public:
	void registerIntrinsics(MLIRIntrinsicManager& manager) override;
	~MLIRMathIntrinsicPlugin() override = default;
};


template <typename T>   
bool replaceWithIntrinsic(std::unique_ptr<::mlir::OpBuilder>& builder,
                    const compiler::ir::ProxyCallOperation* call,
                    MLIRLoweringProvider::ValueFrame& frame) {
    auto inputArg = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
    auto result = builder->create<T>(builder->getUnknownLoc(), inputArg.getType(), inputArg);
    frame.setValue(call->getIdentifier(), result);
    return true;
}


template <typename T>   
bool replaceWithIntrinsic_3(std::unique_ptr<::mlir::OpBuilder>& builder,
                    const compiler::ir::ProxyCallOperation* call,
                    MLIRLoweringProvider::ValueFrame& frame) {
    auto inputArg = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
    auto inputArg2 = frame.getValue(call->getInputArguments().at(1)->getIdentifier());
    auto inputArg3 = frame.getValue(call->getInputArguments().at(2)->getIdentifier());
   
	auto result = builder->create<T>(builder->getUnknownLoc(), inputArg.getType(), inputArg, inputArg2, inputArg3);
    frame.setValue(call->getIdentifier(), result);
    return true;
}

void MLIRMathIntrinsicPlugin::registerIntrinsics(MLIRIntrinsicManager& manager) {
	// Register abs intrinsic
	{
		manager.addIntrinsic(
		    reinterpret_cast<void*>(static_cast<int32_t(*)(int32_t)>(&std::abs)),
		    replaceWithIntrinsic<::mlir::LLVM::AbsOp>);
        manager.addIntrinsic(
		    reinterpret_cast<void*>(static_cast<int64_t(*)(int64_t)>(&std::abs)),
		    replaceWithIntrinsic<::mlir::LLVM::AbsOp>);    
        manager.addIntrinsic(
		    reinterpret_cast<void*>(static_cast<float(*)(float)>(&std::fabs)),
		    replaceWithIntrinsic<::mlir::LLVM::FAbsOp>);    
        manager.addIntrinsic(
		    reinterpret_cast<void*>(static_cast<double(*)(double)>(&std::fabs)),
		    replaceWithIntrinsic<::mlir::LLVM::FAbsOp>);        
	} 
	// Register fmod intrinsic
	{
		manager.addIntrinsic(
		    reinterpret_cast<void*>(static_cast<float(*)(float, float)>(&std::fmod)),
		    [](std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
		       MLIRLoweringProvider::ValueFrame& frame) -> bool {
			    auto x = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
			    auto y = frame.getValue(call->getInputArguments().at(1)->getIdentifier());
			    auto result = builder->create<::mlir::LLVM::FRemOp>(builder->getUnknownLoc(), x.getType(), x, y);
			    frame.setValue(call->getIdentifier(), result);
			    return true;
		    });
		manager.addIntrinsic(
		    reinterpret_cast<void*>(static_cast<double(*)(double, double)>(&std::fmod)),
		    [](std::unique_ptr<::mlir::OpBuilder>& builder, const compiler::ir::ProxyCallOperation* call,
		       MLIRLoweringProvider::ValueFrame& frame) -> bool {
			    auto x = frame.getValue(call->getInputArguments().at(0)->getIdentifier());
			    auto y = frame.getValue(call->getInputArguments().at(1)->getIdentifier());
			    auto result = builder->create<::mlir::LLVM::FRemOp>(builder->getUnknownLoc(), x.getType(), x, y);
			    frame.setValue(call->getIdentifier(), result);
			    return true;
		    });         
	}	
	// Register fma intrinsic
	{
		manager.addIntrinsic(
		    reinterpret_cast<void*>(static_cast<float(*)(float, float, float)>(&std::fma)),
		    replaceWithIntrinsic_3<::mlir::LLVM::FMAOp>);
		manager.addIntrinsic(
		    reinterpret_cast<void*>(static_cast<double(*)(double, double, double)>(&std::fma)),
		    replaceWithIntrinsic_3<::mlir::LLVM::FMAOp>);         
	}
	// Register sin intrinsic
	{
		manager.addIntrinsic(
		    reinterpret_cast<void*>(static_cast<float(*)(float)>(&std::sin)),
		    replaceWithIntrinsic<::mlir::LLVM::SinOp>);
        manager.addIntrinsic(
		    reinterpret_cast<void*>(static_cast<double(*)(double)>(&std::sin)),
		    replaceWithIntrinsic<::mlir::LLVM::SinOp>);         
	}

	// Register exp intrinsic
	{
		manager.addIntrinsic(
		    reinterpret_cast<void*>(static_cast<float(*)(float)>(&std::exp)),
		    replaceWithIntrinsic<::mlir::LLVM::ExpOp>);
		manager.addIntrinsic(
		    reinterpret_cast<void*>(static_cast<double(*)(double)>(&std::exp)),
		    replaceWithIntrinsic<::mlir::LLVM::ExpOp>);         
	}

	// Register exp2 intrinsic
	{
		manager.addIntrinsic(
		    reinterpret_cast<void*>(static_cast<float(*)(float)>(&std::exp2)),
		    replaceWithIntrinsic<::mlir::LLVM::Exp2Op>);
		manager.addIntrinsic(
		    reinterpret_cast<void*>(static_cast<double(*)(double)>(&std::exp2)),
		    replaceWithIntrinsic<::mlir::LLVM::Exp2Op>);         
	}
}

void RegisterMLIRMathIntrinsicPlugin() {
	MLIRIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<MLIRMathIntrinsicPlugin>());
}

} // namespace nautilus
