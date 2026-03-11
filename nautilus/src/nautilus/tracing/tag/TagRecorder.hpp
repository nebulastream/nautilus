
#pragma once

#include "Tag.hpp"

namespace nautilus::tracing {

/**
 * @brief The tag recorder derives the tag from at a specific execution position.
 * To this end, it derives the backtrace, which is a unique identifier of an instruction.
 */
class TagRecorder final {
public:
	static constexpr size_t MAX_TAG_SIZE = 256;

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
	 */
	explicit TagRecorder(TagAddress startAddress);

private:
	Tag* createReferenceTag();
	Tag* createReferenceTagBacktrace();
	Tag* createReferenceTagBuildin();

	// The start address, which is used as the start to calculate tags for sub instructions.
	const TagAddress startAddress;
	// The tag recorder stores the individual tags in a trie of addresses, to minimize space consumption.
	Tag rootTagTrieNode = Tag();
	bool useBuiltinTagCreation;
};
} // namespace nautilus::tracing
