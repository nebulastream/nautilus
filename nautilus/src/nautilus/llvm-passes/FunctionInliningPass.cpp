#include "InliningUtils.hpp"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/DIBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Type.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/CrashRecoveryContext.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"

namespace nautilus::passes {

class NautilusInlineRegistrationPass : public llvm::PassInfoMixin<NautilusInlineRegistrationPass> {
public:
	static llvm::PreservedAnalyses run(llvm::Module& M, llvm::ModuleAnalysisManager& MAM) {
		auto& ctx = M.getContext();

		// Find registration functions
		auto* bitcodeRegistrationFunction = findBitcodeRegistrationFunction(M);
		auto* symbolRegistrationFunction = findSymbolRegistrationFunction(M);
		if (bitcodeRegistrationFunction == nullptr || symbolRegistrationFunction == nullptr) {
			return llvm::PreservedAnalyses::all();
		}

		// Convert clang annotations to function attributes that are directly connected to a function
		annotationsToAttributes(M);

		llvm::Function* ctor = nullptr;
		std::shared_ptr<llvm::IRBuilder<>> ctorBuilder;
		for (auto& F : M) {
			if (!F.isDeclaration()) {

				// Check whether a function has the inline tag
				bool toInline = false;
				for (auto& attr : F.getAttributes().getFnAttrs()) {
					if (attr.getKindAsString().starts_with("naut_inline")) {
						toInline = true;
					}
				}
				if (toInline) {
					// catch errors so that non-inlinable functions dont completely break compilation
					llvm::CrashRecoveryContext CRC;
					bool success = CRC.RunSafely([&] {
						// Serialize function to LLVM IR bitcode
						auto result = serializeFunctionWithDependencySymbols(F);
						if (!result.has_value()) {
							CRC.HandleExit(1);
						}

						// Create initializer function with single basic block
						// This will be executed when the program starts
						if (!ctor) {
							ctor = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), false),
							                              llvm::GlobalValue::InternalLinkage, M.getName() + ".ir_ctor",
							                              &M);
							auto* BB = llvm::BasicBlock::Create(ctx, "entry", ctor);
							ctorBuilder = std::make_shared<llvm::IRBuilder<>>(BB);
							ctorBuilder->CreateRetVoid();
							ctorBuilder->SetInsertPoint(&BB->front());
							appendToGlobalCtors(M, ctor, 65535);
						}

						// Insert registry calls into the initializer function to populate them at runtime
						insertBitcodeRegistryCall(ctorBuilder, bitcodeRegistrationFunction, F, std::get<0>(*result));
						insertSymbolRegistryCalls(ctorBuilder, symbolRegistrationFunction, std::get<1>(*result));

						success = true;
					});
				}
			}
		}

		return llvm::PreservedAnalyses::all();
	}

private:
	static void annotationsToAttributes(llvm::Module& M) {
		auto global_annos = M.getNamedGlobal("llvm.global.annotations");
		if (global_annos) {
			if (auto* ca = llvm::dyn_cast<llvm::ConstantArray>(global_annos->getOperand(0))) {
				for (unsigned i = 0; i < ca->getNumOperands(); i++) {
					if (auto* cs = llvm::dyn_cast<llvm::ConstantStruct>(ca->getOperand(i))) {
						if (auto* fn = llvm::dyn_cast<llvm::Function>(cs->getOperand(0)->stripPointerCasts())) {
							if (auto* annoGV =
							        llvm::dyn_cast<llvm::GlobalVariable>(cs->getOperand(1)->stripPointerCasts())) {
								if (auto* cda = llvm::dyn_cast<llvm::ConstantDataArray>(annoGV->getInitializer())) {
									auto annoStr = cda->getAsCString();
									fn->addFnAttr(annoStr);
								}
							}
						}
					}
				}
			}
		}
	}

	static llvm::Function* findBitcodeRegistrationFunction(llvm::Module& M) {
		for (auto& F : M) {
			if (F.getName().contains("registerBitcodePleaseIgnoreThisThanks")) {
				return &F;
			}
		}
		return nullptr;
	}

	static llvm::Function* findSymbolRegistrationFunction(llvm::Module& M) {
		for (auto& F : M) {
			if (F.getName().contains("registerSymbolPleaseIgnoreThisThanks")) {
				return &F;
			}
		}
		return nullptr;
	}
};

} // namespace nautilus::passes

extern "C" LLVM_ATTRIBUTE_WEAK LLVM_ATTRIBUTE_VISIBILITY_DEFAULT llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
	return {LLVM_PLUGIN_API_VERSION, "NautilusInlineRegistrationPass", "0.0.1", [](llvm::PassBuilder& PB) {
		        PB.registerPipelineStartEPCallback([](llvm::ModulePassManager& MPM, auto) {
			        MPM.addPass(nautilus::passes::NautilusInlineRegistrationPass {});
		        });
	        }};
}
