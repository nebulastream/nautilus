#pragma once

#include "nautilus/compiler/backends/nir/NirBackend.hpp"
#include "nautilus/nir/Module.hpp"
#include "nautilus/options.hpp"
#include <asmjit/core.h>
#include <cstdint>
#include <string>
#include <unordered_map>

// The architectures that have a NIR lowering; the others keep the bridge to the 0.3 IR until theirs lands.
#if defined(__x86_64__) || defined(_M_X64)
#define NAUTILUS_ASMJIT_NIR_LOWERING 1
#endif

namespace nautilus::compiler {
class CompilationStatistics;
class DumpHandler;
} // namespace nautilus::compiler

namespace nautilus::compiler::asmjit {

/// What lowering a module leaves in the JIT runtime.
struct NirLowerResult {
	void* basePtr = nullptr;                        ///< Single JIT allocation base; release exactly once.
	std::unordered_map<std::string, void*> jitPtrs; ///< Per-function pointers within the allocation.
	uint64_t codeSize = 0;                          ///< Total emitted machine-code size in bytes.
	std::string asmjitIR;                           ///< AsmJit builder node list (only captured when dumping).
	std::string assembly;                           ///< Generated assembly listing (only captured when dumping).
};

/// Lowers every function of the SSA module `module` into one JIT allocation of `runtime`, with the lowering of the
/// host architecture (`X64NirLowering.cpp`, `A64NirLowering.cpp`). Statistics counters appear under `asmjit.lowering.*`
/// and `asmjit.peephole.*`. Returns an empty result when the runtime refuses the code.
NirLowerResult lowerNirModule(const nir::Module& module, ::asmjit::JitRuntime& runtime, const engine::Options& options,
                              const DumpHandler& dumpHandler, CompilationStatistics* statistics,
                              const nirbackend::HostEnvironment& host);

} // namespace nautilus::compiler::asmjit
