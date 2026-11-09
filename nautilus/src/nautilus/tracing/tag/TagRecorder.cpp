

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
#pragma GCC diagnostic ignored "-Wframe-address"
	auto** const frame0 = static_cast<void**>(__builtin_frame_address(0));
	// Look the stack up in the cache before descending the trie: consecutive operations of a traced function share
	// their whole stack but for a few frames, and a static loop revisits the same call sites again and again. The
	// three innermost return addresses pick the set; a hit is the entry whose stack is, frame for frame, the walked
	// one.
	if (cache != nullptr) {
		if (auto* hit = lookupCached(frame0)) {
			return hit;
		}
	}
	TagAddress addresses[MAX_TAG_SIZE + 1];
	size_t depth = 0;
	auto** frame = frame0;
	bool found = false;
	for (size_t i = 0; i <= MAX_TAG_SIZE && frame != nullptr; i++) {
		auto tagAddress = (TagAddress) frame[1];
		if (tagAddress == startAddress) {
			found = true;
			break;
		}
		addresses[depth++] = tagAddress;
		frame = static_cast<void**>(frame[0]);
	}
	if (!found) {
		throw TagCreationException("Stack is too deep. This could indicate the use "
		                           "of recursive control-flow,"
		                           " which is not supported in Nautilus code.");
	}
	if (cache == nullptr) {
		cache =
		    static_cast<CacheEntry*>(arena.allocate(CACHE_SETS * CACHE_WAYS * sizeof(CacheEntry), alignof(CacheEntry)));
		std::memset(static_cast<void*>(cache), 0, CACHE_SETS * CACHE_WAYS * sizeof(CacheEntry));
	}
	auto* currentTagNode = &rootTagThreeNode;
	for (size_t i = 0; i < depth; i++) {
		currentTagNode = currentTagNode->append(addresses[i], arena);
	}
	if (depth >= CACHE_PREFIX && depth <= MAX_CACHED_DEPTH) {
		const auto key = prefixKey(addresses[0], addresses[1], addresses[2]);
		auto* set = &cache[((key >> 32) & (CACHE_SETS - 1)) * CACHE_WAYS];
		// a free way, else the one the key picks: a set holds the stacks of the call sites that collide in it, so a
		// loop over a few more sites than ways does not make every operation descend the trie again
		size_t way = 0;
		while (way < CACHE_WAYS && set[way].node != nullptr) {
			++way;
		}
		auto* entry = &set[way < CACHE_WAYS ? way : (key >> 16) % CACHE_WAYS];
		entry->key = key;
		entry->node = currentTagNode;
		entry->depth = depth;
		std::memcpy(entry->stack, addresses, depth * sizeof(TagAddress));
	}
	return currentTagNode;
}

Tag* TagRecorder::lookupCached(void** frame0) const {
	auto** f = frame0;
	const auto a0 = static_cast<TagAddress>(reinterpret_cast<uintptr_t>(f[1]));
	f = static_cast<void**>(f[0]);
	if (a0 == startAddress || f == nullptr) {
		return nullptr;
	}
	const auto a1 = static_cast<TagAddress>(reinterpret_cast<uintptr_t>(f[1]));
	f = static_cast<void**>(f[0]);
	if (a1 == startAddress || f == nullptr) {
		return nullptr;
	}
	const auto a2 = static_cast<TagAddress>(reinterpret_cast<uintptr_t>(f[1]));
	auto** const rest = static_cast<void**>(f[0]);
	if (a2 == startAddress) {
		return nullptr;
	}
	const auto key = prefixKey(a0, a1, a2);
	const auto* set = &cache[((key >> 32) & (CACHE_SETS - 1)) * CACHE_WAYS];
	for (size_t way = 0; way < CACHE_WAYS; ++way) {
		const auto& entry = set[way];
		if (entry.key != key || entry.node == nullptr || entry.stack[0] != a0 || entry.stack[1] != a1 ||
		    entry.stack[2] != a2) {
			continue;
		}
		// the remaining frames must be those of the entry, and the stack must end where the entry's does
		auto** g = rest;
		size_t d = CACHE_PREFIX;
		for (;; ++d) {
			if (g == nullptr) {
				break;
			}
			const auto address = static_cast<TagAddress>(reinterpret_cast<uintptr_t>(g[1]));
			if (address == startAddress) {
				if (d == entry.depth) {
					return entry.node;
				}
				break;
			}
			if (d >= entry.depth || address != entry.stack[d]) {
				break;
			}
			g = static_cast<void**>(g[0]);
		}
	}
	return nullptr;
}

Tag* TagRecorder::createReferenceTag() {
	if (useBuiltinTagCreation) {
		return createReferenceTagBuildin();
	}
	return createReferenceTagBacktrace();
}

} // namespace nautilus::tracing
