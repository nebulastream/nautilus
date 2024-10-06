

#include "nautilus/compiler/backends/mlir/MLIRUtility.hpp>
#include "nautilus/compiler/backends/mlir/JITCompiler.hpp>
#include "nautilus/compiler/backends/mlir/LLVMIROptimizer.hpp>
#include "nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp>
#include "nautilus/compiler/backends/mlir/MLIRPassManager.hpp>
#include <Util/Logger/Logger.hpp>
#include <mlir/AsmParser/AsmParser.h>
#include <mlir/Parser/Parser.h>

namespace nautilus::compiler::mlir {

void MLIRUtility::writeMLIRModuleToFile(mlir::OwningOpRef<mlir::ModuleOp>& mlirModule, std::string mlirFilepath) {
	std::string mlirString;
	llvm::raw_string_ostream llvmStringStream(mlirString);
	auto* basicError = new std::error_code();
	llvm::raw_fd_ostream fileStream(mlirFilepath, *basicError);

	auto* opPrintFlags = new mlir::OpPrintingFlags();
	mlirModule->print(llvmStringStream, *opPrintFlags);
	if (!mlirFilepath.empty()) {
		fileStream.write(mlirString.c_str(), mlirString.length());
	}
	NES_DEBUG(mlirString.c_str());
}

int MLIRUtility::loadAndExecuteModuleFromString(const std::string& mlirString, const std::string& moduleString) {
	mlir::MLIRContext context;
	mlir::ParserConfig config(&context);
	auto mlirModule = mlir::parseSourceString<mlir::ModuleOp>(mlirString, config);

	// Take the MLIR module from the MLIRLoweringProvider and apply lowering and
	// optimization passes.
	if (!MLIR::MLIRPassManager::lowerAndOptimizeMLIRModule(mlirModule, {}, {})) {
		NES_FATAL_ERROR("Could not lower and optimize MLIR");
	}

	// Lower MLIR module to LLVM IR and create LLVM IR optimization pipeline.
	auto optPipeline = MLIR::LLVMIROptimizer::getLLVMOptimizerPipeline(/*inlining*/ false);

	// JIT compile LLVM IR module and return engine that provides access compiled
	// execute function.
	auto engine = MLIR::JITCompiler::jitCompileModule(mlirModule, optPipeline, {}, {});
	if (!engine->invoke(moduleString)) {
		return -1;
	} else
		return 0;
}

std::unique_ptr<mlir::ExecutionEngine>
MLIRUtility::compileNESIRToMachineCode(std::shared_ptr<NES::Nautilus::IR::IRGraph> ir) {
	mlir::MLIRContext context;
	auto loweringProvider = std::make_unique<MLIR::MLIRLoweringProvider>(context);
	auto module = loweringProvider->generateModuleFromIR(ir);
	// Take the MLIR module from the MLIRLoweringProvider and apply lowering and
	// optimization passes.
	if (MLIR::MLIRPassManager::lowerAndOptimizeMLIRModule(module, {}, {})) {
		NES_FATAL_ERROR("Could not lower and optimize MLIR");
	}

	// Lower MLIR module to LLVM IR and create LLVM IR optimization pipeline.
	auto optPipeline = MLIR::LLVMIROptimizer::getLLVMOptimizerPipeline(/*inlining*/ false);

	// JIT compile LLVM IR module and return engine that provides access compiled
	// execute function.
	return MLIR::JITCompiler::jitCompileModule(module, optPipeline, loweringProvider->getJitProxyFunctionSymbols(),
	                                           loweringProvider->getJitProxyTargetAddresses());
}
} // namespace nautilus::compiler::mlir
