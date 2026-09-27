#pragma once

#include "nautilus/compiler/backends/CompilationBackend.hpp"

namespace nautilus::compiler::mlir {

/**
 * @brief Compilation backend that uses MLIR.
 */
class MLIRCompilationBackend : public CompilationBackend {
public:
	MLIRCompilationBackend();
	std::unique_ptr<Executable> compile(const std::shared_ptr<ir::IRGraph>& ir, const DumpHandler& dumpHandler,
	                                    const engine::Options& options,
	                                    CompilationStatistics* statistics = nullptr) const override;

	[[nodiscard]] ExceptionPropagationMode getExceptionPropagationMode() const override {
		return ExceptionPropagationMode::NativeUnwind;
	}

	/// LLVM's pipeline folds constants, simplifies the CFG and eliminates
	/// dead code itself, so those IR passes would only repeat that work
	/// before the lowering -- but it is slower on a graph whose block
	/// arguments were never pruned (see `IROptimizationLevel`).
	[[nodiscard]] IROptimizationLevel irOptimizationLevel() const override {
		return IROptimizationLevel::ArgumentPruning;
	}
};

} // namespace nautilus::compiler::mlir
