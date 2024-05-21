#pragma once

#include "nautilus/compiler/backends/CompilationBackend.hpp"

namespace nautilus::compiler::mlir {

/**
 * @brief Compilation backend that uses MLIR.
 */
class MLIRCompilationBackend : public CompilationBackend {
public:
	std::unique_ptr<Executable> compile(std::shared_ptr<ir::IRGraph> ir) override;
};

} // namespace nautilus::compiler::mlir
