#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/profiling/profiler.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>

using namespace nautilus;
using namespace nautilus::profiling;
using namespace nautilus::engine;

TEST_CASE("Profiling plugin: linkable", "[profiling][smoke]") {
	SUCCEED("nautilus-profiling plugin is built and linkable");
}

// ============================================================================
// Basic auto-instrumented pipeline.
// ============================================================================

val<int32_t> profiledNormalize(val<int32_t*> data, val<int32_t> size) {
	for (val<int32_t> i = 0; i < size; i = i + 1) {
		data[i] = data[i] * 3 - 10;
	}
	return size;
}

val<int32_t> profiledFilter(val<int32_t*> data, val<int32_t> size) {
	for (val<int32_t> i = 0; i < size; i = i + 1) {
		if (data[i] < 0) {
			data[i] = 0;
		}
	}
	return size;
}

val<int32_t> profiledAggregate(val<int32_t*> data, val<int32_t> size) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < size; i = i + 1) {
		sum = sum + data[i];
	}
	return sum;
}

TEST_CASE("Profiler: auto-instrumented multi-function pipeline", "[profiling][compiled]") {
	nautilus::testing::forEachBackend(
	    [](NautilusEngine& engine) {
		    auto module = engine.createModule();
		    module.registerFunction<val<int32_t>(val<int32_t*>, val<int32_t>)>("normalize", profiledNormalize);
		    module.registerFunction<val<int32_t>(val<int32_t*>, val<int32_t>)>("filter", profiledFilter);
		    module.registerFunction<val<int32_t>(val<int32_t*>, val<int32_t>)>("aggregate", profiledAggregate);
		    auto profiler = Profiler::enableForEngine(engine);
		    auto compiled = module.compile();

		    auto normalize = compiled.getFunction<int32_t(int32_t*, int32_t)>("normalize");
		    auto filter = compiled.getFunction<int32_t(int32_t*, int32_t)>("filter");
		    auto aggregate = compiled.getFunction<int32_t(int32_t*, int32_t)>("aggregate");

		    constexpr int N = 64;
		    int32_t data[N];

		    profiler->start();
		    for (int iter = 0; iter < 500; ++iter) {
			    for (int i = 0; i < N; ++i) {
				    data[i] = i - 20;
			    }
			    normalize(data, N);
			    filter(data, N);
			    aggregate(data, N);
		    }
		    profiler->stop();

		    auto& regions = profiler->getRegions();
		    REQUIRE(regions.size() == 3);

		    for (auto* r : regions) {
			    REQUIRE(r->callCount() == 500);
		    }

		    std::cout << "\n=== Profiling: " << engine.getNameOfBackend() << " backend ===" << std::endl;
		    profiler->report();
	    },
	    /* include_interpreter */ false);
}

// ============================================================================
// Single function auto-instrumented.
// ============================================================================

val<int32_t> computeFn(val<int32_t> x) {
	return x * x + x + 1;
}

TEST_CASE("Profiler: single function auto-instrumented", "[profiling][compiled]") {
	nautilus::testing::forEachBackend(
	    [](NautilusEngine& engine) {
		    auto module = engine.createModule();
		    module.registerFunction<val<int32_t>(val<int32_t>)>("compute", computeFn);
		    auto profiler = Profiler::enableForEngine(engine);
		    auto compiled = module.compile();

		    auto compute = compiled.getFunction<int32_t(int32_t)>("compute");

		    profiler->start();
		    for (int i = 0; i < 100; ++i) {
			    REQUIRE(compute(i) == i * i + i + 1);
		    }
		    profiler->stop();

		    REQUIRE(profiler->getRegions().size() == 1);
		    REQUIRE(profiler->getRegions()[0]->callCount() == 100);
		    REQUIRE(profiler->getRegions()[0]->name() == "compute");
	    },
	    /* include_interpreter */ false);
}

// ============================================================================
// Native calls with system profiler correlation.
// ============================================================================

static void nativeScale(int32_t* data, int32_t size) {
	for (int round = 0; round < 50; ++round) {
		for (int i = 0; i < size; ++i) {
			data[i] = data[i] * 3 - 10;
		}
	}
}

static int32_t nativeHash(int32_t value) {
	uint32_t h = static_cast<uint32_t>(value);
	for (int round = 0; round < 50; ++round) {
		h ^= h >> 16;
		h *= 0x45d9f3b;
		h ^= h >> 16;
		h *= 0x119de1f3;
	}
	return static_cast<int32_t>(h);
}

static int64_t nativeReduce(int32_t* data, int32_t size) {
	int64_t sum = 0;
	for (int round = 0; round < 50; ++round) {
		for (int i = 0; i < size; ++i) {
			sum += data[i];
		}
	}
	return sum;
}

void scaleFn(val<int32_t*> data, val<int32_t> size) {
	invoke(nativeScale, data, size);
}

void hashAllFn(val<int32_t*> data, val<int32_t> size) {
	for (val<int32_t> i = 0; i < size; i = i + 1) {
		data[i] = invoke(nativeHash, data[i]);
	}
}

val<int64_t> reduceFn(val<int32_t*> data, val<int32_t> size) {
	return invoke(nativeReduce, data, size);
}

TEST_CASE("Profiler: native calls with perf correlation", "[profiling][compiled][demo][perf]") {
	nautilus::testing::forEachBackend(
	    [](NautilusEngine& engine) {
		    auto module = engine.createModule();
		    module.registerFunction<void(val<int32_t*>, val<int32_t>)>("scale", scaleFn);
		    module.registerFunction<void(val<int32_t*>, val<int32_t>)>("hashAll", hashAllFn);
		    module.registerFunction<val<int64_t>(val<int32_t*>, val<int32_t>)>("reduce", reduceFn);
		    auto profiler = Profiler::enableForEngine(engine);
		    auto compiled = module.compile();

		    auto scale = compiled.getFunction<void(int32_t*, int32_t)>("scale");
		    auto hashAll = compiled.getFunction<void(int32_t*, int32_t)>("hashAll");
		    auto reduce = compiled.getFunction<int64_t(int32_t*, int32_t)>("reduce");

		    profiler->start();

		    constexpr int N = 1024;
		    int32_t data[N];

		    for (int iter = 0; iter < 10000; ++iter) {
			    for (int i = 0; i < N; ++i) {
				    data[i] = (i * 7 + iter) % 100 - 30;
			    }
			    scale(data, N);
			    hashAll(data, N);
			    reduce(data, N);
		    }

		    profiler->stop();

		    std::cout << "\n=== Native-call pipeline: " << engine.getNameOfBackend() << " backend ===" << std::endl;
		    profiler->report();

		    REQUIRE(profiler->getRegions().size() == 3);
		    for (auto* r : profiler->getRegions()) {
			    REQUIRE(r->callCount() == 10000);
			    REQUIRE(r->totalDurationNs() > 0);
		    }
	    },
	    /* include_interpreter */ false);
}

// ============================================================================
// Chrome Trace export.
// ============================================================================

val<int32_t> exportTestFn(val<int32_t> x) {
	return x + 1;
}

TEST_CASE("Profiler: exportChromeTrace writes valid JSON", "[profiling][compiled]") {
	nautilus::testing::forEachBackend(
	    [](NautilusEngine& engine) {
		    auto module = engine.createModule();
		    module.registerFunction<val<int32_t>(val<int32_t>)>("exportTest", exportTestFn);
		    auto profiler = Profiler::enableForEngine(engine);
		    auto compiled = module.compile();

		    auto fn = compiled.getFunction<int32_t(int32_t)>("exportTest");

		    profiler->start();
		    fn(42);
		    profiler->stop();

		    auto tmpDir = std::filesystem::temp_directory_path() / "nautilus_export_test";
		    std::filesystem::create_directories(tmpDir);
		    auto path = tmpDir / "trace.json";

		    profiler->exportChromeTrace(path.string());

		    std::ifstream f(path);
		    REQUIRE(f.is_open());
		    std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

		    REQUIRE(content.front() == '[');
		    REQUIRE(content.find("exportTest") != std::string::npos);
		    REQUIRE(content.find("\"ph\": \"X\"") != std::string::npos);
		    REQUIRE(content.find("\"cat\": \"region\"") != std::string::npos);

		    std::filesystem::remove_all(tmpDir);
	    },
	    /* include_interpreter */ false);
}

// ============================================================================
// Lifecycle tests.
// ============================================================================

static int32_t busyWork(int32_t x) {
	volatile int32_t v = x;
	for (int i = 0; i < 100; ++i) {
		v = v * 3 + 1;
	}
	return v;
}

val<int32_t> lifecycleFn(val<int32_t> x) {
	return invoke(busyWork, x);
}

TEST_CASE("Profiler: start resets, stop preserves data", "[profiling][lifecycle]") {
	nautilus::testing::forEachBackend(
	    [](NautilusEngine& engine) {
		    auto module = engine.createModule();
		    module.registerFunction<val<int32_t>(val<int32_t>)>("work", lifecycleFn);
		    auto profiler = Profiler::enableForEngine(engine);
		    auto compiled = module.compile();

		    auto fn = compiled.getFunction<int32_t(int32_t)>("work");

		    REQUIRE(profiler->getState() == Profiler::State::IDLE);

		    profiler->start();
		    REQUIRE(profiler->getState() == Profiler::State::RUNNING);

		    fn(42);
		    REQUIRE(profiler->getRegions()[0]->callCount() == 1);

		    profiler->stop();
		    REQUIRE(profiler->getState() == Profiler::State::IDLE);

		    // Data survives stop().
		    REQUIRE(profiler->getRegions()[0]->callCount() == 1);

		    // Second start() resets data.
		    profiler->start();
		    REQUIRE(profiler->getRegions()[0]->callCount() == 0);
		    profiler->stop();
	    },
	    /* include_interpreter */ false);
}

val<int32_t> pauseFn(val<int32_t> x) {
	return invoke(busyWork, x);
}

TEST_CASE("Profiler: pause preserves data, resume does not reset", "[profiling][lifecycle]") {
	nautilus::testing::forEachBackend(
	    [](NautilusEngine& engine) {
		    auto module = engine.createModule();
		    module.registerFunction<val<int32_t>(val<int32_t>)>("work", pauseFn);
		    auto profiler = Profiler::enableForEngine(engine);
		    auto compiled = module.compile();

		    auto fn = compiled.getFunction<int32_t(int32_t)>("work");

		    profiler->start();
		    fn(1);
		    fn(2);
		    REQUIRE(profiler->getRegions()[0]->callCount() == 2);

		    // Pause: data preserved.
		    profiler->pause();
		    REQUIRE(profiler->getState() == Profiler::State::PAUSED);
		    auto countAfterPause = profiler->getRegions()[0]->callCount();

		    // Calls during pause are not recorded.
		    fn(3);
		    REQUIRE(profiler->getRegions()[0]->callCount() == countAfterPause);

		    // Resume: data NOT reset, new calls accumulate.
		    profiler->start();
		    REQUIRE(profiler->getRegions()[0]->callCount() == countAfterPause);
		    fn(4);
		    REQUIRE(profiler->getRegions()[0]->callCount() == countAfterPause + 1);

		    profiler->stop();
	    },
	    /* include_interpreter */ false);
}

TEST_CASE("Profiler: region with no calls returns safe defaults", "[profiling]") {
	nautilus::testing::forEachBackend(
	    [](NautilusEngine& engine) {
		    auto module = engine.createModule();
		    module.registerFunction<val<int32_t>(val<int32_t>)>("unused", computeFn);
		    auto profiler = Profiler::enableForEngine(engine);
		    auto compiled = module.compile();

		    profiler->start();
		    // No calls.
		    profiler->stop();

		    auto* r = profiler->getRegions()[0];
		    REQUIRE(r->callCount() == 0);
		    REQUIRE(r->totalDurationNs() == 0);
		    REQUIRE(r->minDurationNs() == 0);
		    REQUIRE(r->maxDurationNs() == 0);
		    REQUIRE(r->avgDurationNs() == 0.0);
		    REQUIRE(r->counterTotal(HwCounter::CPU_CYCLES) == 0);
		    REQUIRE(r->counterAvg(HwCounter::CPU_CYCLES) == 0.0);
	    },
	    /* include_interpreter */ false);
}

// ============================================================================
// Auto-instrumentation with multiple functions (add/mul).
// ============================================================================

val<int32_t> autoAdd(val<int32_t> a, val<int32_t> b) {
	return a + b;
}

val<int32_t> autoMul(val<int32_t> a, val<int32_t> b) {
	return a * b;
}

TEST_CASE("Profiler: auto-instrumentation creates named regions", "[profiling][compiled][auto]") {
	nautilus::testing::forEachBackend(
	    [](NautilusEngine& engine) {
		    auto module = engine.createModule();
		    module.registerFunction<val<int32_t>(val<int32_t>, val<int32_t>)>("add", autoAdd);
		    module.registerFunction<val<int32_t>(val<int32_t>, val<int32_t>)>("mul", autoMul);
		    auto profiler = Profiler::enableForEngine(engine);
		    auto compiled = module.compile();

		    auto add = compiled.getFunction<int32_t(int32_t, int32_t)>("add");
		    auto mul = compiled.getFunction<int32_t(int32_t, int32_t)>("mul");

		    profiler->start();
		    for (int i = 0; i < 100; ++i) {
			    REQUIRE(add(3, 4) == 7);
			    REQUIRE(mul(3, 4) == 12);
		    }
		    profiler->stop();

		    // Verify regions were auto-created for both functions.
		    auto& regions = profiler->getRegions();
		    REQUIRE(regions.size() == 2);

		    ProfileRegion* addRegion = nullptr;
		    ProfileRegion* mulRegion = nullptr;
		    for (auto* r : regions) {
			    if (r->name() == "add") {
				    addRegion = r;
			    }
			    if (r->name() == "mul") {
				    mulRegion = r;
			    }
		    }
		    REQUIRE(addRegion != nullptr);
		    REQUIRE(mulRegion != nullptr);
		    REQUIRE(addRegion->callCount() == 100);
		    REQUIRE(mulRegion->callCount() == 100);

		    profiler->report();
	    },
	    /* include_interpreter */ false);
}

// ============================================================================
// Multiple independent functions profiled together.
// ============================================================================

static int32_t nativeWork(int32_t x) {
	volatile int32_t result = x;
	for (int i = 0; i < 100; ++i) {
		result = result * 3 + 1;
	}
	return result;
}

val<int32_t> innerFunc(val<int32_t> x) {
	return invoke(nativeWork, x);
}

val<int32_t> outerFunc(val<int32_t> x) {
	auto a = invoke(nativeWork, x);
	auto b = invoke(nativeWork, a);
	return a + b;
}

TEST_CASE("Profiler: multiple functions profiled independently", "[profiling][compiled][auto]") {
	nautilus::testing::forEachBackend(
	    [](NautilusEngine& engine) {
		    auto module = engine.createModule();
		    module.registerFunction<val<int32_t>(val<int32_t>)>("inner", innerFunc);
		    module.registerFunction<val<int32_t>(val<int32_t>)>("outer", outerFunc);
		    auto profiler = Profiler::enableForEngine(engine);
		    auto compiled = module.compile();

		    auto inner = compiled.getFunction<int32_t(int32_t)>("inner");
		    auto outer = compiled.getFunction<int32_t(int32_t)>("outer");

		    profiler->start();
		    for (int i = 0; i < 200; ++i) {
			    inner(i);
			    outer(i);
		    }
		    profiler->stop();

		    ProfileRegion* innerRegion = nullptr;
		    ProfileRegion* outerRegion = nullptr;
		    for (auto* r : profiler->getRegions()) {
			    if (r->name() == "inner") {
				    innerRegion = r;
			    }
			    if (r->name() == "outer") {
				    outerRegion = r;
			    }
		    }
		    REQUIRE(innerRegion != nullptr);
		    REQUIRE(outerRegion != nullptr);
		    REQUIRE(innerRegion->callCount() == 200);
		    REQUIRE(outerRegion->callCount() == 200);

		    profiler->report();
	    },
	    /* include_interpreter */ false);
}
