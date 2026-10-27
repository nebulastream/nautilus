#pragma once

#include "nautilus/compiler/backends/CompilationBackend.hpp"

namespace nautilus::compiler::nirbackend {

/// A 0.3 backend that compiles the NIR of the same trace instead of the IR: the pseudo-backend `nirtbc`. The pipeline
/// attaches the SSA module to the IR graph under `tracing.nirChain`, and this backend hands it to the `tbc` backend on
/// NIR. It lets every execution test run through the 0.3 tracer's mapping to NIR, the conversion and a NIR backend
/// before the cut-over (M4.2), when the engine uses the NIR interface directly and this class is deleted.
class NirChainBackend : public CompilationBackend {
public:
	std::unique_ptr<Executable> compile(const std::shared_ptr<ir::IRGraph>& ir, const DumpHandler& dumpHandler,
	                                    const engine::Options& options,
	                                    CompilationStatistics* statistics = nullptr) const override;

	[[nodiscard]] ExceptionPropagationMode getExceptionPropagationMode() const override {
		return ExceptionPropagationMode::CapturedHostRethrow;
	}

	/// The IR passes are not needed: the IR is only the carrier of the NIR.
	[[nodiscard]] IROptimizationLevel irOptimizationLevel() const override {
		return IROptimizationLevel::None;
	}
};

} // namespace nautilus::compiler::nirbackend
