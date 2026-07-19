#include "nautilus/common/Arena.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstddef>

namespace nautilus::common {

namespace {

struct DestructionProbe {
	explicit DestructionProbe(size_t& destructionCount) : destructionCount(destructionCount) {
	}

	~DestructionProbe() {
		++destructionCount;
	}

	size_t& destructionCount;
};

} // namespace

TEST_CASE("Arena checkpoint reuses inline storage", "[Arena]") {
	Arena arena;
	auto checkpoint = arena.checkpoint();
	auto* first = arena.allocate(64, alignof(std::max_align_t));

	arena.rewind(checkpoint);

	REQUIRE(arena.allocate(64, alignof(std::max_align_t)) == first);
}

TEST_CASE("Arena checkpoint reuses retained heap chunks", "[Arena]") {
	Arena arena;
	auto checkpoint = arena.checkpoint();
	auto* first = arena.allocate(Arena::INLINE_BUFFER_SIZE, alignof(std::max_align_t));
	const auto capacity = arena.capacity();

	arena.rewind(checkpoint);
	auto* reused = arena.allocate(Arena::INLINE_BUFFER_SIZE, alignof(std::max_align_t));

	REQUIRE(reused == first);
	REQUIRE(arena.capacity() == capacity);
}

TEST_CASE("Arena rewind destroys only post-checkpoint objects", "[Arena]") {
	Arena arena;
	size_t destructions = 0;
	[[maybe_unused]] auto* beforeCheckpoint = arena.create<DestructionProbe>(destructions);
	auto checkpoint = arena.checkpoint();
	[[maybe_unused]] auto* afterCheckpoint = arena.create<DestructionProbe>(destructions);

	arena.rewind(checkpoint);

	REQUIRE(destructions == 1);
	arena.softReset();
	REQUIRE(destructions == 2);
}

} // namespace nautilus::common
