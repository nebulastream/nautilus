#pragma once

#include "nautilus/options.hpp"
#include <string>

namespace nautilus::compiler::mlir {

// Configuration for emitting DWARF debug information during MLIR compilation.
// Populated from the public engine::Options by fromEngineOptions() so the rest
// of the backend does not depend on the string-keyed Options header.
//
// Only `enableDebug` and `enablePerf` are user-settable (`debug` / `perf`).
// Everything else here is an implementation constant: earlier revisions
// exposed each of these individually (source mode, source directory,
// DWARF producer/version, per-record perf toggles, ...), but every one of
// them only ever had one sensible value in practice, so the knob was pure
// surface area without a real choice behind it.
struct DebugInfoOptions {
	// Axis B: prioritise stepping fidelity over performance. When true, LLVM
	// optimization is clamped to -O0 (unless `optimizationLevel` overrides
	// it), the MLIR inliner is skipped, and every `$N` gets a shadow alloca +
	// dbg.declare so a debugger can print it. `debug`.
	bool enableDebug = false;

	// Absolute path to the Nautilus-IR dump DWARF line numbers point into.
	// Always synthesized: a unique path in the temp directory, or -- for a
	// perf-only compile -- the working directory, since the dump has to
	// outlive the compile for `perf inject`/`perf annotate` to find it later.
	std::string sourceFile;

	// DWARF version emitted as an LLVM module flag. 4 is most compatible with
	// older GDBs; LLVM's modern default is 5.
	int dwarfVersion = 4;

	// Emit perf jitdump records (JIT_CODE_LOAD / JIT_CODE_DEBUG_INFO /
	// JIT_CODE_UNWINDING_INFO) for every JIT-linked object and keep the
	// requested optimization level, so `perf record` can symbolize and
	// attribute samples inside *production-optimized* JIT-compiled code.
	// Linux/ELF only; a no-op with a warning elsewhere. Unlike `enableDebug`,
	// this never clamps anything itself -- it only pulls in Axis A metadata
	// (below) and installs the jitdump writer. `perf`.
	bool enablePerf = false;

	// Whether perf jitdump records include DWARF-derived line tables
	// (JIT_CODE_DEBUG_INFO). Always true when `enablePerf` is true -- a
	// perf-only compile with no line table would defeat the point of it.
	bool perfEmitDebugInfo = true;

	// Whether perf jitdump records include .eh_frame-derived unwind info
	// (JIT_CODE_UNWINDING_INFO), needed for `perf record --call-graph dwarf`
	// to walk out of a JIT frame. Always true when `enablePerf` is true.
	bool perfEmitUnwindInfo = true;

	// Emit one jitdump symbol per region() scope, qualified by its nesting
	// (`execute::outer::hot`), so regions are visible in a perf profile.
	// Always true when `enablePerf` is true: the DWARF inlined-subroutine
	// chain that gives a region its own GDB backtrace frame cannot travel
	// through the jitdump -- the format has no scope tree -- so without this
	// a region collapses silently into its enclosing function.
	bool perfRegionSymbols = true;

	// Add the `frame-pointer=all` function attribute to every generated
	// function when perf support is active, so the default frame-pointer
	// based unwinder (`perf record -g`) can walk out of a JIT frame -- at
	// -O3 LLVM omits frame pointers by default. Always true when `enablePerf`
	// is true.
	bool perfFramePointers = true;

	// Axis A: whether *any* debug metadata is emitted at all -- line tables,
	// per-op scopes, region() inlined-subroutines. Cheap: no codegen impact
	// of its own. Both `enableDebug` (for a debugger) and `enablePerf` (for
	// perf's jitdump line tables) need this; only `enableDebug` additionally
	// wants the fidelity measures gated on it directly (see above).
	[[nodiscard]] bool emitDebugInfo() const {
		return enableDebug || enablePerf;
	}
};

// Build a DebugInfoOptions from the string-keyed engine::Options.
DebugInfoOptions debugInfoOptionsFromEngineOptions(const engine::Options& options);

// Synthesizes a path under the temp directory (ignoring the perf-only cwd
// default), for use when the configured source directory turns out not to be
// writable.
std::string debugSourceFallbackPath(const std::string& extension);

} // namespace nautilus::compiler::mlir
