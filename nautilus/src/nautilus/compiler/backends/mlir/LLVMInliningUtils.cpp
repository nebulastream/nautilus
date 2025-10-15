#include "nautilus/compiler/backends/mlir/LLVMInliningUtils.hpp"
#include "fmt/format.h"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/inline.hpp"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Linker/Linker.h"
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/FileCollector.h>
#include <llvm/Transforms/Utils/Cloning.h>

namespace nautilus::compiler::mlir {

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

// look up invoked function in the bitcode registry and retrieve if available
std::optional<std::unique_ptr<llvm::Module>> loadBitcodeIfAvailable(void* fnPtr, llvm::LLVMContext& ctx) {
	// look up the function pointer in the bitcode registry
	auto bitcodeStr = InlineFunctionRegistry::instance().getBitcode(fnPtr);
	if (bitcodeStr.empty()) {
		return std::nullopt; // function not found in registry, cant inline
	}

	// deserialize bitcode module
	auto buffer = llvm::MemoryBuffer::getMemBuffer(llvm::StringRef(bitcodeStr.data(), bitcodeStr.size()), "", false);
	llvm::Expected<std::unique_ptr<llvm::Module>> moduleOrErr = llvm::parseBitcodeFile(buffer->getMemBufferRef(), ctx);
	if (!moduleOrErr) {
		logAllUnhandledErrors(moduleOrErr.takeError(), llvm::errs(), "Bitcode parse error: ");
		return std::nullopt; // if deserialization fails, fall back to regular invocation
	}

	// rewrite names of function dependencies to runtime addresses for linkage + recursive inlining
	for (auto& func : *moduleOrErr.get()) {
		if (func.isDeclaration() && !func.isIntrinsic()) {
			auto it = InlineFunctionRegistry::instance().getSymbolTable().find(func.getName().str());
			if (it != InlineFunctionRegistry::instance().getSymbolTable().end()) {
				auto hexStr = ptrToHex(it->second);

				auto* existingFunc = moduleOrErr.get()->getFunction(hexStr);
				if (!existingFunc) {
					func.setName(hexStr);
				} else {
					func.replaceAllUsesWith(existingFunc);
					func.removeFromParent();
				}
			} else {
				llvm::errs() << "Symbol registry error. Undefined function " << func.getName()
				             << " not contained in symbol registry.\n";
				return std::nullopt;
			}
		}
	}

	// same for external global variables
	for (auto& globalVar : moduleOrErr.get()->globals()) {
		if (globalVar.isDeclaration()) {
			auto it = InlineFunctionRegistry::instance().getSymbolTable().find(globalVar.getName().str());
			if (it != InlineFunctionRegistry::instance().getSymbolTable().end()) {
				auto hexStr = ptrToHex(it->second);

				auto* existingGV = moduleOrErr.get()->getGlobalVariable(hexStr);
				if (!existingGV) {
					globalVar.setName(hexStr);
				} else {
					globalVar.replaceAllUsesWith(existingGV);
					globalVar.removeFromParent();
				}
			} else {
				llvm::errs() << "Symbol registry error. Global variable " << globalVar.getName()
				             << " not contained in symbol registry.\n";
				return std::nullopt;
			}
		}
	}

	return std::optional(std::move(*moduleOrErr));
}

void fixFunctionNameConflicts(const llvm::Module& moduleToOptimize, llvm::Module& inlineModule) {
	/* inlining may introduce symbol conflicts that don't occur in the host program
	 * e.g. two internally linked functions from different source files with the same mangled name
	 * If that's the case, we need to update function names to avoid conflicts
	 * target functions that refer to the same host function are correctly deduplicated before using the function ptr
	 */
	for (auto& func1 : inlineModule) {
		if (!func1.hasFnAttribute("is_target")) {
			continue;
		}
		int i = 0;
		auto originalName = func1.getName().str();
		while (moduleToOptimize.getFunction(func1.getName())) {
			auto newName = originalName + "_" + std::to_string(i++);
			func1.setName(newName);
		}
	}
}

void inlineFunctions(llvm::Module& moduleToOptimize) {
	// repeat until module wasnt modified during an iteration
	// this is key for recursive inlining, where inlinable candidates may appear later during processing
	std::unordered_map<void*, llvm::Function*> inlinedFunctions {};
	bool doAnotherIteration; // true if there could still be inlinable functions
	do {
		doAnotherIteration = false;
		std::vector<llvm::Function*> functionListView;
		for (auto& F : moduleToOptimize) {
			functionListView.push_back(&F);
		}

		// iterate over all function calls in the LLVM module
		for (auto originalFunction : functionListView) {
			if (originalFunction->isIntrinsic() || !originalFunction->isDeclaration()) {
				continue;
			}

			// try to parse function name into a function pointer
			// (the name of an inlinable function will represent the runtime address of the function in hex notation)
			void* fnPtr = hexToPtr(originalFunction->getName());
			if (!fnPtr) {
				continue; // not a valid pointer, cant be inlined
			}

			// deduplicate functions that were previously retrieved already
			if (auto it = inlinedFunctions.find(fnPtr); it != inlinedFunctions.end()) {
				originalFunction->replaceAllUsesWith(it->second);
				originalFunction->removeFromParent();
				continue;
			}

			// try to load bitcode for the current function
			auto optInlineModule = loadBitcodeIfAvailable(fnPtr, moduleToOptimize.getContext());
			if (!optInlineModule.has_value()) {
				continue;
			}
			auto inlineModule = std::move(optInlineModule.value());

			fixFunctionNameConflicts(moduleToOptimize, *inlineModule);

			// find the function counterpart inside the deserialized llvm module
			std::string inlinableFunctionFName;
			for (auto& func : *inlineModule) {
				if (!func.isDeclaration() && func.hasFnAttribute("is_target")) {
					inlinableFunctionFName = func.getName().str();
					break;
				}
			}

			// suppress some warnings
			inlineModule->setTargetTriple(moduleToOptimize.getTargetTriple());
			inlineModule->setDataLayout(moduleToOptimize.getDataLayout().getStringRepresentation());

			if (moduleToOptimize.getFunction(inlinableFunctionFName) != nullptr) {
				// this should never happen if the deduplication logic works correctly
				throw RuntimeException("Inlining error: symbol '" + inlinableFunctionFName + "' doubly defined");
			}

			// link original module with the inline function module (merges two llvm modules)
			if (llvm::Linker::linkModules(moduleToOptimize, std::move(inlineModule))) {
				llvm::errs() << "Failed to link modules\n";
				continue;
			}

			// find the inlinable function in the original module after linking
			llvm::Function* inlinableFunction = nullptr;
			for (auto& func : moduleToOptimize) {
				if (!func.isDeclaration() && func.getName().str() == inlinableFunctionFName) {
					inlinableFunction = &func;
					break;
				}
			}
			if (inlinableFunction == nullptr) {
				llvm::errs() << "Failed to find inline function after linking modules'" << inlinableFunctionFName
				             << "'\n";
				continue;
			}

			// set function attributes for inlining
			inlinableFunction->addFnAttr(
			    llvm::Attribute::AlwaysInline); // TODO maybe use inline hint + tuned threshold instead
			inlinableFunction->removeFnAttr(llvm::Attribute::NoInline);
			inlinableFunction->removeFnAttr(llvm::Attribute::OptimizeNone);
			inlinedFunctions.insert({fnPtr, inlinableFunction});

			// replace uses of original function with inlinable version and give inlining instruction to optimizer
			originalFunction->replaceAllUsesWith(inlinableFunction);
			originalFunction->removeFromParent();
			doAnotherIteration = true;
		}
	} while (doAnotherIteration);

	StripDebugInfo(moduleToOptimize); // suppress more warnings from function cloning
}
} // namespace nautilus::compiler::mlir
