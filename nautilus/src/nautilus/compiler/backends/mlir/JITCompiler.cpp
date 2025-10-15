
#include "nautilus/compiler/backends/mlir/JITCompiler.hpp"
#include "fmt/format.h"
#include "nautilus/compiler/backends/mlir/LLVMInliningUtils.hpp"
#include "nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp"
#include "nautilus/inline.hpp"
#include <mlir/ExecutionEngine/OptUtils.h>
#include <mlir/Target/LLVMIR/Dialect/Builtin/BuiltinToLLVMIRTranslation.h>
#include <mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h>
#include <mlir/Target/LLVMIR/Export.h>

namespace nautilus::compiler::mlir {

std::unique_ptr<::mlir::ExecutionEngine> JITCompiler::jitCompileModule(
    ::mlir::OwningOpRef<::mlir::ModuleOp>& mlirModule, const llvm::function_ref<llvm::Error(llvm::Module*)> optPipeline,
    const std::vector<std::string>& jitProxyFunctionSymbols, const std::vector<void*>& jitProxyFunctionTargetAddresses,
    const engine::Options& nautilusOptions) {

	// Register the translation from MLIR to LLVM IR, which must happen before we
	// can JIT-compile.
	::mlir::registerBuiltinDialectTranslation(*mlirModule->getContext());
	::mlir::registerLLVMDialectTranslation(*mlirModule->getContext());

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

		if (nautilusOptions.getOptionOrDefault("mlir.inline_invoke_calls", false)) {
			// TODO possible room for some optimization as it always adds *all* symbols from the symbol registry,
			// regardless of whether they are used

			// iterate over the symbol table of the inline registry and the symbols to the symbol map
			// this is used to link inlined functions against the host program
			for (const auto& [key, value] : InlineFunctionRegistry::instance().getSymbolTable()) {
				// translate the runtime function address to hex representation to match the names used by the inline
				// functions
				auto hexStr = ptrToHex(value);
				symbolMap[interner(hexStr)] = {llvm::orc::ExecutorAddr::fromPtr(value), llvm::JITSymbolFlags::Exported};
			}
		}
		return symbolMap;
	};
	auto& engine = maybeEngine.get();
	engine->registerSymbols(runtimeSymbolMap);
	return std::move(engine);
}
} // namespace nautilus::compiler::mlir
