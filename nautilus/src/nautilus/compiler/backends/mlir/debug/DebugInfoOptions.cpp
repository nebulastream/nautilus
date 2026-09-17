
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
// unless this is a perf-only compile (see below).
//
// A perf-only compile (`enablePerf` without `enableDebug`) needs the working
// directory instead: the IR dump has to outlive the compile and be findable
// next to perf.data when `perf inject` / `perf annotate` run, minutes or
// hours later -- and $TMPDIR is routinely cleaned between record and report.
std::filesystem::path sourceDir(const DebugInfoOptions& opts) {
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

std::string synthesizeSourcePath(const DebugInfoOptions& opts, const std::string& extension) {
	return synthesizeSourcePathIn(sourceDir(opts), extension);
}

// Cell 4 (`enableDebug=true, enablePerf=true`) composes literally: the debug
// clamp wins, so a jitdump produced this way profiles -O0 code -- not a
// correctness problem (an explicit `optimizationLevel` still overrides it),
// but exactly the trap this issue is otherwise about avoiding. Warn once per
// process rather than refuse: inspecting the jitdump of a build you are also
// stepping through is a legitimate thing to want.
void warnOnce() {
	static std::atomic_flag warned = ATOMIC_FLAG_INIT;
	if (!warned.test_and_set()) {
		std::cerr << "nautilus: debug and perf are both set; the optimization level stays clamped to what debug "
		             "requests (-O0 by default), so the jitdump profiles debug-clamped code, not "
		             "production-optimized code. Set optimizationLevel explicitly to override, or drop debug for a "
		             "representative profile.\n";
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
	opts.enableDebug = options.getOptionOrDefault("debug", false);
	opts.enablePerf = options.getOptionOrDefault("perf", false);

	if (opts.enableDebug && opts.enablePerf) {
		warnOnce();
	}

	if (opts.emitDebugInfo()) {
		opts.sourceFile = makeAbsolute(synthesizeSourcePath(opts, "ir"));
	}
	return opts;
}

std::string debugSourceFallbackPath(const std::string& extension) {
	return synthesizeSourcePathIn(tempDir(), extension);
}

} // namespace nautilus::compiler::mlir
