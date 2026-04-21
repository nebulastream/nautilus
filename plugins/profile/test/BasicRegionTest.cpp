#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/profile/Profiler.hpp"
#include "nautilus/profile/Recorder.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

// Function under test: a single region bracketing a cheap arithmetic op.
// Uses named begin/end so failures point at which emit went missing.
static val<int32_t> addWithRegion(val<int32_t> x, val<int32_t> y) {
	nautilus::profile::startRegion("basic_region");
	auto sum = x + y;
	nautilus::profile::endRegion("basic_region");
	return sum;
}

// Same, but expressed through ScopedRegion. Exercises the RAII path.
static val<int32_t> addWithScopedRegion(val<int32_t> x, val<int32_t> y) {
	nautilus::profile::ScopedRegion region("scoped_region");
	return x + y;
}

// A counter emitted inside a region.
static val<int32_t> addWithCounter(val<int32_t> x, val<int32_t> y) {
	nautilus::profile::startRegion("counter_region");
	auto sum = x + y;
	nautilus::profile::traceCounter("sum_counter", val<int64_t>(7));
	nautilus::profile::endRegion("counter_region");
	return sum;
}

TEST_CASE("Profile plugin: linkable", "[profile][smoke]") {
	nautilus::profile::clearRecordedEvents();
	SUCCEED("nautilus-profile plugin is built and linkable");
}

TEST_CASE("Profile plugin: startRegion/endRegion emit recorded events", "[profile]") {
	// Step 1 only wires the CPP backend end-to-end; MLIR/BC come in step 2.
	nautilus::testing::forEachBackend(
	    [](engine::NautilusEngine& engine) {
		    nautilus::profile::clearRecordedEvents();
		    auto f = engine.registerFunction(addWithRegion);
		    REQUIRE(f(2, 3) == 5);

		    auto events = nautilus::profile::takeRecordedEvents();
		    REQUIRE(events.size() == 2);
		    CHECK(events[0].kind == nautilus::profile::Event::Kind::Begin);
		    CHECK(events[0].name == "basic_region");
		    CHECK(events[1].kind == nautilus::profile::Event::Kind::End);
		    CHECK(events[1].name == "basic_region");
	    },
	    /*include_interpreter=*/true, /*tweak=*/ {}, /*include_asmjit=*/false);
}

TEST_CASE("Profile plugin: ScopedRegion emits paired begin/end", "[profile]") {
	nautilus::testing::forEachBackend(
	    [](engine::NautilusEngine& engine) {
		    nautilus::profile::clearRecordedEvents();
		    auto f = engine.registerFunction(addWithScopedRegion);
		    REQUIRE(f(10, 32) == 42);

		    auto events = nautilus::profile::takeRecordedEvents();
		    REQUIRE(events.size() == 2);
		    CHECK(events[0].kind == nautilus::profile::Event::Kind::Begin);
		    CHECK(events[0].name == "scoped_region");
		    CHECK(events[1].kind == nautilus::profile::Event::Kind::End);
		    CHECK(events[1].name == "scoped_region");
	    },
	    /*include_interpreter=*/true, /*tweak=*/ {}, /*include_asmjit=*/false);
}

TEST_CASE("Profile plugin: traceCounter records value", "[profile]") {
	nautilus::testing::forEachBackend(
	    [](engine::NautilusEngine& engine) {
		    nautilus::profile::clearRecordedEvents();
		    auto f = engine.registerFunction(addWithCounter);
		    REQUIRE(f(1, 1) == 2);

		    auto events = nautilus::profile::takeRecordedEvents();
		    REQUIRE(events.size() == 3);
		    CHECK(events[0].kind == nautilus::profile::Event::Kind::Begin);
		    CHECK(events[1].kind == nautilus::profile::Event::Kind::CounterI64);
		    CHECK(events[1].name == "sum_counter");
		    CHECK(events[1].value == 7);
		    CHECK(events[2].kind == nautilus::profile::Event::Kind::End);
	    },
	    /*include_interpreter=*/true, /*tweak=*/ {}, /*include_asmjit=*/false);
}

} // namespace nautilus::engine
