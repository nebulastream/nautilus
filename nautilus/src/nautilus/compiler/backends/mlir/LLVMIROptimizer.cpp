

#include "nautilus/compiler/backends/mlir/LLVMIROptimizer.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/FileCollector.h>
#include <mlir/ExecutionEngine/OptUtils.h>

namespace nautilus::compiler::mlir {

std::function<llvm::Error(llvm::Module*)> LLVMIROptimizer::getLLVMOptimizerPipeline() {
	// Return LLVM optimizer pipeline.
	return [](llvm::Module* llvmIRModule) {
		// Currently, we do not increase the sizeLevel requirement of the
		// optimizingTransformer beyond 0.
		constexpr int SIZE_LEVEL = 0;
		// Create A target-specific target machine for the host
		auto tmBuilderOrError = llvm::orc::JITTargetMachineBuilder::detectHost();
		// NES_ASSERT2_FMT(tmBuilderOrError, "Failed to create a
		// JITTargetMachineBuilder for the host");
		auto targetMachine = tmBuilderOrError->createTargetMachine();
		llvm::TargetMachine* targetMachinePtr = targetMachine->get();
		targetMachinePtr->setOptLevel(llvm::CodeGenOptLevel::Aggressive);

		// Add target-specific attributes to the 'execute' function.
		llvmIRModule->getFunction("execute")->addAttributeAtIndex(~0, llvm::Attribute::get(llvmIRModule->getContext(), "target-cpu", targetMachinePtr->getTargetCPU()));
		llvmIRModule->getFunction("execute")->addAttributeAtIndex(~0, llvm::Attribute::get(llvmIRModule->getContext(), "target-features", targetMachinePtr->getTargetFeatureString()));
		llvmIRModule->getFunction("execute")->addAttributeAtIndex(~0, llvm::Attribute::get(llvmIRModule->getContext(), "tune-cpu", targetMachinePtr->getTargetCPU()));
		llvm::SMDiagnostic Err;

		// Load LLVM IR module from proxy inlining input path (We assert that it
		// exists in CompilationOptions). if (options.isProxyInlining()) {
		//     auto proxyFunctionsIR =
		//     llvm::parseIRFile(options.getProxyInliningInputPath(), Err,
		//     llvmIRModule->getContext());
		// Link the module with our generated LLVM IR module and optimize the linked
		// LLVM IR module (inlining happens during optimization).
		//     llvm::Linker::linkModules(*llvmIRModule, std::move(proxyFunctionsIR),
		//     llvm::Linker::Flags::OverrideFromSrc);
		// }
		auto optPipeline = ::mlir::makeOptimizingTransformer(3, SIZE_LEVEL, targetMachinePtr);
		auto optimizedModule = optPipeline(llvmIRModule);

		// Print debug information to file/console if set in options.

		std::string llvmIRString;
		llvm::raw_string_ostream llvmStringStream(llvmIRString);
		llvmIRModule->print(llvmStringStream, nullptr);
		// auto* basicError = new std::error_code();
		std::cout << llvmIRString << std::endl;
		/*if (options.isDumpToConsole() || options.isDumpToFile()) {
		    // Write the llvmIRModule to a string.
		    std::string llvmIRString;
		    llvm::raw_string_ostream llvmStringStream(llvmIRString);
		    llvmIRModule->print(llvmStringStream, nullptr);
		    auto* basicError = new std::error_code();

		    // Dump the generated llvmIRModule.
		    dumpHelper.dump("5. LLVM.ll", llvmIRString);
		}
		 */
		return optimizedModule;
	};
}
} // namespace nautilus::compiler::mlir
