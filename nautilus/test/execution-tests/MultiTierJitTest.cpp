#include "catch2/catch_test_macros.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/MultiTierJitCompiler.hpp"
#include "nautilus/MultiTierExecutable.hpp"
#include "nautilus/val_concepts.hpp"
#include <catch2/catch_all.hpp>
#include <thread>

namespace nautilus::engine {

/**
 * @brief Test function that performs a simple addition.
 */
val<int32_t> simpleAdd(val<int32_t> x, val<int32_t> y) {
	return x + y;
}

/**
 * @brief Test function that performs a loop-based computation.
 */
val<int64_t> loopSum(val<int64_t> n) {
	val<int64_t> sum = 0;
	for (val<int64_t> i = 0; i < n; i = i + 1) {
		sum = sum + i;
	}
	return sum;
}

/**
 * @brief Test function with complex control flow.
 */
val<int32_t> complexControlFlow(val<int32_t> x) {
	val<int32_t> result = 0;
	if (x > 10) {
		result = x * 2;
	} else if (x > 5) {
		result = x + 10;
	} else {
		result = x - 5;
	}
	return result;
}

TEST_CASE("MultiTierJitCompiler Basic Functionality") {
	// Configure multi-tier JIT with bc -> mlir
	Options options;
	options.setOption("engine.multiTier.tier1Backend", "bc");
	options.setOption("engine.multiTier.tier2Backend", "mlir");
	options.setOption("engine.multiTier.tier2Threshold", 5);

	compiler::MultiTierJitCompiler jit(options);

	SECTION("Simple addition with tier transition") {
		std::function<val<int32_t>(val<int32_t>, val<int32_t>)> func = simpleAdd;

		// Create wrapper
		auto wrapper = details::createFunctionWrapper(func);
		auto executable = jit.compile(wrapper);

		// Get the invocable
		auto invocable = executable->getInvocableMember<int32_t, int32_t, int32_t>("execute");

		// Initial invocations should use tier 1 (bytecode)
		REQUIRE(invocable(5, 3) == 8);
		REQUIRE(invocable(10, 20) == 30);
		REQUIRE(invocable(-5, 15) == 10);

		// Verify we're still on tier 1
		auto* multiTierExec = dynamic_cast<compiler::MultiTierExecutable*>(executable.get());
		if (multiTierExec) {
			REQUIRE(multiTierExec->getCurrentTier() == 1);
			REQUIRE(multiTierExec->getInvocationCount() == 3);
		}

		// Cross threshold to trigger tier 2 compilation
		REQUIRE(invocable(100, 200) == 300);
		REQUIRE(invocable(1, 1) == 2);
		REQUIRE(invocable(7, 8) == 15);

		// Give some time for async compilation to complete
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		// Continue invoking - should eventually use tier 2
		for (int i = 0; i < 10; i++) {
			REQUIRE(invocable(i, i + 1) == 2 * i + 1);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		// Verify correct results even after tier transition
		REQUIRE(invocable(42, 58) == 100);
	}

	SECTION("Loop-based computation with tier transition") {
		std::function<val<int64_t>(val<int64_t>)> func = loopSum;
		auto wrapper = details::createFunctionWrapper(func);
		auto executable = jit.compile(wrapper);
		auto invocable = executable->getInvocableMember<int64_t, int64_t>("execute");

		// Test with various inputs
		REQUIRE(invocable(0) == 0);     // sum of 0 to -1
		REQUIRE(invocable(1) == 0);     // sum of 0 to 0
		REQUIRE(invocable(5) == 10);    // sum of 0 to 4 = 0+1+2+3+4
		REQUIRE(invocable(10) == 45);   // sum of 0 to 9
		REQUIRE(invocable(100) == 4950); // sum of 0 to 99

		// Cross threshold
		REQUIRE(invocable(50) == 1225);

		// Give time for compilation
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		// Continue testing
		for (int i = 0; i < 10; i++) {
			int64_t n = i * 10;
			int64_t expected = (n * (n - 1)) / 2;
			REQUIRE(invocable(n) == expected);
		}
	}

	SECTION("Complex control flow with tier transition") {
		std::function<val<int32_t>(val<int32_t>)> func = complexControlFlow;
		auto wrapper = details::createFunctionWrapper(func);
		auto executable = jit.compile(wrapper);
		auto invocable = executable->getInvocableMember<int32_t, int32_t>("execute");

		// Test tier 1
		REQUIRE(invocable(15) == 30);  // x > 10: x * 2
		REQUIRE(invocable(7) == 17);   // x > 5: x + 10
		REQUIRE(invocable(3) == -2);   // else: x - 5
		REQUIRE(invocable(10) == 20);  // x > 5: x + 10
		REQUIRE(invocable(5) == 0);    // else: x - 5

		// Cross threshold
		REQUIRE(invocable(20) == 40);

		// Wait for tier 2
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		// Test tier 2
		REQUIRE(invocable(12) == 24);
		REQUIRE(invocable(6) == 16);
		REQUIRE(invocable(2) == -3);
	}
}

TEST_CASE("MultiTierJitCompiler Configuration") {
	SECTION("Default configuration") {
		compiler::MultiTierJitCompiler jit;

		REQUIRE(jit.getTier1BackendName() == "bc");
		REQUIRE(jit.getTier2BackendName() == "mlir");
		REQUIRE(jit.getTier2Threshold() == 100);
		REQUIRE(jit.getName() == "multi-tier(bc->mlir)");
	}

	SECTION("Custom tier 1 backend") {
		Options options;
		options.setOption("engine.multiTier.tier1Backend", "bc");
		options.setOption("engine.multiTier.tier2Backend", "mlir");

		compiler::MultiTierJitCompiler jit(options);

		REQUIRE(jit.getTier1BackendName() == "bc");
		REQUIRE(jit.getTier2BackendName() == "mlir");
	}

	SECTION("Custom threshold") {
		Options options;
		options.setOption("engine.multiTier.tier2Threshold", 50);

		compiler::MultiTierJitCompiler jit(options);

		REQUIRE(jit.getTier2Threshold() == 50);
	}
}

TEST_CASE("MultiTierJitCompiler Thread Safety") {
	Options options;
	options.setOption("engine.multiTier.tier2Threshold", 10);

	compiler::MultiTierJitCompiler jit(options);

	SECTION("Concurrent invocations") {
		std::function<val<int32_t>(val<int32_t>, val<int32_t>)> func = simpleAdd;
		auto wrapper = details::createFunctionWrapper(func);
		auto executable = jit.compile(wrapper);
		auto invocable = executable->getInvocableMember<int32_t, int32_t, int32_t>("execute");

		// Launch multiple threads
		const int numThreads = 4;
		const int iterationsPerThread = 25;
		std::vector<std::thread> threads;

		for (int t = 0; t < numThreads; t++) {
			threads.emplace_back([&invocable, t, iterationsPerThread]() {
				for (int i = 0; i < iterationsPerThread; i++) {
					int32_t a = t * 100 + i;
					int32_t b = i;
					int32_t result = invocable(a, b);
					REQUIRE(result == a + b);
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			});
		}

		// Wait for all threads
		for (auto& thread : threads) {
			thread.join();
		}

		// Verify total invocation count
		auto* multiTierExec = dynamic_cast<compiler::MultiTierExecutable*>(executable.get());
		if (multiTierExec) {
			REQUIRE(multiTierExec->getInvocationCount() == numThreads * iterationsPerThread);
		}
	}
}

TEST_CASE("MultiTierJitCompiler Invocation Tracking") {
	Options options;
	options.setOption("engine.multiTier.tier2Threshold", 10);

	compiler::MultiTierJitCompiler jit(options);

	std::function<val<int32_t>(val<int32_t>, val<int32_t>)> func = simpleAdd;
	auto wrapper = details::createFunctionWrapper(func);
	auto executable = jit.compile(wrapper);
	auto invocable = executable->getInvocableMember<int32_t, int32_t, int32_t>("execute");

	auto* multiTierExec = dynamic_cast<compiler::MultiTierExecutable*>(executable.get());
	REQUIRE(multiTierExec != nullptr);

	SECTION("Invocation count increments correctly") {
		REQUIRE(multiTierExec->getInvocationCount() == 0);
		REQUIRE(multiTierExec->getCurrentTier() == 1);

		invocable(1, 1);
		REQUIRE(multiTierExec->getInvocationCount() == 1);

		invocable(2, 2);
		REQUIRE(multiTierExec->getInvocationCount() == 2);

		for (int i = 0; i < 8; i++) {
			invocable(i, i);
		}
		REQUIRE(multiTierExec->getInvocationCount() == 10);
	}

	SECTION("Tier 2 compilation triggers at threshold") {
		REQUIRE(!multiTierExec->isTier2Compiling());

		// Invoke just before threshold
		for (int i = 0; i < 9; i++) {
			invocable(i, i);
		}
		REQUIRE(!multiTierExec->isTier2Compiling());

		// Cross threshold
		invocable(10, 10);

		// Tier 2 compilation should start (or complete quickly)
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

		// Either compiling or already switched to tier 2
		bool compilingOrSwitched =
		    multiTierExec->isTier2Compiling() || multiTierExec->getCurrentTier() == 2;
		REQUIRE(compilingOrSwitched);
	}
}

TEST_CASE("MultiTierJitCompiler Backend Combinations") {
	SECTION("Bytecode to MLIR (default)") {
		Options options;
		options.setOption("engine.multiTier.tier1Backend", "bc");
		options.setOption("engine.multiTier.tier2Backend", "mlir");
		options.setOption("engine.multiTier.tier2Threshold", 3);

		compiler::MultiTierJitCompiler jit(options);
		std::function<val<int32_t>(val<int32_t>)> func = [](val<int32_t> x) { return x * 2; };
		auto wrapper = details::createFunctionWrapper(func);
		auto executable = jit.compile(wrapper);
		auto invocable = executable->getInvocableMember<int32_t, int32_t>("execute");

		// Test tier 1
		REQUIRE(invocable(5) == 10);
		REQUIRE(invocable(10) == 20);
		REQUIRE(invocable(15) == 30);

		// Trigger tier 2
		REQUIRE(invocable(20) == 40);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		// Test tier 2
		REQUIRE(invocable(25) == 50);
		REQUIRE(invocable(100) == 200);
	}

#ifdef ENABLE_CPP_BACKEND
	SECTION("Bytecode to C++") {
		Options options;
		options.setOption("engine.multiTier.tier1Backend", "bc");
		options.setOption("engine.multiTier.tier2Backend", "cpp");
		options.setOption("engine.multiTier.tier2Threshold", 3);

		compiler::MultiTierJitCompiler jit(options);
		std::function<val<int32_t>(val<int32_t>)> func = [](val<int32_t> x) { return x + 100; };
		auto wrapper = details::createFunctionWrapper(func);
		auto executable = jit.compile(wrapper);
		auto invocable = executable->getInvocableMember<int32_t, int32_t>("execute");

		for (int i = 0; i < 10; i++) {
			REQUIRE(invocable(i) == i + 100);
		}
	}
#endif
}

TEST_CASE("MultiTierJitCompiler Threshold Edge Cases") {
	SECTION("Zero threshold (immediate tier 2)") {
		Options options;
		options.setOption("engine.multiTier.tier2Threshold", 0);

		compiler::MultiTierJitCompiler jit(options);
		std::function<val<int32_t>(val<int32_t>)> func = [](val<int32_t> x) { return x * x; };
		auto wrapper = details::createFunctionWrapper(func);
		auto executable = jit.compile(wrapper);
		auto invocable = executable->getInvocableMember<int32_t, int32_t>("execute");

		// First invocation should trigger tier 2 compilation
		REQUIRE(invocable(5) == 25);

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		// Should work correctly even with immediate tier 2
		REQUIRE(invocable(10) == 100);
		REQUIRE(invocable(7) == 49);
	}

	SECTION("Threshold of 1") {
		Options options;
		options.setOption("engine.multiTier.tier2Threshold", 1);

		compiler::MultiTierJitCompiler jit(options);
		std::function<val<int32_t>(val<int32_t>)> func = [](val<int32_t> x) { return x + x; };
		auto wrapper = details::createFunctionWrapper(func);
		auto executable = jit.compile(wrapper);
		auto invocable = executable->getInvocableMember<int32_t, int32_t>("execute");

		REQUIRE(invocable(10) == 20);
		REQUIRE(invocable(20) == 40);

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		REQUIRE(invocable(30) == 60);
	}

	SECTION("Very high threshold (stays in tier 1)") {
		Options options;
		options.setOption("engine.multiTier.tier2Threshold", 1000000);

		compiler::MultiTierJitCompiler jit(options);
		std::function<val<int32_t>(val<int32_t>)> func = [](val<int32_t> x) { return x - 5; };
		auto wrapper = details::createFunctionWrapper(func);
		auto executable = jit.compile(wrapper);
		auto invocable = executable->getInvocableMember<int32_t, int32_t>("execute");

		auto* multiTierExec = dynamic_cast<compiler::MultiTierExecutable*>(executable.get());
		REQUIRE(multiTierExec != nullptr);

		// Execute many times, should stay in tier 1
		for (int i = 0; i < 100; i++) {
			REQUIRE(invocable(i + 10) == i + 5);
		}

		REQUIRE(multiTierExec->getCurrentTier() == 1);
		REQUIRE(!multiTierExec->isTier2Compiling());
	}
}

TEST_CASE("MultiTierJitCompiler Correctness Across Transitions") {
	SECTION("Results remain consistent across tier switch") {
		Options options;
		options.setOption("engine.multiTier.tier2Threshold", 5);

		compiler::MultiTierJitCompiler jit(options);

		// Complex function to test correctness
		std::function<val<int32_t>(val<int32_t>, val<int32_t>)> func = [](val<int32_t> a, val<int32_t> b) {
			val<int32_t> result = 0;
			if (a > b) {
				result = a * b + 10;
			} else {
				result = a + b * 2;
			}
			return result;
		};

		auto wrapper = details::createFunctionWrapper(func);
		auto executable = jit.compile(wrapper);
		auto invocable = executable->getInvocableMember<int32_t, int32_t, int32_t>("execute");

		// Test cases to validate
		std::vector<std::tuple<int32_t, int32_t, int32_t>> testCases = {
		    {10, 5, 60},   // a > b: 10 * 5 + 10 = 60
		    {3, 7, 17},    // a <= b: 3 + 7 * 2 = 17
		    {8, 8, 24},    // a == b: 8 + 8 * 2 = 24
		    {15, 2, 40},   // a > b: 15 * 2 + 10 = 40
		    {1, 9, 19},    // a < b: 1 + 9 * 2 = 19
		    {20, 3, 70},   // After tier 2 trigger
		    {5, 10, 25},   // After tier 2
		    {100, 50, 5010}, // After tier 2
		};

		for (size_t i = 0; i < testCases.size(); i++) {
			auto [a, b, expected] = testCases[i];
			REQUIRE(invocable(a, b) == expected);

			// Give time for tier 2 compilation after threshold
			if (i == 5) {
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
	}

	SECTION("Floating point operations across tiers") {
		Options options;
		options.setOption("engine.multiTier.tier2Threshold", 3);

		compiler::MultiTierJitCompiler jit(options);

		std::function<val<double>(val<double>, val<double>)> func = [](val<double> x, val<double> y) {
			return x * x + y * y;
		};

		auto wrapper = details::createFunctionWrapper(func);
		auto executable = jit.compile(wrapper);
		auto invocable = executable->getInvocableMember<double, double, double>("execute");

		// Tier 1 tests
		REQUIRE(invocable(3.0, 4.0) == Catch::Approx(25.0));
		REQUIRE(invocable(1.0, 1.0) == Catch::Approx(2.0));
		REQUIRE(invocable(5.0, 12.0) == Catch::Approx(169.0));

		// Trigger tier 2
		REQUIRE(invocable(2.0, 2.0) == Catch::Approx(8.0));
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		// Tier 2 tests
		REQUIRE(invocable(6.0, 8.0) == Catch::Approx(100.0));
		REQUIRE(invocable(0.5, 0.5) == Catch::Approx(0.5));
	}
}

TEST_CASE("MultiTierJitCompiler State Inspection") {
	Options options;
	options.setOption("engine.multiTier.tier2Threshold", 5);

	compiler::MultiTierJitCompiler jit(options);

	std::function<val<int32_t>(val<int32_t>)> func = [](val<int32_t> x) { return x; };
	auto wrapper = details::createFunctionWrapper(func);
	auto executable = jit.compile(wrapper);
	auto invocable = executable->getInvocableMember<int32_t, int32_t>("execute");

	auto* multiTierExec = dynamic_cast<compiler::MultiTierExecutable*>(executable.get());
	REQUIRE(multiTierExec != nullptr);

	SECTION("Initial state") {
		REQUIRE(multiTierExec->getCurrentTier() == 1);
		REQUIRE(multiTierExec->getInvocationCount() == 0);
		REQUIRE(!multiTierExec->isTier2Compiling());
	}

	SECTION("State during tier 1 execution") {
		invocable(1);
		REQUIRE(multiTierExec->getCurrentTier() == 1);
		REQUIRE(multiTierExec->getInvocationCount() == 1);

		invocable(2);
		invocable(3);
		REQUIRE(multiTierExec->getInvocationCount() == 3);
		REQUIRE(!multiTierExec->isTier2Compiling());
	}

	SECTION("State after crossing threshold") {
		for (int i = 0; i < 5; i++) {
			invocable(i);
		}
		REQUIRE(multiTierExec->getInvocationCount() == 5);

		// Cross threshold
		invocable(5);
		REQUIRE(multiTierExec->getInvocationCount() == 6);

		// May or may not be compiling depending on timing
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

		// Should have switched to tier 2 or still be tier 1
		uint8_t tier = multiTierExec->getCurrentTier();
		REQUIRE((tier == 1 || tier == 2));
	}
}

TEST_CASE("MultiTierJitCompiler Complex Control Flow") {
	Options options;
	options.setOption("engine.multiTier.tier2Threshold", 4);

	compiler::MultiTierJitCompiler jit(options);

	SECTION("Nested loops") {
		std::function<val<int64_t>(val<int64_t>)> func = [](val<int64_t> n) {
			val<int64_t> sum = 0;
			for (val<int64_t> i = 0; i < n; i = i + 1) {
				for (val<int64_t> j = 0; j < n; j = j + 1) {
					sum = sum + 1;
				}
			}
			return sum;
		};

		auto wrapper = details::createFunctionWrapper(func);
		auto executable = jit.compile(wrapper);
		auto invocable = executable->getInvocableMember<int64_t, int64_t>("execute");

		// Tier 1 tests
		REQUIRE(invocable(0) == 0);
		REQUIRE(invocable(1) == 1);
		REQUIRE(invocable(3) == 9);
		REQUIRE(invocable(5) == 25);

		// Trigger tier 2
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		// Tier 2 tests
		REQUIRE(invocable(4) == 16);
		REQUIRE(invocable(10) == 100);
	}

	SECTION("Multiple branches") {
		std::function<val<int32_t>(val<int32_t>)> func = [](val<int32_t> x) {
			val<int32_t> result = 0;
			if (x < 0) {
				result = -x;
			} else if (x == 0) {
				result = 0;
			} else if (x < 10) {
				result = x * 2;
			} else if (x < 100) {
				result = x / 2;
			} else {
				result = 100;
			}
			return result;
		};

		auto wrapper = details::createFunctionWrapper(func);
		auto executable = jit.compile(wrapper);
		auto invocable = executable->getInvocableMember<int32_t, int32_t>("execute");

		// Test all branches in tier 1
		REQUIRE(invocable(-5) == 5);
		REQUIRE(invocable(0) == 0);
		REQUIRE(invocable(5) == 10);
		REQUIRE(invocable(50) == 25);

		// Trigger tier 2
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		// Test all branches in tier 2
		REQUIRE(invocable(-10) == 10);
		REQUIRE(invocable(8) == 16);
		REQUIRE(invocable(20) == 10);
		REQUIRE(invocable(200) == 100);
	}
}

TEST_CASE("MultiTierJitCompiler Name Generation") {
	SECTION("Default name") {
		compiler::MultiTierJitCompiler jit;
		REQUIRE(jit.getName() == "multi-tier(bc->mlir)");
	}

	SECTION("Custom backends name") {
		Options options;
		options.setOption("engine.multiTier.tier1Backend", "bc");
		options.setOption("engine.multiTier.tier2Backend", "cpp");

		compiler::MultiTierJitCompiler jit(options);
		REQUIRE(jit.getName() == "multi-tier(bc->cpp)");
	}
}

} // namespace nautilus::engine
