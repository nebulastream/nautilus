#ifndef NAUTILUS_LLVMINLININGUTILS_H
#define NAUTILUS_LLVMINLININGUTILS_H

#include "LLVMInliningUtils.h"
#include <llvm/Transforms/Utils/Cloning.h>


namespace nautilus::compiler::mlir {

bool inlineFunctionCalls(llvm::Module &M);

}

#endif
