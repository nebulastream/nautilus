
#include <catch2/catch_test_macros.hpp>
#include "nautilus/tracing/tag/SharedHashMap.hpp"
#include <sys/wait.h>
#include <string>

using namespace nautilus::tracing;

// Simple key type for testing (simulates Snapshot with tagIndex)
struct TestKey {
	uint32_t tagIndex;
	uint64_t hash;

	bool operator==(const TestKey& other) const {
		return tagIndex == other.tagIndex && hash == other.hash;
	}
};

namespace std {
template <>
struct hash<TestKey> {
	size_t operator()(const TestKey& k) const {
		return std::hash<uint32_t>{}(k.tagIndex) ^ (std::hash<uint64_t>{}(k.hash) << 1);
	}
};
} // namespace std

struct TestValue {
	uint16_t blockIndex;
	uint16_t operationIndex;
};

TEST_CASE("SharedHashMap basic operations", "[SharedHashMap]") {
	auto map = SharedHashMap<TestKey, TestValue>::create(100);

	SECTION("initial state") {
		REQUIRE(map.size() == 0);
		REQUIRE(map.capacity() == 100);
	}

	SECTION("insert and find") {
		TestKey key{1, 12345};
		TestValue val{10, 20};

		bool isNew = map.insert(key, val);
		REQUIRE(isNew);
		REQUIRE(map.size() == 1);

		auto* found = map.find(key);
		REQUIRE(found != nullptr);
		REQUIRE(found->blockIndex == 10);
		REQUIRE(found->operationIndex == 20);
	}

	SECTION("update existing key") {
		TestKey key{1, 100};
		map.insert(key, {1, 1});
		REQUIRE(map.size() == 1);

		bool isNew = map.insert(key, {2, 2});
		REQUIRE_FALSE(isNew);
		REQUIRE(map.size() == 1);

		auto* found = map.find(key);
		REQUIRE(found->blockIndex == 2);
	}

	SECTION("find non-existent key") {
		TestKey key{999, 999};
		auto* found = map.find(key);
		REQUIRE(found == nullptr);
	}

	SECTION("contains") {
		TestKey key{5, 50};
		REQUIRE_FALSE(map.contains(key));

		map.insert(key, {1, 1});
		REQUIRE(map.contains(key));
	}

	SECTION("operator[]") {
		TestKey key{7, 70};
		map[key] = {100, 200};
		REQUIRE(map.size() == 1);

		auto* found = map.find(key);
		REQUIRE(found->blockIndex == 100);
		REQUIRE(found->operationIndex == 200);

		// Access again returns same value
		map[key].operationIndex = 300;
		REQUIRE(map.find(key)->operationIndex == 300);
		REQUIRE(map.size() == 1);
	}

	SECTION("multiple inserts with collision handling") {
		for (uint32_t i = 1; i <= 50; i++) {
			TestKey key{i, i * 100};
			map.insert(key, {static_cast<uint16_t>(i), static_cast<uint16_t>(i * 2)});
		}
		REQUIRE(map.size() == 50);

		// Verify all can be found
		for (uint32_t i = 1; i <= 50; i++) {
			TestKey key{i, i * 100};
			auto* found = map.find(key);
			REQUIRE(found != nullptr);
			REQUIRE(found->blockIndex == i);
		}
	}

	SECTION("clear") {
		map.insert({1, 1}, {1, 1});
		map.insert({2, 2}, {2, 2});
		REQUIRE(map.size() == 2);

		map.clear();
		REQUIRE(map.size() == 0);
		REQUIRE_FALSE(map.contains({1, 1}));
	}
}

TEST_CASE("SharedHashMap full map", "[SharedHashMap]") {
	auto map = SharedHashMap<TestKey, TestValue>::create(10);

	// Fill the map
	for (uint32_t i = 1; i <= 10; i++) {
		map.insert({i, i}, {static_cast<uint16_t>(i), 0});
	}
	REQUIRE(map.size() == 10);

	// Next insert should throw
	REQUIRE_THROWS_AS(map.insert({999, 999}, {0, 0}), std::runtime_error);
}

TEST_CASE("SharedHashMap with fork", "[SharedHashMap][fork]") {
	auto map = SharedHashMap<TestKey, TestValue>::create(100);

	// Parent inserts something
	map.insert({1, 100}, {10, 20});
	REQUIRE(map.size() == 1);

	pid_t pid = fork();
	if (pid == 0) {
		// Child process - map is still valid (copy of wrapper, same shared memory)
		// Child can see parent's data
		auto* found = map.find({1, 100});
		if (found == nullptr || found->blockIndex != 10) {
			_exit(1);
		}

		// Child adds more data
		map.insert({2, 200}, {30, 40});
		map.insert({3, 300}, {50, 60});

		_exit(0);
	} else {
		int status;
		waitpid(pid, &status, 0);
		REQUIRE(WIFEXITED(status));
		REQUIRE(WEXITSTATUS(status) == 0);

		// Parent can see child's modifications
		REQUIRE(map.size() == 3);
		REQUIRE(map.contains({2, 200}));
		REQUIRE(map.contains({3, 300}));
	}
	// Destructor only cleans up in parent (creator process)
}

TEST_CASE("SharedHashMap load factor", "[SharedHashMap]") {
	auto map = SharedHashMap<TestKey, TestValue>::create(100);

	REQUIRE(map.loadFactor() == 0.0f);

	for (uint32_t i = 1; i <= 50; i++) {
		map.insert({i, i}, {0, 0});
	}
	REQUIRE(map.loadFactor() == 0.5f);
}

TEST_CASE("SharedHashMap move semantics", "[SharedHashMap]") {
	auto map1 = SharedHashMap<TestKey, TestValue>::create(100);
	map1.insert({1, 1}, {10, 20});
	map1.insert({2, 2}, {30, 40});
	REQUIRE(map1.size() == 2);

	// Move construct
	auto map2 = std::move(map1);
	REQUIRE(map2.size() == 2);
	REQUIRE(map2.contains({1, 1}));

	// Move assign
	auto map3 = SharedHashMap<TestKey, TestValue>::create(50);
	map3 = std::move(map2);
	REQUIRE(map3.size() == 2);
}

// Key type with struct padding (uint64_t followed by uint32_t creates 4 bytes of padding)
// This mimics the Snapshot struct layout
struct PaddedKey {
	uint64_t largeField;  // 8 bytes
	uint32_t smallField;  // 4 bytes + 4 bytes padding to align to 8-byte boundary

	bool operator==(const PaddedKey& other) const {
		return largeField == other.largeField && smallField == other.smallField;
	}
};

// Verify that PaddedKey actually has padding
static_assert(sizeof(PaddedKey) == 16, "PaddedKey should have 4 bytes of padding");

namespace std {
template <>
struct hash<PaddedKey> {
	size_t operator()(const PaddedKey& k) const {
		return std::hash<uint64_t>{}(k.largeField) ^ (std::hash<uint32_t>{}(k.smallField) << 1);
	}
};
} // namespace std

TEST_CASE("SharedHashMap with struct padding", "[SharedHashMap][padding]") {
	// This test verifies that the isEmpty() function correctly handles
	// struct padding bytes. The map entries are memset to 0 (including padding),
	// so isEmpty() must also zero padding bytes when comparing.
	auto map = SharedHashMap<PaddedKey, TestValue>::create(100);

	SECTION("insert into fresh map with padded key") {
		// If isEmpty() doesn't handle padding correctly, the first insert
		// will fail because the padded entry won't match a value-initialized key
		PaddedKey key{12345, 42};
		TestValue val{10, 20};

		REQUIRE(map.size() == 0);
		REQUIRE(map.capacity() == 100);

		bool isNew = map.insert(key, val);
		REQUIRE(isNew);
		REQUIRE(map.size() == 1);

		auto* found = map.find(key);
		REQUIRE(found != nullptr);
		REQUIRE(found->blockIndex == 10);
		REQUIRE(found->operationIndex == 20);
	}

	SECTION("multiple inserts with padded keys") {
		for (uint32_t i = 1; i <= 50; i++) {
			PaddedKey key{i * 1000ULL, i};
			map.insert(key, {static_cast<uint16_t>(i), static_cast<uint16_t>(i * 2)});
		}
		REQUIRE(map.size() == 50);

		// Verify all can be found
		for (uint32_t i = 1; i <= 50; i++) {
			PaddedKey key{i * 1000ULL, i};
			auto* found = map.find(key);
			REQUIRE(found != nullptr);
			REQUIRE(found->blockIndex == i);
		}
	}

	SECTION("operator[] with padded key") {
		PaddedKey key{99999, 7};
		map[key] = {100, 200};
		REQUIRE(map.size() == 1);

		auto* found = map.find(key);
		REQUIRE(found != nullptr);
		REQUIRE(found->blockIndex == 100);
	}
}
