#include "nautilus/llvm-passes/FunctionInliningPass.hpp"

extern "C" LLVM_ATTRIBUTE_WEAK LLVM_ATTRIBUTE_VISIBILITY_DEFAULT PassPluginLibraryInfo llvmGetPassPluginInfo() {
	return {LLVM_PLUGIN_API_VERSION, "NautilusInlineRegistrationPass", "0.0.1", [](PassBuilder& PB) {
		        PB.registerPipelineStartEPCallback(
		            [](ModulePassManager& MPM, auto) { MPM.addPass(NautilusInlineRegistrationPass {}); });
	        }};
}
