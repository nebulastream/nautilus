
#include "nautilus/compiler/backends/mlir/JITCompiler.hpp"
#include "nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp"
#include <mlir/ExecutionEngine/OptUtils.h>
#include <mlir/Target/LLVMIR/Dialect/Builtin/BuiltinToLLVMIRTranslation.h>
#include <mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h>
#include <mlir/Target/LLVMIR/Export.h>

namespace nautilus::compiler::mlir {

std::unique_ptr<::mlir::ExecutionEngine>
JITCompiler::jitCompileModule(::mlir::OwningOpRef<::mlir::ModuleOp>& mlirModule,
                              const llvm::function_ref<llvm::Error(llvm::Module*)> optPipeline,
                              const std::vector<std::string>& jitProxyFunctionSymbols,
                              const std::vector<void*>& jitProxyFunctionTargetAddresses) {

	// Register the translation from MLIR to LLVM IR, which must happen before we
	// can JIT-compile.
	::mlir::registerBuiltinDialectTranslation(*mlirModule->getContext());
	::mlir::registerLLVMDialectTranslation(*mlirModule->getContext());

	// Convert the module to LLVM IR in a new LLVM IR context.
	llvm::LLVMContext llvmContext;
	auto llvmModule = ::mlir::translateModuleToLLVMIR(mlirModule->getOperation(), llvmContext);
	if (!llvmModule) {
		llvm::errs() << "Failed to emit LLVM IR\n";
	}

	// Initialize information about the local machine in LLVM.
	LLVMInitializeNativeTarget();
	LLVMInitializeNativeAsmPrinter();

	// Create MLIR execution engine (wrapper around LLVM ExecutionEngine).
	::mlir::ExecutionEngineOptions options;
	options.jitCodeGenOptLevel = llvm::CodeGenOptLevel::Aggressive;
	options.transformer = optPipeline;
	auto maybeEngine = ::mlir::ExecutionEngine::create(*mlirModule, options);

	assert(maybeEngine && "failed to construct an execution engine");

	// We register all external functions (symbols) that we do not inline.
	const auto runtimeSymbolMap = [&](llvm::orc::MangleAndInterner interner) {
		auto symbolMap = llvm::orc::SymbolMap();
		for (int i = 0; i < (int) jitProxyFunctionSymbols.size(); ++i) {
			auto address = jitProxyFunctionTargetAddresses.at(i);
			symbolMap[interner(jitProxyFunctionSymbols.at(i))] = {llvm::orc::ExecutorAddr::fromPtr(address),
			                                                      llvm::JITSymbolFlags::Exported};
		}
		return symbolMap;
	};
	auto& engine = maybeEngine.get();
	engine->registerSymbols(runtimeSymbolMap);
	return std::move(engine);
}
} // namespace nautilus::compiler::mlir
