

#include "nautilus/compiler/backends/mlir/MLIRPassManager.hpp"
#include "nautilus/compiler/backends/mlir/debug/EmitDbgValuePass.hpp"
#include "nautilus/compiler/backends/mlir/debug/NormalizeInlineLocationsPass.hpp"
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

	const bool debugEnabled = debugInfo.enableDebug;
	// Tier 1 ("nautilus-ir" mode) depends on FileLineColLocs attached by
	// MLIRLoweringProvider that reference positions in the Nautilus IR dump.
	// MLIR's inliner rewrites locations into inlinedAt chains, one level per
	// inlined hop, and a perf-only compile keeps the inliner: that nesting is
	// what makes a profile attribute an inlined callee's time to where it was
	// called from. It also leaves some frames with no source position at all,
	// which the debug-info passes below must not be handed -- see
	// NormalizeInlineLocationsPass. Stepping, by contrast, wants predictable
	// un-inlined frames,
	// so `enableDebug` alone -- not `emitDebugInfo()` -- keeps the skip for a
	// debugger session.
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
	//
	// NOT simply `emitDebugInfo()`: these passes need *real* per-op locations
	// to fuse scopes onto, and there are only two sources of those --
	// LocationSnapshot just above (`enableDebug`, either source mode) or
	// MLIRLoweringProvider's locationMap_-driven FileLineColLocs
	// (`sourceMode == "nautilus-ir"`, either axis). A perf-only compile in
	// "mlir" mode has neither -- `mlir` source mode is explicitly out of
	// scope for perf (see the issue) and MLIRCompilationBackend never calls
	// setDebugInfo() for it -- so every op still carries the placeholder
	// `Query_1`/line-0 location getNameLoc() falls back to. Running
	// EmitDbgValuePass over that (fusing per-op scopes, wrapping region
	// CallSiteLoc chains, building a `DILexicalBlockAttr` per block) produces
	// malformed metadata -- verified to crash MLIR->LLVM translation, not
	// just emit an empty line table.
	const bool hasRealLocations = debugEnabled || debugInfo.sourceMode == "nautilus-ir";
	if (debugInfo.emitDebugInfo() && hasRealLocations) {
		// DIScopeForLLVMFuncOpPass dereferences the file location of an
		// inlined-call frame without checking that one was found, at the first
		// level and again at every level it recurses through. A frame the
		// inliner left without a source position -- routine for a materialized
		// constant -- therefore crashes the compile outright. This drops
		// exactly those frames and keeps the rest, at any depth.
		passManager.addPass(createNormalizeInlineLocationsPass());
		// Full emission (not the default LineTablesOnly) is only needed
		// when stepping: it keeps the DILocalVariables / dbg.value records
		// EmitDbgValuePass is about to insert for `$N`. A perf-only compile
		// has no such records to keep, and jitdump only reads line tables,
		// so LineTablesOnly is both sufficient and cheaper there.
		mlir::LLVM::DIScopeForLLVMFuncOpPassOptions scopeOpts;
		scopeOpts.emissionKind =
		    debugEnabled ? mlir::LLVM::DIEmissionKind::Full : mlir::LLVM::DIEmissionKind::LineTablesOnly;
		passManager.addPass(mlir::LLVM::createDIScopeForLLVMFuncOpPass(scopeOpts));
		// Emit llvm.intr.dbg.value intrinsics for each Nautilus SSA
		// value so GDB/LLDB can resolve `p $N` at a breakpoint (when
		// `enableDebug`), and/or fuse per-block and region() scopes onto
		// every op so a line table exists at all (always, when this pass
		// runs).  Must run after the subprogram pass because dbg.value's
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
