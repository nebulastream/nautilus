
#include "nautilus/compiler/backends/mlir/JITCompiler.hpp"
#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <llvm/Support/Error.h>
#include <llvm/Support/MemoryBuffer.h>
#include <mlir/Target/LLVMIR/Dialect/Builtin/BuiltinToLLVMIRTranslation.h>
#include <mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h>

namespace nautilus::compiler::mlir {
namespace {

std::unique_ptr<MLIRJit> configureJit(llvm::Expected<std::unique_ptr<MLIRJit>> maybeJit,
                                      const std::vector<std::string>& symbols, const std::vector<void*>& addresses) {
	if (!maybeJit) {
		throw RuntimeException("Could not construct MLIR JIT: " + llvm::toString(maybeJit.takeError()));
	}
	if (symbols.size() != addresses.size()) {
		throw RuntimeException("MLIR JIT symbol and address vectors differ in size");
	}
	auto jit = std::move(*maybeJit);
	const auto runtimeSymbolMap = [&](llvm::orc::MangleAndInterner interner) {
		auto symbolMap = llvm::orc::SymbolMap();
		for (std::size_t index = 0; index < symbols.size(); ++index) {
			if (symbols[index].empty() || addresses[index] == nullptr) {
				throw RuntimeException("MLIR JIT external symbol is invalid");
			}
			const auto symbol = interner(symbols[index]);
			const auto address = llvm::orc::ExecutorAddr::fromPtr(addresses[index]);
			if (const auto found = symbolMap.find(symbol);
			    found != symbolMap.end() && found->second.getAddress() != address) {
				throw RuntimeException("MLIR JIT external symbol has conflicting addresses");
			}
			symbolMap[symbol] = {address, llvm::JITSymbolFlags::Exported};
		}
		if (const auto& hook = getLLVMBackendHooks().jitSymbolContributor) {
			hook([&](const std::string& name, void* address) {
				if (name.empty() || address == nullptr) {
					throw RuntimeException("MLIR JIT contributed symbol is invalid");
				}
				const auto symbol = interner(name);
				const auto target = llvm::orc::ExecutorAddr::fromPtr(address);
				if (const auto found = symbolMap.find(symbol);
				    found != symbolMap.end() && found->second.getAddress() != target) {
					throw RuntimeException("MLIR JIT contributed symbol conflicts with an external symbol");
				}
				symbolMap[symbol] = {target, llvm::JITSymbolFlags::Exported};
			});
		}
		return symbolMap;
	};
	if (auto error = jit->registerSymbols(runtimeSymbolMap)) {
		throw RuntimeException("Could not register MLIR JIT symbols: " + llvm::toString(std::move(error)));
	}
	if (auto error = jit->initialize()) {
		throw RuntimeException("Could not initialize MLIR JIT: " + llvm::toString(std::move(error)));
	}
	return jit;
}

} // namespace

std::unique_ptr<MLIRJit> JITCompiler::jitCompileModule(
    ::mlir::OwningOpRef<::mlir::ModuleOp>& mlirModule, llvm::function_ref<llvm::Error(llvm::Module*)> optPipeline,
    const std::vector<std::string>& jitProxyFunctionSymbols, const std::vector<void*>& jitProxyFunctionTargetAddresses,
    llvm::CodeGenOptLevel codeGenOptLevel, bool enableDebuggerSupport, bool enablePerfSupport, bool perfEmitDebugInfo,
    bool perfEmitUnwindInfo, bool perfRegionSymbols, bool enableJitSymbolRegistration,
    const std::string& compilationUnitId, std::shared_ptr<MLIRJit::ObjectCapture> objectCapture) {
	::mlir::registerBuiltinDialectTranslation(*mlirModule->getContext());
	::mlir::registerLLVMDialectTranslation(*mlirModule->getContext());

	MLIRJit::Options jitOptions;
	jitOptions.codeGenOptLevel = codeGenOptLevel;
	jitOptions.transformer = optPipeline;
	jitOptions.enableDebuggerSupport = enableDebuggerSupport;
	jitOptions.enablePerfSupport = enablePerfSupport;
	jitOptions.perfEmitDebugInfo = perfEmitDebugInfo;
	jitOptions.perfEmitUnwindInfo = perfEmitUnwindInfo;
	jitOptions.perfRegionSymbols = perfRegionSymbols;
	jitOptions.enableJitSymbolRegistration = enableJitSymbolRegistration;
	jitOptions.compilationUnitId = compilationUnitId;
	jitOptions.objectCapture = std::move(objectCapture);
	return configureJit(MLIRJit::create(*mlirModule, jitOptions), jitProxyFunctionSymbols,
	                    jitProxyFunctionTargetAddresses);
}

std::unique_ptr<MLIRJit> JITCompiler::jitLoadObject(std::string_view object, const std::vector<std::string>& symbols,
                                                    const std::vector<void*>& addresses,
                                                    const MLIRJit::Options& options) {
	auto objectBuffer =
	    llvm::MemoryBuffer::getMemBufferCopy(llvm::StringRef(object.data(), object.size()), "cached-module.o");
	return configureJit(MLIRJit::createFromObject(std::move(objectBuffer), options), symbols, addresses);
}

} // namespace nautilus::compiler::mlir
