#pragma once

#include "nautilus/compiler/backends/CompilationBackend.hpp"

namespace nautilus::compiler::bc {

/**
 * @brief Compilation backend that uses a byte code interpreter.
 */
class BCInterpreterBackend : public CompilationBackend {
public:
	std::unique_ptr<Executable> compile(std::shared_ptr<ir::IRGraph> ir) override;
};

} // namespace nautilus::compiler::bc
