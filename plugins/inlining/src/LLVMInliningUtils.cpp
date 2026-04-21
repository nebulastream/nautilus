#include "LLVMInliningUtils.hpp"
#include "fmt/format.h"
#include "nautilus/inline.hpp"
#include "llvm/ADT/StringSet.h"
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
std::optional<std::unique_ptr<llvm::Module>>
loadBitcodeIfAvailable(void* fnPtr, llvm::LLVMContext& ctx, const std::unordered_map<std::string, void*>& symbolTable) {
	// look up the function pointer in the bitcode registry
	auto bitcodeStr = InlineFunctionRegistry::instance().getBitcode(fnPtr);
	if (!bitcodeStr.has_value()) {
		return std::nullopt; // function not found in registry, cant inline
	}

	// deserialize bitcode module
	auto buffer = llvm::MemoryBuffer::getMemBuffer(llvm::StringRef(bitcodeStr->data(), bitcodeStr->size()), "", false);
	llvm::Expected<std::unique_ptr<llvm::Module>> moduleOrErr = llvm::parseBitcodeFile(buffer->getMemBufferRef(), ctx);
	if (!moduleOrErr) {
		logAllUnhandledErrors(moduleOrErr.takeError(), llvm::errs(), "Bitcode parse error: ");
		return std::nullopt; // if deserialization fails, fall back to regular invocation
	}

	auto& inlineModule = **moduleOrErr;

	// rewrite names of function dependencies to runtime addresses for linkage + recursive inlining
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

	// same for external global variables
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
	/* inlining may introduce symbol conflicts that don't occur in the host program
	 * e.g. two internally linked functions from different source files with the same mangled name
	 * If that's the case, we need to update function names to avoid conflicts
	 * target functions that refer to the same host function are correctly deduplicated before using the function ptr
	 */
	// Snapshot the host module's function names once. Each subsequent rename
	// checks the set in O(1); with n `is_target` functions this replaces the
	// previous O(n * hostFnCount) scan.
	llvm::StringSet<> hostFunctionNames;
	for (const auto& func : moduleToOptimize) {
		hostFunctionNames.insert(func.getName());
	}

	for (auto& func1 : inlineModule) {
		if (!func1.hasFnAttribute("is_target")) {
			continue;
		}
		if (!hostFunctionNames.contains(func1.getName())) {
			continue;
		}
		auto originalName = func1.getName().str();
		for (int i = 0;; ++i) {
			auto newName = originalName + "_" + std::to_string(i);
			if (!hostFunctionNames.contains(newName)) {
				func1.setName(newName);
				break;
			}
		}
	}
}

// Safety cap for the fixed-point inlining loop below. Recursive inlining
// converges in practice well under this bound; if it doesn't, we prefer to
// emit a diagnostic and fall back to the partially-inlined state rather
// than hang the JIT.
constexpr int MAX_INLINE_ITERATIONS = 32;

void inlineFunctions(llvm::Module& moduleToOptimize) {
	// Snapshot the symbol table once per inlining call: the table is
	// populated by static initializers at program startup and by
	// `dlopen`-loaded shared object ctors; snapshotting avoids holding the
	// registry mutex across llvm::Linker and keeps repeated lookups cheap.
	const auto symbolTable = InlineFunctionRegistry::instance().getSymbolTable();

	// repeat until module wasnt modified during an iteration
	// this is key for recursive inlining, where inlinable candidates may appear later during processing
	std::unordered_map<void*, llvm::Function*> inlinedFunctions {};
	bool doAnotherIteration; // true if there could still be inlinable functions
	int iteration = 0;
	do {
		if (++iteration > MAX_INLINE_ITERATIONS) {
			llvm::errs() << "inlineFunctions: fixed-point iteration cap (" << MAX_INLINE_ITERATIONS
			             << ") reached; aborting further inlining\n";
			break;
		}
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
			auto optInlineModule = loadBitcodeIfAvailable(fnPtr, moduleToOptimize.getContext(), symbolTable);
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
				// Should not happen if fixFunctionNameConflicts did its job,
				// but if it does we prefer to skip this candidate (falling
				// back to the non-inlined proxy-call path) rather than abort
				// the entire JIT compile by throwing out of this hook.
				llvm::errs() << "Inlining skipped: symbol '" << inlinableFunctionFName << "' doubly defined.\n";
				continue;
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
