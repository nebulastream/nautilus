#pragma once

#include "nautilus/compiler/backends/CompilationBackend.hpp"

namespace nautilus::compiler::cuda {

class CUDACompilationBackend : public CompilationBackend {
public:
	std::unique_ptr<Executable> compile(const std::shared_ptr<ir::IRGraph>& ir, const DumpHandler& dumpHandler,
	                                    const engine::Options& options,
	                                    CompilationStatistics* statistics = nullptr) const override;
	/// The kernel classification reads the `kernel` attribute of the 0.3 trace
	[[nodiscard]] bool needs03Frontend() const override {
		return true;
	}
};

} // namespace nautilus::compiler::cuda
