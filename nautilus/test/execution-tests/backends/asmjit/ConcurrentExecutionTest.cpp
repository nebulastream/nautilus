#include "ExecutionTest.hpp"
#include "catch2/catch_test_macros.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include <atomic>
#include <catch2/catch_all.hpp>
#include <thread>
#include <vector>

#if defined(ENABLE_TRACING) && defined(ENABLE_ASMJIT_BACKEND)

namespace nautilus::engine {

namespace {

val<int64_t> squarePlusSeven(val<int64_t> x) {
	return x * x + 7;
}

val<int64_t> sumUpTo(val<int64_t> n) {
	val<int64_t> result = 0;
	for (val<int64_t> i = 1; i <= n; i = i + 1) {
		result = result + i;
	}
	return result;
}

val<int32_t> concMultiply(val<int32_t> a, val<int32_t> b) {
	return a * b;
}

} // namespace

// Verifies that a single module compiled with the AsmJit backend can be invoked
// concurrently from multiple threads. AsmJit produces stable, reentrant native
// function pointers with no per-call shared mutable state, so this should be
// safe; this test guards that property.
TEST_CASE("Concurrent AsmJit Module Execution") {
	Options options;
	options.setOption("engine.backend", std::string("asmjit"));
	auto engine = NautilusEngine(options);

	// Compile one module with several functions exactly once.
	auto module = engine.createModule();
	module.registerFunction("square7", squarePlusSeven);
	module.registerFunction("sum_to", sumUpTo);
	module.registerFunction("multiply", concMultiply);
	auto compiled = module.compile();

	auto square7 = compiled.getFunction<int64_t(int64_t)>("square7");
	auto sumTo = compiled.getFunction<int64_t(int64_t)>("sum_to");
	auto mul = compiled.getFunction<int32_t(int32_t, int32_t)>("multiply");

	constexpr int NUM_THREADS = 8;
	constexpr int ITERATIONS = 5000;

	SECTION("each thread owns its handle copies") {
		std::atomic<int> errors {0};
		std::vector<std::thread> threads;
		threads.reserve(NUM_THREADS);

		for (int t = 0; t < NUM_THREADS; ++t) {
			threads.emplace_back([=, &errors]() {
				// Each thread gets its own copies of the handles (own resolve cache).
				auto localSquare = square7;
				auto localSum = sumTo;
				auto localMul = mul;
				for (int i = 0; i < ITERATIONS; ++i) {
					const int64_t x = (t + 1) * 3 + i;
					const int64_t n = (i % 50) + 1;
					const int32_t a = t + 1;
					const int32_t b = i % 100;
					if (localSquare(x) != x * x + 7) {
						errors.fetch_add(1, std::memory_order_relaxed);
					}
					if (localSum(n) != n * (n + 1) / 2) {
						errors.fetch_add(1, std::memory_order_relaxed);
					}
					if (localMul(a, b) != a * b) {
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

	SECTION("all threads share a single handle copy") {
		// Stresses the guarded first-call resolve() under contention: all threads
		// invoke the same ModuleFunction instance concurrently.
		std::atomic<int> errors {0};
		std::vector<std::thread> threads;
		threads.reserve(NUM_THREADS);

		for (int t = 0; t < NUM_THREADS; ++t) {
			threads.emplace_back([&square7, &errors, t]() {
				for (int i = 0; i < ITERATIONS; ++i) {
					const int64_t x = (t + 1) + i;
					if (square7(x) != x * x + 7) {
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
}

} // namespace nautilus::engine

#endif
