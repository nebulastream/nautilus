
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

// Directory the synthesized source file is written to: the process's working
// directory by default, since a debugger (and the IDE driving it) resolves a
// path there far more reliably than one under the per-user $TMPDIR — on macOS
// that is a /var/folders/... path an IDE will not have in its source roots, and
// it differs between a terminal run and an IDE-launched one. Falls back to the
// temp directory when the working directory is unusable (deleted, or not
// writable), which is where the file used to live unconditionally.
std::filesystem::path sourceDir(const engine::Options& options) {
	auto configured = options.getOptionOrDefault<std::string>("mlir.debug.source_dir", "");
	if (!configured.empty()) {
		return configured;
	}
	std::error_code ec;
	auto cwd = std::filesystem::current_path(ec);
	if (ec) {
		return tempDir();
	}
	return cwd;
}

// Generate a path like `<dir>/nautilus_debug_<pid>_<counter>.<ext>` that is
// unique across parallel compilations within the same process.
std::string synthesizeSourcePath(const engine::Options& options, const std::string& extension) {
	static std::atomic<uint64_t> counter {0};
	auto name =
	    "nautilus_debug_" + std::to_string(::getpid()) + "_" + std::to_string(counter.fetch_add(1)) + "." + extension;
	return (sourceDir(options) / name).string();
}

// DWARF that names a relative file leaves the debugger resolving it against
// DW_AT_comp_dir, which for a JIT module is not a directory the user controls.
// Anchoring every path to the working directory up front removes that whole
// class of "source not found" from the IDE side.
std::string makeAbsolute(const std::string& path) {
	std::error_code ec;
	auto absolute = std::filesystem::absolute(path, ec);
	if (ec) {
		return path;
	}
	return absolute.lexically_normal().string();
}

} // namespace

DebugInfoOptions debugInfoOptionsFromEngineOptions(const engine::Options& options) {
	DebugInfoOptions opts;
	opts.enable = options.getOptionOrDefault("mlir.debug.enable", false);
	opts.sourceMode = options.getOptionOrDefault<std::string>("mlir.debug.source_mode", "mlir");
	opts.sourceFile = options.getOptionOrDefault<std::string>("mlir.debug.source_file", "");
	opts.producer = options.getOptionOrDefault<std::string>("mlir.debug.producer", "Nautilus JIT");
	opts.dwarfVersion = options.getOptionOrDefault("mlir.debug.dwarf_version", 4);
	opts.registerWithDebugger = options.getOptionOrDefault("mlir.debug.register_with_debugger", true);

	if (opts.enable) {
		if (opts.sourceFile.empty()) {
			const std::string ext = (opts.sourceMode == "nautilus-ir") ? "ir" : "mlir";
			opts.sourceFile = synthesizeSourcePath(options, ext);
		}
		opts.sourceFile = makeAbsolute(opts.sourceFile);
	}
	return opts;
}

} // namespace nautilus::compiler::mlir
