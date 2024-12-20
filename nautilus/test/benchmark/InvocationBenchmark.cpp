#include "nautilus/Engine.hpp"
#include <benchmark/benchmark.h>

static nautilus::val<int> smallFunction(nautilus::val<int> i) {
	return i * 2;
}

static void BM_smallFunctionCompiler(benchmark::State& state) {
	nautilus::engine::Options options;
	options.setOption("engine.Compilation", true);
	auto engine = nautilus::engine::NautilusEngine(options);
	auto fn = engine.registerFunction(smallFunction);

	// Perform setup here
	for (auto _ : state) {
		benchmark::DoNotOptimize(fn(32));
	}
}

static void BM_smallFunctionInterpreter(benchmark::State& state) {
	nautilus::engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = nautilus::engine::NautilusEngine(options);
	auto fn = engine.registerFunction(smallFunction);

	// Perform setup here
	for (auto _ : state) {
		benchmark::DoNotOptimize(fn(32));
	}
}

// Register the function as a benchmark
BENCHMARK(BM_smallFunctionCompiler);
BENCHMARK(BM_smallFunctionInterpreter);
// Run the benchmark
BENCHMARK_MAIN();