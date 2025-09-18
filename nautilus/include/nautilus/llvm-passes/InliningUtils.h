#pragma once

#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DIBuilder.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/ModuleUtils.h"
#include <memory>
#include <string>
#include <tuple>
#include <vector>

using namespace llvm;

struct SymbolInfo {
	std::string name;
	llvm::Function* ptr;
};

static std::optional<std::tuple<std::string, std::vector<SymbolInfo>>>
serializeFunctionWithDependencySymbols(Function& inlineFunction);

static void insertBitcodeRegistryCall(std::shared_ptr<IRBuilder<>> builder, Function* bitcodeRegistrationFunction,
                                      Function& targetFunction, std::string& bitcodeStr);

static void insertSymbolRegistryCalls(std::shared_ptr<IRBuilder<>> builder, Function* symbolRegistrationFunction,
                                      std::vector<SymbolInfo>& symbols);

static bool cloneOperand(Value* V, std::unordered_set<Value*>& clonedSet, Module& wrapperModule,
                         ValueToValueMapTy& v2vMap, std::vector<SymbolInfo>& symbols, Function& inlineFunction) {
	if (!V || clonedSet.contains(V))
		return true;

	clonedSet.insert(V);

	// handle global values (functions, variables, ...)
	if (auto* GV = dyn_cast<GlobalValue>(V)) {
		if (GV == &inlineFunction)
			return true;

		if (auto* F = dyn_cast<Function>(GV)) {
			// Clone function declarations
			if (!F->isIntrinsic()) {
				auto* decl = Function::Create(F->getFunctionType(), GlobalValue::ExternalLinkage, F->getAddressSpace(),
				                              inlineFunction.getName() + "." + F->getName(), &wrapperModule);
				decl->copyAttributesFrom(F);
				if (decl->hasPersonalityFn())
					decl->setPersonalityFn(nullptr);
				v2vMap[F] = decl;

				symbols.push_back({decl->getName().str(), F});
			} else {
				// intrinsics require special care
				auto* existing = wrapperModule.getFunction(F->getName());
				if (!existing) {
					auto* decl = Function::Create(F->getFunctionType(), F->getLinkage(), F->getAddressSpace(),
					                              F->getName(), &wrapperModule);
					decl->copyAttributesFrom(F);
					v2vMap[F] = decl;
				} else {
					v2vMap[F] = existing;
				}
			}
		} else if (auto* GVVar = dyn_cast<GlobalVariable>(GV)) {
			// only works for constant variables so far; other variables would have to be linked at runtime
			if (GVVar->hasInitializer() && GVVar->isConstant()) {
				// clone initializer operands
				for (unsigned i = 0; i < GVVar->getInitializer()->getNumOperands(); ++i) {
					if (!cloneOperand(GVVar->getInitializer()->getOperand(i), clonedSet, wrapperModule, v2vMap, symbols,
					                  inlineFunction)) {
						return false;
					}
				}

				auto* init = cast<Constant>(MapValue(GVVar->getInitializer(), v2vMap));

				auto* newGV = new GlobalVariable(wrapperModule, GVVar->getValueType(), true, GVVar->getLinkage(), init,
				                                 GVVar->getName(), nullptr, GVVar->getThreadLocalMode(),
				                                 GVVar->getAddressSpace());
				newGV->setUnnamedAddr(GVVar->getUnnamedAddr());
				newGV->setAlignment(GVVar->getAlign());
				v2vMap[GVVar] = newGV;
			} else {
				errs() << "Non-constant/external global variable detected in nautilus inlining pass. Please remove the "
				          "NAUT_INLINE tag from "
				          "function: "
				       << inlineFunction.getName() << "\n";
				return false;
			}
		}
	} else if (auto* C = dyn_cast<Constant>(V)) {
		// process operands
		for (unsigned i = 0; i < C->getNumOperands(); ++i) {
			if (!cloneOperand(C->getOperand(i), clonedSet, wrapperModule, v2vMap, symbols, inlineFunction)) {
				return false;
			}
		}

		SmallVector<Constant*, 4> newOperands;
		for (unsigned i = 0; i < C->getNumOperands(); ++i) {
			auto* oldOp = C->getOperand(i);
			auto* newOp = dyn_cast<Constant>(v2vMap.lookup(oldOp));
			newOperands.push_back(newOp);
		}

		// build new constant
		Constant* newC = nullptr;
		if (auto* CS = dyn_cast<ConstantStruct>(C)) {
			newC = ConstantStruct::get(CS->getType(), ArrayRef<Constant*>(newOperands));
		} else if (auto* CA = dyn_cast<ConstantArray>(C)) {
			newC = ConstantArray::get(CA->getType(), ArrayRef<Constant*>(newOperands));
		} else if (dyn_cast<ConstantVector>(C)) {
			newC = ConstantVector::get(ArrayRef<Constant*>(newOperands));
		} else if (auto* CDA = dyn_cast<ConstantDataArray>(C)) {
			newC = ConstantArray::get(cast<ArrayType>(CDA->getType()), ArrayRef<Constant*>(newOperands));
		} else if (auto* CI = dyn_cast<ConstantInt>(C)) {
			newC = ConstantInt::get(CI->getContext(), CI->getValue());
		} else if (auto* CFP = dyn_cast<ConstantFP>(C)) {
			newC = ConstantFP::get(CFP->getType(), CFP->getValueAPF());
		} else if (auto* CPN = dyn_cast<ConstantPointerNull>(C)) {
			newC = ConstantPointerNull::get(CPN->getType());
		} else if (auto* CU = dyn_cast<UndefValue>(C)) {
			newC = UndefValue::get(CU->getType());
		} else {
			// tough luck
			llvm::errs() << C << "\n";
			errs() << "Unknown LLVM::Constant case found in nautilus inlining pass. Please remove the NAUT_INLINE tag "
			          "from "
			          "function: "
			       << inlineFunction.getName() << "\n";
			return false;
		}
		if (newC != nullptr) {
			v2vMap[V] = newC;
		}
	}
	return true;
}

// return false if we encountered an illegal LLVM::Constant case
static bool cloneValuesAndCollectDependencySymbols(Function& inlineFunction, Module& wrapperModule,
                                                   ValueToValueMapTy& v2vMap, std::vector<SymbolInfo>& symbols) {
	std::unordered_set<Value*> clonedSet;

	for (auto& BB : inlineFunction) {
		for (auto& I : BB) {
			for (Value* V : I.operands()) {
				if (!cloneOperand(V, clonedSet, wrapperModule, v2vMap, symbols, inlineFunction)) {
					return false;
				}
			}
		}
	}
	return true;
}

std::optional<std::tuple<std::string, std::vector<SymbolInfo>>>
serializeFunctionWithDependencySymbols(Function& inlineFunction) {
	std::vector<SymbolInfo> symbols;
	// Create module holding the extracted function and its dependencies
	Module wrapperModule = Module("func_module", inlineFunction.getContext());

	// Clone inline function to the wrapper module
	Function* clonedFunc = Function::Create(inlineFunction.getFunctionType(), inlineFunction.getLinkage(),
	                                        inlineFunction.getName(), &wrapperModule);
	ValueToValueMapTy v2vMap;
	for (auto originalIt = inlineFunction.arg_begin(), cloneIt = clonedFunc->arg_begin();
	     originalIt != inlineFunction.arg_end(); ++originalIt, ++cloneIt)
		v2vMap[&*originalIt] = &*cloneIt;
	SmallVector<ReturnInst*, 8> returnInst;
	CloneFunctionInto(clonedFunc, &inlineFunction, v2vMap, CloneFunctionChangeType::DifferentModule, returnInst);
	clonedFunc->addFnAttr("is_target");
	clonedFunc->setLinkage(GlobalValue::ExternalLinkage);

	// Clone declarations of all dependencies and store in symbol vector
	if (!cloneValuesAndCollectDependencySymbols(inlineFunction, wrapperModule, v2vMap, symbols)) {
		clonedFunc->dropAllReferences();
		clonedFunc->deleteBody();
		return std::nullopt;
	}

	// clone personality function of the inline function if it exists (otherwise some functions cant be properly
	// inlined)
	if (inlineFunction.hasPersonalityFn()) {
		Constant* srcPersonalityConst = inlineFunction.getPersonalityFn();
		if (auto* srcPersonality = dyn_cast<Function>(srcPersonalityConst)) {
			auto* clonedPersonality = Function::Create(srcPersonality->getFunctionType(), srcPersonality->getLinkage(),
			                                           srcPersonality->getName(), &wrapperModule);
			for (auto originalIt = srcPersonality->arg_begin(), cloneIt = clonedPersonality->arg_begin();
			     originalIt != srcPersonality->arg_end(); ++originalIt, ++cloneIt)
				v2vMap[&*originalIt] = &*cloneIt;

			SmallVector<ReturnInst*, 8> personalityReturnInst;
			CloneFunctionInto(clonedPersonality, srcPersonality, v2vMap, CloneFunctionChangeType::LocalChangesOnly,
			                  personalityReturnInst);

			clonedFunc->setPersonalityFn(clonedPersonality);
			v2vMap[srcPersonality] = clonedPersonality;
		}
	}
	RemapFunction(*clonedFunc, v2vMap, RF_None);

	StripDebugInfo(wrapperModule); // to suppress some llvm warning messages for invalid debug info

	// Verify integrity of the new module
	// will automatically print the precise reasons if the verification fails, which is key for debugging functions that
	// cant be inlined yet)
	if (verifyModule(wrapperModule, &errs())) {
		errs() << "\n Failed to serialize inline function (IR verification failed; possibly due to use of global "
		          "variables). To get rid of this warning, remove the NAUT_INLINE tag from this function: "
		       << inlineFunction.getName() << "\n";
		return std::nullopt;
	}

	// serialize wrapperModule to bitcode string
	SmallVector<char, 0> buffer;
	raw_svector_ostream OS(buffer);
	WriteBitcodeToFile(wrapperModule, OS);
	std::string bitcodeStr(buffer.begin(), buffer.end());
	return std::make_optional(std::make_tuple(std::move(bitcodeStr), std::move(symbols)));
}

void insertBitcodeRegistryCall(std::shared_ptr<IRBuilder<>> builder, Function* bitcodeRegistrationFunction,
                               Function& targetFunction, std::string& bitcodeStr) {
	auto& ctx = builder->getContext();
	ArrayRef<uint8_t> bitcode((const uint8_t*) bitcodeStr.data(), bitcodeStr.size());

	// Create types
	auto* int8Ty = IntegerType::get(ctx, 8);
	auto* int8PtrTy = PointerType::get(int8Ty, 0);

	// Create LLVM constant that holds the bitcode string
	auto* bitcodeConstant = ConstantDataArray::get(ctx, bitcode);
	auto* bitcodeGV =
	    new GlobalVariable(*targetFunction.getParent(), bitcodeConstant->getType(), true, GlobalValue::PrivateLinkage,
	                       bitcodeConstant, targetFunction.getName() + ".bitcode");
	bitcodeGV->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
	auto* bitcodePtr =
	    builder->CreateBitCast(builder->CreateConstGEP2_32(bitcodeConstant->getType(), bitcodeGV, 0, 0), int8PtrTy);
	auto* bitcodeLen = ConstantInt::get(Type::getInt64Ty(ctx), bitcode.size());

	// Insert call to registration function
	auto* funcPtr = ConstantExpr::getBitCast(&targetFunction, int8PtrTy);
	builder->CreateCall(bitcodeRegistrationFunction, {funcPtr, bitcodePtr, bitcodeLen});
}

void insertSymbolRegistryCalls(std::shared_ptr<IRBuilder<>> builder, Function* symbolRegistrationFunction,
                               std::vector<SymbolInfo>& symbols) {
	LLVMContext& ctx = builder->getContext();

	// Create types
	auto* int8Ty = IntegerType::get(ctx, 8);
	auto* int8PtrTy = PointerType::get(int8Ty, 0);

	for (auto& symbol : symbols) {
		ArrayRef<uint8_t> symbolName((const uint8_t*) symbol.name.data(), symbol.name.size());

		// Create LLVM values holding the symbol names and pointers
		auto* symbolNameConstant = ConstantDataArray::get(ctx, symbolName);
		auto* symbolNameGV = new GlobalVariable(*symbolRegistrationFunction->getParent(), symbolNameConstant->getType(),
		                                        true, GlobalValue::PrivateLinkage, symbolNameConstant, symbol.name);
		symbolNameGV->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
		auto* symbolNamePtr = builder->CreateBitCast(
		    builder->CreateConstGEP2_32(symbolNameConstant->getType(), symbolNameGV, 0, 0), int8PtrTy);
		auto* symbolNameLen = ConstantInt::get(Type::getInt64Ty(ctx), symbol.name.size());

		// Insert call to registration function
		auto* symbolPtr = builder->CreateBitCast(symbol.ptr, int8PtrTy);
		builder->CreateCall(symbolRegistrationFunction, {symbolNamePtr, symbolNameLen, symbolPtr});
	}
}
