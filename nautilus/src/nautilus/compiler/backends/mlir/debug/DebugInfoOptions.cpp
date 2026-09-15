
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

// Directory the synthesized source file is written to: the temp directory,
// unless `mlir.debug.source_dir` names one.
//
// Pointing it at the working directory (`mlir.debug.source_dir=.`) is worth
// knowing about when driving the debugger from an IDE: a per-user $TMPDIR path
// -- on macOS a /var/folders/... one -- is typically outside the IDE's source
// roots, so it cannot open the file the DWARF names. The default stays in the
// temp directory so that a compile leaves nothing behind in the user's tree.
std::filesystem::path sourceDir(const engine::Options& options) {
	auto configured = options.getOptionOrDefault<std::string>("mlir.debug.source_dir", "");
	if (!configured.empty()) {
		return configured;
	}
	return tempDir();
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

	opts.enablePerf = options.getOptionOrDefault("mlir.perf.enable", false);
	opts.perfEmitDebugInfo = options.getOptionOrDefault("mlir.perf.emit_debug_info", true);
	opts.perfEmitUnwindInfo = options.getOptionOrDefault("mlir.perf.emit_unwind_info", true);
	// Interim: perf mode implies debug-info emission so jitdump's
	// JIT_CODE_DEBUG_INFO records have line tables to draw from. Follow-up
	// work decouples "emit metadata" from "clamp optimization for stepping"
	// so a perf-only compile stays at the user's chosen optimization level.
	if (opts.enablePerf) {
		opts.enable = true;
	}

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
