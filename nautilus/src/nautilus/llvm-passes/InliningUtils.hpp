#pragma once
#include "llvm/IR/Module.h"
#include "llvm/Passes/PassBuilder.h"
#include <map>
#include <memory>
#include <string>
#include <tuple>

namespace nautilus::passes {

using SymbolMap = std::map<llvm::GlobalValue*, std::string>;

void insertBitcodeRegistryCall(std::shared_ptr<llvm::IRBuilder<>> builder, llvm::Function* bitcodeRegistrationFunction,
                               llvm::Function& targetFunction, std::string& bitcodeStr);

void insertSymbolRegistryCalls(std::shared_ptr<llvm::IRBuilder<>> builder, llvm::Function* symbolRegistrationFunction,
                               SymbolMap& symbols);

std::optional<std::string> serializeFunctionWithDependencySymbols(llvm::Function& inlineFunction, SymbolMap& symbols);

} // namespace nautilus::passes
