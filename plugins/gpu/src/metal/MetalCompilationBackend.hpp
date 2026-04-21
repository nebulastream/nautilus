#pragma once

#include "nautilus/compiler/backends/CompilationBackend.hpp"

namespace nautilus::compiler::metal {

class MetalCompilationBackend : public CompilationBackend {
public:
	std::unique_ptr<Executable> compile(const std::shared_ptr<ir::IRGraph>& ir, const DumpHandler& dumpHandler,
	                                    const engine::Options& options,
	                                    CompilationStatistics* statistics = nullptr) const override;
};

} // namespace nautilus::compiler::metal
