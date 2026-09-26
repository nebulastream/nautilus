#include "ExecutionTest.hpp"
#include "catch2/catch_test_macros.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/function.hpp"
#include <atomic>
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

// -- Synchronization kernel for the executable-lifetime regression test (issue #449) --
//
// moduleFreedTestKernel parks the calling thread *inside a compiled call* until the
// test explicitly releases it, so the test can force a concurrent executable swap
// while callers are provably mid-call rather than relying on timing.

std::atomic<int> moduleFreedTestLiveCallers {0};
std::atomic<bool> moduleFreedTestRelease {false};

void moduleFreedTestEnter() {
	moduleFreedTestLiveCallers.fetch_add(1, std::memory_order_release);
}

void moduleFreedTestSpin() {
	while (!moduleFreedTestRelease.load(std::memory_order_acquire)) {
		std::this_thread::yield();
	}
}

val<int32_t> moduleFreedTestKernel(val<int32_t> x) {
	invoke(moduleFreedTestEnter);
	invoke(moduleFreedTestSpin);
	return x;
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
#elif defined(ENABLE_TRACING) && defined(ENABLE_TBC_BACKEND)
	return "tbc";
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
	nautilus::testing::forEachBackend([](engine::NautilusEngine& engine) { moduleTests(engine); }, false);
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

// Regression test for issue #449: ModuleState::executable used to be a unique_ptr, so
// CompiledModule::setExecutable() destroyed the previous executable (and its JIT'd
// code) immediately, even while a ModuleFunction handle was still mid-call inside it.
// The version counter that operator() checks is read *before* the call, not held
// across it, so a caller that passed the check could have its code freed out from
// under it before it returned.
//
// Unlike "Module Concurrent Swap Test" above (which ping-pongs a single executable
// back and forth and therefore never actually destroys one while under contention),
// this test uses two distinct, independently compiled executables and a rendezvous
// (moduleFreedTestKernel blocks inside its call until released) to force a swap to
// happen deterministically while every reader thread is provably parked mid-call in
// the executable being replaced, instead of relying on a statistical race window.
TEST_CASE("Module Executable Freed While Callers In Flight Test") {
	auto backend = getThreadSafeBackend();
	if (backend.empty()) {
		SKIP("No thread-safe compilation backend available");
	}

	moduleFreedTestLiveCallers.store(0, std::memory_order_relaxed);
	moduleFreedTestRelease.store(false, std::memory_order_relaxed);

	engine::Options options;
	options.setOption("engine.backend", backend);
	auto engine = engine::NautilusEngine(options);

	auto compileKernelModule = [&] {
		auto module = engine.createModule();
		module.registerFunction<val<int32_t>(val<int32_t>)>("kernel", moduleFreedTestKernel);
		return module.compile();
	};

	// Two independently compiled executables for the same kernel: compiledA is the
	// one readers below resolve against and that gets swapped out; exeB is what
	// replaces it, so the swap genuinely drops compiledA's own reference.
	auto compiledA = compileKernelModule();
	auto compiledB = compileKernelModule();
	auto exeB = compiledB.releaseExecutable();

	auto fn = compiledA.getFunction<int32_t(int32_t)>("kernel");

	constexpr int NUM_READER_THREADS = 4;
	std::atomic<int> errors {0};

	// Each reader gets its own copy of the handle (own cache, shared ModuleState) and
	// resolves independently against compiledA's executable, then blocks inside the
	// call until released below.
	std::vector<std::thread> readers;
	readers.reserve(NUM_READER_THREADS);
	for (int t = 0; t < NUM_READER_THREADS; ++t) {
		readers.emplace_back([&errors, localFn = fn, t]() mutable {
			if (localFn(t) != t) {
				errors.fetch_add(1, std::memory_order_relaxed);
			}
		});
	}

	// Wait until every reader is parked inside its call into compiledA's executable.
	while (moduleFreedTestLiveCallers.load(std::memory_order_acquire) < NUM_READER_THREADS) {
		std::this_thread::yield();
	}

	// Swap in a different executable while all readers are still mid-call. Before the
	// fix, this would free compiledA's executable (and its JIT'd code) while readers
	// are parked inside it; the fix keeps it alive via the shared_ptr each reader's
	// resolved handle already captured.
	compiledA.setExecutable(std::move(exeB));

	// Let the readers resume and return back into (what was) the original executable.
	moduleFreedTestRelease.store(true, std::memory_order_release);

	for (auto& r : readers) {
		r.join();
	}

	REQUIRE(errors.load() == 0);
}

// Regression test for issue #506: the #449 fix only protects callers while the cached
// impl that captured the executable stays alive. Threads that share *one* handle share
// its cache, and the first of them to re-resolve after a swap used to overwrite the
// cached std::function in place: a data race with the other threads calling through it,
// and, because that functor held the last reference to the old executable, a free of the
// JIT'd code they were still executing.
//
// Readers here all call the same handle (no copies) and park inside compiledA's code. After
// the swap, one more caller goes through that same handle, re-resolves it against exeB, and
// parks too. Only then are all callers released and allowed to return into compiledA's code.
TEST_CASE("Module Shared Handle Re-resolve While Callers In Flight Test") {
	auto backend = getThreadSafeBackend();
	if (backend.empty()) {
		SKIP("No thread-safe compilation backend available");
	}

	moduleFreedTestLiveCallers.store(0, std::memory_order_relaxed);
	moduleFreedTestRelease.store(false, std::memory_order_relaxed);

	engine::Options options;
	options.setOption("engine.backend", backend);
	auto engine = engine::NautilusEngine(options);

	auto compileKernelModule = [&] {
		auto module = engine.createModule();
		module.registerFunction<val<int32_t>(val<int32_t>)>("kernel", moduleFreedTestKernel);
		return module.compile();
	};

	auto compiledA = compileKernelModule();
	auto compiledB = compileKernelModule();
	auto exeB = compiledB.releaseExecutable();

	const auto fn = compiledA.getFunction<int32_t(int32_t)>("kernel");

	constexpr int NUM_READER_THREADS = 4;
	std::atomic<int> errors {0};

	auto callShared = [&errors, &fn](int32_t x) {
		if (fn(x) != x) {
			errors.fetch_add(1, std::memory_order_relaxed);
		}
	};

	std::vector<std::thread> callers;
	callers.reserve(NUM_READER_THREADS + 1);
	for (int t = 0; t < NUM_READER_THREADS; ++t) {
		callers.emplace_back(callShared, t);
	}
	while (moduleFreedTestLiveCallers.load(std::memory_order_acquire) < NUM_READER_THREADS) {
		std::this_thread::yield();
	}

	// compiledA's module state now points at exeB, so the shared handle's cached impl is
	// the only thing still owning compiledA's executable.
	compiledA.setExecutable(std::move(exeB));

	// Re-resolve the shared handle while the readers are still parked in compiledA's code.
	callers.emplace_back(callShared, NUM_READER_THREADS);
	while (moduleFreedTestLiveCallers.load(std::memory_order_acquire) < NUM_READER_THREADS + 1) {
		std::this_thread::yield();
	}

	moduleFreedTestRelease.store(true, std::memory_order_release);
	for (auto& c : callers) {
		c.join();
	}

	REQUIRE(errors.load() == 0);
}

// A handle keeps every executable it has resolved against alive (see #506), but only
// once per distinct executable: swapping the same executables in and out repeatedly must
// not grow its cache. Each retained implementation holds a shared_ptr to its executable,
// so use_count() observes exactly how many implementations still reference it.
TEST_CASE("Module Function Retention Bounded Across Repeated Swaps Test") {
	auto backend = getAnyBackend();
	if (backend.empty()) {
		SKIP("No compilation backend available");
	}

	engine::Options interpOptions;
	interpOptions.setOption("engine.Compilation", false);
	auto interpEngine = engine::NautilusEngine(interpOptions);
	auto module = interpEngine.createModule();
	module.registerFunction("add_one", addOne);
	auto compiled = module.compile();

	engine::Options compileOptions;
	compileOptions.setOption("engine.backend", backend);
	auto compileEngine = engine::NautilusEngine(compileOptions);
	auto compileAddOne = [&] {
		auto donorModule = compileEngine.createModule();
		donorModule.registerFunction("add_one", addOne);
		return donorModule.compile().releaseExecutable();
	};
	auto exeA = compileAddOne();
	auto exeB = compileAddOne();
	REQUIRE(exeA.use_count() == 1);
	REQUIRE(exeB.use_count() == 1);

	constexpr int SWAP_ITERATIONS = 100;
	{
		auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");
		for (int i = 0; i < SWAP_ITERATIONS; ++i) {
			compiled.setExecutable(exeA);
			REQUIRE(fn(i) == i + 1);
			compiled.setExecutable(exeB);
			REQUIRE(fn(i) == i + 1);
			compiled.releaseExecutable();
			REQUIRE(fn(i) == i + 1);
		}

		// One retained implementation per executable, however many swaps the handle saw.
		REQUIRE(exeA.use_count() == 2);
		REQUIRE(exeB.use_count() == 2);
	}

	// Destroying the handle releases everything it retained.
	REQUIRE(exeA.use_count() == 1);
	REQUIRE(exeB.use_count() == 1);
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
	nautilus::testing::forEachBackend([](engine::NautilusEngine& engine) { moduleLoopTests(engine); }, false);
}

} // namespace nautilus::engine
