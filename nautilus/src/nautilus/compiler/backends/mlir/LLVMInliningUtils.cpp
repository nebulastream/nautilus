#include "LLVMInliningUtils.h"
#include "fmt/format.h"
#include "nautilus/inline.hpp"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Linker/Linker.h"
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/FileCollector.h>
#include <llvm/Transforms/Utils/Cloning.h>

namespace nautilus::compiler::mlir {

// look up invoked function in the bitcode registry and retrieve if available
std::optional<std::unique_ptr<llvm::Module>> loadBitcodeIfAvailable(const llvm::Function& callee,
                                                                    llvm::LLVMContext& ctx) {
	auto name = callee.getName();
	void* fnPtr = nullptr;
	if (name.starts_with("0x")) {
		uintptr_t addr;
		if (!name.getAsInteger(0, addr)) {
			fnPtr = reinterpret_cast<void*>(addr);
		}
	}

	if (fnPtr == nullptr) { // not a valid address
		return std::nullopt;
	}

	// parse name of invoked function to ptr and look up
	auto bitcodeStr = InlineFunctionRegistry::instance().getBitcode(fnPtr);
	if (bitcodeStr.empty()) {
		return std::nullopt; // not found, cant inline
	}

	// deserialize bitcode module
	auto buffer = llvm::MemoryBuffer::getMemBuffer(llvm::StringRef(bitcodeStr.data(), bitcodeStr.size()), "", false);
	llvm::Expected<std::unique_ptr<llvm::Module>> moduleOrErr = llvm::parseBitcodeFile(buffer->getMemBufferRef(), ctx);
	if (!moduleOrErr) {
		logAllUnhandledErrors(moduleOrErr.takeError(), llvm::errs(), "Bitcode parse error: ");
		return std::nullopt; // if deserialization fails, fall back to regular invocation
	}

	// rewrite names of function dependencies to runtime addresses for possible recursive inlining
	for (auto& F : *moduleOrErr.get()) {
		if (F.isDeclaration() && !F.isIntrinsic()) {
			auto it = InlineFunctionRegistry::instance().getSymbolTable()->find(F.getName().str());
			if (it != InlineFunctionRegistry::instance().getSymbolTable()->end()) {
				// build string from ptr
				auto hexStr = fmt::format("0x{:X}", reinterpret_cast<uintptr_t>(it->second));

				auto* funcInModule = moduleOrErr.get()->getFunction(hexStr);
				if (!funcInModule) {
					F.setName(hexStr);
				} else {
					F.replaceAllUsesWith(funcInModule);
				}
			}
		}
	}
	return std::optional(std::move(*moduleOrErr));
}

bool inlineFunctionCalls(llvm::Module& M) {
	std::vector<llvm::CallBase*> callsToInline; // store inlinable calls here to avoid manipulating M during the loop
	std::unordered_map<llvm::Function*, llvm::Function*> functionMap {};

	std::vector<llvm::Function*> functionListView;
	for (auto& F : M) {
		functionListView.push_back(&F);
	}

	// iterate over all function calls
	for (auto* F : functionListView) {
		for (auto& BB : *F) {
			for (auto& I : BB) {
				if (auto* callInst = dyn_cast<llvm::CallBase>(&I)) {
					if (auto* callee = callInst->getCalledFunction()) {
						if (auto it = functionMap.find(callee);
						    it != functionMap.end()) { // early deduplication in same run
							callInst->setCalledFunction(it->second);
							callsToInline.push_back(callInst);
							continue;
						}

						// try to load bitcode
						auto optModule = loadBitcodeIfAvailable(*callee, M.getContext());

						if (optModule.has_value()) { // bitcode exists in registry
							// find the name of the target function
							std::string fName;
							for (auto& F : *optModule.value()) {
								if (!F.isDeclaration() && F.hasFnAttribute("is_target")) {
									fName = F.getName().str();
									break;
								}
							}

							// suppress some warnings
							optModule.value()->setTargetTriple(M.getTargetTriple());
							optModule.value()->setDataLayout(M.getDataLayout().getStringRepresentation());

							if (llvm::Function* F2 = M.getFunction(fName)) { // deduplicate in recursive case
								callInst->setCalledFunction(F2);
								callsToInline.push_back(callInst);
								functionMap.insert({callee, F2});
								continue;
							}

							// link modules together
							if (llvm::Linker::linkModules(M, std::move(optModule.value()))) {
								llvm::errs() << "Failed to link modules\n";
								return false;
							}

							// find the target function after linking
							llvm::Function* srcFunc = nullptr;
							for (auto& F : M) {
								if (!F.isDeclaration() && F.getName().str() == fName) {
									srcFunc = &F;
									break;
								}
							}
							if (srcFunc == nullptr) {
								llvm::errs()
								    << "Failed to find inline function after linking modules'" << fName << "'\n";
								continue;
							}

							// store call in list for inlining
							callInst->setCalledFunction(srcFunc);
							callsToInline.push_back(callInst);
							functionMap.insert({callee, srcFunc});
						}
					}
				}
			}
		}
	}

	for (auto* call : callsToInline) {
		llvm::InlineFunctionInfo IFI;
		bool success = llvm::InlineFunction(*call, IFI).isSuccess();
		if (!success) {
			// llvm::errs() << "Failed to inline function " << call->getName() << "\n";
		};
		// M.print(llvm::outs(), nullptr);
	}
	return !callsToInline.empty();
}
} // namespace nautilus::compiler::mlir
