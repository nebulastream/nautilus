

#include "nautilus/compiler/backends/mlir/LLVMIROptimizer.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/FileCollector.h>
#include <mlir/ExecutionEngine/OptUtils.h>

namespace nautilus::compiler::mlir {

int getOptimizationLevel(const engine::Options& options) {
	return options.getOptionOrDefault("mlir.optimizationLevel", 3);
}

LLVMIROptimizer::LLVMIROptimizer() = default;
LLVMIROptimizer::~LLVMIROptimizer() = default;

std::function<llvm::Error(llvm::Module*)> LLVMIROptimizer::getLLVMOptimizerPipeline(const engine::Options& options,
                                                                                    const DumpHandler& handler) {
	// Return LLVM optimizer pipeline.
	return [options, &handler](llvm::Module* llvmIRModule) {
		// Currently, we do not increase the sizeLevel requirement of the
		// optimizingTransformer beyond 0.
		constexpr int SIZE_LEVEL = 0;
		// Create A target-specific target machine for the host
		auto tmBuilderOrError = llvm::orc::JITTargetMachineBuilder::detectHost();
		auto targetMachine = tmBuilderOrError->createTargetMachine();
		llvm::TargetMachine* targetMachinePtr = targetMachine->get();
		targetMachinePtr->setOptLevel(llvm::CodeGenOptLevel::Aggressive);

		// Add target-specific attributes to all non-declaration functions in the module.
		for (auto& func : *llvmIRModule) {
			if (func.isDeclaration()) {
				continue;
			}
			func.addAttributeAtIndex(
			    ~0, llvm::Attribute::get(llvmIRModule->getContext(), "target-cpu", targetMachinePtr->getTargetCPU()));
			func.addAttributeAtIndex(~0, llvm::Attribute::get(llvmIRModule->getContext(), "target-features",
			                                                  targetMachinePtr->getTargetFeatureString()));
			func.addAttributeAtIndex(
			    ~0, llvm::Attribute::get(llvmIRModule->getContext(), "tune-cpu", targetMachinePtr->getTargetCPU()));
		}

		// Apply optional pre-optimization module transforms installed by plugins
		// (e.g. the inlining plugin's JIT-time LLVM inliner).
		if (options.getOptionOrDefault("mlir.inline_invoke_calls", false)) {
			if (const auto& hook = getLLVMBackendHooks().preOptModuleTransform) {
				hook(*llvmIRModule);
			}
		}

		auto optPipeline =
		    ::mlir::makeOptimizingTransformer(getOptimizationLevel(options), SIZE_LEVEL, targetMachinePtr);
		auto optimizedModule = optPipeline(llvmIRModule);

		handler.dump("after_llvm_generation", "ll", [&]() {
			std::string llvmIRString;
			llvm::raw_string_ostream llvmStringStream(llvmIRString);
			llvmIRModule->print(llvmStringStream, nullptr);
			return llvmIRString;
		});

		return optimizedModule;
	};
}
} // namespace nautilus::compiler::mlir
