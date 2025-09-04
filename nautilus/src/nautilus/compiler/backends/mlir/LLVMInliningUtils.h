#pragma once

#include "LLVMInliningUtils.h"
#include <llvm/Transforms/Utils/Cloning.h>

namespace nautilus::compiler::mlir {

bool inlineFunctionCalls(llvm::Module& M);

}
