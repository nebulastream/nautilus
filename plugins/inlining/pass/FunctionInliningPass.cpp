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

// Lowest allowed ctor priority (65535): ensures the generated
// `M.ir_ctor` that populates `InlineFunctionRegistry` runs AFTER the
// function-local static inside `InlineFunctionRegistry::instance()` has been
// initialized in any reachable TU ordering. Do not lower without first
// switching `instance()` to use an explicit `std::call_once` guard.
constexpr uint32_t NAUTILUS_INLINE_CTOR_PRIORITY = 65535;

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

		SymbolMap symbolMap;
		std::unordered_map<llvm::Function*, std::string> bitcodes;
		llvm::Function* ctor = nullptr;
		std::shared_ptr<llvm::IRBuilder<>> ctorBuilder;
		for (auto& F : M) {
			if (!F.isDeclaration()) {

				// Check whether a function has the inline tag
				bool toInline = false;
				for (auto& attr : F.getAttributes().getFnAttrs()) {
					if (attr.getKindAsString().starts_with("nautilus_inline")) {
						toInline = true;
					}
				}
				if (toInline) {
					// Catch errors so that non-inlinable functions don't completely
					// break compilation. Two orthogonal failure modes are tracked:
					//   crashFree:  RunSafely returns false if the lambda SIGSEGVs
					//               (malformed bitcode, etc). We just log and skip.
					//   serialized: set inside the lambda iff the result was stored
					//               into `bitcodes`. A missing optional result is a
					//               soft failure — we log but don't abort compilation.
					bool serialized = false;
					llvm::CrashRecoveryContext CRC;
					const bool crashFree = CRC.RunSafely([&] {
						auto result = serializeFunctionWithDependencySymbols(F, symbolMap);
						if (result.has_value()) {
							bitcodes.insert({&F, std::move(*result)});
							serialized = true;
						}
					});
					if (!crashFree || !serialized) {
						llvm::errs() << "Failed to serialize inline function. To get rid of this warning, remove the "
						                "NAUTILUS_INLINE tag from this function: "
						             << F.getName() << "\n";
					}
				}
			}
		}

		if (!bitcodes.empty()) {
			// Create initializer function with single basic block
			// This will be executed when the program starts
			ctor = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), false),
			                              llvm::GlobalValue::InternalLinkage, M.getName() + ".ir_ctor", &M);
			auto* BB = llvm::BasicBlock::Create(ctx, "entry", ctor);
			ctorBuilder = std::make_shared<llvm::IRBuilder<>>(BB);

			// Insert registry calls into the initializer function to populate them at runtime
			for (auto bitcode : bitcodes) {
				insertBitcodeRegistryCall(ctorBuilder, bitcodeRegistrationFunction, *bitcode.first, bitcode.second);
			}
			insertSymbolRegistryCalls(ctorBuilder, symbolRegistrationFunction, symbolMap);

			ctorBuilder->CreateRetVoid();
			appendToGlobalCtors(M, ctor, NAUTILUS_INLINE_CTOR_PRIORITY);
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
		        PB.registerOptimizerLastEPCallback([](llvm::ModulePassManager& MPM, auto&&...) {
			        MPM.addPass(nautilus::passes::NautilusInlineRegistrationPass {});
		        });
	        }};
}
