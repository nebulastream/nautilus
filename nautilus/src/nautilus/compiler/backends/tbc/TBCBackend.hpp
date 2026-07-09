#pragma once

#include "nautilus/compiler/backends/CompilationBackend.hpp"

namespace nautilus::compiler::tbc {

/**
 * @brief The TBC (threaded bytecode) backend: a from-scratch, high-performance
 * bytecode interpreter.
 *
 * Compared to the legacy bc backend it uses tail-call (musttail) or
 * computed-goto dispatch over one flat instruction stream, interpreter-native
 * calls between module functions (no FFI round trip), a contiguous per-thread
 * VM stack with in-frame allocas, and default-on compare-and-branch fusion and
 * immediate folding. It never allocates executable memory, so it runs on
 * platforms that forbid runtime code generation (iOS).
 */
class TBCBackend : public CompilationBackend {
public:
	std::unique_ptr<Executable> compile(const std::shared_ptr<ir::IRGraph>& ir, const DumpHandler& dumpHandler,
	                                    const engine::Options& options,
	                                    CompilationStatistics* statistics = nullptr) const override;
};

} // namespace nautilus::compiler::tbc
