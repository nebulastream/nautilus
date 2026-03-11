#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>

/**
 * @brief Tests that functions execute correctly when compiled with debug.enabled=true.
 *
 * All test functions are defined locally to avoid ODR violations with other
 * translation units that include the common header functions.
 *
 * Verifies:
 * - The TagRecorder lifetime fix (Tag* pointers in Snapshots remain valid after
 *   TraceContext::trace() returns, so SourceLocationResolutionPhase can safely
 *   walk parent_ chains via getAddressChain()).
 * - The full debug pipeline (DebugInfoPass, DebugVariablePass) produces a
 *   working executable whose results match the non-debug path.
 */

namespace nautilus::engine {

// ---------------------------------------------------------------------------
// Local test functions (defined here to avoid ODR violations)
// ---------------------------------------------------------------------------

static val<int32_t> debugAdd(val<int32_t> x, val<int32_t> y) {
	return x + y;
}

static val<int32_t> debugMul(val<int32_t> x, val<int32_t> y) {
	return x * y;
}

static val<int32_t> debugIfElse(val<int32_t> x, val<int32_t> threshold) {
	if (x > threshold) {
		return x - threshold;
	} else {
		return threshold - x;
	}
}

static val<int64_t> debugSumLoop(val<int64_t> n) {
	val<int64_t> sum = (int64_t) 0;
	for (val<int64_t> i = (int64_t) 1; i <= n; i = i + (int64_t) 1) {
		sum = sum + i;
	}
	return sum;
}

static val<int32_t> debugLoadArray(val<int32_t*> arr, val<int32_t> idx) {
	return arr[idx];
}

static int32_t debugHelper(int32_t x) {
	return x * 2;
}

static val<int32_t> debugRuntimeCall(val<int32_t> x) {
	return invoke<>(debugHelper, x);
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static engine::NautilusEngine makeDebugEngine() {
	engine::Options opts;
	opts.setOption("engine.backend", "mlir");
	opts.setOption("debug.enabled", true);
	return engine::NautilusEngine(opts);
}

// ============================================================================
// Tests
// ============================================================================

TEST_CASE("Debug mode: arithmetic produces correct results", "[debug][execution]") {
	auto eng = makeDebugEngine();

	SECTION("addition") {
		auto f = eng.registerFunction(debugAdd);
		REQUIRE(f(1, 2) == 3);
		REQUIRE(f(0, 0) == 0);
		REQUIRE(f(-5, 5) == 0);
		REQUIRE(f(100, 200) == 300);
	}

	SECTION("multiplication") {
		auto f = eng.registerFunction(debugMul);
		REQUIRE(f(3, 4) == 12);
		REQUIRE(f(0, 42) == 0);
		REQUIRE(f(-2, 3) == -6);
	}
}

TEST_CASE("Debug mode: control flow executes correctly", "[debug][execution]") {
	auto eng = makeDebugEngine();

	SECTION("if-else branch") {
		// Compare debug and non-debug results to ensure debug mode doesn't
		// alter the compiled function's behaviour.
		engine::Options normalOpts;
		normalOpts.setOption("engine.backend", "mlir");
		auto normalEngine = engine::NautilusEngine(normalOpts);
		auto fNormal = normalEngine.registerFunction(debugIfElse);
		auto fDebug = eng.registerFunction(debugIfElse);
		for (int32_t x : {-5, 0, 3, 5, 7, 10}) {
			for (int32_t t : {-3, 0, 5, 8}) {
				REQUIRE(fDebug(x, t) == fNormal(x, t));
			}
		}
	}

	SECTION("loop: sum 1..n") {
		auto f = eng.registerFunction(debugSumLoop);
		REQUIRE(f((int64_t) 0) == 0);
		REQUIRE(f((int64_t) 1) == 1);
		REQUIRE(f((int64_t) 10) == 55);
		REQUIRE(f((int64_t) 100) == 5050);
	}
}

TEST_CASE("Debug mode: pointer/memory operations work correctly", "[debug][execution]") {
	auto eng = makeDebugEngine();

	SECTION("array load") {
		auto f = eng.registerFunction(debugLoadArray);
		int32_t arr[4] = {10, 20, 30, 40};
		REQUIRE(f(arr, 0) == 10);
		REQUIRE(f(arr, 1) == 20);
		REQUIRE(f(arr, 2) == 30);
		REQUIRE(f(arr, 3) == 40);
	}
}

TEST_CASE("Debug mode: runtime calls work correctly", "[debug][execution]") {
	auto eng = makeDebugEngine();

	SECTION("invoke helper") {
		auto f = eng.registerFunction(debugRuntimeCall);
		REQUIRE(f(0) == 0);
		REQUIRE(f(5) == 10);
		REQUIRE(f(-3) == -6);
	}
}

/// Ensure that compiling with debug.enabled=true gives the same results as
/// the default (non-debug) path, covering the TagRecorder lifetime regression.
TEST_CASE("Debug mode: results match non-debug compilation", "[debug][execution][regression]") {
	engine::Options debugOpts;
	debugOpts.setOption("engine.backend", "mlir");
	debugOpts.setOption("debug.enabled", true);

	engine::Options normalOpts;
	normalOpts.setOption("engine.backend", "mlir");

	auto debugEngine = engine::NautilusEngine(debugOpts);
	auto normalEngine = engine::NautilusEngine(normalOpts);

	auto fDebug = debugEngine.registerFunction(debugAdd);
	auto fNormal = normalEngine.registerFunction(debugAdd);

	for (int32_t x = -10; x <= 10; ++x) {
		for (int32_t y = -10; y <= 10; ++y) {
			REQUIRE(fDebug(x, y) == fNormal(x, y));
		}
	}
}

} // namespace nautilus::engine
