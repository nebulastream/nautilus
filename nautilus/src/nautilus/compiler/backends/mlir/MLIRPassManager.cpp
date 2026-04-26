

#include "nautilus/compiler/backends/mlir/MLIRPassManager.hpp"
#include "nautilus/compiler/backends/mlir/debug/EmitDbgValuePass.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <mlir/Conversion/ArithToLLVM/ArithToLLVM.h>
#include <mlir/Conversion/ControlFlowToLLVM/ControlFlowToLLVM.h>
#include <mlir/Conversion/FuncToLLVM/ConvertFuncToLLVMPass.h>
#include <mlir/Conversion/MathToLLVM/MathToLLVM.h>
#include <mlir/Conversion/ReconcileUnrealizedCasts/ReconcileUnrealizedCasts.h>
#include <mlir/Dialect/LLVMIR/Transforms/Passes.h>
#include <mlir/ExecutionEngine/OptUtils.h>
#include <mlir/Pass/PassManager.h>
#include <mlir/Transforms/LocationSnapshot.h>
#include <mlir/Transforms/Passes.h>

namespace nautilus::compiler::mlir {

using namespace ::mlir;

/**
 * @brief Takes a OptimizationPass Enum and returns the corresponding mlir
 * optimization pass.
 *
 * @param optimizationPass: Used to get the correct mlir optimization pass.
 * @return std::unique_ptr<mlir::Pass>: MLIR optimization pass corresponding to
 * supplied Enum.
 */
std::unique_ptr<mlir::Pass> getMLIROptimizationPass(MLIRPassManager::OptimizationPass optimizationPass) {
	switch (optimizationPass) {
	case MLIRPassManager::OptimizationPass::Inline:
		return mlir::createInlinerPass();
	}
	throw NotImplementedException("pass is not supported");
}

int MLIRPassManager::lowerAndOptimizeMLIRModule(mlir::OwningOpRef<mlir::ModuleOp>& module,
                                                const std::vector<OptimizationPass>& optimizationPasses,
                                                const DebugInfoOptions& debugInfo) {
	mlir::PassManager passManager(module->getContext());

	const bool debugEnabled = debugInfo.enable;
	// Tier 1 ("nautilus-ir" mode) depends on FileLineColLocs attached by
	// MLIRLoweringProvider that reference positions in the Nautilus IR dump.
	// MLIR's inliner rewrites locations into inlinedAt chains and the
	// interaction with a non-MLIR source file is not yet validated, so we
	// skip the inliner in that mode to keep stepping predictable.
	const bool skipInliner = debugEnabled && debugInfo.sourceMode == "nautilus-ir";

	if (!skipInliner) {
		if (!optimizationPasses.empty()) {
			for (auto optimizationPass : optimizationPasses) {
				passManager.addPass(getMLIROptimizationPass(optimizationPass));
			}
		} else {
			passManager.addPass(mlir::createInlinerPass());
		}
	}

	// In "mlir" source mode the dumped-and-inlined MLIR is itself the
	// "source".  LocationSnapshot writes the current IR to a file and
	// rewrites every op's location to a FileLineColLoc pointing into that
	// file, giving GDB a real file:line mapping to step through.
	if (debugEnabled && debugInfo.sourceMode == "mlir") {
		mlir::LocationSnapshotOptions snapshotOpts;
		snapshotOpts.fileName = debugInfo.sourceFile;
		passManager.addPass(mlir::createLocationSnapshot(snapshotOpts));
	}

	// Apply lowering passes.
	passManager.addPass(mlir::createConvertMathToLLVMPass());
	passManager.addPass(mlir::createConvertFuncToLLVMPass());
	passManager.addPass(mlir::createConvertControlFlowToLLVMPass());
	passManager.addPass(mlir::createArithToLLVMConversionPass());
	passManager.addPass(mlir::createReconcileUnrealizedCastsPass());

	// Materialize a DISubprogram on every llvm.func.  Required so that
	// the FileLineColLocs attached above translate into valid DWARF line
	// tables.  For "nautilus-ir" mode MLIRLoweringProvider may already
	// have attached a DISubprogramAttr; this pass is a no-op on functions
	// that already have one.
	if (debugEnabled) {
		// Request Full emission (not the default LineTablesOnly) so
		// the DWARF emitter keeps DILocalVariables / dbg.value
		// records that EmitDbgValuePass is about to insert.  With
		// LineTablesOnly, GDB sees line info but `info scope` / `p $N`
		// report nothing.
		mlir::LLVM::DIScopeForLLVMFuncOpPassOptions scopeOpts;
		scopeOpts.emissionKind = mlir::LLVM::DIEmissionKind::Full;
		passManager.addPass(mlir::LLVM::createDIScopeForLLVMFuncOpPass(scopeOpts));
		// Emit llvm.intr.dbg.value intrinsics for each Nautilus SSA
		// value so GDB/LLDB can resolve `p $N` at a breakpoint.  Must
		// run after the subprogram pass because dbg.value's
		// DILocalVariable requires a DISubprogram scope.
		passManager.addPass(createEmitDbgValuePass());
	}

	// Run passes.
	if (mlir::failed(passManager.run(*module))) {
		llvm::errs() << "MLIRPassManager::lowerAndOptimizeMLIRModule: Failed to "
		                "apply passes to generated MLIR"
		             << '\n';
		return 1;
	}
	return 0;
}
} // namespace nautilus::compiler::mlir
