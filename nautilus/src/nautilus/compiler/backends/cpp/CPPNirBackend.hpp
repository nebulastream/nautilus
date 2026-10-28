#pragma once

#include "nautilus/compiler/backends/nir/NirBackend.hpp"
#include <string>

namespace nautilus::compiler::cpp {

/// The `cpp` backend on NIR (M4.3): prints C++ from the module and compiles it with the system compiler.
class CPPNirBackend : public nirbackend::NirBackend {
public:
	std::unique_ptr<Executable> compile(std::shared_ptr<const nir::Module> module,
	                                    const nirbackend::CompileContext& context) const override;

	/// As `compile`, with the name of the compilation unit that names the generated files.
	std::unique_ptr<Executable> compileNamed(std::shared_ptr<const nir::Module> module,
	                                         const nirbackend::CompileContext& context, const std::string& unit) const;
};

} // namespace nautilus::compiler::cpp
