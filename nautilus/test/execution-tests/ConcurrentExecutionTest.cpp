#include "ExecutionTest.hpp"
#include "catch2/catch_test_macros.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <thread>
#include <vector>

namespace nautilus::engine {

#if defined(ENABLE_TRACING) && defined(ENABLE_BC_BACKEND)

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

TEST_CASE("BC Backend Concurrent Execution") {
	auto engine = nautilus::testing::makeEngine("bc");

	SECTION("concurrent arithmetic") {
		auto f = engine.registerFunction(squarePlusSeven);
		constexpr int numThreads = 8;
		std::vector<int64_t> results(numThreads, 0);
		std::vector<std::thread> threads;
		threads.reserve(numThreads);

		for (int t = 0; t < numThreads; t++) {
			threads.emplace_back([&f, &results, t]() {
				// Each thread calls the function many times with its own input.
				for (int iter = 0; iter < 1000; iter++) {
					auto val = static_cast<int64_t>(t + 1);
					auto r = f(val);
					results[t] = r;
				}
			});
		}

		for (auto& th : threads) {
			th.join();
		}

		for (int t = 0; t < numThreads; t++) {
			auto val = static_cast<int64_t>(t + 1);
			REQUIRE(results[t] == val * val + 7);
		}
	}

	SECTION("concurrent loop") {
		auto f = engine.registerFunction(sumUpTo);
		constexpr int numThreads = 8;
		std::vector<int64_t> results(numThreads, 0);
		std::vector<std::thread> threads;
		threads.reserve(numThreads);

		for (int t = 0; t < numThreads; t++) {
			threads.emplace_back([&f, &results, t]() {
				for (int iter = 0; iter < 100; iter++) {
					auto n = static_cast<int64_t>((t + 1) * 10);
					auto r = f(n);
					results[t] = r;
				}
			});
		}

		for (auto& th : threads) {
			th.join();
		}

		for (int t = 0; t < numThreads; t++) {
			auto n = static_cast<int64_t>((t + 1) * 10);
			auto expected = n * (n + 1) / 2;
			REQUIRE(results[t] == expected);
		}
	}
}

#endif

} // namespace nautilus::engine
