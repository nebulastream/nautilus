
#pragma once

#include <asmjit/core/builder.h>

namespace nautilus::compiler {
class CompilationStatistics;
} // namespace nautilus::compiler

namespace nautilus::compiler::asmjit {

/// Post-register-allocation peephole for the AsmJit AArch64 backend.
///
/// Mirrors `X64PostRAPeepholePass` minus the zero-idiom rewrite, which ARM64's
/// dedicated zero register (`xzr`/`wzr`) makes unnecessary:
///
///  1. Same-register move elimination (allow-list below).
///  2. A `b` to the label bound right after it is removed.
///  3. `b.cond A; b B; A:` becomes `b.!cond B; A:` (and `cbz`/`cbnz` invert
///     likewise). Rules 2 and 3 run to a fixpoint.
///
/// Rule 1 allow-list:
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
/// @ref CompilationStatistics (if non-null) under the keys
/// `asmjit.peephole.selfMovesRemoved` and `asmjit.peephole.jumpsRemoved`.
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
