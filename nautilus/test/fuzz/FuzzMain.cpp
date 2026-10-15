#include "Harness.hpp"
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <nautilus/config.hpp>
#include <string>
#ifdef ENABLE_LOGGING
#include <spdlog/spdlog.h>
#endif

// libFuzzer entry points. The mutation/coverage loop and main() come from the
// libFuzzer runtime (linked via -fsanitize=fuzzer); all the work lives in
// nautilus::fuzz::runOne (Harness.hpp), shared with the replay driver.

// Runs once before the first input. Everything here is per-process setup that
// would otherwise be redone (or spam the log) on every one of the many inputs
// a long session executes.
extern "C" int LLVMFuzzerInitialize(int* /*argc*/, char*** /*argv*/) {
	// With ENABLE_STACKTRACE, libnautilus installs backward-cpp's deadly-signal
	// handlers from a static initializer (exceptions/Stacktrace.cpp). libFuzzer
	// deliberately never replaces a handler that is already installed, so
	// backward-cpp's would print its trace and kill the process before
	// libFuzzer could write the crash input -- every finding (each one is an
	// abort()) would be lost. libFuzzer installs its own handlers right after
	// this hook returns, so handing the signals back to SIG_DFL here is enough;
	// under a sanitizer, libFuzzer's handler still prints the sanitizer's stack.
	for (int sig : {SIGABRT, SIGBUS, SIGFPE, SIGILL, SIGSEGV, SIGTRAP, SIGSYS, SIGXCPU, SIGXFSZ, SIGQUIT}) {
		std::signal(sig, SIG_DFL);
	}
#ifdef ENABLE_LOGGING
	// Each input compiles one kernel per config, and the engine logs every
	// compilation at info level: at fuzzing rates that drowns the libFuzzer
	// status lines and the Finding report in the log. NAUTILUS_FUZZ_LOG=1 keeps
	// the engine's normal logging, e.g. while replaying a single finding.
	if (std::getenv("NAUTILUS_FUZZ_LOG") == nullptr) {
		spdlog::set_level(spdlog::level::warn);
	}
#endif
	// Record the differential matrix in the log, so a finding (or a clean run)
	// can always be traced back to exactly which peers took part.
	const auto& configs = nautilus::fuzz::activeConfigs();
	if (configs.empty()) {
		// A typo in NAUTILUS_FUZZ_BACKENDS would otherwise "fuzz" nothing and
		// report a clean run.
		std::fprintf(stderr, "nautilus-fuzz: no differential peers selected (check NAUTILUS_FUZZ_BACKENDS)\n");
		std::exit(2);
	}
	std::fprintf(stderr, "nautilus-fuzz: %zu differential peers:", configs.size());
	for (const auto& config : configs) {
		std::fprintf(stderr, " %s/%s", config.backend.c_str(), config.label.c_str());
	}
	std::fprintf(stderr, "\n");
	return 0;
}

namespace {
// NAUTILUS_FUZZ_SELFTEST=1 turns every input into a synthetic finding, reported
// and aborted on exactly like a real one. CI uses it as a canary for the whole
// crash pipeline -- signal handling, artifact writing, the report's signature
// line -- which a harness that never finds anything would otherwise never
// exercise (see README.md "Continuous fuzzing").
const bool selfTest = [] {
	const char* env = std::getenv("NAUTILUS_FUZZ_SELFTEST");
	return env != nullptr && std::string(env) == "1";
}();
} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
	if (selfTest) {
		nautilus::fuzz::Finding finding;
		finding.backend = "selftest";
		finding.config = "default";
		finding.typeName = "none";
		finding.shape = "none";
		finding.program = "(NAUTILUS_FUZZ_SELFTEST canary, input of " + std::to_string(size) + " bytes)";
		finding.expected = "0";
		finding.got = "1";
		nautilus::fuzz::printFinding(finding);
		std::abort();
	}
	nautilus::fuzz::runOne(data, size);
	return 0;
}
