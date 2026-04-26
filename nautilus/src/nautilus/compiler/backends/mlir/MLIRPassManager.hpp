#pragma once
#include "nautilus/compiler/backends/mlir/debug/DebugInfoOptions.hpp"
#include <mlir/IR/BuiltinOps.h>
#include <mlir/Pass/Pass.h>
#include <vector>

namespace nautilus::compiler::mlir {

// The MLIRPassManager takes a generated MLIR module,
// and applies configured lowering & optimization passes to it.
class MLIRPassManager {
public:
	enum class OptimizationPass : uint8_t { Inline };

	// Run the lowering/optimization pipeline on `module`.  When
	// `debugInfo.enable` is true, inserts the
	// Nautilus debug-info passes (location snapshot for mlir source
	// mode, DIScopeForLLVMFuncOp to materialize a DISubprogram on every
	// llvm.func).  Returns non-zero on pass-pipeline failure.
	static int lowerAndOptimizeMLIRModule(::mlir::OwningOpRef<::mlir::ModuleOp>& module,
	                                      const std::vector<OptimizationPass>& optimizationPasses,
	                                      const DebugInfoOptions& debugInfo = {});
};
} // namespace nautilus::compiler::mlir
