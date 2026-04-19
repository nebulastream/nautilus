
#pragma once

#include <asmjit/core/builder.h>

namespace nautilus::compiler {
class CompilationStatistics;
} // namespace nautilus::compiler

namespace nautilus::compiler::asmjit {

/// Post-register-allocation peephole for the AsmJit AArch64 backend.
///
/// Mirrors `X64PostRAPeepholePass` but implements only Rule 1 (same-register
/// move elimination), because ARM64 already has a dedicated zero register
/// (`xzr`/`wzr`) that makes the x86 zero-idiom rewrite unnecessary.
///
/// Allow-list:
///   * `mov Xd, Xm` when Xd == Xm (64-bit GPR; encoded as `orr Xd, xzr, Xm`).
///
/// Explicitly NOT removed:
///   * `mov Wd, Wm` — writes to a W-register zero-extend the upper 32 bits,
///     so self-move may be load-bearing. Conservative.
///   * `fmov Sd, Sm` / `fmov Dd, Dm` — these zero the upper bits of the
///     V register (bits [127:32] for S, [127:64] for D) and are therefore
///     NOT pure no-ops. Removing them leaves stale upper bits that may be
///     observed when the same V register is later accessed at a wider width.
///
/// Per-run rewrite counts are recorded into the supplied
/// @ref CompilationStatistics (if non-null) under the key
/// `asmjit.peephole.selfMovesRemoved`.
class A64PostRAPeepholePass : public ::asmjit::Pass {
public:
	explicit A64PostRAPeepholePass(CompilationStatistics* statistics) noexcept
	    : ::asmjit::Pass("A64PostRAPeepholePass"), statistics_(statistics) {
	}

	::asmjit::Error run(::asmjit::Zone* zone, ::asmjit::Logger* logger) override;

private:
	CompilationStatistics* statistics_ = nullptr;
};

} // namespace nautilus::compiler::asmjit
