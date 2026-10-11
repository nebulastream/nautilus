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

	/// LLVM folds constants and eliminates dead code itself, but it does not
	/// recover from every CFG shape a trace leaves behind: a forwarding block
	/// that empty-block elimination would remove gives a search loop two
	/// headers, and LLVM then keeps its data-dependent branch instead of
	/// turning it into selects (binarySearch ran 3x slower). The full IR
	/// pipeline costs about 1% of an MLIR compile.
	[[nodiscard]] IROptimizationLevel irOptimizationLevel() const override {
		return IROptimizationLevel::Full;
	}
};

} // namespace nautilus::compiler::mlir
