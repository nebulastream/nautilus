#pragma once

#include "nautilus/common/Arena.hpp"
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

	/// The LLVM pipeline behind this backend (`makeOptimizingTransformer` at
	/// the requested `optimizationLevel`, -O3 by default) runs InstCombine,
	/// SCCP, SimplifyCFG, ADCE and function-attribute inference over the
	/// lowered module. Between them they subsume every pass in the
	/// Nautilus-IR optimization group, so the group is pure compile-time
	/// overhead on the way to this backend.
	[[nodiscard]] bool benefitsFromIROptimizationPasses() const override {
		return false;
	}

private:
	/// Arena pool for the per-compile lowering scratch state (block frames,
	/// value and block maps). Every compile() draws one arena, so the
	/// hundreds of small map nodes a lowering allocates become a handful of
	/// chunk allocations, and the chunks are recycled across compiles. The
	/// pool is internally synchronized; compile() may run concurrently on
	/// the main thread and on a tiered-promotion thread.
	mutable common::ArenaPool loweringArenaPool_;
};

} // namespace nautilus::compiler::mlir
