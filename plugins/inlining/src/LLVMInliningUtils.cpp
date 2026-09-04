#include "LLVMInliningUtils.hpp"
#include "fmt/format.h"
#include "nautilus/inline.hpp"
#include "llvm/ADT/StringSet.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/Linker/Linker.h"
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/MemoryBuffer.h>
#include <stdexcept>

namespace nautilus::compiler::mlir {

namespace {

[[noreturn]] void throwInvalidUDF(llvm::StringRef entryPoint, llvm::StringRef reason) {
	throw std::runtime_error(fmt::format("Invalid LLVM module for UDF '{}': {}", entryPoint, reason));
}

std::unique_ptr<llvm::Module> parseUDFModule(llvm::StringRef entryPoint, const std::string& bitcode,
                                             llvm::LLVMContext& context) {
	auto buffer = llvm::MemoryBuffer::getMemBuffer(llvm::StringRef(bitcode.data(), bitcode.size()), "", false);
	auto moduleOrError = llvm::parseBitcodeFile(buffer->getMemBufferRef(), context);
	if (!moduleOrError) {
		throwInvalidUDF(entryPoint, llvm::toString(moduleOrError.takeError()));
	}
	return std::move(*moduleOrError);
}

void validateUDFModule(llvm::StringRef entryPoint, const llvm::Module& udfModule, const llvm::Module& targetModule) {
	const auto* entryFunction = udfModule.getFunction(entryPoint);
	if (entryFunction == nullptr || entryFunction->isDeclaration()) {
		throwInvalidUDF(entryPoint, "the named entry point is not defined");
	}
	if (entryFunction->hasLocalLinkage()) {
		throwInvalidUDF(entryPoint, "the entry point must have external linkage");
	}

	for (const auto& function : udfModule) {
		if (!function.isDeclaration() && &function != entryFunction && !function.hasLocalLinkage()) {
			throwInvalidUDF(entryPoint,
			                fmt::format("helper function '{}' must be internal or private", function.getName()));
		}
	}
	for (const auto& global : udfModule.globals()) {
		if (!global.isDeclaration() && !global.hasLocalLinkage() && !global.getName().starts_with("llvm.")) {
			throwInvalidUDF(entryPoint, fmt::format("global '{}' must be internal or private", global.getName()));
		}
	}
	for (const auto& alias : udfModule.aliases()) {
		if (!alias.hasLocalLinkage()) {
			throwInvalidUDF(entryPoint, fmt::format("alias '{}' must be internal or private", alias.getName()));
		}
	}
	for (const auto& ifunc : udfModule.ifuncs()) {
		if (!ifunc.hasLocalLinkage()) {
			throwInvalidUDF(entryPoint,
			                fmt::format("indirect function '{}' must be internal or private", ifunc.getName()));
		}
	}

	if (const auto* existing = targetModule.getFunction(entryPoint)) {
		if (!existing->isDeclaration()) {
			throwInvalidUDF(entryPoint, "the generated module already defines that symbol");
		}
		if (existing->getFunctionType() != entryFunction->getFunctionType()) {
			throwInvalidUDF(entryPoint, "the entry-point signature does not match the invoke declaration");
		}
	}
}

void mergeUDFModules(llvm::Module& targetModule, const engine::UDFRegistry::Snapshot& registry) {
	if (registry == nullptr) {
		return;
	}

	for (const auto& [entryPoint, bitcode] : *registry) {
		auto udfModule = parseUDFModule(entryPoint, bitcode, targetModule.getContext());
		validateUDFModule(entryPoint, *udfModule, targetModule);

		// The UDF is compiled for this process. Adopt the generated module's
		// target metadata and let LLVM uniquify local helper symbols.
		udfModule->setTargetTriple(targetModule.getTargetTriple());
		udfModule->setDataLayout(targetModule.getDataLayout().getStringRepresentation());
		if (llvm::Linker::linkModules(targetModule, std::move(udfModule))) {
			throwInvalidUDF(entryPoint, "LLVM could not merge the module");
		}

		auto* mergedEntry = targetModule.getFunction(entryPoint);
		if (mergedEntry == nullptr || mergedEntry->isDeclaration()) {
			throwInvalidUDF(entryPoint, "the entry point disappeared while merging the module");
		}
		mergedEntry->addFnAttr(llvm::Attribute::AlwaysInline);
		mergedEntry->removeFnAttr(llvm::Attribute::NoInline);
		mergedEntry->removeFnAttr(llvm::Attribute::OptimizeNone);
	}
}

} // namespace

void* hexToPtr(const llvm::StringRef& hexString) {
	if (hexString.starts_with("0x")) {
		uintptr_t addr;
		if (!hexString.getAsInteger(0, addr)) {
			return reinterpret_cast<void*>(addr);
		}
	}
	return nullptr;
}

std::string ptrToHex(const void* ptr) {
	return fmt::format("0x{:X}", reinterpret_cast<uintptr_t>(ptr));
}

std::optional<std::unique_ptr<llvm::Module>>
loadBitcodeIfAvailable(void* fnPtr, llvm::LLVMContext& ctx, const std::unordered_map<std::string, void*>& symbolTable) {
	auto bitcodeStr = InlineFunctionRegistry::instance().getBitcode(fnPtr);
	if (!bitcodeStr.has_value()) {
		return std::nullopt;
	}

	auto buffer = llvm::MemoryBuffer::getMemBuffer(llvm::StringRef(bitcodeStr->data(), bitcodeStr->size()), "", false);
	auto moduleOrErr = llvm::parseBitcodeFile(buffer->getMemBufferRef(), ctx);
	if (!moduleOrErr) {
		logAllUnhandledErrors(moduleOrErr.takeError(), llvm::errs(), "Bitcode parse error: ");
		return std::nullopt;
	}

	auto& inlineModule = **moduleOrErr;
	for (auto& func : inlineModule) {
		if (!func.isDeclaration() || func.isIntrinsic()) {
			continue;
		}
		auto it = symbolTable.find(func.getName().str());
		if (it == symbolTable.end()) {
			llvm::errs() << "Symbol registry error. Undefined function " << func.getName()
			             << " not contained in symbol registry.\n";
			return std::nullopt;
		}
		auto hexStr = ptrToHex(it->second);
		if (auto* existingFunc = inlineModule.getFunction(hexStr)) {
			func.replaceAllUsesWith(existingFunc);
			func.removeFromParent();
		} else {
			func.setName(hexStr);
		}
	}

	for (auto& globalVar : inlineModule.globals()) {
		if (!globalVar.isDeclaration()) {
			continue;
		}
		auto it = symbolTable.find(globalVar.getName().str());
		if (it == symbolTable.end()) {
			llvm::errs() << "Symbol registry error. Global variable " << globalVar.getName()
			             << " not contained in symbol registry.\n";
			return std::nullopt;
		}
		auto hexStr = ptrToHex(it->second);
		if (auto* existingGV = inlineModule.getGlobalVariable(hexStr)) {
			globalVar.replaceAllUsesWith(existingGV);
			globalVar.removeFromParent();
		} else {
			globalVar.setName(hexStr);
		}
	}

	return std::optional(std::move(*moduleOrErr));
}

void fixFunctionNameConflicts(const llvm::Module& moduleToOptimize, llvm::Module& inlineModule) {
	llvm::StringSet<> hostFunctionNames;
	for (const auto& func : moduleToOptimize) {
		hostFunctionNames.insert(func.getName());
	}

	for (auto& func : inlineModule) {
		if (!func.hasFnAttribute("is_target") || !hostFunctionNames.contains(func.getName())) {
			continue;
		}
		auto originalName = func.getName().str();
		for (int i = 0;; ++i) {
			auto newName = originalName + "_" + std::to_string(i);
			if (!hostFunctionNames.contains(newName)) {
				func.setName(newName);
				break;
			}
		}
	}
}

constexpr int MAX_INLINE_ITERATIONS = 32;

void inlineFunctions(llvm::Module& moduleToOptimize, const engine::UDFRegistry::Snapshot& udfRegistry) {
	// A symbolic invoke already emitted a declaration with the UDF's public
	// entry-point name. Merging resolves it; local helper names stay isolated.
	mergeUDFModules(moduleToOptimize, udfRegistry);

	const auto symbolTable = InlineFunctionRegistry::instance().getSymbolTable();
	std::unordered_map<void*, llvm::Function*> inlinedFunctions {};
	bool doAnotherIteration;
	int iteration = 0;
	do {
		if (++iteration > MAX_INLINE_ITERATIONS) {
			llvm::errs() << "inlineFunctions: fixed-point iteration cap (" << MAX_INLINE_ITERATIONS
			             << ") reached; aborting further inlining\n";
			break;
		}
		doAnotherIteration = false;
		std::vector<llvm::Function*> functionListView;
		for (auto& function : moduleToOptimize) {
			functionListView.push_back(&function);
		}

		for (auto* originalFunction : functionListView) {
			if (originalFunction->isIntrinsic() || !originalFunction->isDeclaration()) {
				continue;
			}

			void* fnPtr = hexToPtr(originalFunction->getName());
			if (!fnPtr) {
				continue;
			}
			if (auto it = inlinedFunctions.find(fnPtr); it != inlinedFunctions.end()) {
				originalFunction->replaceAllUsesWith(it->second);
				originalFunction->removeFromParent();
				continue;
			}

			auto optInlineModule = loadBitcodeIfAvailable(fnPtr, moduleToOptimize.getContext(), symbolTable);
			if (!optInlineModule.has_value()) {
				continue;
			}
			auto inlineModule = std::move(optInlineModule.value());
			fixFunctionNameConflicts(moduleToOptimize, *inlineModule);

			std::string inlinableFunctionName;
			for (auto& function : *inlineModule) {
				if (!function.isDeclaration() && function.hasFnAttribute("is_target")) {
					inlinableFunctionName = function.getName().str();
					break;
				}
			}

			inlineModule->setTargetTriple(moduleToOptimize.getTargetTriple());
			inlineModule->setDataLayout(moduleToOptimize.getDataLayout().getStringRepresentation());
			if (moduleToOptimize.getFunction(inlinableFunctionName) != nullptr) {
				llvm::errs() << "Inlining skipped: symbol '" << inlinableFunctionName << "' doubly defined.\n";
				continue;
			}
			if (llvm::Linker::linkModules(moduleToOptimize, std::move(inlineModule))) {
				llvm::errs() << "Failed to link modules\n";
				continue;
			}

			auto* inlinableFunction = moduleToOptimize.getFunction(inlinableFunctionName);
			if (inlinableFunction == nullptr || inlinableFunction->isDeclaration()) {
				llvm::errs() << "Failed to find inline function after linking modules '" << inlinableFunctionName
				             << "'\n";
				continue;
			}
			inlinableFunction->addFnAttr(llvm::Attribute::AlwaysInline);
			inlinableFunction->removeFnAttr(llvm::Attribute::NoInline);
			inlinableFunction->removeFnAttr(llvm::Attribute::OptimizeNone);
			inlinedFunctions.insert({fnPtr, inlinableFunction});

			originalFunction->replaceAllUsesWith(inlinableFunction);
			originalFunction->removeFromParent();
			doAnotherIteration = true;
		}
	} while (doAnotherIteration);

	StripDebugInfo(moduleToOptimize);
}
} // namespace nautilus::compiler::mlir
