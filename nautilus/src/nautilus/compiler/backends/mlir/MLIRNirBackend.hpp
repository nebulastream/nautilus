#pragma once

#include "nautilus/compiler/backends/nir/NirBackend.hpp"
#include <string>

namespace nautilus::compiler::mlir {

/// The `mlir` backend on NIR (M4.5): lowers the SSA module to MLIR (`cf`, `arith`, `func`, `llvm`), runs the MLIR and
/// LLVM pipelines and links the result with ORC.
class MLIRNirBackend : public nirbackend::NirBackend {
public:
	MLIRNirBackend();

	std::unique_ptr<Executable> compile(std::shared_ptr<const nir::Module> module,
	                                    const nirbackend::CompileContext& context) const override;

	/// As `compile`, with the id of the compilation unit that names the code in profiles.
	std::unique_ptr<Executable> compileNamed(std::shared_ptr<const nir::Module> module,
	                                         const nirbackend::CompileContext& context, const std::string& unit) const;
};

} // namespace nautilus::compiler::mlir
