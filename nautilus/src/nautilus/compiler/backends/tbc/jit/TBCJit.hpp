#pragma once

#include "nautilus/compiler/backends/tbc/TBCCode.hpp"
#include "nautilus/compiler/backends/tbc/TBCVm.hpp"
#include <cstdint>
#include <memory>
#include <string>

// Copy-and-patch JIT execution mode for the tbc backend (tbc.mode=jit).
// Compiled only when ENABLE_TBC_JIT is on.

namespace nautilus::compiler::tbc::jit {

/// True when this build can stitch AND execute copy-and-patch code:
/// a generated stencil table exists for the target, the host compiler can
/// call through a preserve_none function pointer (Clang 19+), and ASan is
/// off (same restriction as the interpreter's tail-call skin).
bool jitRuntimeAvailable();

/// Stitch every function of `program` into one executable span. Returns
/// nullptr (and sets `whyNot`) when the program cannot be stitched — e.g. an
/// instruction has no stencil in a stale generated table; callers then leave
/// the program to the interpreter. `program.functions`/`program.callsites`
/// must be final: their element addresses are patched into the code.
std::unique_ptr<TBCJitCode> stitchProgram(const TBCProgram& program, std::string* whyNot);

/// Execute `program.functions[functionIndex]` through its stitched entry.
/// Same contract as tbc::invoke (raw 64-bit argument/result slots); requires
/// `program.jit` to be set.
uint64_t invokeJit(const TBCProgram& program, uint32_t functionIndex, const uint64_t* args, size_t argCount);

} // namespace nautilus::compiler::tbc::jit

// Plain-ABI helpers defined in TBCStencilHelpers.cpp; the stitcher patches
// their addresses into stencil holes. Signatures must match the declarations
// in TBCStencilSource.cpp (the stencils were compiled against them).
extern "C" {
uint64_t* tbcJitPushFrame(nautilus::compiler::tbc::VMContext* ctx, const void* callee, const void* site,
                          uint64_t* callerFp, void* nativeReturnAddress, uint64_t dstRegRaw) noexcept;
uint64_t tbcJitExtCall(nautilus::compiler::tbc::VMContext* ctx, const void* site, uint64_t* fp,
                       uint64_t dstRegRaw) noexcept;
uint64_t tbcJitIndCall(nautilus::compiler::tbc::VMContext* ctx, const void* site, uint64_t* fp, uint64_t dstRegRaw,
                       void* target) noexcept;
}
