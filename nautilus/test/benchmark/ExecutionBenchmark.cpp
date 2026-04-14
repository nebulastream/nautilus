
#include "BenchmarkUtil.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include <catch2/catch_all.hpp>
#include <cstdlib>
#include <memory>

namespace nautilus::engine {

// ---------------------------------------------------------------------------
// JIT benchmark functions (val<T>). Defined locally to avoid ODR violations
// with the shared test headers that TracingBenchmark.cpp already includes.
// ---------------------------------------------------------------------------
static val<int8_t> benchAdd(val<int8_t> x) {
	val<int8_t> y = (int8_t) 2;
	return y + x;
}

static val<int32_t> benchFibonacci(val<int32_t> n) {
	val<int32_t> a = 0, b = 1, c;
	for (val<int> i = 2; i <= n; i = i + 1) {
		c = a + b;
		a = b;
		b = c;
	}
	return b;
}

static val<int32_t> benchSumLoop(val<int32_t> upperLimit) {
	val<int32_t> sum = val<int32_t>(0);
	for (val<int32_t> i = val<int32_t>(0); i < upperLimit; i = i + val<int32_t>(1)) {
		sum = sum + i;
	}
	return sum;
}

// Collatz with int64_t to avoid overflow (3n+1 exceeds int32_t for many inputs).
static val<int64_t> benchCollatz(val<int64_t> n) {
	val<int64_t> steps = (int64_t) 0;
	while (n != (int64_t) 1) {
		if (n % (int64_t) 2 == (int64_t) 0) {
			n = n / (int64_t) 2;
		} else {
			n = (int64_t) 3 * n + (int64_t) 1;
		}
		steps = steps + (int64_t) 1;
	}
	return steps;
}

static val<int32_t> benchIfThenElse(val<int32_t> value) {
	if (value == 42) {
		return val<int32_t>(1);
	}
	return val<int32_t>(0);
}

static val<int32_t> benchGcd(val<int32_t> a, val<int32_t> b) {
	while (b != 0) {
		val<int32_t> temp = b;
		b = a % b;
		a = temp;
	}
	return a;
}

static val<int32_t> benchArraySum(val<int32_t*> array, val<int32_t> length) {
	val<int32_t> sum = val<int32_t>(0);
	for (val<int32_t> i = 0; i < length; i = i + 1) {
		val<int32_t> value = array[i];
		sum = sum + value;
	}
	return sum;
}

// ---------------------------------------------------------------------------
// Native C++ baselines (plain functions, no JIT overhead)
// ---------------------------------------------------------------------------
static int8_t nativeAdd(int8_t x) {
	return (int8_t) 2 + x;
}

static int32_t nativeFibonacci(int32_t n) {
	int32_t a = 0, b = 1, c;
	for (int i = 2; i <= n; i++) {
		c = a + b;
		a = b;
		b = c;
	}
	return b;
}

static int32_t nativeSumLoop(int32_t upperLimit) {
	int32_t sum = 0;
	for (int32_t i = 0; i < upperLimit; i++) {
		sum += i;
	}
	return sum;
}

static int64_t nativeCollatz(int64_t n) {
	int64_t steps = 0;
	while (n != 1) {
		if (n % 2 == 0) {
			n = n / 2;
		} else {
			n = 3 * n + 1;
		}
		steps++;
	}
	return steps;
}

static int32_t nativeIfThenElse(int32_t value) {
	if (value == 42) {
		return 1;
	}
	return 0;
}

static int32_t nativeGcd(int32_t a, int32_t b) {
	while (b != 0) {
		int32_t temp = b;
		b = a % b;
		a = temp;
	}
	return a;
}

static int32_t nativeArraySum(int32_t* array, int32_t length) {
	int32_t sum = 0;
	for (int32_t i = 0; i < length; i++) {
		sum += array[i];
	}
	return sum;
}

TEST_CASE("Execution Benchmark", "[execution]") {

	auto backends = benchmark::getEnabledBackends();

	// Pre-allocate shared array buffer for arraySum benchmarks.
	constexpr int32_t ARRAY_ELEMS = 1024 * 1024;
	auto* arrayBuffer = (int32_t*) malloc(ARRAY_ELEMS * sizeof(int32_t));
	for (int32_t i = 0; i < ARRAY_ELEMS; i++) {
		arrayBuffer[i] = i % 100;
	}

	// Compile all functions ONCE per backend, then only measure execution.
	// CompiledFunction/CompiledModule are move-only, so we wrap in shared_ptr
	// to allow Catch2 benchmark lambdas (which must be copyable) to capture them.
	for (auto& backend : backends) {
		auto op = engine::Options();
		op.setOption("mlir.eager_compilation", true);
		op.setOption("engine.backend", backend);
		op.setOption("engine.compilationStrategy", std::string("legacy"));

		// --- add ---
		{
			auto engine = engine::NautilusEngine(op);
			auto fn = std::make_shared<decltype(engine.registerFunction(benchAdd))>(engine.registerFunction(benchAdd));
			Catch::Benchmark::Benchmark("execution/" + backend + "/add")
			    .operator=([fn](Catch::Benchmark::Chronometer meter) { meter.measure([&] { return (*fn)(42); }); });
		}

		// --- fibonacci ---
		{
			auto engine = engine::NautilusEngine(op);
			auto fn = std::make_shared<decltype(engine.registerFunction(benchFibonacci))>(
			    engine.registerFunction(benchFibonacci));
			Catch::Benchmark::Benchmark("execution/" + backend + "/fibonacci")
			    .operator=([fn](Catch::Benchmark::Chronometer meter) { meter.measure([&] { return (*fn)(15000); }); });
		}

		// --- sumLoop ---
		{
			auto engine = engine::NautilusEngine(op);
			auto fn = std::make_shared<decltype(engine.registerFunction(benchSumLoop))>(
			    engine.registerFunction(benchSumLoop));
			Catch::Benchmark::Benchmark("execution/" + backend + "/sumLoop")
			    .operator=([fn](Catch::Benchmark::Chronometer meter) { meter.measure([&] { return (*fn)(10000); }); });
		}

		// --- collatz (int64_t to avoid overflow) ---
		{
			auto engine = engine::NautilusEngine(op);
			auto fn = std::make_shared<decltype(engine.registerFunction(benchCollatz))>(
			    engine.registerFunction(benchCollatz));
			Catch::Benchmark::Benchmark("execution/" + backend + "/collatz")
			    .operator=([fn](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&] { return (*fn)((int64_t) 837799); });
			    });
		}

		// --- ifThenElse ---
		{
			auto engine = engine::NautilusEngine(op);
			auto fn = std::make_shared<decltype(engine.registerFunction(benchIfThenElse))>(
			    engine.registerFunction(benchIfThenElse));
			Catch::Benchmark::Benchmark("execution/" + backend + "/ifThenElse")
			    .operator=([fn](Catch::Benchmark::Chronometer meter) { meter.measure([&] { return (*fn)(42); }); });
		}

		// --- gcd (two-argument, uses module API) ---
		{
			auto engine = engine::NautilusEngine(op);
			auto module = engine.createModule();
			module.registerFunction<val<int32_t>(val<int32_t>, val<int32_t>)>("gcd", benchGcd);
			auto compiled = std::make_shared<CompiledModule>(module.compile());
			auto func = compiled->getFunction<int32_t(int32_t, int32_t)>("gcd");
			Catch::Benchmark::Benchmark("execution/" + backend + "/gcd")
			    .operator=([compiled, func](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&] { return func(46368, 28657); });
			    });
		}

		// --- arraySum (pointer workload, skip on bc: too slow for 1M element interpreted loop) ---
		if (backend != "bc") {
			auto engine = engine::NautilusEngine(op);
			auto module = engine.createModule();
			module.registerFunction<val<int32_t>(val<int32_t*>, val<int32_t>)>("sum", benchArraySum);
			auto compiled = std::make_shared<CompiledModule>(module.compile());
			auto func = compiled->getFunction<int32_t(int32_t*, int32_t)>("sum");
			Catch::Benchmark::Benchmark("execution/" + backend + "/arraySum")
			    .operator=([compiled, func, arrayBuffer](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&] { return func(arrayBuffer, ARRAY_ELEMS); });
			    });
		}
	}

	// Native C++ baselines (no JIT, direct function calls).
	Catch::Benchmark::Benchmark("execution/native/add").operator=([](Catch::Benchmark::Chronometer meter) {
		meter.measure([] { return nativeAdd(42); });
	});
	Catch::Benchmark::Benchmark("execution/native/fibonacci").operator=([](Catch::Benchmark::Chronometer meter) {
		meter.measure([] { return nativeFibonacci(15000); });
	});
	Catch::Benchmark::Benchmark("execution/native/sumLoop").operator=([](Catch::Benchmark::Chronometer meter) {
		meter.measure([] { return nativeSumLoop(10000); });
	});
	Catch::Benchmark::Benchmark("execution/native/collatz").operator=([](Catch::Benchmark::Chronometer meter) {
		meter.measure([] { return nativeCollatz((int64_t) 837799); });
	});
	Catch::Benchmark::Benchmark("execution/native/ifThenElse").operator=([](Catch::Benchmark::Chronometer meter) {
		meter.measure([] { return nativeIfThenElse(42); });
	});
	Catch::Benchmark::Benchmark("execution/native/gcd").operator=([](Catch::Benchmark::Chronometer meter) {
		meter.measure([] { return nativeGcd(46368, 28657); });
	});
	Catch::Benchmark::Benchmark("execution/native/arraySum")
	    .operator=([arrayBuffer](Catch::Benchmark::Chronometer meter) {
		    meter.measure([&] { return nativeArraySum(arrayBuffer, ARRAY_ELEMS); });
	    });

	free(arrayBuffer);

	// Interpreted baseline (no compilation, lightweight workloads only).
	{
		auto op = engine::Options();
		op.setOption("engine.Compilation", false);
		auto eng = engine::NautilusEngine(op);
		auto fn = std::make_shared<decltype(eng.registerFunction(benchAdd))>(eng.registerFunction(benchAdd));
		Catch::Benchmark::Benchmark("execution/interpreted/add").operator=([fn](Catch::Benchmark::Chronometer meter) {
			meter.measure([&] { return (*fn)(42); });
		});
	}
	{
		auto op = engine::Options();
		op.setOption("engine.Compilation", false);
		auto eng = engine::NautilusEngine(op);
		auto fn =
		    std::make_shared<decltype(eng.registerFunction(benchIfThenElse))>(eng.registerFunction(benchIfThenElse));
		Catch::Benchmark::Benchmark("execution/interpreted/ifThenElse")
		    .operator=([fn](Catch::Benchmark::Chronometer meter) { meter.measure([&] { return (*fn)(42); }); });
	}
}

} // namespace nautilus::engine
