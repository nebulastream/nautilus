#include "catch2/catch_test_macros.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/TieredCompilation.hpp"
#include "nautilus/config.hpp"
#include <catch2/catch_all.hpp>
#include <chrono>
#include <thread>
#include <vector>

namespace nautilus::engine {

val<int32_t> tieredAddOne(val<int32_t> x) {
	return x + 1;
}

val<int64_t> tieredSum(val<int64_t> a, val<int64_t> b) {
	return a + b;
}

val<int32_t> tieredMultiply(val<int32_t> a, val<int32_t> b) {
	return a * b;
}

/// Returns a pair of backend names suitable for tiered compilation (tier0, tier1).
/// Returns empty strings if not enough backends available.
std::pair<std::string, std::string> getTieredBackends() {
#if defined(ENABLE_TRACING) && defined(ENABLE_BC_BACKEND) && defined(ENABLE_MLIR_BACKEND)
	return {"bc", "mlir"};
#elif defined(ENABLE_TRACING) && defined(ENABLE_BC_BACKEND) && defined(ENABLE_C_BACKEND)
	return {"bc", "cpp"};
#else
	return {"", ""};
#endif
}

TEST_CASE("Tiered Compilation - Tier 0 Executes Correctly") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier0Backend.empty()) {
		SKIP("Need at least two compilation backends for tiered compilation");
	}

	auto engine = NautilusEngine();
	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);
	module.registerFunction("sum", tieredSum);
	module.registerFunction("multiply", tieredMultiply);

	TieredCompilationConfig config;
	config.tier0.backend = tier0Backend;
	config.tier1.backend = tier1Backend;
	config.tier1.threshold = 10000; // high threshold so we stay at tier 0

	auto compiled = module.compileTiered(config);

	auto addOneFn = compiled.getFunction<int32_t(int32_t)>("add_one");
	auto sumFn = compiled.getFunction<int64_t(int64_t, int64_t)>("sum");
	auto mulFn = compiled.getFunction<int32_t(int32_t, int32_t)>("multiply");

	// Tier 0 should produce correct results
	REQUIRE(addOneFn(5) == 6);
	REQUIRE(addOneFn(0) == 1);
	REQUIRE(addOneFn(-1) == 0);
	REQUIRE(sumFn(3, 4) == 7);
	REQUIRE(sumFn(0, 0) == 0);
	REQUIRE(sumFn(-5, 10) == 5);
	REQUIRE(mulFn(3, 4) == 12);
	REQUIRE(mulFn(0, 100) == 0);

	// Should NOT be promoted yet
	REQUIRE_FALSE(compiled.isPromoted());
}

TEST_CASE("Tiered Compilation - Promotion After Threshold") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier0Backend.empty()) {
		SKIP("Need at least two compilation backends for tiered compilation");
	}

	constexpr int THRESHOLD = 100;

	auto engine = NautilusEngine();
	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);

	TieredCompilationConfig config;
	config.tier0.backend = tier0Backend;
	config.tier1.backend = tier1Backend;
	config.tier1.threshold = THRESHOLD;

	auto compiled = module.compileTiered(config);
	auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");

	// Call until we pass the threshold
	for (int i = 0; i <= THRESHOLD; ++i) {
		REQUIRE(fn(i) == i + 1);
	}

	// Wait for background promotion to complete
	compiled.waitForPromotion();

	REQUIRE(compiled.isPromoted());
}

TEST_CASE("Tiered Compilation - Results Correct After Promotion") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier0Backend.empty()) {
		SKIP("Need at least two compilation backends for tiered compilation");
	}

	constexpr int THRESHOLD = 50;

	auto engine = NautilusEngine();
	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);
	module.registerFunction("sum", tieredSum);

	TieredCompilationConfig config;
	config.tier0.backend = tier0Backend;
	config.tier1.backend = tier1Backend;
	config.tier1.threshold = THRESHOLD;

	auto compiled = module.compileTiered(config);
	auto addOneFn = compiled.getFunction<int32_t(int32_t)>("add_one");
	auto sumFn = compiled.getFunction<int64_t(int64_t, int64_t)>("sum");

	// Call past threshold
	for (int i = 0; i <= THRESHOLD; ++i) {
		REQUIRE(addOneFn(i) == i + 1);
	}

	// Wait for promotion
	compiled.waitForPromotion();
	REQUIRE(compiled.isPromoted());

	// Verify results are still correct after tier 1 swap
	REQUIRE(addOneFn(5) == 6);
	REQUIRE(addOneFn(0) == 1);
	REQUIRE(addOneFn(-100) == -99);
	REQUIRE(sumFn(10, 20) == 30);
	REQUIRE(sumFn(-5, 5) == 0);
}

TEST_CASE("Tiered Compilation - Concurrent Invocations During Promotion") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier0Backend.empty()) {
		SKIP("Need at least two compilation backends for tiered compilation");
	}

#if !defined(ENABLE_MLIR_BACKEND)
	// Only MLIR produces thread-safe native code
	SKIP("Need MLIR backend for thread-safe concurrent test");
#endif

	constexpr int THRESHOLD = 200;
	constexpr int NUM_THREADS = 4;
	constexpr int ITERATIONS = 2000;

	auto engine = NautilusEngine();
	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);

	TieredCompilationConfig config;
	config.tier0.backend = tier0Backend;
	config.tier1.backend = tier1Backend;
	config.tier1.threshold = THRESHOLD;

	auto compiled = module.compileTiered(config);
	auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");

	std::atomic<int> errors {0};
	std::vector<std::thread> threads;

	for (int t = 0; t < NUM_THREADS; ++t) {
		threads.emplace_back([&errors, localFn = fn]() mutable {
			for (int i = 0; i < ITERATIONS; ++i) {
				if (localFn(i) != i + 1) {
					errors.fetch_add(1, std::memory_order_relaxed);
				}
			}
		});
	}

	for (auto& th : threads) {
		th.join();
	}

	REQUIRE(errors.load() == 0);

	// After all those invocations, promotion should have happened
	compiled.waitForPromotion();
	REQUIRE(compiled.isPromoted());
}

TEST_CASE("Tiered Compilation - Custom Backend Per Tier") {
	// Test that different backend combinations work
	std::vector<std::pair<std::string, std::string>> combos;

#if defined(ENABLE_TRACING)
#if defined(ENABLE_BC_BACKEND) && defined(ENABLE_MLIR_BACKEND)
	combos.emplace_back("bc", "mlir");
#endif
#if defined(ENABLE_BC_BACKEND) && defined(ENABLE_C_BACKEND)
	combos.emplace_back("bc", "cpp");
#endif
#endif

	if (combos.empty()) {
		SKIP("Need at least two compilation backends");
	}

	for (auto& [t0, t1] : combos) {
		DYNAMIC_SECTION("tier0=" + t0 + " tier1=" + t1) {
			constexpr int THRESHOLD = 10;

			auto engine = NautilusEngine();
			auto module = engine.createModule();
			module.registerFunction("add_one", tieredAddOne);

			TieredCompilationConfig config;
			config.tier0.backend = t0;
			config.tier1.backend = t1;
			config.tier1.threshold = THRESHOLD;

			auto compiled = module.compileTiered(config);
			auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");

			// Tier 0
			for (int i = 0; i <= THRESHOLD; ++i) {
				REQUIRE(fn(i) == i + 1);
			}

			compiled.waitForPromotion();
			REQUIRE(compiled.isPromoted());

			// Tier 1
			REQUIRE(fn(42) == 43);
			REQUIRE(fn(-1) == 0);
		}
	}
}

TEST_CASE("Tiered Compilation - Very High Threshold Never Promotes") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier0Backend.empty()) {
		SKIP("Need at least two compilation backends for tiered compilation");
	}

	auto engine = NautilusEngine();
	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);

	TieredCompilationConfig config;
	config.tier0.backend = tier0Backend;
	config.tier1.backend = tier1Backend;
	config.tier1.threshold = 999999999; // very high

	auto compiled = module.compileTiered(config);
	auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");

	// Call many times but not enough to hit threshold
	for (int i = 0; i < 1000; ++i) {
		REQUIRE(fn(i) == i + 1);
	}

	REQUIRE_FALSE(compiled.isPromoted());
}

TEST_CASE("Tiered Compilation - Options Based Configuration") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier0Backend.empty()) {
		SKIP("Need at least two compilation backends for tiered compilation");
	}

	Options options;
	options.setOption("engine.tier0.backend", tier0Backend);
	options.setOption("engine.tier1.backend", tier1Backend);
	options.setOption("engine.tier1.threshold", 50);

	auto engine = NautilusEngine(options);
	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);

	auto compiled = module.compileTiered();
	auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");

	// Call past threshold
	for (int i = 0; i <= 50; ++i) {
		REQUIRE(fn(i) == i + 1);
	}

	compiled.waitForPromotion();
	REQUIRE(compiled.isPromoted());
	REQUIRE(fn(100) == 101);
}

TEST_CASE("Tiered Compilation - Multiple Functions In Module") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier0Backend.empty()) {
		SKIP("Need at least two compilation backends for tiered compilation");
	}

	constexpr int THRESHOLD = 30;

	auto engine = NautilusEngine();
	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);
	module.registerFunction("sum", tieredSum);
	module.registerFunction("multiply", tieredMultiply);

	TieredCompilationConfig config;
	config.tier0.backend = tier0Backend;
	config.tier1.backend = tier1Backend;
	config.tier1.threshold = THRESHOLD;

	auto compiled = module.compileTiered(config);
	auto addFn = compiled.getFunction<int32_t(int32_t)>("add_one");
	auto sumFn = compiled.getFunction<int64_t(int64_t, int64_t)>("sum");
	auto mulFn = compiled.getFunction<int32_t(int32_t, int32_t)>("multiply");

	// Mixed calls across all functions — invocation counter is per-module
	for (int i = 0; i <= THRESHOLD; ++i) {
		if (i % 3 == 0) {
			REQUIRE(addFn(i) == i + 1);
		} else if (i % 3 == 1) {
			REQUIRE(sumFn(i, i) == 2 * i);
		} else {
			REQUIRE(mulFn(i, 2) == i * 2);
		}
	}

	compiled.waitForPromotion();
	REQUIRE(compiled.isPromoted());

	// All functions should work after promotion
	REQUIRE(addFn(99) == 100);
	REQUIRE(sumFn(100, 200) == 300);
	REQUIRE(mulFn(7, 8) == 56);
}

} // namespace nautilus::engine
