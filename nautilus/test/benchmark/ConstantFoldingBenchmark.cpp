
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include <catch2/catch_all.hpp>

#ifdef ENABLE_BC_BACKEND

namespace nautilus::engine::constant_folding_bench {

// Simple expression with constants that benefit from folding
val<int32_t> constantExpr(val<int32_t> x) {
	val<int32_t> a = 10;
	val<int32_t> b = 20;
	val<int32_t> c = a + b; // foldable to 30
	val<int32_t> d = c * 2; // foldable to 60
	return x + d;
}

// Fibonacci - loop-heavy, fewer folding opportunities
val<int32_t> fibFunc(val<int32_t> n) {
	val<int32_t> a = 0, b = 1, c;
	for (val<int32_t> i = 2; i <= n; i = i + 1) {
		c = a + b;
		a = b;
		b = c;
	}
	return b;
}

// Array sum with constant stride
val<int32_t> arraySum(val<int32_t*> array, val<int32_t> length) {
	val<int32_t> sum = val<int32_t>(0);
	val<int32_t> stride = 1; // foldable constant
	for (val<int32_t> i = 0; i < length; i = i + stride) {
		val<int32_t> value = array[i];
		sum = sum + value;
	}
	return sum;
}

// Expression with deeply nested constant computation
val<int32_t> deepConstExpr(val<int32_t> x) {
	val<int32_t> a = 2;
	val<int32_t> b = 3;
	val<int32_t> c = a + b;     // 5
	val<int32_t> d = c * a;     // 10
	val<int32_t> e = d + b;     // 13
	val<int32_t> f = e * c;     // 65
	val<int32_t> g = f + d;     // 75
	val<int32_t> h = g * a + b; // 153
	return x + h;
}

void runBenchmark(Catch::Benchmark::Chronometer& meter, Options& options,
                  std::function<void(Catch::Benchmark::Chronometer&, Options&)> func) {
	func(meter, options);
}

void runConstantExprBenchmark(Catch::Benchmark::Chronometer& meter, Options& options) {
	auto engine = engine::NautilusEngine(options);
	auto func = engine.registerFunction(constantExpr);
	meter.measure([&] { return func(42); });
}

void runFibBenchmark(Catch::Benchmark::Chronometer& meter, Options& options) {
	auto engine = engine::NautilusEngine(options);
	auto func = engine.registerFunction(fibFunc);
	meter.measure([&] { return func(15000); });
}

void runArraySumBenchmark(Catch::Benchmark::Chronometer& meter, Options& options) {
	auto engine = engine::NautilusEngine(options);
	auto func = engine.registerFunction(arraySum);
	auto size = 1024 * 1024;
	auto buffer = (int32_t*) malloc(size * sizeof(int32_t));
	for (int i = 0; i < size; i++) {
		buffer[i] = i % 100;
	}
	meter.measure([&] { return func(buffer, size); });
	free(buffer);
}

void runDeepConstExprBenchmark(Catch::Benchmark::Chronometer& meter, Options& options) {
	auto engine = engine::NautilusEngine(options);
	auto func = engine.registerFunction(deepConstExpr);
	meter.measure([&] { return func(42); });
}

static auto benchmarks =
    std::vector<std::tuple<std::string, std::function<void(Catch::Benchmark::Chronometer& meter, Options& options)>>> {
        {"constantExpr", runConstantExprBenchmark},
        {"fibonacci", runFibBenchmark},
        {"arraySum", runArraySumBenchmark},
        {"deepConstExpr", runDeepConstExprBenchmark},
    };

TEST_CASE("Constant Folding Benchmark - BC Backend") {
	for (auto& [name, func] : benchmarks) {
		// Benchmark with constant folding enabled
		Catch::Benchmark::Benchmark("bc_fold_on_" + name)
		    .operator=([&func](Catch::Benchmark::Chronometer meter) {
			    auto op = engine::Options();
			    op.setOption("engine.backend", std::string("bc"));
			    op.setOption("engine.constantFolding", true);
			    op.setOption("engine.traceMode", std::string("lazyTracing"));
			    func(meter, op);
		    });

		// Benchmark with constant folding disabled
		Catch::Benchmark::Benchmark("bc_fold_off_" + name)
		    .operator=([&func](Catch::Benchmark::Chronometer meter) {
			    auto op = engine::Options();
			    op.setOption("engine.backend", std::string("bc"));
			    op.setOption("engine.constantFolding", false);
			    op.setOption("engine.traceMode", std::string("lazyTracing"));
			    func(meter, op);
		    });
	}
}

} // namespace nautilus::engine::constant_folding_bench

#endif
