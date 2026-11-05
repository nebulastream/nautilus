

#include "TagRecorder.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/tracing/exceptions/TagCreationException.hpp"
#include <cstring>
#include <execinfo.h>

namespace nautilus::tracing {

#pragma GCC diagnostic ignored "-Wframe-address"

TagRecorder::TagRecorder(TagAddress startAddress, common::Arena& arena) : startAddress(startAddress), arena(arena) {
	useBuiltinTagCreation = __builtin_return_address(1) != nullptr;
}

// check if gnu backtrace is available.
#if defined(BACKWARD_HAS_BACKTRACE) & !defined(HOST_IS_MUSL)
TagVector TagRecorder::createBaseTag() {
	void* tagBuffer[MAX_TAG_SIZE];
	int size = backtrace(tagBuffer, MAX_TAG_SIZE);
	std::vector<TagAddress> addresses;
	for (int i = 0; i < size; i++) {
		addresses.emplace_back((TagAddress) tagBuffer[i]);
	}
	return {addresses};
}
#else

// Walks the frame-pointer chain in a single linear pass; see the comment on createReferenceTagBuildin
// below for why this replaces resolving each frame independently via __builtin_return_address(N).
__attribute__((noinline)) TagVector TagRecorder::createBaseTag() {
	std::vector<TagAddress> addresses;
	auto** frame = static_cast<void**>(__builtin_frame_address(0));
	for (size_t i = 0; i < MAX_TAG_SIZE && frame != nullptr; i++) {
		addresses.emplace_back((TagAddress) frame[1]);
		frame = static_cast<void**>(frame[0]);
	}
	return addresses;
}

#endif

TagAddress TagRecorder::getBaseAddress(TagVector& tag1, TagVector& tag2) {
	auto& tag1Addresses = tag1.getAddresses();
	auto& tag2Addresses = tag2.getAddresses();
	auto size = std::min(tag1Addresses.size(), tag2Addresses.size());
	size_t index = 0;
	while (index < size && tag1Addresses[index] == tag2Addresses[index]) {
		index++;
	}
	while (index < size && tag1Addresses[index] != tag2Addresses[index]) {
		index++;
	}
	if (index >= size) {
		return -1;
	}
	return tag1Addresses[index];
}

Tag* TagRecorder::createReferenceTagBacktrace() {
	auto* currentTagNode = &rootTagThreeNode;
	void* tagBuffer[MAX_TAG_SIZE];
	int size = backtrace(tagBuffer, MAX_TAG_SIZE);
	for (int i = 0; i < size; i++) {
		auto tagAddress = (TagAddress) tagBuffer[i];
		if (tagAddress == startAddress) {
			return currentTagNode;
		}
		currentTagNode = currentTagNode->append(tagAddress, arena);
	}
	throw TagCreationException("Stack is too deep. This could indicate the use "
	                           "of recursive control-flow,"
	                           " which is not supported in Nautilus code.");
}

// Walks the frame-pointer chain in a single linear pass instead of resolving each frame's return
// address independently via __builtin_return_address(N), which re-walks N frames from the top every
// call and makes a loop over depth 0..d cost O(d^2) frame dereferences in total (see issue #426).
// This relies on the frame-pointer chain being intact, which the project guarantees globally via
// -fno-omit-frame-pointer. The layout (frame[0] = saved/previous frame pointer, frame[1] = return
// address) matches the frame-record convention used by both the x86-64 SysV and AArch64 AAPCS64 ABIs.
__attribute__((noinline)) Tag* TagRecorder::createReferenceTagBuildin() {
	if (cache == nullptr && tagsCreated + 1 < CACHE_AFTER) {
		// the first tags of a recorder: a function or region of a few operations descends the trie directly
		++tagsCreated;
		auto* node = &rootTagThreeNode;
#pragma GCC diagnostic ignored "-Wframe-address"
		auto** frames = static_cast<void**>(__builtin_frame_address(0));
		for (size_t i = 0; i <= MAX_TAG_SIZE && frames != nullptr; i++) {
			auto tagAddress = (TagAddress) frames[1];
			if (tagAddress == startAddress) {
				return node;
			}
			node = node->append(tagAddress, arena);
			frames = static_cast<void**>(frames[0]);
		}
		throw TagCreationException("Stack is too deep. This could indicate the use "
		                           "of recursive control-flow,"
		                           " which is not supported in Nautilus code.");
	}
	// Walk the frames first, into a small array, and look the stack up before descending the trie: consecutive
	// operations of a traced function share their whole stack but for a few frames, and a static loop revisits the
	// same call sites again and again.
	TagAddress addresses[MAX_TAG_SIZE + 1];
	size_t depth = 0;
	uint64_t hash = 0x9E3779B97F4A7C15ull;
#pragma GCC diagnostic ignored "-Wframe-address"
	auto** frame = static_cast<void**>(__builtin_frame_address(0));
	bool found = false;
	for (size_t i = 0; i <= MAX_TAG_SIZE && frame != nullptr; i++) {
		auto tagAddress = (TagAddress) frame[1];
		if (tagAddress == startAddress) {
			found = true;
			break;
		}
		addresses[depth++] = tagAddress;
		hash = (hash + tagAddress) * 0xC2B2AE3D27D4EB4Full;
		frame = static_cast<void**>(frame[0]);
	}
	if (!found) {
		throw TagCreationException("Stack is too deep. This could indicate the use "
		                           "of recursive control-flow,"
		                           " which is not supported in Nautilus code.");
	}
	if (cache == nullptr) {
		cache = static_cast<CacheEntry*>(arena.allocate(CACHE_SIZE * sizeof(CacheEntry), alignof(CacheEntry)));
		std::memset(static_cast<void*>(cache), 0, CACHE_SIZE * sizeof(CacheEntry));
	}
	CacheEntry* entry = nullptr;
	if (cache != nullptr && depth <= MAX_CACHED_DEPTH) {
		entry = &cache[(hash >> 32) & (CACHE_SIZE - 1)];
		if (entry->node != nullptr && entry->hash == hash && entry->depth == depth &&
		    std::memcmp(entry->stack, addresses, depth * sizeof(TagAddress)) == 0) {
			return entry->node;
		}
	}
	auto* currentTagNode = &rootTagThreeNode;
	for (size_t i = 0; i < depth; i++) {
		currentTagNode = currentTagNode->append(addresses[i], arena);
	}
	if (entry != nullptr) {
		entry->hash = hash;
		entry->node = currentTagNode;
		entry->depth = depth;
		std::memcpy(entry->stack, addresses, depth * sizeof(TagAddress));
	}
	return currentTagNode;
}

Tag* TagRecorder::createReferenceTag() {
	if (useBuiltinTagCreation) {
		return createReferenceTagBuildin();
	}
	return createReferenceTagBacktrace();
}

} // namespace nautilus::tracing
