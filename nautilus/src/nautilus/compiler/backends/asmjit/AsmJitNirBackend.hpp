#pragma once

#include "nautilus/compiler/backends/nir/NirBackend.hpp"

namespace nautilus::compiler::asmjit {

/// The `asmjit` backend on NIR (M4.4): lowers the SSA module straight to AsmJit's `Compiler`, without the 0.3 IR.
class AsmJitNirBackend : public nirbackend::NirBackend {
public:
	std::unique_ptr<Executable> compile(std::shared_ptr<const nir::Module> module,
	                                    const nirbackend::CompileContext& context) const override;
};

} // namespace nautilus::compiler::asmjit
