
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
	[[nodiscard]] inline __attribute__((always_inline)) Tag* createTag() {
		return this->createReferenceTag();
	}

	/**
	 * @brief Create a new tag recorder with a fixed start address.
	 * @param startAddress
	 */
	explicit TagRecorder(TagAddress startAddress);

	/**
	 * @brief Re-interns a tag serialized as its root-first address path into this
	 * recorder's trie. Used by the fork tracer to transfer tags between processes;
	 * paths over addresses already present yield the existing nodes, so tag identity
	 * is preserved across the handoff.
	 */
	Tag* internTagPath(const TagAddress* addresses, size_t count);

	/// Root node of this recorder's tag trie. Serialized payloads refer to it as
	/// node id 0 (see serialization::SnapshotEncoder).
	Tag* rootTag() noexcept;

	/// Single interning step: returns the child of @p parent carrying @p address,
	/// creating it if absent. Building tags edge-by-edge lets the fork tracer's
	/// serialization intern each unique trie node once per payload instead of
	/// re-walking every operation's full root path.
	Tag* internTagStep(Tag* parent, TagAddress address);

private:
	static TagAddress getBaseAddress(TagVector& tag1, TagVector& tag2);

	static TagVector createBaseTag();

	Tag* createReferenceTag();
	Tag* createReferenceTagBacktrace();
	Tag* createReferenceTagBuildin();

	// The start address, which is used as the start to calculate tags for sub instructions.
	const TagAddress startAddress;
	// The tag recorder stores the individual tags in a trie of addresses, to minimize space consumption.
	Tag rootTagThreeNode = Tag();
	bool useBuiltinTagCreation;
};
} // namespace nautilus::tracing
