
#include "nautilus/compiler/backends/mlir/debug/DebugInfoOptions.hpp"
#include "nautilus/compiler/backends/DebugSource.hpp"
#include <atomic>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>

namespace nautilus::compiler::mlir {

namespace {

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

} // namespace

DebugInfoOptions debugInfoOptionsFromEngineOptions(const engine::Options& options) {
	DebugInfoOptions opts;
	opts.enableDebug = options.getOptionOrDefault("debug", false);
	opts.enablePerf = options.getOptionOrDefault("perf", false);
	opts.enableSampleSymbols = options.getOptionOrDefault("perf.sample", false);

	if (opts.enableDebug && opts.enablePerf) {
		warnOnce();
	}

	if (opts.emitDebugInfo()) {
		// a profiling-only compile writes next to perf.data, see DebugSource.hpp
		opts.sourceFile = newDebugSourcePath(opts.emitPerfMetadata() && !opts.enableDebug, "ir");
	}
	return opts;
}

void writeDebugSourceFile(DebugInfoOptions& opts, const std::string& text) {
	if (!opts.sourceFile.empty()) {
		opts.sourceFile = writeDebugSource(opts.sourceFile, text);
	}
}

std::string debugSourceFallbackPath(const std::string& extension) {
	return compiler::debugSourceFallbackPath(extension);
}

} // namespace nautilus::compiler::mlir
