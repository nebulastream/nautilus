#pragma once

#include "nautilus/compiler/backends/CompilationBackend.hpp"

namespace nautilus::compiler::bc {

/**
 * @brief Compilation backend that uses a byte code interpreter.
 */
class BCInterpreterBackend : public CompilationBackend {
public:
	std::unique_ptr<Executable> compile(const std::shared_ptr<ir::IRGraph>& ir,const DumpHandler& dumpHandler, const engine::Options& options) const override;
};

} // namespace nautilus::compiler::bc
