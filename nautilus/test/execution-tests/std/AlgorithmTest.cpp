
#include "nautilus/Engine.hpp"
#include "nautilus/std/algorithm.h"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

val<int32_t> minTest(val<int32_t> a, val<int32_t> b) {
	return min(a, b);
}

val<int32_t> maxTest(val<int32_t> a, val<int32_t> b) {
	return max(a, b);
}

val<int32_t> clampTest(val<int32_t> v, val<int32_t> lo, val<int32_t> hi) {
	return clamp(v, lo, hi);
}

void fillTest(val<int32_t*> dst, val<size_t> count, val<int32_t> value) {
	fill(dst, count, value);
}

val<bool> equalTest(val<const int32_t*> a, val<size_t> count, val<const int32_t*> b) {
	return equal(a, count, b);
}

val<size_t> countTest(val<const int32_t*> first, val<size_t> n, val<int32_t> value) {
	return count(first, n, value);
}

void sortTest(val<int32_t*> first, val<size_t> count) {
	sort(first, count);
}

val<bool> binarySearchTest(val<const int32_t*> first, val<size_t> count, val<int32_t> value) {
	return binary_search(first, count, value);
}

void reverseTest(val<int32_t*> first, val<size_t> count) {
	reverse(first, count);
}

void runAlgorithmTests(engine::NautilusEngine& engine) {
	SECTION("min") {
		auto f = engine.registerFunction(minTest);
		REQUIRE(f(3, 5) == 3);
		REQUIRE(f(7, 2) == 2);
		REQUIRE(f(-1, 1) == -1);
		REQUIRE(f(4, 4) == 4);
	}
	SECTION("max") {
		auto f = engine.registerFunction(maxTest);
		REQUIRE(f(3, 5) == 5);
		REQUIRE(f(7, 2) == 7);
		REQUIRE(f(-1, 1) == 1);
		REQUIRE(f(4, 4) == 4);
	}
	SECTION("clamp") {
		auto f = engine.registerFunction(clampTest);
		REQUIRE(f(5, 0, 10) == 5);
		REQUIRE(f(-5, 0, 10) == 0);
		REQUIRE(f(15, 0, 10) == 10);
	}
	SECTION("fill") {
		auto f = engine.registerFunction(fillTest);
		std::array<int32_t, 5> arr = {};
		f(arr.data(), 5, 42);
		for (auto& v : arr) {
			REQUIRE(v == 42);
		}
	}
	SECTION("equal") {
		auto f = engine.registerFunction(equalTest);
		std::array<int32_t, 4> a = {1, 2, 3, 4};
		std::array<int32_t, 4> b = {1, 2, 3, 4};
		REQUIRE(f(a.data(), 4, b.data()) == true);
	}
	SECTION("count") {
		auto f = engine.registerFunction(countTest);
		std::array<int32_t, 6> arr = {1, 2, 3, 2, 2, 4};
		REQUIRE(f(arr.data(), 6, 2) == 3);
		REQUIRE(f(arr.data(), 6, 5) == 0);
	}
	SECTION("sort") {
		auto f = engine.registerFunction(sortTest);
		std::array<int32_t, 5> arr = {5, 3, 1, 4, 2};
		f(arr.data(), 5);
		REQUIRE(arr == std::array<int32_t, 5> {1, 2, 3, 4, 5});
	}
	SECTION("binary_search") {
		auto f = engine.registerFunction(binarySearchTest);
		std::array<int32_t, 5> arr = {1, 2, 3, 4, 5};
		REQUIRE(f(arr.data(), 5, 3) == true);
		REQUIRE(f(arr.data(), 5, 6) == false);
	}
	SECTION("reverse") {
		auto f = engine.registerFunction(reverseTest);
		std::array<int32_t, 5> arr = {1, 2, 3, 4, 5};
		f(arr.data(), 5);
		REQUIRE(arr == std::array<int32_t, 5> {5, 4, 3, 2, 1});
	}
}

TEST_CASE("AlgorithmTest - Interpreter") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	runAlgorithmTests(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("AlgorithmTest - Compiler") {
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
				runAlgorithmTests(engine);
			}
		}
	}
}
#endif
} // namespace nautilus::engine
