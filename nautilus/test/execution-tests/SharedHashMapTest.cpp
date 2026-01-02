
#include "nautilus/tracing/tag/SharedHashMap.hpp"
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <sys/wait.h>

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
		return std::hash<uint32_t> {}(k.tagIndex) ^ (std::hash<uint64_t> {}(k.hash) << 1);
	}
};
} // namespace std

struct TestValue {
	uint16_t blockIndex;
	uint16_t operationIndex;
};

TEST_CASE("Map basic operations", "[Map]") {
	auto map = Map<TestKey, TestValue>(128);

	SECTION("initial state") {
		REQUIRE(map.size() == 0);
		REQUIRE(map.capacity() == 128);
	}

	SECTION("insert and get") {
		TestKey key {1, 12345};
		TestValue val {10, 20};

		bool isNew = map.insert(key, val);
		REQUIRE(isNew);
		REQUIRE(map.size() == 1);

		auto* found = map.get(key);
		REQUIRE(found != nullptr);
		REQUIRE(found->blockIndex == 10);
		REQUIRE(found->operationIndex == 20);
	}

	SECTION("update existing key") {
		TestKey key {1, 100};
		map.insert(key, {1, 1});
		REQUIRE(map.size() == 1);

		bool isNew = map.insert(key, {2, 2});
		REQUIRE_FALSE(isNew);
		REQUIRE(map.size() == 1);

		auto* found = map.get(key);
		REQUIRE(found->blockIndex == 2);
	}

	SECTION("get non-existent key") {
		TestKey key {999, 999};
		auto* found = map.get(key);
		REQUIRE(found == nullptr);
	}

	SECTION("contains") {
		TestKey key {5, 50};
		REQUIRE_FALSE(map.contains(key));

		map.insert(key, {1, 1});
		REQUIRE(map.contains(key));
	}

	SECTION("multiple inserts with collision handling") {
		for (uint32_t i = 1; i <= 50; i++) {
			TestKey key {i, i * 100};
			map.insert(key, {static_cast<uint16_t>(i), static_cast<uint16_t>(i * 2)});
		}
		REQUIRE(map.size() == 50);

		// Verify all can be found
		for (uint32_t i = 1; i <= 50; i++) {
			TestKey key {i, i * 100};
			auto* found = map.get(key);
			REQUIRE(found != nullptr);
			REQUIRE(found->blockIndex == i);
		}
	}
}

TEST_CASE("Map full map", "[Map]") {
	auto map = Map<TestKey, TestValue>(16);

	// Fill the map
	for (uint32_t i = 1; i <= 16; i++) {
		map.insert({i, i}, {static_cast<uint16_t>(i), 0});
	}
	REQUIRE(map.size() == 16);

	// Next insert should throw
	REQUIRE_THROWS_AS(map.insert({999, 999}, {0, 0}), nautilus::RuntimeException);
}

TEST_CASE("Map with fork", "[Map][fork]") {
	auto map = Map<TestKey, TestValue>(128);

	// Parent inserts something
	map.insert({1, 100}, {10, 20});
	REQUIRE(map.size() == 1);

	pid_t pid = fork();
	if (pid == 0) {
		// Child process - map is still valid (copy of wrapper, same shared memory)
		// Child can see parent's data
		auto* found = map.get({1, 100});
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

TEST_CASE("Map move semantics", "[Map]") {
	auto map1 = Map<TestKey, TestValue>(128);
	map1.insert({1, 1}, {10, 20});
	map1.insert({2, 2}, {30, 40});
	REQUIRE(map1.size() == 2);

	// Move construct
	auto map2 = std::move(map1);
	REQUIRE(map2.size() == 2);
	REQUIRE(map2.contains({1, 1}));

	// Move assign
	auto map3 = Map<TestKey, TestValue>(50);
	map3 = std::move(map2);
	REQUIRE(map3.size() == 2);
}

// Key type with struct padding (uint64_t followed by uint32_t creates 4 bytes of padding)
// This mimics the Snapshot struct layout
struct PaddedKey {
	uint64_t largeField; // 8 bytes
	uint32_t smallField; // 4 bytes + 4 bytes padding to align to 8-byte boundary

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
		return std::hash<uint64_t> {}(k.largeField) ^ (std::hash<uint32_t> {}(k.smallField) << 1);
	}
};
} // namespace std

TEST_CASE("Map with struct padding", "[Map][padding]") {
	// This test verifies that the isEmpty() function correctly handles
	// struct padding bytes. The map entries are memset to 0 (including padding),
	// so isEmpty() must also zero padding bytes when comparing.
	auto map = Map<PaddedKey, TestValue>(8192);

	SECTION("insert into fresh map with padded key") {
		// If isEmpty() doesn't handle padding correctly, the first insert
		// will fail because the padded entry won't match a value-initialized key
		PaddedKey key {12345, 42};
		TestValue val {10, 20};

		REQUIRE(map.size() == 0);
		REQUIRE(map.capacity() == 8192);

		bool isNew = map.insert(key, val);
		REQUIRE(isNew);
		REQUIRE(map.size() == 1);

		auto* found = map.get(key);
		REQUIRE(found != nullptr);
		REQUIRE(found->blockIndex == 10);
		REQUIRE(found->operationIndex == 20);
	}

	SECTION("multiple inserts with padded keys") {
		for (uint32_t i = 1; i <= 5000; i++) {
			PaddedKey key {i * 1000ULL, i};
			map.insert(key, {static_cast<uint16_t>(i), static_cast<uint16_t>(i * 2)});
		}
		REQUIRE(map.size() == 5000);

		// Verify all can be found
		for (uint32_t i = 1; i <= 5000; i++) {
			PaddedKey key {i * 1000ULL, i};
			auto* found = map.get(key);
			REQUIRE(found != nullptr);
			REQUIRE(found->blockIndex == i);
		}
	}
}
