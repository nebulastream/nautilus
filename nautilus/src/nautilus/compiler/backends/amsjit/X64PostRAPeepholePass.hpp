
#pragma once

#include <asmjit/core/builder.h>

namespace nautilus::compiler {
class CompilationStatistics;
} // namespace nautilus::compiler

namespace nautilus::compiler::asmjit {

/// Post-register-allocation peephole for the AsmJit x86-64 backend.
///
/// Runs as an ordinary ::asmjit::Pass. Because the pass is appended to the
/// Compiler's pass list AFTER `X86RAPass` (which is registered inside
/// `x86::Compiler::onAttach`), every operand it observes is either a
/// physical register, a memory reference, or an immediate — never a virtual
/// register. This enables correct same-register detection.
///
/// Two rules are applied, in linear time, with constant-time decisions per
/// node plus a bounded 16-instruction forward scan for EFLAGS liveness used
/// only by Rule 2:
///
///  1. Same-register move elimination (r64 mov, and 128-bit vector moves).
///     Crucially does NOT remove `mov r32, r32` even when src==dst, since
///     that form is intentionally used for its zero-extension side effect.
///
///  2. Zero idiom: `mov r, 0` → `xor r32, r32` (shorter encoding, breaks
///     dependency). Gated by a forward EFLAGS-liveness scan to avoid
///     corrupting a downstream flag consumer.
///
/// Per-run rewrite counts are recorded into the supplied
/// @ref CompilationStatistics (if non-null) under the keys
/// `asmjit.peephole.selfMovesRemoved` and `asmjit.peephole.zeroIdiomsApplied`.
class X64PostRAPeepholePass : public ::asmjit::Pass {
public:
	explicit X64PostRAPeepholePass(CompilationStatistics* statistics) noexcept
	    : ::asmjit::Pass("X64PostRAPeepholePass"), statistics_(statistics) {
	}

	::asmjit::Error run(::asmjit::Zone* zone, ::asmjit::Logger* logger) override;

private:
	CompilationStatistics* statistics_ = nullptr;
};

} // namespace nautilus::compiler::asmjit
