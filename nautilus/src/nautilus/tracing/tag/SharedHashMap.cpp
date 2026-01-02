#include "SharedHashMap.hpp"
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>

namespace nautilus::tracing {

// Thread-local cache for SharedHashMap allocations
struct SharedHashMapCache {
	void* memory = nullptr;
	size_t size = 0;
	pid_t creatorPid = 0;
};

static thread_local SharedHashMapCache hashMapCache;

void* allocateSharedHashMapMemory(size_t requiredSize) {
	void* memory = nullptr;

	// Try to reuse cached memory if it's large enough
	if (hashMapCache.memory != nullptr && hashMapCache.size >= requiredSize) {
		memory = hashMapCache.memory;
		// HashMap requires memset to clear old keys for proper empty slot detection
		std::memset(memory, 0, requiredSize);
		// Mark as in-use by clearing cache
		hashMapCache.memory = nullptr;
		hashMapCache.size = 0;
	} else {
		// Free old cached memory if it exists but is too small
		if (hashMapCache.memory != nullptr) {
			munmap(hashMapCache.memory, hashMapCache.size);
			hashMapCache.memory = nullptr;
			hashMapCache.size = 0;
		}

		// Allocate new memory
		memory = mmap(nullptr, requiredSize, PROT_READ | PROT_WRITE,
		              MAP_SHARED | MAP_ANONYMOUS | MAP_POPULATE, -1, 0);
	}

	return memory;
}

void releaseSharedHashMapMemory(void* memory, size_t size, pid_t creatorPid) {
	if (memory == nullptr) {
		return;
	}

	if (getpid() != creatorPid) {
		// Not the creator process, don't cache or free
		return;
	}

	// Cache this memory for reuse
	// If we already have cached memory, free the smaller one
	if (hashMapCache.memory != nullptr) {
		if (hashMapCache.size >= size) {
			// Keep the cached one, free the new one
			munmap(memory, size);
		} else {
			// Free the old cached one, cache the new larger one
			munmap(hashMapCache.memory, hashMapCache.size);
			hashMapCache.memory = memory;
			hashMapCache.size = size;
			hashMapCache.creatorPid = creatorPid;
		}
	} else {
		// No cached memory, cache this one
		hashMapCache.memory = memory;
		hashMapCache.size = size;
		hashMapCache.creatorPid = creatorPid;
	}
}

} // namespace nautilus::tracing