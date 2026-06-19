#include "ExecutionTest.hpp"
#include "ExplicitControlFlowFunctions.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

// Native-control-flow twins computing the same function as the explicit kernels,
// defined locally (the shared kernel headers are non-inline and already owned by
// ExecutionTest.cpp in this binary).
namespace {
val<int32_t> ifThenElseImplicitTwin(val<int32_t> value) {
	val<int32_t> iw = 1;
	if (value == 42) {
		iw = iw + 1;
	} else {
		iw = iw + 42;
	}
	return iw + 42;
}
val<int32_t> sumLoopImplicitTwin(val<int32_t> upperLimit) {
	val<int32_t> agg = 1;
	for (val<int32_t> start = 0; start < upperLimit; start = start + 1) {
		agg = agg + 10;
	}
	return agg;
}
val<int32_t> gcdImplicitTwin(val<int32_t> a, val<int32_t> b) {
	while (b != 0) {
		val<int32_t> t = b;
		b = a % b;
		a = t;
	}
	return a;
}
} // namespace

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

	SECTION("whileBreak") {
		auto f = engine.registerFunction(whileBreakExplicit);
		REQUIRE(f(3) == 3);    // 0+1+2
		REQUIRE(f(5) == 10);   // 0+1+2+3+4
		REQUIRE(f(100) == 10); // breaks at i==5
	}

	SECTION("forBreak") {
		auto f = engine.registerFunction(forBreakExplicit);
		REQUIRE(f(3) == 3);
		REQUIRE(f(5) == 10);
		REQUIRE(f(100) == 10);
	}

	SECTION("forContinue") {
		auto f = engine.registerFunction(forContinueExplicit);
		REQUIRE(f(5) == 4);   // 1+3
		REQUIRE(f(10) == 25); // 1+3+5+7+9
		REQUIRE(f(0) == 0);
	}

	SECTION("whileContinue") {
		auto f = engine.registerFunction(whileContinueExplicit);
		REQUIRE(f(0) == 0);
		REQUIRE(f(5) == 2);  // odds in [0,5): 1,3
		REQUIRE(f(10) == 5); // 1,3,5,7,9
	}

	SECTION("nestedBreak") {
		auto f = engine.registerFunction(nestedBreakExplicit);
		// inner loop runs j=0,1 then breaks at j==2 -> 2 increments per outer iter
		REQUIRE(f(0) == 0);
		REQUIRE(f(1) == 1);  // inner break at j==2 unreachable (n=1) -> 1
		REQUIRE(f(3) == 6);  // 3 outer * 2 inner
		REQUIRE(f(5) == 10); // 5 outer * 2 inner
	}

	SECTION("max") {
		auto f = engine.registerFunction(maxExplicit);
		REQUIRE(f(5, 3) == 5);
		REQUIRE(f(3, 7) == 7);
		REQUIRE(f(4, 4) == 4);
		REQUIRE(f(-2, -9) == -2);
	}

	SECTION("forStride") {
		auto f = engine.registerFunction(forStrideExplicit);
		REQUIRE(f(0) == 0);
		REQUIRE(f(5) == 6);   // 0+2+4
		REQUIRE(f(10) == 20); // 0+2+4+6+8
	}
}

// Compiles each explicit kernel and the native-control-flow kernel that computes
// the same function, and asserts they produce identical results over an input
// sweep -- a robust, end-to-end check that the explicit API lowers to behaviour
// identical to the implicit path (no fragile IR-string comparison).
void explicitVsImplicitEquivalence(engine::NautilusEngine& engine) {
	SECTION("ifThenElse") {
		auto exp = engine.registerFunction(ifThenElseExplicit);
		auto imp = engine.registerFunction(ifThenElseImplicitTwin);
		for (int32_t v : {-5, 0, 1, 41, 42, 43, 100}) {
			REQUIRE(exp(v) == imp(v));
		}
	}
	SECTION("sumLoop") {
		auto exp = engine.registerFunction(sumLoopExplicit);
		auto imp = engine.registerFunction(sumLoopImplicitTwin);
		for (int32_t n : {0, 1, 2, 5, 10, 50}) {
			REQUIRE(exp(n) == imp(n));
		}
	}
	SECTION("gcd") {
		auto exp = engine.registerFunction(gcdExplicit);
		auto imp = engine.registerFunction(gcdImplicitTwin);
		for (auto [a, b] :
		     std::vector<std::pair<int32_t, int32_t>> {{12, 8}, {48, 36}, {7, 0}, {0, 5}, {17, 13}, {100, 60}}) {
			REQUIRE(exp(a, b) == imp(a, b));
		}
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

TEST_CASE("Explicit vs Implicit Equivalence") {
	nautilus::testing::forEachBackendWithTraceMode(
	    [](engine::NautilusEngine& engine) { explicitVsImplicitEquivalence(engine); });
}

// Mixing explicit constructs with implicit native control flow in the same traced
// function is rejected with a clear error. (In the interpreter there is no tracing,
// so the function runs natively and is not rejected — hence this is compiled-only.)
TEST_CASE("Explicit Control Flow Mixing Rejected") {
	nautilus::testing::forEachBackendWithTraceMode([](engine::NautilusEngine& engine) {
		REQUIRE_THROWS(engine.registerFunction(mixedExplicitAndNativeIf));
		// A native if placed before the explicit construct is also rejected.
		REQUIRE_THROWS(engine.registerFunction(nativeIfThenExplicit));
		// An implicit native if inside an explicit loop body is also rejected.
		REQUIRE_THROWS(engine.registerFunction(explicitLoopWithNativeIf));
		REQUIRE_THROWS(engine.registerFunction(explicitWhileWithNativeIf));
	});
}
#endif

} // namespace nautilus::engine
