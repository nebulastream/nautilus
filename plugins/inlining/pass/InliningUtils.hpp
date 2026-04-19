#pragma once
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Passes/PassBuilder.h"
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

namespace nautilus::passes {

// Pointer-keyed map of extracted globals/functions to the randomized unique
// names used in the serialized bitcode. Unordered since iteration order is
// irrelevant — names are UUID-randomized and the bitcode writer does not
// depend on key order.
using SymbolMap = std::unordered_map<llvm::GlobalValue*, std::string>;

void insertBitcodeRegistryCall(std::shared_ptr<llvm::IRBuilder<>> builder, llvm::Function* bitcodeRegistrationFunction,
                               llvm::Function& targetFunction, std::string& bitcodeStr);

void insertSymbolRegistryCalls(std::shared_ptr<llvm::IRBuilder<>> builder, llvm::Function* symbolRegistrationFunction,
                               SymbolMap& symbols);

std::optional<std::string> serializeFunctionWithDependencySymbols(llvm::Function& inlineFunction, SymbolMap& symbols);

} // namespace nautilus::passes
