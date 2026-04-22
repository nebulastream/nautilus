#pragma once

#include <llvm/ADT/StringRef.h>
#include <string>

namespace llvm {
class Module;
} // namespace llvm

namespace nautilus::compiler::mlir::detail {

// For each non-declaration function in the module, add a wrapper function
// named "_mlir_<name>" with signature `void(void**)` that unpacks arguments
// from the type-erased argument array and forwards to the original.
//
// Ported from llvm-project mlir/lib/ExecutionEngine/ExecutionEngine.cpp
// (Apache-2.0 with LLVM exception). Kept in its own translation unit for
// clean attribution; logic is preserved verbatim.
void packFunctionArguments(llvm::Module* module);

std::string makePackedFunctionName(llvm::StringRef name);

} // namespace nautilus::compiler::mlir::detail
