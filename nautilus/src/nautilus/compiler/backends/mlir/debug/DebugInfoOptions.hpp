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

	// Absolute path where the "source" file is written.  When empty, a
	// unique path under $TMPDIR is synthesized per compilation.
	std::string sourceFile;

	// DW_AT_producer string that shows up in DWARF (e.g. `gdb` calls this
	// "produced by" on `info source`).
	std::string producer = "Nautilus JIT";

	// DWARF version emitted as an LLVM module flag.  4 is most compatible
	// with older GDBs; LLVM's modern default is 5.
	int dwarfVersion = 4;

	// When true, the backend automatically attaches
	// llvm::JITEventListener::createGDBRegistrationListener() to the OrcJIT.
	// Turn off if you want to manage the listener list yourself via
	// engine::Options::addMLIRJitEventListener().
	bool autoRegisterGdbListener = true;
};

// Build a DebugInfoOptions from the string-keyed engine::Options.
DebugInfoOptions debugInfoOptionsFromEngineOptions(const engine::Options& options);

} // namespace nautilus::compiler::mlir
