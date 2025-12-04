#pragma once

#include "LLVMInliningUtils.hpp"
#include <llvm/Transforms/Utils/Cloning.h>

namespace nautilus::compiler::mlir {

// convert a hex string to a pointer
void* hexToPtr(const llvm::StringRef& hexString);

// convert a pointer to a hex string
std::string ptrToHex(const void* ptr);

// find inlinable functions in the module and replace them with defined function from the bitcode registry
void inlineFunctions(llvm::Module& moduleToOptimize);

} // namespace nautilus::compiler::mlir
