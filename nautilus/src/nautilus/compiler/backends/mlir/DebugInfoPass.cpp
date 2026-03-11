
#include "DebugInfoPass.hpp"
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DebugInfo.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>

namespace nautilus::compiler::mlir {

void DebugInfoPass::run(llvm::Module& module) {
	auto* executeFunc = module.getFunction("execute");
	if (executeFunc == nullptr) {
		return;
	}

	llvm::DIBuilder diBuilder(module);

	// Create a DICompileUnit — required anchor for all debug info.
	// Use a placeholder file; real files come from per-instruction debug locations.
	auto* diFile = diBuilder.createFile("nautilus_jit.cpp", ".");
	auto* diCU = diBuilder.createCompileUnit(llvm::dwarf::DW_LANG_C_plus_plus, diFile, "nautilus", false, "", 0);

	// Create a subroutine type for the execute function (no parameter info needed for stepping).
	auto* funcType = diBuilder.createSubroutineType(diBuilder.getOrCreateTypeArray({}));

	// Create DISubprogram attached to the execute function.
	auto* diSP = diBuilder.createFunction(diCU, "execute", "execute", diFile, 1, funcType, 1,
	                                      llvm::DINode::FlagPrototyped, llvm::DISubprogram::SPFlagDefinition);
	executeFunc->setSubprogram(diSP);

	// For any instruction missing a DebugLoc, assign one from a neighbor so
	// LLVM doesn't drop the subprogram during optimization.
	for (auto& bb : *executeFunc) {
		llvm::DebugLoc lastLoc;
		for (auto& inst : bb) {
			if (inst.getDebugLoc()) {
				lastLoc = inst.getDebugLoc();
			} else if (lastLoc) {
				inst.setDebugLoc(lastLoc);
			}
		}
		// Forward pass: fill any still-missing locs from the front.
		if (!lastLoc) {
			// Create a fallback loc at line 1.
			lastLoc = llvm::DILocation::get(module.getContext(), 1, 0, diSP);
		}
		for (auto& inst : bb) {
			if (!inst.getDebugLoc()) {
				inst.setDebugLoc(lastLoc);
			}
		}
	}

	diBuilder.finalize();
}

} // namespace nautilus::compiler::mlir
