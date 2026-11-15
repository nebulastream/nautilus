// Frontend work packages of M2 (docs/plans/nautilus-0.4/16-implementation-plan.md §8): cheaper paused passes (M2.2),
// the copy counters (M2.3), tracing diagnostics (M2.4), explicit control flow (M2.5) and constant data (M2.6).

#include "ExecutionTest.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/control_flow.hpp"
#include "nautilus/region.hpp"
#include "nautilus/static.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_std.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <string>
#include <vector>

#ifdef ENABLE_TRACING

namespace nautilus::engine {

namespace {

/// A backend that takes the NIR pipeline: the statistics of the tracer do not depend on it.
std::string pinnedBackend() {
	for (const auto* name : {"tbc", "asmjit", "cpp", "mlir"}) {
		for (const auto& available : testing::availableBackends()) {
			if (available == name) {
				return available;
			}
		}
	}
	return {};
}

int64_t counter(const compiler::CompilationStatistics& stats, const char* key) {
	const auto* value = stats.find(key);
	REQUIRE(value != nullptr);
	return std::get<int64_t>(*value);
}

int chainLength = 24;

val<int64_t> rerunChain(val<int64_t> x) {
	val<int64_t> r = 0;
	for (static_val<int> i = 0; i < chainLength; i++) {
		if (x > i) {
			r = r + 1;
		}
	}
	return r;
}

val<int64_t> rerunChainSwallowing(val<int64_t> x) {
	val<int64_t> r = 0;
	for (static_val<int> i = 0; i < chainLength; i++) {
		try {
			if (x > i) {
				r = r + 1;
			}
		} catch (...) {
			// the traced code swallows whatever the tracer throws to leave a finished pass
		}
	}
	return r;
}

val<int64_t> rerunRegionsAfterBranch(val<int64_t> x, val<int64_t> y) {
	val<int64_t> r = 0;
	if (y > 5) {
		r = 1000;
	}
	for (static_val<int> i = 0; i < 6; i++) {
		region([&]() {
			if (x > i) {
				r = r + 1;
			}
		});
	}
	return r;
}

struct RerunRun {
	int64_t paused = 0, unwound = 0, fallbacks = 0, iterations = 0, replayed = 0, snapshots = 0;
	int64_t result = 0;
};

template <typename F>
RerunRun runWith(bool unwind, F fn, int64_t argument) {
	auto engine = testing::makeEngine(
	    pinnedBackend(), [&](Options& options) { options.setOption("tracing.unwindPausedPasses", unwind); });
	auto function = engine.registerFunction(fn);
	RerunRun run;
	run.result = function(argument);
	auto stats = function.getStatistics();
	REQUIRE(stats != nullptr);
	run.paused = counter(*stats, "tracing.pausedOperations");
	run.unwound = counter(*stats, "tracing.unwoundPasses");
	run.fallbacks = counter(*stats, "tracing.unwindFallbacks");
	run.iterations = counter(*stats, "tracing.iterations");
	run.replayed = counter(*stats, "tracing.replayedOperations");
	run.snapshots = counter(*stats, "tracing.snapshots");
	return run;
}

} // namespace

TEST_CASE("Paused passes: the rest of a finished pass costs nothing with tracing.unwindPausedPasses (R3, R4)") {
	if (pinnedBackend().empty()) {
		SKIP("No compilation backend available");
	}
	const auto off = runWith(false, rerunChain, 10);
	const auto on = runWith(true, rerunChain, 10);
	CHECK(off.result == 10);
	CHECK(on.result == 10);
	// the same paths are explored with and without unwinding
	CHECK(on.iterations == off.iterations);
	CHECK(on.replayed == off.replayed);
	// every re-run of a chain runs the remaining branches out on the paused path; unwinding ends it at the merge
	CHECK(off.paused > 100);
	CHECK(off.unwound == 0);
	CHECK(on.paused == 0);
	CHECK(on.unwound > 0);
	CHECK(on.fallbacks == 0);
}

TEST_CASE("Paused passes: a body that catches everything falls back to the paused path") {
	if (pinnedBackend().empty()) {
		SKIP("No compilation backend available");
	}
	const auto off = runWith(false, rerunChainSwallowing, 7);
	const auto on = runWith(true, rerunChainSwallowing, 7);
	CHECK(off.result == 7);
	CHECK(on.result == 7);
	CHECK(on.iterations == off.iterations);
	// the exception is swallowed where it is thrown: the scope notices that the body returned, and does not throw again
	CHECK(on.unwound > 0);
	CHECK(on.fallbacks > 0);
	CHECK(on.paused > 0);
}

TEST_CASE("Paused passes: a region in a re-run is found without a snapshot of its call site (R2)") {
	if (pinnedBackend().empty()) {
		SKIP("No compilation backend available");
	}
	for (const bool unwind : {false, true}) {
		DYNAMIC_SECTION("unwindPausedPasses=" << unwind) {
			auto engine = testing::makeEngine(
			    pinnedBackend(), [&](Options& options) { options.setOption("tracing.unwindPausedPasses", unwind); });
			auto function = engine.registerFunction(rerunRegionsAfterBranch);
			CHECK(function(3, 0) == 3);
			CHECK(function(3, 9) == 1003);
			CHECK(function(100, 0) == 6);
			CHECK(function(-1, 9) == 1000);
		}
	}
}

} // namespace nautilus::engine

#endif // ENABLE_TRACING
