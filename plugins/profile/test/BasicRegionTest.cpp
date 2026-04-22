#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/profile/Instrument.hpp"
#include "nautilus/profile/JitSymbols.hpp"
#include "nautilus/profile/Profiler.hpp"
#include "nautilus/profile/Recorder.hpp"
#include "nautilus/profile/Sampler.hpp"
#include "nautilus/profile/TraceWriter.hpp"
#include <catch2/catch_all.hpp>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <thread>

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

TEST_CASE("Profile plugin: flushTrace writes valid Chrome trace JSON", "[profile]") {
	nautilus::profile::clearRecordedEvents();
	auto engine = nautilus::testing::makeEngine("cpp");
	auto f = engine.registerFunction(addWithRegion);
	REQUIRE(f(1, 2) == 3);

	auto path = std::filesystem::temp_directory_path() / "nautilus-profile-flush-test.json";
	REQUIRE(nautilus::profile::flushTrace(path.string()));

	std::ifstream in(path);
	REQUIRE(in.is_open());
	std::stringstream buf;
	buf << in.rdbuf();
	auto text = buf.str();

	// Structural checks — we don't ship a JSON parser, so grep.
	CHECK(text.find("\"traceEvents\":[") != std::string::npos);
	CHECK(text.find("\"ph\":\"B\"") != std::string::npos);
	CHECK(text.find("\"ph\":\"E\"") != std::string::npos);
	CHECK(text.find("\"name\":\"basic_region\"") != std::string::npos);
	CHECK(text.find("\"cat\":\"nautilus\"") != std::string::npos);
	CHECK(text.find("\"cat\":\"__metadata\"") != std::string::npos);

	std::filesystem::remove(path);

	// flushTrace must drain the buffer.
	CHECK(nautilus::profile::takeRecordedEvents().empty());
}

static val<int32_t> plainAdd(val<int32_t> x, val<int32_t> y) {
	return x + y;
}

TEST_CASE("Profile plugin: instrument() wraps a function with a ScopedRegion", "[profile]") {
	nautilus::testing::forEachBackend(
	    [](engine::NautilusEngine& engine) {
		    nautilus::profile::clearRecordedEvents();
		    auto instrumented = nautilus::profile::instrument("auto_wrap", plainAdd);
		    auto f = engine.registerFunction(std::function<val<int32_t>(val<int32_t>, val<int32_t>)>(instrumented));
		    REQUIRE(f(20, 22) == 42);

		    auto events = nautilus::profile::takeRecordedEvents();
		    REQUIRE(events.size() == 2);
		    CHECK(events[0].kind == nautilus::profile::Event::Kind::Begin);
		    CHECK(events[0].name == "auto_wrap");
		    CHECK(events[1].kind == nautilus::profile::Event::Kind::End);
		    CHECK(events[1].name == "auto_wrap");
	    },
	    /*include_interpreter=*/true, /*tweak=*/ {}, /*include_asmjit=*/false);
}

TEST_CASE("Profile plugin: openModule groups regions onto a named track", "[profile]") {
	nautilus::profile::clearRecordedEvents();
	nautilus::profile::openModule("module_A");
	{
		auto engine = nautilus::testing::makeEngine("cpp");
		auto f = engine.registerFunction(addWithRegion);
		REQUIRE(f(4, 5) == 9);
	}
	nautilus::profile::closeModule("module_A");

	auto events = nautilus::profile::takeRecordedEvents();
	REQUIRE(events.size() == 2);
	CHECK(events[0].module == "module_A");
	CHECK(events[1].module == "module_A");
	CHECK(events[0].timestamp_us <= events[1].timestamp_us);
	CHECK(events[0].tid == events[1].tid);
}

TEST_CASE("Profile plugin: JIT symbol table lookups", "[profile]") {
	nautilus::profile::clearJitSymbols();

	nautilus::profile::JitSymbol a {};
	a.addr = 0x1000;
	a.size = 0x100;
	a.name = "func_a";
	a.module = "mod1";
	nautilus::profile::registerJitCode(std::move(a));

	nautilus::profile::JitSymbol b {};
	b.addr = 0x3000;
	b.size = 0x200;
	b.name = "func_b";
	b.module = "mod1";
	nautilus::profile::registerJitCode(std::move(b));

	auto hit1 = nautilus::profile::lookupJitSymbol(0x1050);
	REQUIRE(hit1.has_value());
	CHECK(hit1->name == "func_a");

	auto hit2 = nautilus::profile::lookupJitSymbol(0x30ff);
	REQUIRE(hit2.has_value());
	CHECK(hit2->name == "func_b");

	auto miss = nautilus::profile::lookupJitSymbol(0x2000);
	CHECK(!miss.has_value());

	nautilus::profile::unregisterJitCode(0x1000);
	auto gone = nautilus::profile::lookupJitSymbol(0x1050);
	CHECK(!gone.has_value());
}

#ifdef ENABLE_MLIR_BACKEND
TEST_CASE("Profile plugin: MLIR compile populates the JIT symbol table", "[profile][mlir]") {
	nautilus::profile::ensureMLIRBackendHookInstalled();
	nautilus::profile::clearJitSymbols();

	auto engine = nautilus::testing::makeEngine("mlir");
	auto f = engine.registerFunction(plainAdd);
	REQUIRE(f(3, 4) == 7);

	// After the first call, the MLIR lookup path should have fired the
	// observer hook and registered the function's address.
	const auto* exec = f.getExecutable();
	REQUIRE(exec != nullptr);
	// We don't have the raw pointer here, so fall back to a range walk via
	// writePerfMap which reads the full registry.
	auto path = nautilus::profile::writePerfMap();
	REQUIRE_FALSE(path.empty());

	std::ifstream in(path);
	REQUIRE(in.is_open());
	std::stringstream buf;
	buf << in.rdbuf();
	auto text = buf.str();

	INFO("perf map contents:\n" << text);
	// The lookup key used by CompiledFunction is "execute"; the MLIR observer
	// appends @mlir as the module suffix when writing the perf map.
	CHECK(text.find("@mlir") != std::string::npos);
	CHECK(text.find("execute") != std::string::npos);

	std::filesystem::remove(path);
}
#endif

TEST_CASE("Profile plugin: SIGPROF sampler captures frames", "[profile][sampler]") {
	nautilus::profile::clearRecordedEvents();
	nautilus::profile::SamplerConfig cfg;
	cfg.interval = std::chrono::microseconds(500); // 2 kHz — generous for CI
	nautilus::profile::startSampler(cfg);
	nautilus::profile::registerThisThread();

	// Spin for a bit so the sampler has multiple chances to fire.
	auto stop = std::chrono::steady_clock::now() + std::chrono::milliseconds(100);
	volatile uint64_t sink = 0;
	while (std::chrono::steady_clock::now() < stop) {
		for (int i = 0; i < 10000; ++i) {
			sink += static_cast<uint64_t>(i);
		}
	}

	// unregisterThisThread drains the current slot into the recorder; the
	// samples land as Event::Kind::Sample entries. stopSampler afterwards
	// only drains still-registered threads (none in this test).
	nautilus::profile::unregisterThisThread();
	nautilus::profile::stopSampler();

	auto events = nautilus::profile::takeRecordedEvents();
	size_t samples = 0;
	for (const auto& ev : events) {
		if (ev.kind == nautilus::profile::Event::Kind::Sample) {
			++samples;
			CHECK_FALSE(ev.stack.empty());
		}
	}
	// 100ms at 2 kHz should yield ~200 samples. CI only needs >=1 to prove
	// the signal path works; under load or ASan some can be dropped.
	CHECK(samples >= 1);
}

} // namespace nautilus::engine
