#include "catch2/catch_test_macros.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include <catch2/catch_all.hpp>
#include <thread>
#include <vector>

namespace nautilus::engine {

// -- Loop and control-flow functions used by multi-function module tests -------

val<int32_t> moduleSumLoop(val<int32_t> n) {
	val<int32_t> agg = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		agg = agg + 10;
	}
	return agg;
}

val<int32_t> moduleFibonacci(val<int32_t> n) {
	val<int32_t> a = 0, b = 1;
	for (val<int32_t> i = 2; i <= n; i = i + 1) {
		val<int32_t> c = a + b;
		a = b;
		b = c;
	}
	return b;
}

val<int32_t> moduleCollatz(val<int32_t> n) {
	val<int32_t> steps = 0;
	while (n != 1) {
		if (n % 2 == 0) {
			n = n / 2;
		} else {
			n = 3 * n + 1;
		}
		steps = steps + 1;
	}
	return steps;
}

val<int32_t> moduleNestedIfLoop(val<int32_t> n) {
	val<int32_t> result = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		if (i % 3 == 0) {
			result = result + 1;
		} else if (i % 3 == 1) {
			result = result + 2;
		} else {
			result = result + 3;
		}
	}
	return result;
}

val<int32_t> moduleNestedLoop(val<int32_t> n) {
	val<int32_t> agg = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		for (val<int32_t> j = 0; j < n; j = j + 1) {
			agg = agg + 1;
		}
	}
	return agg;
}

val<int32_t> moduleFactorial(val<int32_t> n) {
	val<int32_t> result = 1;
	while (n > 1) {
		result = result * n;
		n = n - 1;
	}
	return result;
}

val<int32_t> addOne(val<int32_t> x) {
	return x + 1;
}

val<int64_t> sum(val<int64_t> a, val<int64_t> b) {
	return a + b;
}

val<int32_t> multiply(val<int32_t> a, val<int32_t> b) {
	return a * b;
}

/// Returns a backend name suitable for thread-safe compiled execution.
/// Prefers MLIR (native code, stateless), falls back to CPP. Returns empty if none available.
/// Requires ENABLE_TRACING since compilation depends on the tracing pipeline.
std::string getThreadSafeBackend() {
#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)
	return "mlir";
#elif defined(ENABLE_TRACING) && defined(ENABLE_C_BACKEND)
	return "cpp";
#else
	return "";
#endif
}

/// Returns any available backend name. Returns empty if none available.
/// Requires ENABLE_TRACING since compilation depends on the tracing pipeline.
std::string getAnyBackend() {
#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)
	return "mlir";
#elif defined(ENABLE_TRACING) && defined(ENABLE_C_BACKEND)
	return "cpp";
#elif defined(ENABLE_TRACING) && defined(ENABLE_BC_BACKEND)
	return "bc";
#else
	return "";
#endif
}

void moduleTests(engine::NautilusEngine& engine) {
	SECTION("twoFunctionsWithDifferentSignatures") {
		auto module = engine.createModule();
		module.registerFunction<val<int32_t>(val<int32_t>)>("add_one", addOne);
		module.registerFunction<val<int64_t>(val<int64_t>, val<int64_t>)>("sum", sum);
		auto compiled = module.compile();

		auto addOneFn = compiled.getFunction<int32_t(int32_t)>("add_one");
		auto sumFn = compiled.getFunction<int64_t(int64_t, int64_t)>("sum");

		REQUIRE(addOneFn(5) == 6);
		REQUIRE(addOneFn(0) == 1);
		REQUIRE(addOneFn(-1) == 0);
		REQUIRE(sumFn(3, 4) == 7);
		REQUIRE(sumFn(0, 0) == 0);
		REQUIRE(sumFn(-5, 10) == 5);
	}

	SECTION("functionPointerRegistration") {
		auto module = engine.createModule();
		module.registerFunction("add_one", addOne);
		module.registerFunction("multiply", multiply);
		auto compiled = module.compile();

		auto addOneFn = compiled.getFunction<int32_t(int32_t)>("add_one");
		auto mulFn = compiled.getFunction<int32_t(int32_t, int32_t)>("multiply");

		REQUIRE(addOneFn(10) == 11);
		REQUIRE(mulFn(3, 4) == 12);
		REQUIRE(mulFn(0, 100) == 0);
	}

	SECTION("lambdaRegistration") {
		auto module = engine.createModule();
		module.registerFunction<val<int32_t>(val<int32_t>)>("double_it",
		                                                    [](val<int32_t> x) -> val<int32_t> { return x * 2; });
		module.registerFunction<val<int32_t>(val<int32_t>, val<int32_t>)>(
		    "add", [](val<int32_t> a, val<int32_t> b) -> val<int32_t> { return a + b; });
		auto compiled = module.compile();

		auto doubleIt = compiled.getFunction<int32_t(int32_t)>("double_it");
		auto addFn = compiled.getFunction<int32_t(int32_t, int32_t)>("add");

		REQUIRE(doubleIt(5) == 10);
		REQUIRE(doubleIt(-3) == -6);
		REQUIRE(addFn(1, 2) == 3);
	}

	SECTION("threeFunctions") {
		auto module = engine.createModule();
		module.registerFunction("add_one", addOne);
		module.registerFunction("sum", sum);
		module.registerFunction("multiply", multiply);
		auto compiled = module.compile();

		auto addOneFn = compiled.getFunction<int32_t(int32_t)>("add_one");
		auto sumFn = compiled.getFunction<int64_t(int64_t, int64_t)>("sum");
		auto mulFn = compiled.getFunction<int32_t(int32_t, int32_t)>("multiply");

		REQUIRE(addOneFn(99) == 100);
		REQUIRE(sumFn(100, 200) == 300);
		REQUIRE(mulFn(7, 8) == 56);
	}
}

TEST_CASE("Module Single Function Test") {
	auto backend = getAnyBackend();
	if (backend.empty()) {
		SKIP("No compilation backend available");
	}
	engine::Options options;
	options.setOption("engine.backend", backend);
	auto engine = engine::NautilusEngine(options);

	auto module = engine.createModule();
	module.registerFunction("add_one", addOne);
	auto compiled = module.compile();
	REQUIRE(compiled.getExecutable() != nullptr);
	auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");
	REQUIRE(fn(5) == 6);
}

TEST_CASE("Module Interpreter Test") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	moduleTests(engine);
}

TEST_CASE("Module Compiler Test") {
	std::vector<std::string> backends = {};
#ifdef ENABLE_TRACING
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	backends.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif
#endif
	for (auto& backend : backends) {
		DYNAMIC_SECTION(backend) {
			engine::Options options;
			options.setOption("engine.backend", backend);
			auto engine = engine::NautilusEngine(options);
			moduleTests(engine);
		}
	}
}

TEST_CASE("Module Runtime Swap Test") {
	auto backend = getAnyBackend();
	if (backend.empty()) {
		SKIP("No compilation backend available");
	}

	// Start in interpreted mode, get stable function handles, then swap to compiled.
	engine::Options interpOptions;
	interpOptions.setOption("engine.Compilation", false);
	auto interpEngine = engine::NautilusEngine(interpOptions);

	auto module = interpEngine.createModule();
	module.registerFunction("add_one", addOne);
	module.registerFunction("sum", sum);
	auto compiled = module.compile();

	// Get function handles while in interpreted mode
	auto addOneFn = compiled.getFunction<int32_t(int32_t)>("add_one");
	auto sumFn = compiled.getFunction<int64_t(int64_t, int64_t)>("sum");

	// Works interpreted
	REQUIRE(compiled.getExecutable() == nullptr);
	REQUIRE(addOneFn(5) == 6);
	REQUIRE(sumFn(10, 20) == 30);

	// Now compile the same functions with a real backend
	engine::Options compileOptions;
	compileOptions.setOption("engine.backend", backend);
	auto compileEngine = engine::NautilusEngine(compileOptions);
	auto compileModule = compileEngine.createModule();
	compileModule.registerFunction("add_one", addOne);
	compileModule.registerFunction("sum", sum);
	auto donor = compileModule.compile();

	// Transfer the executable to our interpreted module
	compiled.setExecutable(donor.releaseExecutable());

	// Same handles, now backed by compiled code
	REQUIRE(compiled.getExecutable() != nullptr);
	REQUIRE(addOneFn(5) == 6);
	REQUIRE(sumFn(10, 20) == 30);

	// Swap back to interpreted
	compiled.setExecutable(nullptr);
	REQUIRE(compiled.getExecutable() == nullptr);
	REQUIRE(addOneFn(5) == 6);
	REQUIRE(sumFn(10, 20) == 30);
}

TEST_CASE("Module Function Handle Stability Test") {
	auto backend = getAnyBackend();
	if (backend.empty()) {
		SKIP("No compilation backend available");
	}

	// Verify handles are copyable and all copies see the same swap
	engine::Options interpOptions;
	interpOptions.setOption("engine.Compilation", false);
	auto interpEngine = engine::NautilusEngine(interpOptions);

	auto module = interpEngine.createModule();
	module.registerFunction("add_one", addOne);
	auto compiled = module.compile();

	auto fn1 = compiled.getFunction<int32_t(int32_t)>("add_one");
	auto fn2 = fn1; // copy the handle
	REQUIRE(fn1(5) == 6);
	REQUIRE(fn2(5) == 6);

	// Compile and swap
	engine::Options compileOptions;
	compileOptions.setOption("engine.backend", backend);
	auto compileEngine = engine::NautilusEngine(compileOptions);
	auto compileModule = compileEngine.createModule();
	compileModule.registerFunction("add_one", addOne);
	auto donor = compileModule.compile();
	compiled.setExecutable(donor.releaseExecutable());

	// Both copies automatically pick up the compiled version
	REQUIRE(fn1(5) == 6);
	REQUIRE(fn2(5) == 6);
}

TEST_CASE("Module Concurrent Swap Test") {
	auto backend = getThreadSafeBackend();
	if (backend.empty()) {
		SKIP("No thread-safe compilation backend available");
	}

	// Multiple threads call ModuleFunction handles while another thread swaps the executable.
	// Verifies no crashes, data races, or incorrect results under contention.

	engine::Options interpOptions;
	interpOptions.setOption("engine.Compilation", false);
	auto interpEngine = engine::NautilusEngine(interpOptions);

	auto module = interpEngine.createModule();
	module.registerFunction("add_one", addOne);
	module.registerFunction("sum", sum);
	auto compiled = module.compile();

	auto addOneFn = compiled.getFunction<int32_t(int32_t)>("add_one");
	auto sumFn = compiled.getFunction<int64_t(int64_t, int64_t)>("sum");

	// Compile an executable we can swap in and out.
	// Use a thread-safe backend (MLIR produces native code, BC interpreter has shared register file).
	engine::Options compileOptions;
	compileOptions.setOption("engine.backend", backend);
	auto compileEngine = engine::NautilusEngine(compileOptions);
	auto compileModule = compileEngine.createModule();
	compileModule.registerFunction("add_one", addOne);
	compileModule.registerFunction("sum", sum);
	auto donor = compileModule.compile();
	auto exe = donor.releaseExecutable();

	constexpr int NUM_READER_THREADS = 4;
	constexpr int ITERATIONS_PER_THREAD = 10000;
	constexpr int SWAP_ITERATIONS = 200;

	std::atomic<bool> stop {false};
	std::atomic<int> errors {0};

	// Reader threads: each gets its own copy of the handle (shares ModuleState).
	// Calls functions in a tight loop, verifies correctness while swaps happen.
	std::vector<std::thread> readers;
	for (int t = 0; t < NUM_READER_THREADS; ++t) {
		readers.emplace_back([&errors, &stop, localAdd = addOneFn, localSum = sumFn, t]() mutable {
			for (int i = 0; i < ITERATIONS_PER_THREAD && !stop.load(std::memory_order_relaxed); ++i) {
				if (localAdd(i) != i + 1) {
					errors.fetch_add(1, std::memory_order_relaxed);
				}
				int64_t a = t * 100 + i;
				int64_t b = i;
				if (localSum(a, b) != a + b) {
					errors.fetch_add(1, std::memory_order_relaxed);
				}
			}
		});
	}

	// Writer thread: swap executable back and forth
	std::thread writer([&]() {
		for (int i = 0; i < SWAP_ITERATIONS && !stop.load(std::memory_order_relaxed); ++i) {
			// Swap in compiled
			compiled.setExecutable(std::move(exe));
			// Swap back to interpreted, recover the executable
			exe = compiled.releaseExecutable();
		}
	});

	writer.join();
	stop.store(true, std::memory_order_relaxed);
	for (auto& r : readers) {
		r.join();
	}

	REQUIRE(errors.load() == 0);
}

TEST_CASE("Module Concurrent Readers Test") {
	auto backend = getThreadSafeBackend();
	if (backend.empty()) {
		SKIP("No thread-safe compilation backend available");
	}

	// All threads call compiled functions concurrently — no swaps.
	// Verifies that multiple resolve() calls don't race with each other.

	engine::Options options;
	options.setOption("engine.backend", backend);
	auto engine = engine::NautilusEngine(options);

	auto module = engine.createModule();
	module.registerFunction("add_one", addOne);
	module.registerFunction("multiply", multiply);
	auto compiled = module.compile();

	auto addOneFn = compiled.getFunction<int32_t(int32_t)>("add_one");
	auto mulFn = compiled.getFunction<int32_t(int32_t, int32_t)>("multiply");

	constexpr int NUM_THREADS = 8;
	constexpr int ITERATIONS = 5000;

	std::atomic<int> errors {0};
	std::vector<std::thread> threads;

	for (int t = 0; t < NUM_THREADS; ++t) {
		threads.emplace_back([&, t]() {
			// Each thread gets its own copy of the handle — all share the same ModuleState
			auto localAdd = addOneFn;
			auto localMul = mulFn;
			for (int i = 0; i < ITERATIONS; ++i) {
				if (localAdd(i) != i + 1) {
					errors.fetch_add(1, std::memory_order_relaxed);
				}
				if (localMul(t + 1, i) != (t + 1) * i) {
					errors.fetch_add(1, std::memory_order_relaxed);
				}
			}
		});
	}

	for (auto& th : threads) {
		th.join();
	}

	REQUIRE(errors.load() == 0);
}

// ---------------------------------------------------------------------------
// Module tests with multiple loop-based / complex-control-flow functions
// ---------------------------------------------------------------------------

void moduleLoopTests(engine::NautilusEngine& engine) {
	SECTION("twoLoopFunctions") {
		auto module = engine.createModule();
		module.registerFunction<val<int32_t>(val<int32_t>)>("sumLoop", moduleSumLoop);
		module.registerFunction<val<int32_t>(val<int32_t>)>("fibonacci", moduleFibonacci);
		auto compiled = module.compile();

		auto sumFn = compiled.getFunction<int32_t(int32_t)>("sumLoop");
		auto fibFn = compiled.getFunction<int32_t(int32_t)>("fibonacci");

		REQUIRE(sumFn(0) == 0);
		REQUIRE(sumFn(1) == 10);
		REQUIRE(sumFn(5) == 50);
		REQUIRE(fibFn(2) == 1);
		REQUIRE(fibFn(6) == 8);
		REQUIRE(fibFn(10) == 55);
	}

	SECTION("threeLoopFunctionsWithNestedControlFlow") {
		auto module = engine.createModule();
		module.registerFunction<val<int32_t>(val<int32_t>)>("collatz", moduleCollatz);
		module.registerFunction<val<int32_t>(val<int32_t>)>("nestedIfLoop", moduleNestedIfLoop);
		module.registerFunction<val<int32_t>(val<int32_t>)>("nestedLoop", moduleNestedLoop);
		auto compiled = module.compile();

		auto collatzFn = compiled.getFunction<int32_t(int32_t)>("collatz");
		auto nestedIfFn = compiled.getFunction<int32_t(int32_t)>("nestedIfLoop");
		auto nestedLoopFn = compiled.getFunction<int32_t(int32_t)>("nestedLoop");

		// collatz(6) -> 6,3,10,5,16,8,4,2,1 = 8 steps
		REQUIRE(collatzFn(6) == 8);
		REQUIRE(collatzFn(1) == 0);
		// nestedIfLoop(6): i=0 +1, i=1 +2, i=2 +3, i=3 +1, i=4 +2, i=5 +3 = 12
		REQUIRE(nestedIfFn(6) == 12);
		REQUIRE(nestedIfFn(0) == 0);
		// nestedLoop(4) = 4*4 = 16
		REQUIRE(nestedLoopFn(4) == 16);
		REQUIRE(nestedLoopFn(0) == 0);
	}

	SECTION("mixOfSimpleAndLoopFunctions") {
		auto module = engine.createModule();
		module.registerFunction<val<int32_t>(val<int32_t>)>("addOne", addOne);
		module.registerFunction<val<int32_t>(val<int32_t>)>("sumLoop", moduleSumLoop);
		module.registerFunction<val<int32_t>(val<int32_t>)>("factorial", moduleFactorial);
		auto compiled = module.compile();

		auto addOneFn = compiled.getFunction<int32_t(int32_t)>("addOne");
		auto sumFn = compiled.getFunction<int32_t(int32_t)>("sumLoop");
		auto factFn = compiled.getFunction<int32_t(int32_t)>("factorial");

		REQUIRE(addOneFn(5) == 6);
		REQUIRE(sumFn(3) == 30);
		REQUIRE(factFn(5) == 120);
		REQUIRE(factFn(1) == 1);
	}

	SECTION("allSixFunctionsTogether") {
		auto module = engine.createModule();
		module.registerFunction<val<int32_t>(val<int32_t>)>("addOne", addOne);
		module.registerFunction<val<int32_t>(val<int32_t>)>("sumLoop", moduleSumLoop);
		module.registerFunction<val<int32_t>(val<int32_t>)>("fibonacci", moduleFibonacci);
		module.registerFunction<val<int32_t>(val<int32_t>)>("collatz", moduleCollatz);
		module.registerFunction<val<int32_t>(val<int32_t>)>("nestedIfLoop", moduleNestedIfLoop);
		module.registerFunction<val<int32_t>(val<int32_t>)>("factorial", moduleFactorial);
		auto compiled = module.compile();

		auto addOneFn = compiled.getFunction<int32_t(int32_t)>("addOne");
		auto sumFn = compiled.getFunction<int32_t(int32_t)>("sumLoop");
		auto fibFn = compiled.getFunction<int32_t(int32_t)>("fibonacci");
		auto collatzFn = compiled.getFunction<int32_t(int32_t)>("collatz");
		auto nestedIfFn = compiled.getFunction<int32_t(int32_t)>("nestedIfLoop");
		auto factFn = compiled.getFunction<int32_t(int32_t)>("factorial");

		REQUIRE(addOneFn(99) == 100);
		REQUIRE(sumFn(10) == 100);
		REQUIRE(fibFn(10) == 55);
		REQUIRE(collatzFn(6) == 8);
		REQUIRE(nestedIfFn(9) == 18);
		REQUIRE(factFn(6) == 720);
	}
}

TEST_CASE("Module Loop Functions Interpreter Test") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	moduleLoopTests(engine);
}

TEST_CASE("Module Loop Functions Compiler Test") {
	std::vector<std::string> backends = {};
#ifdef ENABLE_TRACING
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	backends.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif
#endif
	for (auto& backend : backends) {
		DYNAMIC_SECTION(backend) {
			engine::Options options;
			options.setOption("engine.backend", backend);
			auto engine = engine::NautilusEngine(options);
			moduleLoopTests(engine);
		}
	}
}

} // namespace nautilus::engine
