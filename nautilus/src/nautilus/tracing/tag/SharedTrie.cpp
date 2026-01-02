#include "SharedTrie.hpp"
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>

namespace nautilus::tracing {

// Thread-local cache for SharedTrie allocations
struct SharedTrieCache {
	void* memory = nullptr;
	size_t size = 0;
	pid_t creatorPid = 0;
};

static thread_local SharedTrieCache trieCache;

void* allocateSharedTrieMemory(size_t requiredSize) {
	void* memory = nullptr;

	// Try to reuse cached memory if it's large enough
	if (trieCache.memory != nullptr && trieCache.size >= requiredSize) {
		memory = trieCache.memory;
		// Mark as in-use by clearing cache
		trieCache.memory = nullptr;
		trieCache.size = 0;
	} else {
		// Free old cached memory if it exists but is too small
		if (trieCache.memory != nullptr) {
			munmap(trieCache.memory, trieCache.size);
			trieCache.memory = nullptr;
			trieCache.size = 0;
		}

		// Allocate new memory
		memory = mmap(nullptr, requiredSize, PROT_READ | PROT_WRITE,
		              MAP_SHARED | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
	}

	return memory;
}

void releaseSharedTrieMemory(void* memory, size_t size, pid_t creatorPid) {
	if (memory == nullptr) {
		return;
	}

	if (getpid() != creatorPid) {
		// Not the creator process, don't cache or free
		return;
	}

	// Cache this memory for reuse
	// If we already have cached memory, free the smaller one
	if (trieCache.memory != nullptr) {
		if (trieCache.size >= size) {
			// Keep the cached one, free the new one
			munmap(memory, size);
		} else {
			// Free the old cached one, cache the new larger one
			munmap(trieCache.memory, trieCache.size);
			trieCache.memory = memory;
			trieCache.size = size;
			trieCache.creatorPid = creatorPid;
		}
	} else {
		// No cached memory, cache this one
		trieCache.memory = memory;
		trieCache.size = size;
		trieCache.creatorPid = creatorPid;
	}
}

} // namespace nautilus::tracing