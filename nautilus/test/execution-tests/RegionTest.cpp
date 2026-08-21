#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/region.hpp"
#include "nautilus/static.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>

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

val<int64_t> regionNested() {
	val<int64_t> sum = 0;
	region([&]() {
		region([&]() { sum = sum + 1; });
		sum = sum + 2;
	});
	return sum;
}

val<int64_t> regionEmptyAndUnnamed() {
	val<int64_t> sum = 0;
	region("empty", [&]() {});
	region([&]() {});
	return sum + 1;
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

	SECTION("region nesting") {
		auto fn = engine.registerFunction(regionNested);
		REQUIRE(fn() == 3);
	}

	SECTION("region empty and unnamed") {
		auto fn = engine.registerFunction(regionEmptyAndUnnamed);
		REQUIRE(fn() == 1);
	}
}

TEST_CASE("Region Compiler Test", "[region]") {
	nautilus::testing::forEachBackendWithTraceMode([](engine::NautilusEngine& engine) { runRegionTests(engine); });
}

} // namespace nautilus::engine
