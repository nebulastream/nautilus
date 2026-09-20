#include "common/ProfilingTestSupport.hpp"
#include "nautilus/profiling/counters.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <string>
#include <thread>
#include <vector>

using namespace nautilus;

// Counter values are never asserted exactly. A hardware counter is affected by
// the scheduler, by other processes sharing the PMU, by frequency scaling and by
// the kernel's multiplexing correction, so an exact expectation would be a flaky
// test on real hardware. What is asserted is what must hold for the plugin to be
// working at all: counters advance, more work costs more, every measured thread
// is accounted for, and the bookkeeping around the window is consistent.

namespace {

constexpr uint16_t WORKERS = 4;

/// Runs @p body on WORKERS threads and joins. No indices: the point of the
/// unified API is that a worker says "start" and the right counters follow from
/// which thread is asking.
template <typename Body>
void onPool(Body&& body) {
	std::vector<std::thread> workers;
	workers.reserve(WORKERS);
	for (uint16_t worker = 0; worker < WORKERS; ++worker) {
		workers.emplace_back([&body]() { body(); });
	}
	for (auto& worker : workers) {
		worker.join();
	}
}

} // namespace

TEST_CASE("an unavailable perf subsystem degrades instead of throwing", "[profiling][counters]") {
	// Deliberately not skipped: this is the contract on a host *without*
	// counters, so it has to run exactly where every other test skips.
	profiling::CounterSet counters;
	REQUIRE_NOTHROW(counters.start());
	REQUIRE_NOTHROW(counters.stop());
	REQUIRE_NOTHROW(counters.stopAll());

	if (!counters.available()) {
		CHECK_FALSE(counters.unavailableReason().empty());
		CHECK(counters.result().empty());
		CHECK_FALSE(counters.running());
	}
}

TEST_CASE("a window counts the work inside it", "[profiling][counters]") {
	NAUTILUS_PROFILING_SKIP_IF_UNAVAILABLE();

	profiling::CounterSet counters {{"cycles", "instructions"}};
	if (!counters.available()) {
		SKIP("could not program cycles/instructions: " << counters.unavailableReason());
	}

	counters.start();
	CHECK(counters.running());
	const auto result = profiling::test::busyLoop(2'000'000);
	counters.stop();
	CHECK_FALSE(counters.running());

	CHECK(result > 0);
	const auto instructions = counters.result().get("instructions");
	const auto cycles = counters.result().get("cycles");
	REQUIRE(instructions.has_value());
	REQUIRE(cycles.has_value());
	// A two-million-iteration loop that survives -O3 cannot retire fewer than
	// one instruction per iteration.
	CHECK(*instructions > 2'000'000.0);
	CHECK(*cycles > 0.0);
}

TEST_CASE("a bigger window costs more than a smaller one", "[profiling][counters]") {
	NAUTILUS_PROFILING_SKIP_IF_UNAVAILABLE();

	const auto measure = [](uint64_t iterations) -> double {
		profiling::CounterSet counters {{"instructions"}};
		if (!counters.available()) {
			return -1.0;
		}
		counters.start();
		profiling::test::busyLoop(iterations);
		counters.stop();
		return counters.result().get("instructions").value_or(-1.0);
	};

	const auto small = measure(100'000);
	const auto large = measure(1'000'000);
	if (small < 0.0 || large < 0.0) {
		SKIP("could not program the instructions counter");
	}

	// Ten times the work. Asserting only "more" rather than a ratio keeps this
	// robust against the fixed overhead of opening and reading the counters.
	CHECK(large > small);
}

TEST_CASE("events the host does not know are dropped, not thrown", "[profiling][counters]") {
	NAUTILUS_PROFILING_SKIP_IF_UNAVAILABLE();

	profiling::CounterSet counters {{"instructions", "definitely-not-a-real-event"}};
	if (!counters.available()) {
		SKIP("could not program the instructions counter: " << counters.unavailableReason());
	}

	// The good event survives, the bad one is reported rather than fatal.
	CHECK(counters.events().size() == 1);
	CHECK(counters.events().front() == "instructions");
	CHECK(counters.unavailableReason().find("definitely-not-a-real-event") != std::string_view::npos);
}

TEST_CASE("reset clears results and keeps the set usable", "[profiling][counters]") {
	NAUTILUS_PROFILING_SKIP_IF_UNAVAILABLE();

	profiling::CounterSet counters {{"instructions"}};
	if (!counters.available()) {
		SKIP("could not program the instructions counter: " << counters.unavailableReason());
	}

	counters.start();
	profiling::test::busyLoop(100'000);
	counters.stop();
	REQUIRE_FALSE(counters.result().empty());

	counters.reset();
	CHECK(counters.result().empty());
	CHECK(counters.perThreadResults().empty());

	counters.start();
	profiling::test::busyLoop(100'000);
	counters.stop();
	CHECK_FALSE(counters.result().empty());
}

TEST_CASE("one thread measures repeated windows without exhausting slots", "[profiling][counters]") {
	NAUTILUS_PROFILING_SKIP_IF_UNAVAILABLE();

	// A thread keeps its slot across windows, so a long-lived worker measuring
	// window after window must not run the capacity down.
	profiling::CounterSet counters {{"instructions"}, /*maxThreads=*/2};
	if (!counters.available()) {
		SKIP("could not program the instructions counter: " << counters.unavailableReason());
	}

	for (int window = 0; window < 8; ++window) {
		profiling::CounterSet::Scope scope {counters};
		profiling::test::busyLoop(50'000);
	}
	CHECK(counters.result().get("instructions").value_or(0.0) > 0.0);
	// Still one thread, so still one per-thread result.
	CHECK(counters.perThreadResults().size() == 1);
	CHECK(counters.unavailableReason().empty());
}

TEST_CASE("the same calls measure a whole pool", "[profiling][counters]") {
	NAUTILUS_PROFILING_SKIP_IF_UNAVAILABLE();

	profiling::CounterSet counters {{"instructions"}};
	if (!counters.available()) {
		SKIP("could not program the instructions counter: " << counters.unavailableReason());
	}

	onPool([&counters]() {
		profiling::CounterSet::Scope scope {counters};
		profiling::test::busyLoop(500'000);
	});
	counters.stopAll();

	const auto total = counters.result().get("instructions");
	REQUIRE(total.has_value());
	CHECK(*total > 0.0);

	const auto perThread = counters.perThreadResults();
	REQUIRE(perThread.size() == WORKERS);
	double summed = 0.0;
	for (const auto& measurement : perThread) {
		const auto value = measurement.get("instructions");
		REQUIRE(value.has_value());
		CHECK(*value > 0.0);
		summed += *value;
	}
	// The aggregate is the sum over threads, so it must account for every
	// worker and cannot be the work of just one.
	CHECK(*total >= summed * 0.99);
	CHECK(*total > summed / WORKERS);
}

TEST_CASE("a pool wider than the slot capacity says so", "[profiling][counters]") {
	NAUTILUS_PROFILING_SKIP_IF_UNAVAILABLE();

	// Deliberately oversubscribed: the threads that find no slot must be
	// reported rather than silently dropped, since a quietly short total is
	// indistinguishable from fast code.
	profiling::CounterSet counters {{"instructions"}, /*maxThreads=*/2};
	if (!counters.available()) {
		SKIP("could not program the instructions counter: " << counters.unavailableReason());
	}
	CHECK(counters.maxThreads() == 2);

	onPool([&counters]() {
		profiling::CounterSet::Scope scope {counters};
		profiling::test::busyLoop(200'000);
	});
	counters.stopAll();

	CHECK(counters.perThreadResults().size() == 2);
	CHECK(counters.unavailableReason().find("no free counter slot") != std::string_view::npos);
}
