#include "nautilus/compiler/ir/passes/FlatPointerMap.hpp"
#include <catch2/catch_all.hpp>
#include <random>
#include <unordered_map>
#include <vector>

namespace nautilus::testing {

using compiler::ir::FlatPointerMap;
using compiler::ir::FlatPointerSet;

TEST_CASE("FlatPointerMap: agrees with std::unordered_map under random inserts and erases") {
	// Keys from one array, so they are as dense and aligned as arena objects;
	// a small key range forces long probe runs, collisions, growth and
	// backward-shift deletion across the wrap-around.
	std::vector<int64_t> objects(300);
	std::mt19937 rng(42);
	std::uniform_int_distribution<size_t> pick(0, objects.size() - 1);
	std::uniform_int_distribution<int> action(0, 9);

	FlatPointerMap<int64_t, int> map;
	std::unordered_map<const int64_t*, int> reference;
	for (int step = 0; step < 20000; ++step) {
		const int64_t* key = &objects[pick(rng)];
		const int a = action(rng);
		if (a < 5) {
			map[key] = step;
			reference[key] = step;
		} else if (a < 8) {
			REQUIRE(map.erase(key) == (reference.erase(key) == 1));
		} else {
			auto [value, inserted] = map.tryEmplace(key);
			auto [it, refInserted] = reference.try_emplace(key, 0);
			REQUIRE(inserted == refInserted);
			REQUIRE(*value == it->second);
		}
		REQUIRE(map.size() == reference.size());
		if (step % 97 == 0) {
			for (const auto& object : objects) {
				const int* found = map.find(&object);
				auto it = reference.find(&object);
				REQUIRE((found != nullptr) == (it != reference.end()));
				if (found != nullptr) {
					REQUIRE(*found == it->second);
				}
			}
		}
	}
}

TEST_CASE("FlatPointerMap: erasing inside a full probe run keeps every key reachable") {
	std::vector<int64_t> objects(64);
	FlatPointerMap<int64_t, size_t> map;
	for (size_t i = 0; i < objects.size(); ++i) {
		map[&objects[i]] = i;
	}
	for (size_t i = 0; i < objects.size(); i += 2) {
		REQUIRE(map.erase(&objects[i]));
		REQUIRE_FALSE(map.erase(&objects[i]));
	}
	for (size_t i = 0; i < objects.size(); ++i) {
		const size_t* value = map.find(&objects[i]);
		if (i % 2 == 0) {
			REQUIRE(value == nullptr);
		} else {
			REQUIRE(value != nullptr);
			REQUIRE(*value == i);
		}
	}
}

TEST_CASE("FlatPointerSet: insert reports novelty, erase and contains agree") {
	int64_t a = 0;
	int64_t b = 0;
	FlatPointerSet<int64_t> set;
	REQUIRE(set.insert(&a));
	REQUIRE_FALSE(set.insert(&a));
	REQUIRE(set.contains(&a));
	REQUIRE_FALSE(set.contains(&b));
	REQUIRE(set.erase(&a));
	REQUIRE_FALSE(set.contains(&a));
	REQUIRE(set.size() == 0);
}

} // namespace nautilus::testing
