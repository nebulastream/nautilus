#pragma once

#include "nautilus/compiler/JitSymbolRegistry.hpp"
#include "nautilus/nir/Module.hpp"
#include "nautilus/options.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace nautilus::compiler {

/// The offset in the generated code at which the code of NIR instruction `inst` of function `function` starts. `inst`
/// is `nir::NONE` for code that belongs to no instruction (the cold paths and the exit of a function).
struct NirCodeMark {
	uint32_t function;
	uint32_t inst;
	uint32_t offset;
};

/// Where the code of a module lies: one block of executable memory, the offset of each function in it, and the marks
/// of its instructions in code order.
struct NirCodeMap {
	uintptr_t base = 0;
	uint32_t size = 0;
	/// By function index. A function runs to the start of the next one.
	std::vector<uint32_t> functionOffsets;
	std::vector<NirCodeMark> marks;
};

/// Whether the options ask for the code of a module to be named for a profiler (`perf` or `perf.sample`).
[[nodiscard]] bool profilesCode(const engine::Options& options);

/// Names the code of a module for a profiler (D14, 05 §7), for the backends that generate it themselves (`asmjit`,
/// the copy-and-patch mode of `tbc`):
///
///  * `perf.sample`: one symbol per function, and per `region()` with its nesting (`execute::outer::hot`), in the
///    process-wide `JitSymbolRegistry`; and the line table (code offset -> line of the text form of the module) that
///    `JitSymbolRegistry::resolveLine` serves, so that the `profiling` plugin attributes a sample to the NIR
///    instruction it landed in;
///  * `perf`: the same symbols and lines in the perf jitdump, for `perf record` and `perf inject --jit`.
///
/// The regions come from the module's debug sections (the NIR tracer records them for these options). The line table
/// needs the text form of the module on disk, written the way `mlir` writes its source file. Returns the registry
/// handle to withdraw when the code is freed (`NO_MODULE` if nothing was published).
ModuleIndex publishNirCode(const nir::Module& module, const NirCodeMap& code, const engine::Options& options,
                           const std::string& unit);

/// A fresh unit name `<prefix>_<pid>_<n>` for the registry.
std::string newCodeUnit(const std::string& prefix);

} // namespace nautilus::compiler
