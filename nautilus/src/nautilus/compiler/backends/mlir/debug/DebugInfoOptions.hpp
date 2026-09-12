#pragma once

#include "nautilus/options.hpp"
#include <string>

namespace nautilus::compiler::mlir {

// Configuration for emitting DWARF debug information during MLIR compilation.
// Populated from the public engine::Options by fromEngineOptions() so the rest
// of the backend does not depend on the string-keyed Options header.
struct DebugInfoOptions {
	// When false, all debug-related plumbing is a no-op and the backend
	// behaves identically to a build with no debug support.
	bool enable = false;

	// "mlir" snapshots the post-inline MLIR to disk and uses its line numbers
	// as "source" locations.  "nautilus-ir" dumps the Nautilus IR to disk
	// instead and lets MLIRLoweringProvider tag every emitted MLIR op with a
	// FileLineColLoc pointing into that dump.
	std::string sourceMode = "mlir";

	// Absolute path where the "source" file is written.  When empty, a unique
	// path in the working directory (or `mlir.debug.source_dir`) is
	// synthesized per compilation.  A relative path given by the user is made
	// absolute, so the DWARF never depends on the debugger's DW_AT_comp_dir.
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
	// step into JIT-compiled code.  Only honoured when `enable` is true.
	bool registerWithDebugger = true;
};

// Build a DebugInfoOptions from the string-keyed engine::Options.
DebugInfoOptions debugInfoOptionsFromEngineOptions(const engine::Options& options);

} // namespace nautilus::compiler::mlir
