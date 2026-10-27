#pragma once

#include "nautilus/compiler/backends/nir/NirBackend.hpp"

namespace nautilus::compiler::tbc {

/// The threaded bytecode backend on NIR (M4.1). It fills the same `TBCProgram` as the 0.3 lowering, so the
/// interpreter, the copy-and-patch stitcher and the executable are shared.
class TBCNirBackend : public nirbackend::NirBackend {
public:
	std::unique_ptr<Executable> compile(std::shared_ptr<const nir::Module> module,
	                                    const nirbackend::CompileContext& context) const override;
};

} // namespace nautilus::compiler::tbc
