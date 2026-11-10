
#pragma once

#include "Tag.hpp"
#include "nautilus/common/Arena.hpp"

namespace nautilus::tracing {

/**
 * @brief The tag recorder derives the tag from at a specific execution position.
 * To this end, it derives the backtrace, which is a unique identifier of an instruction.
 */
class TagRecorder final {
public:
	static constexpr size_t MAX_TAG_SIZE = 256;

	/**
	 * @brief Factory to create a new tag recorder.
	 * @param arena Arena to allocate the trie's nodes from. Must be the same arena that backs the
	 * ExecutionTrace the resulting tags are attached to (they are referenced by Operation::sourceTag
	 * and compared by pointer identity), so tag node addresses stay valid for as long as that trace
	 * -- not merely for the lifetime of this TagRecorder.
	 * @return TagRecorder
	 */
	static inline __attribute__((always_inline)) TagRecorder createTagRecorder(common::Arena& arena) {
		// First we derive the base address, which is the first common address between two bracktraces.
		auto referenceTag1 = createBaseTag();
		auto referenceTag2 = createBaseTag();
		auto baseAddress = getBaseAddress(referenceTag1, referenceTag2);
		return TagRecorder(baseAddress, arena);
	}

	/**
	 * @brief Derive the tag of a specific instruction and returns the tag pointer, which uniquely identifies this
	 * instruction.
	 * @return Tag*
	 */
	[[nodiscard]] inline __attribute__((always_inline)) Tag* createTag() {
		return this->createReferenceTag();
	}

	/**
	 * @brief Create a new tag recorder with a fixed start address.
	 * @param startAddress
	 * @param arena Arena to allocate the trie's nodes from (see @ref createTagRecorder for the lifetime
	 * requirement).
	 */
	explicit TagRecorder(TagAddress startAddress, common::Arena& arena);

private:
	static TagAddress getBaseAddress(TagVector& tag1, TagVector& tag2);

	static TagVector createBaseTag();

	Tag* createReferenceTag();
	Tag* createReferenceTagBacktrace();
	Tag* createReferenceTagBuildin();

	/// A direct-mapped cache from a walked stack to the trie node it ended in, so that the next operation from the same
	/// call site does not descend the trie frame by frame. An entry keeps the stack itself, which a hit compares, so a
	/// collision costs a descent and never yields a wrong tag. Stacks deeper than MAX_CACHED_DEPTH are not cached.
	/// The table is allocated from the arena once a recorder has made CACHE_AFTER tags, so small functions and
	/// regions do not pay for it.
	static constexpr size_t MAX_CACHED_DEPTH = 10;
	static constexpr size_t CACHE_WAYS = 4;
	static constexpr size_t CACHE_SETS = 16;
	static constexpr uint32_t CACHE_AFTER = 24;
	static constexpr size_t CACHE_PREFIX = 3;
	static uint64_t prefixKey(TagAddress a0, TagAddress a1, TagAddress a2) {
		return (((a0 * 0x9E3779B97F4A7C15ull) + a1) * 0xC2B2AE3D27D4EB4Full + a2) * 0xD6E8FEB86659FD93ull;
	}
	/// The node of the walked stack if the cache has it, else null.
	Tag* lookupCached(void** frame0) const;
	struct CacheEntry {
		uint64_t key;
		Tag* node;
		uint64_t depth;
		TagAddress stack[MAX_CACHED_DEPTH];
	};
	CacheEntry* cache = nullptr;
	uint32_t tagsCreated = 0;

	// The start address, which is used as the start to calculate tags for sub instructions.
	const TagAddress startAddress;
	// The tag recorder stores the individual tags in a trie of addresses, to minimize space consumption.
	Tag rootTagThreeNode = Tag();
	// Backing storage for trie nodes appended below the root; see the constructor's doc comment.
	common::Arena& arena;
	bool useBuiltinTagCreation;
};
} // namespace nautilus::tracing
