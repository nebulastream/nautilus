#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/std/algorithm.h"
#include <algorithm>
#include <catch2/catch_all.hpp>
#include <vector>

namespace nautilus::engine {

struct AbsLess {
	bool operator()(int32_t a, int32_t b) const noexcept {
		return (a < 0 ? -a : a) < (b < 0 ? -b : b);
	}
};

void algoSort(val<int32_t*> array, val<size_t> size) {
	nautilus::sort(array, array + size);
}

void algoSortDescending(val<int32_t*> array, val<size_t> size) {
	nautilus::sort<std::greater<>>(array, array + size);
}

void algoStableSortByAbs(val<int32_t*> array, val<size_t> size) {
	nautilus::stable_sort<AbsLess>(array, array + size);
}

void algoPartialSort(val<int32_t*> array, val<size_t> size, val<size_t> k) {
	nautilus::partial_sort(array, array + k, array + size);
}

val<size_t> algoPartialSortCopy(val<int32_t*> array, val<size_t> size, val<int32_t*> out, val<size_t> out_size) {
	auto end = nautilus::partial_sort_copy(array, array + size, out, out + out_size);
	return val<size_t>((static_cast<val<uint64_t>>(end) - static_cast<val<uint64_t>>(out)) /
	                   val<uint64_t>(sizeof(int32_t)));
}

val<int32_t> algoNthElement(val<int32_t*> array, val<size_t> size, val<size_t> n) {
	auto nth = array + n;
	nautilus::nth_element(array, nth, array + size);
	return *nth;
}

val<double> algoMedian(val<double*> array, val<size_t> size) {
	auto nth = array + size / val<size_t>(2);
	nautilus::nth_element(array, nth, array + size);
	return *nth;
}

val<int32_t> algoNthLargest(val<int32_t*> array, val<size_t> size, val<size_t> n) {
	auto nth = array + n;
	nautilus::nth_element<std::greater<>>(array, nth, array + size);
	return *nth;
}

val<bool> algoIsSorted(val<int32_t*> array, val<size_t> size) {
	return nautilus::is_sorted(array, array + size);
}

val<size_t> algoIsSortedUntil(val<int32_t*> array, val<size_t> size) {
	auto until = nautilus::is_sorted_until(array, array + size);
	return val<size_t>((static_cast<val<uint64_t>>(until) - static_cast<val<uint64_t>>(array)) /
	                   val<uint64_t>(sizeof(int32_t)));
}

val<int32_t> algoMinMaxSpread(val<int32_t*> array, val<size_t> size) {
	auto min = nautilus::min_element(array, array + size);
	auto max = nautilus::max_element(array, array + size);
	return *max - *min;
}

void algorithmTest(engine::NautilusEngine& engine) {
	SECTION("sort") {
		auto f = engine.registerFunction(algoSort);
		std::vector<int32_t> values = {5, -3, 9, 0, 2, 2, 7};
		f(values.data(), values.size());
		REQUIRE(values == std::vector<int32_t> {-3, 0, 2, 2, 5, 7, 9});
	}
	SECTION("sort empty range") {
		auto f = engine.registerFunction(algoSort);
		int32_t value = 42;
		f(&value, (size_t) 0);
		REQUIRE(value == 42);
	}
	SECTION("sort descending") {
		auto f = engine.registerFunction(algoSortDescending);
		std::vector<int32_t> values = {5, -3, 9, 0, 2};
		f(values.data(), values.size());
		REQUIRE(values == std::vector<int32_t> {9, 5, 2, 0, -3});
	}
	SECTION("stable_sort with custom comparator") {
		auto f = engine.registerFunction(algoStableSortByAbs);
		std::vector<int32_t> values = {3, -1, 2, 1, -2, -3};
		f(values.data(), values.size());
		REQUIRE(values == std::vector<int32_t> {-1, 1, 2, -2, 3, -3});
	}
	SECTION("partial_sort") {
		auto f = engine.registerFunction(algoPartialSort);
		std::vector<int32_t> values = {8, 3, 7, 1, 9, 2, 6};
		f(values.data(), values.size(), (size_t) 3);
		REQUIRE(values[0] == 1);
		REQUIRE(values[1] == 2);
		REQUIRE(values[2] == 3);
		REQUIRE(std::is_permutation(values.begin(), values.end(), std::vector<int32_t> {8, 3, 7, 1, 9, 2, 6}.begin()));
	}
	SECTION("partial_sort_copy") {
		auto f = engine.registerFunction(algoPartialSortCopy);
		std::vector<int32_t> values = {8, 3, 7, 1, 9};
		std::vector<int32_t> out(3, 0);
		REQUIRE(f(values.data(), values.size(), out.data(), out.size()) == 3);
		REQUIRE(out == std::vector<int32_t> {1, 3, 7});
		REQUIRE(values == std::vector<int32_t> {8, 3, 7, 1, 9});

		std::vector<int32_t> large_out(8, 0);
		REQUIRE(f(values.data(), values.size(), large_out.data(), large_out.size()) == 5);
		REQUIRE(large_out == std::vector<int32_t> {1, 3, 7, 8, 9, 0, 0, 0});
	}
	SECTION("nth_element") {
		auto f = engine.registerFunction(algoNthElement);
		const std::vector<int32_t> input = {8, 3, 7, 1, 9, 2, 6, 4, 5};
		for (size_t n = 0; n < input.size(); n++) {
			auto values = input;
			REQUIRE(f(values.data(), values.size(), n) == static_cast<int32_t>(n + 1));
			for (size_t i = 0; i < n; i++) {
				REQUIRE(values[i] <= values[n]);
			}
			for (size_t i = n + 1; i < values.size(); i++) {
				REQUIRE(values[i] >= values[n]);
			}
		}
	}
	SECTION("nth_element median of doubles") {
		auto f = engine.registerFunction(algoMedian);
		std::vector<double> values = {4.5, 1.5, 3.5, 2.5, 5.5};
		REQUIRE(f(values.data(), values.size()) == 3.5);
	}
	SECTION("nth_element descending") {
		auto f = engine.registerFunction(algoNthLargest);
		std::vector<int32_t> values = {8, 3, 7, 1, 9, 2};
		REQUIRE(f(values.data(), values.size(), (size_t) 1) == 8);
	}
	SECTION("is_sorted") {
		auto f = engine.registerFunction(algoIsSorted);
		std::vector<int32_t> sorted = {1, 2, 2, 5};
		std::vector<int32_t> unsorted = {1, 3, 2, 5};
		REQUIRE(f(sorted.data(), sorted.size()));
		REQUIRE_FALSE(f(unsorted.data(), unsorted.size()));
	}
	SECTION("is_sorted_until") {
		auto f = engine.registerFunction(algoIsSortedUntil);
		std::vector<int32_t> values = {1, 2, 4, 3, 5};
		REQUIRE(f(values.data(), values.size()) == 3);
		std::vector<int32_t> sorted = {1, 2, 3};
		REQUIRE(f(sorted.data(), sorted.size()) == 3);
	}
	SECTION("min_element and max_element") {
		auto f = engine.registerFunction(algoMinMaxSpread);
		std::vector<int32_t> values = {4, -7, 12, 0, 3};
		REQUIRE(f(values.data(), values.size()) == 19);
	}
}

TEST_CASE("AlgorithmTest - Interpreter") {
	auto engine = nautilus::testing::makeEngine("interpreter");
	algorithmTest(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("AlgorithmTest - Compiler") {
	for (const auto& backend : nautilus::testing::availableBackends()) {
		DYNAMIC_SECTION(backend) {
			auto engine = nautilus::testing::makeEngine(backend);
			algorithmTest(engine);
		}
	}
}
#endif
} // namespace nautilus::engine
