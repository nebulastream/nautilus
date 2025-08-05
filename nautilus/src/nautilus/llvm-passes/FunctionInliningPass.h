#ifndef FUNCTIONINLININGPASS_H
#define FUNCTIONINLININGPASS_H

#include "llvm/Passes/PassBuilder.h"

class NautilusInlineRegistrationPass : public llvm::PassInfoMixin<NautilusInlineRegistrationPass> {
public:
	llvm::PreservedAnalyses run(llvm::Module &M, llvm::ModuleAnalysisManager &MAM);
};





#endif
