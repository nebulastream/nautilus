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
	options.setOption("engine.tier1.backend", "bc");
	options.setOption("engine.tier2.backend", "mlir");
	options.setOption("engine.tier2.threshold", 5);

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
		options.setOption("engine.tier1.backend", "bc");
		options.setOption("engine.tier2.backend", "mlir");

		compiler::MultiTierJitCompiler jit(options);

		REQUIRE(jit.getTier1BackendName() == "bc");
		REQUIRE(jit.getTier2BackendName() == "mlir");
	}

	SECTION("Custom threshold") {
		Options options;
		options.setOption("engine.tier2.threshold", 50);

		compiler::MultiTierJitCompiler jit(options);

		REQUIRE(jit.getTier2Threshold() == 50);
	}
}

TEST_CASE("MultiTierJitCompiler Thread Safety") {
	Options options;
	options.setOption("engine.tier2.threshold", 10);

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
	options.setOption("engine.tier2.threshold", 10);

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

} // namespace nautilus::engine
