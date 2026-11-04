#pragma once

#include "nautilus/compiler/backends/tbc/TBCCode.hpp"
#include "nautilus/options.hpp"
#include <chrono>
#include <cstdint>

namespace nautilus::compiler {
class CompilationStatistics;
}

namespace nautilus::compiler::tbc {

/// Sets the dispatch mode and the stack size of a program from the options (`tbc.dispatch`, `tbc.stackSizeKb`).
void configureProgram(TBCProgram& program, const engine::Options& options);

/// The last compile step of both lowerings: stitches the copy-and-patch code when `tbc.mode` asks for it and records
/// the statistics. The function and call-site vectors of `program` must be final.
void finishProgram(TBCProgram& program, const engine::Options& options, CompilationStatistics* statistics,
                   int64_t totalInstructions, int64_t maxRegisters, std::chrono::steady_clock::time_point backendStart);

} // namespace nautilus::compiler::tbc
