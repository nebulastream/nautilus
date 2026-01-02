
#pragma once

#include "SharedTrie.hpp"
#include "Tag.hpp"
#include <sys/mman.h>

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
	 * @return TagRecorder
	 */
	static inline __attribute__((always_inline)) TagRecorder createTagRecorder() {
		// First we derive the base address, which is the first common address between two bracktraces.
		auto referenceTag1 = createBaseTag();
		auto referenceTag2 = createBaseTag();
		auto baseAddress = getBaseAddress(referenceTag1, referenceTag2);
		return TagRecorder(baseAddress);
	}

	/**
	 * @brief Derive the tag of a specific instruction and returns the tag pointer, which uniquely identifies this
	 * instruction.
	 * @return Tag*
	 */
	[[nodiscard]] inline __attribute__((always_inline)) Tag createTag() {
		return this->createReferenceTag();
	}

	/**
	 * @brief Create a new tag recorder with a fixed start address.
	 * @param startAddress
	 */
	explicit TagRecorder(TagAddress startAddress);

private:
	static TagAddress getBaseAddress(TagVector& tag1, TagVector& tag2);

	static TagVector createBaseTag();

	TrieIndex createReferenceTag();
	TrieIndex createReferenceTagBacktrace();
	TrieIndex createReferenceTagBuildin();

	// The start address, which is used as the start to calculate tags for sub instructions.
	const TagAddress startAddress;
	// The tag recorder stores the individual tags in a trie of addresses, to minimize space consumption.
	SharedTrie<TagAddress> trie = SharedTrie<TagAddress>::create(1024 * 1024);
	Tag rootTagThreeNode = trie.root();
	bool useBuiltinTagCreation;
};
} // namespace nautilus::tracing
