
#pragma once

#include "nautilus/compiler/backends/CompilationBackend.hpp"

namespace nautilus::compiler::cpp {

/**
 * @brief Compilation backend that generates cpp code and compiles it at runtime
 */
class CPPCompilationBackend : public CompilationBackend {
public:
	std::unique_ptr<Executable> compile(std::shared_ptr<ir::IRGraph> ir) override;
};

} // namespace nautilus::compiler::cpp
