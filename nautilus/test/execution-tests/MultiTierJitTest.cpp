#include "catch2/catch_test_macros.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/MultiTierJitCompiler.hpp"
#include "nautilus/MultiTierExecutable.hpp"
#include "nautilus/val_concepts.hpp"
#include <catch2/catch_all.hpp>
#include <thread>

namespace nautilus::engine {

val<int32_t> simpleAdd(val<int32_t> x, val<int32_t> y) {
	return x + y;
}

val<int64_t> loopSum(val<int64_t> n) {
	val<int64_t> sum = 0;
	for (val<int64_t> i = 0; i < n; i = i + 1) {
		sum = sum + i;
	}
	return sum;
}

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

/// Helper to get the MultiTierExecutable* from an Executable and wait for tier 2.
static compiler::MultiTierExecutable* getMultiTier(compiler::Executable* exec) {
	auto* mt = dynamic_cast<compiler::MultiTierExecutable*>(exec);
	REQUIRE(mt != nullptr);
	return mt;
}

TEST_CASE("MultiTierJitCompiler Basic Functionality") {
	Options options;
	options.setOption("engine.multiTier.tier1Backend", "bc");
	options.setOption("engine.multiTier.tier2Backend", "mlir");
	options.setOption("engine.multiTier.tier2Threshold", 5);

	compiler::MultiTierJitCompiler jit(options);

	SECTION("Simple addition with tier transition") {
		std::function<val<int32_t>(val<int32_t>, val<int32_t>)> func = simpleAdd;
		auto wrapper = details::createFunctionWrapper(func);
		auto executable = jit.compile(wrapper);
		auto invocable = executable->getInvocableMember<int32_t, int32_t, int32_t>("execute");
		auto* mt = getMultiTier(executable.get());

		// Tier 1 invocations
		REQUIRE(invocable(5, 3) == 8);
		REQUIRE(invocable(10, 20) == 30);
		REQUIRE(invocable(-5, 15) == 10);
		REQUIRE(mt->getCurrentTier() == 1);
		REQUIRE(mt->getInvocationCount() == 3);

		// Cross threshold to trigger tier 2 compilation
		REQUIRE(invocable(100, 200) == 300);
		REQUIRE(invocable(1, 1) == 2);
		REQUIRE(invocable(7, 8) == 15);

		// Wait deterministically for tier 2
		mt->waitForTier2Compilation();
		REQUIRE(mt->getCurrentTier() == 2);

		// Verify tier 2 produces correct results
		for (int i = 0; i < 10; i++) {
			REQUIRE(invocable(i, i + 1) == 2 * i + 1);
		}
		REQUIRE(invocable(42, 58) == 100);
	}

	SECTION("Loop-based computation with tier transition") {
		std::function<val<int64_t>(val<int64_t>)> func = loopSum;
		auto wrapper = details::createFunctionWrapper(func);
		auto executable = jit.compile(wrapper);
		auto invocable = executable->getInvocableMember<int64_t, int64_t>("execute");
		auto* mt = getMultiTier(executable.get());

		REQUIRE(invocable(0) == 0);
		REQUIRE(invocable(1) == 0);
		REQUIRE(invocable(5) == 10);
		REQUIRE(invocable(10) == 45);
		REQUIRE(invocable(100) == 4950);
		REQUIRE(invocable(50) == 1225);

		mt->waitForTier2Compilation();
		REQUIRE(mt->getCurrentTier() == 2);

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
		auto* mt = getMultiTier(executable.get());

		// Tier 1
		REQUIRE(invocable(15) == 30);
		REQUIRE(invocable(7) == 17);
		REQUIRE(invocable(3) == -2);
		REQUIRE(invocable(10) == 20);
		REQUIRE(invocable(5) == 0);
		REQUIRE(invocable(20) == 40);

		mt->waitForTier2Compilation();
		REQUIRE(mt->getCurrentTier() == 2);

		// Tier 2
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
				}
			});
		}

		for (auto& thread : threads) {
			thread.join();
		}

		auto* mt = getMultiTier(executable.get());
		REQUIRE(mt->getInvocationCount() == numThreads * iterationsPerThread);

		mt->waitForTier2Compilation();
		REQUIRE(mt->getCurrentTier() == 2);
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

	auto* mt = getMultiTier(executable.get());

	SECTION("Invocation count increments correctly") {
		REQUIRE(mt->getInvocationCount() == 0);
		REQUIRE(mt->getCurrentTier() == 1);

		invocable(1, 1);
		REQUIRE(mt->getInvocationCount() == 1);

		invocable(2, 2);
		REQUIRE(mt->getInvocationCount() == 2);

		for (int i = 0; i < 8; i++) {
			invocable(i, i);
		}
		REQUIRE(mt->getInvocationCount() == 10);
	}

	SECTION("Tier 2 compilation triggers at threshold") {
		REQUIRE(!mt->isTier2Compiling());

		// fetch_add returns pre-increment value, so count == threshold
		// triggers on the (threshold+1)-th call
		for (int i = 0; i < 10; i++) {
			invocable(i, i);
		}
		REQUIRE(mt->getCurrentTier() == 1);
		REQUIRE(!mt->isTier2Compiling());

		// This invocation sees count == 10 == threshold, triggering tier 2
		invocable(10, 10);

		mt->waitForTier2Compilation();
		REQUIRE(mt->getCurrentTier() == 2);
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
		auto* mt = getMultiTier(executable.get());

		// Tier 1
		REQUIRE(invocable(5) == 10);
		REQUIRE(invocable(10) == 20);
		REQUIRE(invocable(15) == 30);

		// Trigger tier 2
		REQUIRE(invocable(20) == 40);

		mt->waitForTier2Compilation();
		REQUIRE(mt->getCurrentTier() == 2);

		// Tier 2
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
		auto* mt = getMultiTier(executable.get());

		// First invocation triggers tier 2
		REQUIRE(invocable(5) == 25);

		mt->waitForTier2Compilation();
		REQUIRE(mt->getCurrentTier() == 2);

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
		auto* mt = getMultiTier(executable.get());

		REQUIRE(invocable(10) == 20);
		REQUIRE(invocable(20) == 40);

		mt->waitForTier2Compilation();
		REQUIRE(mt->getCurrentTier() == 2);

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
		auto* mt = getMultiTier(executable.get());

		for (int i = 0; i < 100; i++) {
			REQUIRE(invocable(i + 10) == i + 5);
		}

		REQUIRE(mt->getCurrentTier() == 1);
		REQUIRE(!mt->isTier2Compiling());
	}
}

TEST_CASE("MultiTierJitCompiler Correctness Across Transitions") {
	SECTION("Results remain consistent across tier switch") {
		Options options;
		options.setOption("engine.multiTier.tier2Threshold", 5);

		compiler::MultiTierJitCompiler jit(options);

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
		auto* mt = getMultiTier(executable.get());

		// Tier 1 test cases
		REQUIRE(invocable(10, 5) == 60);
		REQUIRE(invocable(3, 7) == 17);
		REQUIRE(invocable(8, 8) == 24);
		REQUIRE(invocable(15, 2) == 40);
		REQUIRE(invocable(1, 9) == 19);
		REQUIRE(invocable(20, 3) == 70);

		mt->waitForTier2Compilation();
		REQUIRE(mt->getCurrentTier() == 2);

		// Same cases on tier 2
		REQUIRE(invocable(5, 10) == 25);
		REQUIRE(invocable(100, 50) == 5010);
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
		auto* mt = getMultiTier(executable.get());

		// Tier 1
		REQUIRE(invocable(3.0, 4.0) == Catch::Approx(25.0));
		REQUIRE(invocable(1.0, 1.0) == Catch::Approx(2.0));
		REQUIRE(invocable(5.0, 12.0) == Catch::Approx(169.0));

		// Trigger tier 2
		REQUIRE(invocable(2.0, 2.0) == Catch::Approx(8.0));

		mt->waitForTier2Compilation();
		REQUIRE(mt->getCurrentTier() == 2);

		// Tier 2
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
	auto* mt = getMultiTier(executable.get());

	SECTION("Initial state") {
		REQUIRE(mt->getCurrentTier() == 1);
		REQUIRE(mt->getInvocationCount() == 0);
		REQUIRE(!mt->isTier2Compiling());
	}

	SECTION("State during tier 1 execution") {
		invocable(1);
		REQUIRE(mt->getCurrentTier() == 1);
		REQUIRE(mt->getInvocationCount() == 1);

		invocable(2);
		invocable(3);
		REQUIRE(mt->getInvocationCount() == 3);
		REQUIRE(!mt->isTier2Compiling());
	}

	SECTION("State after crossing threshold") {
		for (int i = 0; i < 6; i++) {
			invocable(i);
		}
		REQUIRE(mt->getInvocationCount() == 6);

		mt->waitForTier2Compilation();
		REQUIRE(mt->getCurrentTier() == 2);
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
		auto* mt = getMultiTier(executable.get());

		// Tier 1 (threshold=4, need 5 calls to trigger)
		REQUIRE(invocable(0) == 0);
		REQUIRE(invocable(1) == 1);
		REQUIRE(invocable(3) == 9);
		REQUIRE(invocable(5) == 25);
		REQUIRE(invocable(2) == 4);

		mt->waitForTier2Compilation();
		REQUIRE(mt->getCurrentTier() == 2);

		// Tier 2
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
		auto* mt = getMultiTier(executable.get());

		// Tier 1 (threshold=4, need 5 calls to trigger)
		REQUIRE(invocable(-5) == 5);
		REQUIRE(invocable(0) == 0);
		REQUIRE(invocable(5) == 10);
		REQUIRE(invocable(50) == 25);
		REQUIRE(invocable(200) == 100);

		mt->waitForTier2Compilation();
		REQUIRE(mt->getCurrentTier() == 2);

		// Tier 2
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

TEST_CASE("MultiTierJitCompiler Backend Name Interface") {
	SECTION("Query current backend name") {
		Options options;
		options.setOption("engine.multiTier.tier1Backend", "bc");
		options.setOption("engine.multiTier.tier2Backend", "mlir");
		options.setOption("engine.multiTier.tier2Threshold", 2);

		compiler::MultiTierJitCompiler jit(options);
		std::function<val<int32_t>(val<int32_t>)> func = [](val<int32_t> x) { return x * 3; };

		auto wrapper = details::createFunctionWrapper(func);
		auto executable = jit.compile(wrapper);
		auto invocable = executable->getInvocableMember<int32_t, int32_t>("execute");
		auto* mt = getMultiTier(executable.get());

		REQUIRE(mt->getCurrentTier() == 1);
		REQUIRE(mt->getTier1BackendName() == "bc");
		REQUIRE(mt->getTier2BackendName() == "mlir");
		REQUIRE(mt->getCurrentBackendName() == "bc");

		// Cross threshold
		invocable(1);
		invocable(2);
		invocable(3);

		mt->waitForTier2Compilation();
		REQUIRE(mt->getCurrentTier() == 2);
		REQUIRE(mt->getCurrentBackendName() == "mlir");
	}
}

} // namespace nautilus::engine
