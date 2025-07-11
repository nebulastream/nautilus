#pragma once
#include <mlir/IR/BuiltinOps.h>
#include <mlir/Pass/Pass.h>
#include <vector>

namespace nautilus::compiler::mlir {

// The MLIRPassManager takes a generated MLIR module,
// and applies configured lowering & optimization passes to it.
class MLIRPassManager {
public:
	enum class OptimizationPass : uint8_t { Inline };
	static int lowerAndOptimizeMLIRModule(::mlir::OwningOpRef<::mlir::ModuleOp>& module,
	                                      const std::vector<OptimizationPass>& optimizationPasses);
};
} // namespace nautilus::compiler::mlir
