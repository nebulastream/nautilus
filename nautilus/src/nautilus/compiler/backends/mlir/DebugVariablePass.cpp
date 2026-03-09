
#include "DebugVariablePass.hpp"
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DebugInfo.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IntrinsicInst.h>

namespace nautilus::compiler::mlir {

void DebugVariablePass::run(llvm::Module& module, const std::unordered_map<std::string, std::string>& nameMap) {
	auto* executeFunc = module.getFunction("execute");
	if (executeFunc == nullptr) {
		return;
	}

	auto* diSP = executeFunc->getSubprogram();
	if (diSP == nullptr) {
		// DebugInfoPass must run first.
		return;
	}

	llvm::DIBuilder diBuilder(module, false);
	auto* diFile = diSP->getFile();
	auto& ctx = module.getContext();

	// Build a simple i64 base type for positional variable naming.
	// For typed variables, we'd match the LLVM type; this is a best-effort approach.
	auto makeIntType = [&](unsigned bits) -> llvm::DIType* {
		return diBuilder.createBasicType("i" + std::to_string(bits), bits, llvm::dwarf::DW_ATE_signed);
	};
	auto makeFloatType = [&](unsigned bits) -> llvm::DIType* {
		return diBuilder.createBasicType("f" + std::to_string(bits), bits, llvm::dwarf::DW_ATE_float);
	};
	auto makePtrType = [&]() -> llvm::DIType* {
		return diBuilder.createPointerType(makeIntType(8), 64);
	};

	uint32_t varIdx = 0;

	for (auto& bb : *executeFunc) {
		for (auto& inst : bb) {
			// Skip void-typed, terminators, and existing debug intrinsics.
			if (inst.getType()->isVoidTy() || inst.isTerminator()) {
				continue;
			}
			if (llvm::isa<llvm::DbgInfoIntrinsic>(inst)) {
				continue;
			}
			auto* loc = inst.getDebugLoc().get();
			if (loc == nullptr) {
				continue;
			}

			// Determine variable name.
			std::string varName;
			auto valueName = inst.getName().str();
			if (!valueName.empty()) {
				auto it = nameMap.find(valueName);
				varName = (it != nameMap.end()) ? it->second : valueName;
			} else {
				varName = "v" + std::to_string(varIdx++);
			}

			// Choose a DIType based on the LLVM value type.
			llvm::DIType* diType = nullptr;
			auto* valType = inst.getType();
			if (valType->isIntegerTy()) {
				diType = makeIntType(valType->getIntegerBitWidth());
			} else if (valType->isFloatTy()) {
				diType = makeFloatType(32);
			} else if (valType->isDoubleTy()) {
				diType = makeFloatType(64);
			} else if (valType->isPointerTy()) {
				diType = makePtrType();
			} else {
				diType = makeIntType(64);
			}

			auto* diVar = diBuilder.createAutoVariable(diSP, varName, diFile, loc->getLine(), diType);

			// Insert dbg.value immediately after the defining instruction.
			llvm::Instruction* insertBefore = inst.getNextNonDebugInstruction();
			if (insertBefore == nullptr) {
				continue;
			}
			diBuilder.insertDbgValueIntrinsic(&inst, diVar, diBuilder.createExpression(),
			                                 llvm::DILocation::get(ctx, loc->getLine(), loc->getColumn(), diSP),
			                                 insertBefore->getIterator());
		}
	}

	diBuilder.finalize();
}

} // namespace nautilus::compiler::mlir
