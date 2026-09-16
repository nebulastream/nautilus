
#include "nautilus/compiler/backends/mlir/debug/DebugInfoOptions.hpp"
#include <atomic>
#include <cstdlib>
#include <filesystem>
#include <iostream>
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
// unless `mlir.debug.source_dir` names one, or this is a perf-only compile
// (see below).
//
// Pointing it at the working directory (`mlir.debug.source_dir=.`) is worth
// knowing about when driving the debugger from an IDE: a per-user $TMPDIR path
// -- on macOS a /var/folders/... one -- is typically outside the IDE's source
// roots, so it cannot open the file the DWARF names. The default stays in the
// temp directory so that a compile leaves nothing behind in the user's tree.
//
// A perf-only compile (`enablePerf` without `enableDebug`) needs the opposite
// default: the IR dump has to outlive the compile and be findable next to
// perf.data when `perf inject` / `perf annotate` run, minutes or hours later
// -- and $TMPDIR is routinely cleaned between record and report. So it
// defaults to the working directory instead, matching the comment above's own
// recommendation for a long-lived consumer. An explicit `mlir.debug.source_dir`
// always wins over either default.
std::filesystem::path sourceDir(const engine::Options& options, const DebugInfoOptions& opts) {
	auto configured = options.getOptionOrDefault<std::string>("mlir.debug.source_dir", "");
	if (!configured.empty()) {
		return configured;
	}
	if (opts.enablePerf && !opts.enableDebug) {
		return std::filesystem::current_path();
	}
	return tempDir();
}

// Generate a path like `<dir>/nautilus_debug_<pid>_<counter>.<ext>` that is
// unique across parallel compilations within the same process.
std::string synthesizeSourcePathIn(const std::filesystem::path& dir, const std::string& extension) {
	static std::atomic<uint64_t> counter {0};
	auto name =
	    "nautilus_debug_" + std::to_string(::getpid()) + "_" + std::to_string(counter.fetch_add(1)) + "." + extension;
	return (dir / name).string();
}

std::string synthesizeSourcePath(const engine::Options& options, const DebugInfoOptions& opts,
                                 const std::string& extension) {
	return synthesizeSourcePathIn(sourceDir(options, opts), extension);
}

// Cell 4 (`enableDebug=true, enablePerf=true`) composes literally: the debug
// clamp wins, so a jitdump produced this way profiles -O0 code -- not a
// correctness problem (an explicit `mlir.optimizationLevel` still overrides
// it), but exactly the trap this issue is otherwise about avoiding. Warn once
// per process rather than refuse: inspecting the jitdump of a build you are
// also stepping through is a legitimate thing to want.
void warnOnce() {
	static std::atomic_flag warned = ATOMIC_FLAG_INIT;
	if (!warned.test_and_set()) {
		std::cerr << "nautilus: mlir.debug.enable and mlir.perf.enable are both set; the optimization level stays "
		             "clamped to what mlir.debug.enable requests (-O0 by default), so the jitdump profiles "
		             "debug-clamped code, not production-optimized code. Set mlir.optimizationLevel explicitly to "
		             "override, or drop mlir.debug.enable for a representative profile.\n";
	}
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
	opts.enableDebug = options.getOptionOrDefault("mlir.debug.enable", false);
	opts.sourceMode = options.getOptionOrDefault<std::string>("mlir.debug.source_mode", "mlir");
	opts.sourceFile = options.getOptionOrDefault<std::string>("mlir.debug.source_file", "");
	opts.producer = options.getOptionOrDefault<std::string>("mlir.debug.producer", "Nautilus JIT");
	opts.dwarfVersion = options.getOptionOrDefault("mlir.debug.dwarf_version", 4);
	opts.registerWithDebugger = options.getOptionOrDefault("mlir.debug.register_with_debugger", true);

	opts.enablePerf = options.getOptionOrDefault("mlir.perf.enable", false);
	opts.perfEmitDebugInfo = options.getOptionOrDefault("mlir.perf.emit_debug_info", true);
	opts.perfEmitUnwindInfo = options.getOptionOrDefault("mlir.perf.emit_unwind_info", true);
	opts.perfFramePointers = options.getOptionOrDefault("mlir.perf.frame_pointers", true);
	opts.perfRegionSymbols = options.getOptionOrDefault("mlir.perf.region_symbols", true);

	if (opts.enableDebug && opts.enablePerf) {
		warnOnce();
	}

	if (opts.emitDebugInfo()) {
		if (opts.sourceFile.empty()) {
			const std::string ext = (opts.sourceMode == "nautilus-ir") ? "ir" : "mlir";
			opts.sourceFile = synthesizeSourcePath(options, opts, ext);
		}
		opts.sourceFile = makeAbsolute(opts.sourceFile);
	}
	return opts;
}

std::string debugSourceFallbackPath(const std::string& extension) {
	return synthesizeSourcePathIn(tempDir(), extension);
}

} // namespace nautilus::compiler::mlir
