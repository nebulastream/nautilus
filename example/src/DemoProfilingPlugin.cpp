// DemoProfilingPlugin.cpp — automatic profiling of a compiled module.
//
// Demonstrates the nautilus-profiling plugin with automatic function
// instrumentation via the ProfilingIRPass. No manual ProfileRegion
// placement needed — the IR pass inserts timing calls at every
// function entry and return in the module.
//
// The module contains three separate functions:
//   1. scale    — bulk array transform via a native call
//   2. hashAll  — per-element hash in a traced loop
//   3. reduce   — array reduction via a native call
//
// Each is compiled, auto-instrumented, and profiled independently.
//
// Run:
//   ./demo_profiling_plugin
//
// Output:
//   - Per-function timing report on stdout
//   - Chrome Trace JSON → open in chrome://tracing or Perfetto

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <nautilus/Engine.hpp>
#include <nautilus/function.hpp>
#include <nautilus/profiling/profiler.hpp>

using namespace nautilus;
using namespace nautilus::profiling;

// ============================================================================
// Native functions called via invoke() from the compiled functions.
// These are opaque to the tracer — the system profiler captures their stacks.
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

// ============================================================================
// Three nautilus functions — no manual profiling annotations needed.
// The ProfilingIRPass auto-instruments each at compile time.
// ============================================================================

void scale(val<int32_t*> data, val<int32_t> size) {
	invoke(nativeScale, data, size);
}

void hashAll(val<int32_t*> data, val<int32_t> size) {
	for (val<int32_t> i = 0; i < size; i = i + 1) {
		data[i] = invoke(nativeHash, data[i]);
	}
}

val<int64_t> reduce(val<int32_t*> data, val<int32_t> size) {
	return invoke(nativeReduce, data, size);
}

// ============================================================================

int main(int, char*[]) {
	engine::Options options;
	options.setOption("engine.backend", "mlir");
	auto engine = engine::NautilusEngine(options);

	std::cout << "Backend: " << engine.getNameOfBackend() << "\n\n";

	// Install the profiler on the engine so every function the engine compiles
	// gets instrumented automatically.
	auto profiler = Profiler::enableForEngine(engine);
	auto module = engine.createModule();
	module.registerFunction<void(val<int32_t*>, val<int32_t>)>("scale", scale);
	module.registerFunction<void(val<int32_t*>, val<int32_t>)>("hashAll", hashAll);
	module.registerFunction<val<int64_t>(val<int32_t*>, val<int32_t>)>("reduce", reduce);
	auto compiled = module.compile();

	auto scaleFn = compiled.getFunction<void(int32_t*, int32_t)>("scale");
	auto hashFn = compiled.getFunction<void(int32_t*, int32_t)>("hashAll");
	auto reduceFn = compiled.getFunction<int64_t(int32_t*, int32_t)>("reduce");

	constexpr int N = 1024;
	constexpr int ITERS = 10000;
	int32_t data[N];

	profiler->start();
	for (int iter = 0; iter < ITERS; ++iter) {
		for (int i = 0; i < N; ++i) {
			data[i] = (i * 7 + iter) % 100 - 30;
		}
		scaleFn(data, N);
		hashFn(data, N);
		auto result = reduceFn(data, N);
		(void) result;
	}
	profiler->stop();

	std::cout << "=================================================================\n";
	std::cout << "  MODULE PROFILE (" << ITERS << " iterations, N=" << N << ")\n";
	std::cout << "=================================================================\n";
	profiler->report();

	auto outDir = std::filesystem::temp_directory_path() / "nautilus_demo_profiling";
	std::filesystem::create_directories(outDir);

	auto chromePath = outDir / "flame_chart.json";
	profiler->exportChromeTrace(chromePath.string());

	std::cout << "\nChrome Trace: file://" << chromePath << "\n";
	std::cout << "Tip: open in chrome://tracing or https://ui.perfetto.dev\n";

	return 0;
}
