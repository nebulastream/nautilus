#pragma once

#include "nautilus/options.hpp"
#include <string>

namespace nautilus::compiler::mlir {

// Configuration for emitting DWARF debug information during MLIR compilation.
// Populated from the public engine::Options by fromEngineOptions() so the rest
// of the backend does not depend on the string-keyed Options header.
struct DebugInfoOptions {
	// Axis B: prioritise stepping fidelity over performance. When true, LLVM
	// optimization is clamped to -O0, the MLIR inliner is skipped in
	// "nautilus-ir" source mode, and every `$N` gets a shadow alloca +
	// dbg.declare so a debugger can print it. `mlir.debug.enable`.
	bool enableDebug = false;

	// "mlir" snapshots the post-inline MLIR to disk and uses its line numbers
	// as "source" locations.  "nautilus-ir" dumps the Nautilus IR to disk
	// instead and lets MLIRLoweringProvider tag every emitted MLIR op with a
	// FileLineColLoc pointing into that dump.
	std::string sourceMode = "mlir";

	// Absolute path where the "source" file is written.  When empty, a unique
	// path in the temp directory (or `mlir.debug.source_dir`) is synthesized
	// per compilation.  A relative path given by the user is made absolute, so
	// the DWARF never depends on the debugger's DW_AT_comp_dir.
	std::string sourceFile;

	// DW_AT_producer string that shows up in DWARF (e.g. `gdb` calls this
	// "produced by" on `info source`).
	std::string producer = "Nautilus JIT";

	// DWARF version emitted as an LLVM module flag.  4 is most compatible
	// with older GDBs; LLVM's modern default is 5.
	int dwarfVersion = 4;

	// Register every JIT-linked object with the debugger via the GDB JIT
	// interface (__jit_debug_register_code).  Without this the emitted DWARF
	// is present in the object but invisible to GDB/LLDB, so IDEs cannot
	// step into JIT-compiled code.  Only honoured when `enableDebug` is true.
	bool registerWithDebugger = true;

	// Emit perf jitdump records (JIT_CODE_LOAD / JIT_CODE_DEBUG_INFO /
	// JIT_CODE_UNWINDING_INFO) for every JIT-linked object and keep the
	// requested optimization level, so `perf record` can symbolize and
	// attribute samples inside *production-optimized* JIT-compiled code.
	// Linux/ELF only; a no-op with a warning elsewhere. Unlike `enableDebug`,
	// this never clamps anything itself -- it only pulls in Axis A metadata
	// (below) and installs the jitdump writer. `mlir.perf.enable`.
	bool enablePerf = false;

	// Whether perf jitdump records include DWARF-derived line tables
	// (JIT_CODE_DEBUG_INFO). Only meaningful when `enablePerf` is true.
	bool perfEmitDebugInfo = true;

	// Whether perf jitdump records include .eh_frame-derived unwind info
	// (JIT_CODE_UNWINDING_INFO), needed for `perf record --call-graph dwarf`
	// to walk out of a JIT frame. Only meaningful when `enablePerf` is true.
	bool perfEmitUnwindInfo = true;

	// Emit one jitdump symbol per region() scope, qualified by its nesting
	// (`execute::outer::hot`), so regions are visible in a perf profile.
	// The DWARF inlined-subroutine chain that gives a region its own GDB
	// backtrace frame cannot travel through the jitdump -- the format has no
	// scope tree -- so without this a region collapses silently into its
	// enclosing function. Requires `perfEmitDebugInfo` (the scope information
	// is recovered from the emitted DWARF). `mlir.perf.region_symbols`.
	bool perfRegionSymbols = true;

	// Add the `frame-pointer=all` function attribute to every generated
	// function when perf support is active, so the default frame-pointer
	// based unwinder (`perf record -g`) can walk out of a JIT frame -- at
	// -O3 LLVM omits frame pointers by default. `mlir.perf.frame_pointers`.
	// Only honoured when `enablePerf` is true.
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

// Synthesizes a path under the temp directory (ignoring `mlir.debug.source_dir`
// and the perf-only cwd default), for use when the configured source
// directory turns out not to be writable.
std::string debugSourceFallbackPath(const std::string& extension);

} // namespace nautilus::compiler::mlir
