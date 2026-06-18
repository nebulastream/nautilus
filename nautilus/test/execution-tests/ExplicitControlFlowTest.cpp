#include "ExecutionTest.hpp"
#include "ExplicitControlFlowFunctions.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

// Correctness of the explicit If/While/For constructs. Every kernel below traces
// in a single pass (no symbolic path-explosion); a successful compile is itself a
// proof of that, because the explicit primitives throw if a second trace iteration
// occurs.
void explicitControlFlowTest(engine::NautilusEngine& engine) {

	SECTION("ifThenElse") {
		auto f = engine.registerFunction(ifThenElseExplicit);
		REQUIRE(f(42) == 44);
		REQUIRE(f(1) == 85);
		REQUIRE(f(0) == 85);
		REQUIRE(f(-7) == 85);
	}

	SECTION("ifThen") {
		auto f = engine.registerFunction(ifThenExplicit);
		REQUIRE(f(5) == 10);
		REQUIRE(f(1) == 10);
		REQUIRE(f(0) == 0);
		REQUIRE(f(-3) == 0);
	}

	SECTION("nestedIf") {
		auto f = engine.registerFunction(nestedIfExplicit);
		REQUIRE(f(15) == 11); // x>0, x>10 -> 1 + 10
		REQUIRE(f(11) == 11);
		REQUIRE(f(5) == 6); // x>0, x<=10 -> 1 + 5
		REQUIRE(f(1) == 6);
		REQUIRE(f(0) == -1);  // else
		REQUIRE(f(-4) == -1); // else
	}

	SECTION("chainedIf") {
		auto f = engine.registerFunction(chainedIfExplicit);
		REQUIRE(f(-1) == 0);
		REQUIRE(f(0) == 0);
		REQUIRE(f(1) == 1);
		REQUIRE(f(2) == 2);
		REQUIRE(f(3) == 3);
		REQUIRE(f(100) == 3);
	}

	SECTION("conditionalAbs") {
		auto f = engine.registerFunction(conditionalAbsExplicit);
		REQUIRE(f(5) == 5);
		REQUIRE(f(0) == 0);
		REQUIRE(f(-5) == 5);
		REQUIRE(f(-123) == 123);
	}

	SECTION("sumFor") {
		auto f = engine.registerFunction(sumForExplicit);
		REQUIRE(f(0) == 0);
		REQUIRE(f(1) == 0);
		REQUIRE(f(5) == 10); // 0+1+2+3+4
		REQUIRE(f(10) == 45);
	}

	SECTION("sumLoop") {
		auto f = engine.registerFunction(sumLoopExplicit);
		REQUIRE(f(0) == 1);
		REQUIRE(f(1) == 11);
		REQUIRE(f(5) == 51);
		REQUIRE(f(10) == 101);
	}

	SECTION("whileCountdown") {
		auto f = engine.registerFunction(whileCountdownExplicit);
		REQUIRE(f(0) == 0);
		REQUIRE(f(1) == 1);
		REQUIRE(f(5) == 5);
		REQUIRE(f(100) == 100);
	}

	SECTION("gcd") {
		auto f = engine.registerFunction(gcdExplicit);
		REQUIRE(f(12, 8) == 4);
		REQUIRE(f(48, 36) == 12);
		REQUIRE(f(7, 0) == 7);
		REQUIRE(f(0, 5) == 5);
		REQUIRE(f(17, 13) == 1);
	}

	SECTION("nestedLoop") {
		auto f = engine.registerFunction(nestedLoopExplicit);
		REQUIRE(f(0) == 0);
		REQUIRE(f(1) == 1);
		REQUIRE(f(3) == 9);
		REQUIRE(f(4) == 16);
	}

	SECTION("ifInsideLoop") {
		auto f = engine.registerFunction(ifInsideLoopExplicit);
		REQUIRE(f(0) == 0);
		REQUIRE(f(4) == 60); // 10+20+10+20
		REQUIRE(f(5) == 70); // +10
	}

	SECTION("generalFor") {
		auto f = engine.registerFunction(generalForExplicit);
		REQUIRE(f(0) == 0);
		REQUIRE(f(5) == 10);
		REQUIRE(f(10) == 45);
	}
}

TEST_CASE("Explicit Control Flow Interpreter Test") {
	auto engine = nautilus::testing::makeEngine("interpreter");
	explicitControlFlowTest(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("Explicit Control Flow Compiler Test") {
	nautilus::testing::forEachBackendWithTraceMode(
	    [](engine::NautilusEngine& engine) { explicitControlFlowTest(engine); });
}

// Mixing explicit constructs with implicit native control flow in the same traced
// function is rejected with a clear error. (In the interpreter there is no tracing,
// so the function runs natively and is not rejected — hence this is compiled-only.)
TEST_CASE("Explicit Control Flow Mixing Rejected") {
	nautilus::testing::forEachBackendWithTraceMode([](engine::NautilusEngine& engine) {
		REQUIRE_THROWS(engine.registerFunction(mixedExplicitAndNativeIf));
	});
}
#endif

} // namespace nautilus::engine
