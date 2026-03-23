
#include "nautilus/Engine.hpp"
#include "nautilus/std/numeric.h"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

val<int32_t> accumulateTest(val<const int32_t*> first, val<size_t> count, val<int32_t> init) {
	return accumulate(first, count, init);
}

val<int32_t> innerProductTest(val<const int32_t*> first1, val<size_t> count, val<const int32_t*> first2,
                              val<int32_t> init) {
	return inner_product(first1, count, first2, init);
}

void iotaTest(val<int32_t*> first, val<size_t> count, val<int32_t> value) {
	iota(first, count, value);
}

void partialSumTest(val<const int32_t*> first, val<size_t> count, val<int32_t*> d_first) {
	partial_sum(first, count, d_first);
}

void adjacentDiffTest(val<const int32_t*> first, val<size_t> count, val<int32_t*> d_first) {
	adjacent_difference(first, count, d_first);
}

void runNumericTests(engine::NautilusEngine& engine) {
	SECTION("accumulate") {
		auto f = engine.registerFunction(accumulateTest);
		std::array<int32_t, 5> arr = {1, 2, 3, 4, 5};
		REQUIRE(f(arr.data(), 5, 0) == 15);
		REQUIRE(f(arr.data(), 5, 10) == 25);
		REQUIRE(f(arr.data(), 0, 42) == 42);
	}
	SECTION("inner_product") {
		auto f = engine.registerFunction(innerProductTest);
		std::array<int32_t, 3> a = {1, 2, 3};
		std::array<int32_t, 3> b = {4, 5, 6};
		// 1*4 + 2*5 + 3*6 = 4+10+18 = 32
		REQUIRE(f(a.data(), 3, b.data(), 0) == 32);
	}
	SECTION("iota") {
		auto f = engine.registerFunction(iotaTest);
		std::array<int32_t, 5> arr = {};
		f(arr.data(), 5, 10);
		REQUIRE(arr == std::array<int32_t, 5> {10, 11, 12, 13, 14});
	}
	SECTION("partial_sum") {
		auto f = engine.registerFunction(partialSumTest);
		std::array<int32_t, 4> arr = {1, 2, 3, 4};
		std::array<int32_t, 4> result = {};
		f(arr.data(), 4, result.data());
		REQUIRE(result == std::array<int32_t, 4> {1, 3, 6, 10});
	}
	SECTION("adjacent_difference") {
		auto f = engine.registerFunction(adjacentDiffTest);
		std::array<int32_t, 4> arr = {1, 3, 6, 10};
		std::array<int32_t, 4> result = {};
		f(arr.data(), 4, result.data());
		REQUIRE(result == std::array<int32_t, 4> {1, 2, 3, 4});
	}
}

TEST_CASE("NumericTest - Interpreter") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	runNumericTests(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("NumericTest - Compiler") {
	std::vector<std::string> backends = {};
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	backends.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	backends.emplace_back("asmjit");
#endif
	std::vector<std::string> traceModes = {"exceptionBasedTracing", "lazyTracing"};
	for (auto& backend : backends) {
		for (auto& traceMode : traceModes) {
			DYNAMIC_SECTION(backend + "_" + traceMode) {
				engine::Options options;
				options.setOption("engine.backend", backend);
				options.setOption("engine.traceMode", traceMode);
				auto engine = engine::NautilusEngine(options);
				runNumericTests(engine);
			}
		}
	}
}
#endif
} // namespace nautilus::engine
