
#include "nautilus/compiler/backends/mlir/debug/DebugInfoOptions.hpp"
#include <atomic>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <sys/types.h>
#include <unistd.h>

namespace nautilus::compiler::mlir {

namespace {

// Returns $TMPDIR, $TEMP, or "/tmp" if neither is set. The debug
// source file is written under this directory.
std::string tempDir() {
	if (const char* t = std::getenv("TMPDIR")) {
		return t;
	}
	if (const char* t = std::getenv("TEMP")) {
		return t;
	}
	return "/tmp";
}

// Generate a path like
// `$TMPDIR/nautilus_debug_<pid>_<counter>.<ext>` that is unique
// across parallel compilations within the same process.
std::string synthesizeSourcePath(const std::string& extension) {
	static std::atomic<uint64_t> counter {0};
	std::filesystem::path dir = tempDir();
	auto name =
	    "nautilus_debug_" + std::to_string(::getpid()) + "_" + std::to_string(counter.fetch_add(1)) + "." + extension;
	return (dir / name).string();
}

} // namespace

DebugInfoOptions debugInfoOptionsFromEngineOptions(const engine::Options& options) {
	DebugInfoOptions opts;
	opts.enable = options.getOptionOrDefault("mlir.debug.enable", false);
	opts.sourceMode = options.getOptionOrDefault<std::string>("mlir.debug.source_mode", "mlir");
	opts.sourceFile = options.getOptionOrDefault<std::string>("mlir.debug.source_file", "");
	opts.producer = options.getOptionOrDefault<std::string>("mlir.debug.producer", "Nautilus JIT");
	opts.dwarfVersion = options.getOptionOrDefault("mlir.debug.dwarf_version", 4);
	opts.autoRegisterGdbListener = options.getOptionOrDefault("mlir.debug.auto_register_gdb", true);

	if (opts.enable && opts.sourceFile.empty()) {
		const std::string ext = (opts.sourceMode == "nautilus-ir") ? "ir" : "mlir";
		opts.sourceFile = synthesizeSourcePath(ext);
	}
	return opts;
}

} // namespace nautilus::compiler::mlir
