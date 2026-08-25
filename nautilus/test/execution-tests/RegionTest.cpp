#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/region.hpp"
#include "nautilus/static.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <vector>

namespace nautilus::engine {

val<int64_t> regionBasic() {
	val<int64_t> sum = 0;
	region([&]() { sum = sum + 42; });
	return sum;
}

val<int64_t> regionStaticUnroll() {
	val<int64_t> sum = 0;
	for (static_val<int32_t> j = 0; j < 3; j++) {
		region([&]() { sum = sum + val<int32_t>(j); });
	}
	return sum;
}

val<int64_t> regionReplayClosedBody(val<int64_t> n) {
	val<int64_t> sum = 0;
	val<int64_t> i = 0;
	while (i < n) {
		region([&]() { sum = sum + 1; });
		i = i + 1;
	}
	return sum;
}

val<int64_t> regionEscapedValue() {
	val<int64_t> out = 0;
	region([&]() {
		val<int64_t> t = 7;
		out = t;
	});
	return out + 1;
}

val<int64_t> regionInternalBranch(val<int64_t> x) {
	val<int64_t> sum = 0;
	region([&]() {
		if (x > 0) {
			sum = sum + 10;
		} else {
			sum = sum - 1;
		}
	});
	return sum;
}

// Three sequential independent branches in one region: exercises multiple
// local exploration passes (a fresh SymbolicExecutionContext per region --
// see docs/region.md) resolving several internal CMPs within a single
// region invocation, and the lazily-created exit block converging all eight
// combinations' tails.
val<int64_t> regionChainedIf(val<int64_t> a, val<int64_t> b, val<int64_t> c) {
	val<int64_t> sum = 0;
	region([&]() {
		if (a > 0) {
			sum = sum + 1;
		} else {
			sum = sum + 100;
		}
		if (b > 0) {
			sum = sum + 2;
		} else {
			sum = sum + 200;
		}
		if (c > 0) {
			sum = sum + 4;
		} else {
			sum = sum + 400;
		}
	});
	return sum;
}

// Each branch escapes a write to a different captured variable: exercises
// that local re-invocation of the region body doesn't corrupt a captured
// val<T>'s trace ref (val<T>::state is const), so each variable ends up with
// the correctly-merged value regardless of which local pass most recently
// wrote to it.
val<int64_t> regionBranchWritesDifferentVars(val<int64_t> x) {
	val<int64_t> a = 0;
	val<int64_t> b = 0;
	region([&]() {
		if (x > 0) {
			a = 10;
		} else {
			b = 20;
		}
	});
	return a + b;
}

// A native loop inside a region: the loop's back-edge is a CMP whose tag,
// on revisit, is detected via ExecutionTrace::checkTag within a single local
// pass (the same mechanism that already handles top-level loops) rather than
// through SymbolicExecutionContext's cross-pass exploration.
val<int64_t> regionInternalLoop(val<int64_t> n) {
	val<int64_t> sum = 0;
	region([&]() {
		val<int64_t> i = 0;
		while (i < n) {
			sum = sum + 1;
			i = i + 1;
		}
	});
	return sum;
}

val<int64_t> regionNested() {
	val<int64_t> sum = 0;
	region([&]() {
		region([&]() { sum = sum + 1; });
		sum = sum + 2;
	});
	return sum;
}

// A value constructed inside the *inner* region but owned by a container in
// the *outer* region's scope: it is still alive when the inner region ends, so
// it escapes into the enclosing region rather than all the way out to the
// function. Its liveness bookkeeping has to follow it there, because the
// freeValRef for its eventual destruction (below, while the outer region is
// still the active scope) is routed to the outer region too.
val<int64_t> regionNestedEscapeToOuter() {
	val<int64_t> sum = 0;
	region([&]() {
		std::vector<val<int64_t>> v;
		region([&]() { v.push_back(val<int64_t>(7)); });
		sum = sum + v[0];
	}); // v dies here, inside the outer region
	return sum;
}

// The same escape, but the container outlives *both* regions, so the value is
// still alive at each region end in turn -- the case that used to hand the same
// ref to the enclosing function once per region boundary crossed.
val<int64_t> regionNestedEscapeToFunction() {
	val<int64_t> sum = 0;
	std::vector<val<int64_t>> v;
	region([&]() {
		region([&]() { v.push_back(val<int64_t>(7)); });
		sum = sum + v[0];
	});
	return sum + v[0];
}

val<int64_t> regionEmptyAndUnnamed() {
	val<int64_t> sum = 0;
	region("empty", [&]() {});
	region([&]() {});
	return sum + 1;
}

val<int64_t> regionOuterDynamicInnerStatic(val<int64_t> n) {
	val<int64_t> sum = 0;
	val<int64_t> i = 0;
	while (i < n) {
		for (static_val<int32_t> j = 0; j < 3; j++) {
			region([&]() { sum = sum + val<int64_t>(j); });
		}
		i = i + 1;
	}
	return sum;
}

void runRegionTests(engine::NautilusEngine& engine) {
	SECTION("region basic") {
		auto fn = engine.registerFunction(regionBasic);
		REQUIRE(fn() == 42);
	}

	SECTION("region preserves static unrolling") {
		auto fn = engine.registerFunction(regionStaticUnroll);
		REQUIRE(fn() == 3); // 0+1+2; folding to 0+0+0 would give 0
	}

	SECTION("region replay skips closed body") {
		auto fn = engine.registerFunction(regionReplayClosedBody);
		REQUIRE(fn(4) == 4);
		REQUIRE(fn(0) == 0);
	}

	SECTION("region escaped value lifecycle") {
		auto fn = engine.registerFunction(regionEscapedValue);
		REQUIRE(fn() == 8);
	}

	SECTION("region internal branch") {
		auto fn = engine.registerFunction(regionInternalBranch);
		REQUIRE(fn(1) == 10);
		REQUIRE(fn(-1) == -1);
	}

	SECTION("region chained internal branches") {
		auto fn = engine.registerFunction(regionChainedIf);
		REQUIRE(fn(1, 1, 1) == 7);      // 1+2+4
		REQUIRE(fn(-1, -1, -1) == 700); // 100+200+400
		REQUIRE(fn(1, -1, 1) == 205);   // 1+200+4
		REQUIRE(fn(-1, 1, -1) == 502);  // 100+2+400
		REQUIRE(fn(1, 1, -1) == 403);   // 1+2+400
	}

	SECTION("region branches write different captured vars") {
		auto fn = engine.registerFunction(regionBranchWritesDifferentVars);
		REQUIRE(fn(1) == 10);
		REQUIRE(fn(-1) == 20);
	}

	SECTION("region internal loop") {
		auto fn = engine.registerFunction(regionInternalLoop);
		REQUIRE(fn(5) == 5);
		REQUIRE(fn(0) == 0);
	}

	SECTION("region nesting") {
		auto fn = engine.registerFunction(regionNested);
		REQUIRE(fn() == 3);
	}

	SECTION("region nested escape into enclosing region") {
		auto fn = engine.registerFunction(regionNestedEscapeToOuter);
		REQUIRE(fn() == 7);
	}

	SECTION("region nested escape out to function") {
		auto fn = engine.registerFunction(regionNestedEscapeToFunction);
		REQUIRE(fn() == 14);
	}

	SECTION("region empty and unnamed") {
		auto fn = engine.registerFunction(regionEmptyAndUnnamed);
		REQUIRE(fn() == 1);
	}

	SECTION("region outer dynamic inner static") {
		auto fn = engine.registerFunction(regionOuterDynamicInnerStatic);
		REQUIRE(fn(2) == 6); // 2 * (0+1+2); region re-entered under 3 parent-hash values
		REQUIRE(fn(0) == 0);
	}
}

#ifdef ENABLE_TRACING
TEST_CASE("Region Compiler Test", "[region]") {
	nautilus::testing::forEachBackendWithTraceMode([](engine::NautilusEngine& engine) { runRegionTests(engine); });
}
#else
TEST_CASE("Region Compiler Test", "[region]") {
	SKIP("Region compilation requires the tracing/compiler pipeline (ENABLE_TRACING)");
}
#endif

} // namespace nautilus::engine
