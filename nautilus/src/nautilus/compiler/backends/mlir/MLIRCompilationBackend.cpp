

#include "nautilus/compiler/backends/mlir/MLIRCompilationBackend.hpp"
#include "nautilus/compiler/backends/mlir/JITCompiler.hpp"
#include "nautilus/compiler/backends/mlir/LLVMIROptimizer.hpp"
#include "nautilus/compiler/backends/mlir/MLIRExecutable.hpp"
#include "nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp"
#include "nautilus/compiler/backends/mlir/MLIRPassManager.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include <iostream>
#include <mlir/Dialect/Func/Extensions/AllExtensions.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/Target/LLVMIR/Dialect/Builtin/BuiltinToLLVMIRTranslation.h>
#include <mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h>

namespace nautilus::compiler::mlir {

std::unique_ptr<Executable> MLIRCompilationBackend::compile(const std::shared_ptr<ir::IRGraph>& ir, const DumpHandler& dumpHandler, const engine::Options& options) {

	// 1. Create the MLIRLoweringProvider and lower the given NESIR. Return an
	// MLIR module.
	::mlir::DialectRegistry registry;
	::mlir::func::registerAllExtensions(registry);
	registerBuiltinDialectTranslation(registry);
	registerLLVMDialectTranslation(registry);

	::mlir::MLIRContext context(registry);
	context.allowsUnregisteredDialects();

	auto loweringProvider = std::make_unique<MLIRLoweringProvider>(context);
	auto mlirModule = loweringProvider->generateModuleFromIR(ir);
	if (*mlirModule == nullptr) {
		throw RuntimeException("verification of MLIR module failed!");
	};

	// 2.a dump MLIR to console or a file
	dumpHandler.dump("mlir", [&]() {
		::mlir::OpPrintingFlags flags;
		std::string result;
		auto output = llvm::raw_string_ostream(result);
		mlirModule->print(output, flags);
		return result;
	});

	// 2.b Take the MLIR module from the MLIRLoweringProvider and apply lowering
	// and optimization passes.
	if (mlir::MLIRPassManager::lowerAndOptimizeMLIRModule(mlirModule, {}, {})) {
		throw RuntimeException("Could not lower and optimize MLIR module.");
	}

	// 3. Lower MLIR module to LLVM IR and create LLVM IR optimization pipeline.
	auto optPipeline = LLVMIROptimizer::getLLVMOptimizerPipeline(options, dumpHandler);

	// 4. JIT compile LLVM IR module and return engine that provides access
	// compiled execute function.
	auto engine = JITCompiler::jitCompileModule(mlirModule, optPipeline, loweringProvider->getJitProxyFunctionSymbols(), loweringProvider->getJitProxyTargetAddresses());

	// 5. Get execution function from engine. Create and return execution context.
	return std::make_unique<MLIRExecutable>(std::move(engine));
}

} // namespace nautilus::compiler::mlir
