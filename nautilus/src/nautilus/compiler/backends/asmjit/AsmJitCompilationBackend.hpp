
#pragma once

#include "nautilus/compiler/backends/CompilationBackend.hpp"

namespace nautilus::compiler::asmjit {

/**
 * @brief Compilation backend that JIT-compiles IR to native machine code using AsmJit
 */
class AsmJitCompilationBackend : public CompilationBackend {
public:
	std::unique_ptr<Executable> compile(const std::shared_ptr<ir::IRGraph>& ir, const DumpHandler& dumpHandler,
	                                    const engine::Options& options,
	                                    CompilationStatistics* statistics = nullptr) const override;

	/// The graph of the NIR front end carries a module that this backend compiles itself (M4.4).
	[[nodiscard]] bool compilesNir() const override {
		return true;
	}

	[[nodiscard]] ExceptionPropagationMode getExceptionPropagationMode() const override {
		return ExceptionPropagationMode::CapturedHostRethrow;
	}
};

} // namespace nautilus::compiler::asmjit
