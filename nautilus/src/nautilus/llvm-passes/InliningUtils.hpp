#pragma once
#include "llvm/IR/Module.h"
#include "llvm/Passes/PassBuilder.h"
#include <memory>
#include <string>
#include <tuple>

namespace nautilus::passes {

struct SymbolInfo {
	std::string name;
	llvm::GlobalValue* ptr;
};

void insertBitcodeRegistryCall(std::shared_ptr<llvm::IRBuilder<>> builder, llvm::Function* bitcodeRegistrationFunction,
                               llvm::Function& targetFunction, std::string& bitcodeStr);

void insertSymbolRegistryCalls(std::shared_ptr<llvm::IRBuilder<>> builder, llvm::Function* symbolRegistrationFunction,
                               std::vector<SymbolInfo>& symbols);

std::optional<std::tuple<std::string, std::vector<SymbolInfo>>>
serializeFunctionWithDependencySymbols(llvm::Function& inlineFunction);

} // namespace nautilus::passes
