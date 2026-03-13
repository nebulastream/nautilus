#pragma once

#include "nautilus/compiler/backends/CompilationBackend.hpp"

namespace nautilus::compiler::tpde {

/// Compilation backend that generates native x64 code via TPDE's single-pass
/// compiler framework (instruction selection + register allocation + encoding
/// in one pass).  Outputs an ELF object file and loads it into executable
/// memory using TPDE's ElfMapper.
class TPDECompilationBackend : public CompilationBackend {
public:
	std::unique_ptr<Executable> compile(const std::shared_ptr<ir::IRGraph>& ir, const DumpHandler& dumpHandler,
	                                    const engine::Options& options) const override;
};

} // namespace nautilus::compiler::tpde
