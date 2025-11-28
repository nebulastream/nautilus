#include "InliningUtils.hpp"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DIBuilder.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/ExtractGV.h"
#include "llvm/Transforms/IPO/GlobalDCE.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

namespace nautilus::passes {

std::string getUUIDString() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<uint64_t> dis;

	uint64_t part1 = dis(gen);
	uint64_t part2 = dis(gen);

	std::ostringstream oss;
	oss << std::hex << part1 << part2;
	return oss.str();
}

std::string getUniqueName(llvm::GlobalValue* gv, SymbolMap& symbolMap, const llvm::Module* originalModule) {
	if (auto it = symbolMap.find(gv); it != symbolMap.end())
		return it->second;
#ifdef NDEBUG
	auto name = getUUIDString();
#else
	auto name = (originalModule->getName() + gv->getName()).str();
#endif
	symbolMap.insert({gv, name});
	return name;
}

std::optional<std::string> serializeFunctionWithDependencySymbols(llvm::Function& inlineFunction,
                                                                  SymbolMap& symbolMap) {
	auto originalModule = inlineFunction.getParent();
	llvm::ValueToValueMapTy v2v {};
	auto wrapperModule = llvm::CloneModule(*originalModule, v2v);
	auto clonedTarget = wrapperModule->getFunction(inlineFunction.getName());

	llvm::DenseMap<llvm::Value*, llvm::Value*> v2vInverted;
	for (const auto& entry : v2v) {
		v2vInverted[entry.second] = const_cast<llvm::Value*>(entry.first);
	}

	std::vector<llvm::GlobalValue*> targetFunction {};
	targetFunction.push_back(clonedTarget);

	llvm::ModuleAnalysisManager MAM;
	llvm::FunctionAnalysisManager FAM;
	llvm::CGSCCAnalysisManager CGAM;
	llvm::LoopAnalysisManager LAM;

	llvm::PassBuilder PB;
	PB.registerModuleAnalyses(MAM);
	PB.registerFunctionAnalyses(FAM);
	PB.registerCGSCCAnalyses(CGAM);
	PB.registerLoopAnalyses(LAM);
	PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

	llvm::ModulePassManager MPM;
	MPM.addPass(llvm::ExtractGVPass(targetFunction, false, false));
	MPM.addPass(llvm::GlobalDCEPass());
	MPM.run(*wrapperModule, MAM);

	wrapperModule->setDataLayout(originalModule->getDataLayout());
	wrapperModule->setTargetTriple(originalModule->getTargetTriple());
	clonedTarget->addFnAttr("is_target");
	clonedTarget->setLinkage(llvm::GlobalValue::ExternalLinkage);

	StripDebugInfo(*wrapperModule); // to suppress some llvm warning messages for invalid debug info

	for (auto& globalVariable : wrapperModule->globals()) {
		auto originalGV = dyn_cast<llvm::GlobalValue>(v2vInverted[&globalVariable]);
		auto uniqueName = getUniqueName(originalGV, symbolMap, originalModule);
		globalVariable.setName(uniqueName);
		globalVariable.setInitializer(nullptr);
		globalVariable.setLinkage(llvm::GlobalValue::ExternalLinkage);
		// note: even constant variables need to be converted to external linkage because the user code may check
		// address equality on them
	}

	for (auto& function : wrapperModule->functions()) {
		if (!function.isIntrinsic() && function.isDeclaration()) {
			auto originalFunction = dyn_cast<llvm::GlobalValue>(v2vInverted[&function]);
			auto uniqueName = getUniqueName(originalFunction, symbolMap, originalModule);
			function.setName(uniqueName);
			function.setLinkage(llvm::GlobalValue::ExternalLinkage);
			function.removeFnAttr(llvm::Attribute::NoInline);
			function.removeFnAttr(llvm::Attribute::OptimizeNone);
		}
	}

	// serialize wrapperModule to bitcode string
	llvm::SmallVector<char, 0> buffer;
	llvm::raw_svector_ostream OS(buffer);
	WriteBitcodeToFile(*wrapperModule, OS);
	std::string bitcodeStr(buffer.begin(), buffer.end());
	return std::make_optional(std::move(bitcodeStr));
}

// inserts call to the bitcode registry to associate the IR bitstring with the target function
void insertBitcodeRegistryCall(std::shared_ptr<llvm::IRBuilder<>> builder, llvm::Function* bitcodeRegistrationFunction,
                               llvm::Function& targetFunction, std::string& bitcodeStr) {
	auto& ctx = builder->getContext();
	llvm::ArrayRef<uint8_t> bitcode((const uint8_t*) bitcodeStr.data(), bitcodeStr.size());

	// Create types
	auto* int8Ty = llvm::IntegerType::get(ctx, 8);
	auto* int8PtrTy = llvm::PointerType::get(int8Ty, 0);

	// Create LLVM constant that holds the bitcode string
	auto* bitcodeConstant = llvm::ConstantDataArray::get(ctx, bitcode);
	auto* bitcodeGV = new llvm::GlobalVariable(*targetFunction.getParent(), bitcodeConstant->getType(), true,
	                                           llvm::GlobalValue::PrivateLinkage, bitcodeConstant,
	                                           targetFunction.getName() + ".bitcode");
	bitcodeGV->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
	auto* bitcodePtr =
	    builder->CreateBitCast(builder->CreateConstGEP2_32(bitcodeConstant->getType(), bitcodeGV, 0, 0), int8PtrTy);
	auto* bitcodeLen = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), bitcode.size());

	// Insert call to registration function
	auto* funcPtr = llvm::ConstantExpr::getBitCast(&targetFunction, int8PtrTy);
	builder->CreateCall(bitcodeRegistrationFunction, {funcPtr, bitcodePtr, bitcodeLen});
}

// inserts calls to the symbol registry to associate symbol names with function pointers
void insertSymbolRegistryCalls(std::shared_ptr<llvm::IRBuilder<>> builder, llvm::Function* symbolRegistrationFunction,
                               SymbolMap& symbols) {
	llvm::LLVMContext& ctx = builder->getContext();

	// Create types
	auto* int8Ty = llvm::IntegerType::get(ctx, 8);
	auto* int8PtrTy = llvm::PointerType::get(int8Ty, 0);

	for (auto& symbol : symbols) {
		auto val = symbol.first;
		auto& name = symbol.second;
		llvm::ArrayRef<uint8_t> symbolName((const uint8_t*) name.data(), name.size());

		// Create LLVM values holding the symbol names and pointers
		auto* symbolNameConstant = llvm::ConstantDataArray::get(ctx, symbolName);
		auto* symbolNameGV =
		    new llvm::GlobalVariable(*symbolRegistrationFunction->getParent(), symbolNameConstant->getType(), true,
		                             llvm::GlobalValue::PrivateLinkage, symbolNameConstant, name);
		symbolNameGV->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
		auto* symbolNamePtr = builder->CreateBitCast(
		    builder->CreateConstGEP2_32(symbolNameConstant->getType(), symbolNameGV, 0, 0), int8PtrTy);
		auto* symbolNameLen = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), name.size());

		// Insert call to registration function
		auto* symbolPtr = builder->CreateBitCast(val, int8PtrTy);
		builder->CreateCall(symbolRegistrationFunction, {symbolNamePtr, symbolNameLen, symbolPtr});
	}
}
} // namespace nautilus::passes
