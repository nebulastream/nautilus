
#pragma once

#include <cstdint>
#include <cstring>
#include <functional>
#include <stdexcept>
#include <sys/mman.h>
#include <unistd.h>

namespace nautilus::tracing {

// Forward declarations for allocation functions (implemented in SharedHashMap.cpp)
void* allocateSharedHashMapMemory(size_t requiredSize);
void releaseSharedHashMapMemory(void* memory, size_t size, pid_t creatorPid);

/**
 * @brief Header for shared memory hash map.
 */
struct SharedHashMapHeader {
	uint32_t magic;
	uint32_t version;
	pid_t creatorPid;  // PID of process that created this map
	size_t memorySize; // Total size of shared memory region
	size_t capacity;   // Number of slots
	size_t size;       // Number of occupied slots

	static constexpr uint32_t MAGIC = 0x484D4150; // "HMAP"
	static constexpr uint32_t VERSION = 1;
};

/**
 * @brief A hash map entry. Empty slots have key with tagIndex == 0.
 */
template <typename K, typename V>
struct SharedHashMapEntry {
	K key;
	V value;
};

/**
 * @brief A fixed-size hash map backed by shared memory.
 *
 * Uses open addressing with linear probing.
 * Empty slots are identified by key having a sentinel value (zero-initialized).
 *
 * Memory layout:
 *   [SharedHashMapHeader][Entry[0]][Entry[1]]...
 *
 * Automatically allocates shared memory on creation and frees it
 * in the destructor (only in the process that created it).
 *
 * @tparam K Key type (must have a way to identify empty state via zero-init)
 * @tparam V Value type
 * @tparam Hash Hash function for K
 */
template <typename K, typename V, typename Hash = std::hash<K>>
class SharedHashMap {
public:
	using Entry = SharedHashMapEntry<K, V>;

	/**
	 * @brief Calculate required memory size for given capacity.
	 */
	static size_t requiredMemorySize(size_t capacity) {
		return sizeof(SharedHashMapHeader) + capacity * sizeof(Entry);
	}

	/**
	 * @brief Create a new hash map with shared memory allocation.
	 * Uses thread-local cache to avoid repeated mmap/munmap.
	 * @param capacity Number of slots
	 * @return SharedHashMap instance that owns the shared memory
	 */
	static SharedHashMap create(size_t capacity) {
		size_t memSize = requiredMemorySize(capacity);

		void* memory = allocateSharedHashMapMemory(memSize);
		if (memory == MAP_FAILED || memory == nullptr) {
			throw std::runtime_error("Failed to allocate shared memory for SharedHashMap");
		}

		auto* header = static_cast<SharedHashMapHeader*>(memory);
		header->magic = SharedHashMapHeader::MAGIC;
		header->version = SharedHashMapHeader::VERSION;
		header->creatorPid = getpid();
		header->memorySize = memSize;
		header->capacity = capacity;
		header->size = 0;

		auto* entries = reinterpret_cast<Entry*>(static_cast<char*>(memory) + sizeof(SharedHashMapHeader));

		// Zero-initialize all entries (makes keys empty)
		std::memset(entries, 0, capacity * sizeof(Entry));

		return SharedHashMap(header, entries);
	}

	// Delete copy operations (shared memory ownership)
	SharedHashMap(const SharedHashMap&) = delete;
	SharedHashMap& operator=(const SharedHashMap&) = delete;

	// Move operations
	SharedHashMap(SharedHashMap&& other) noexcept : header_(other.header_), entries_(other.entries_) {
		other.header_ = nullptr;
		other.entries_ = nullptr;
	}

	SharedHashMap& operator=(SharedHashMap&& other) noexcept {
		if (this != &other) {
			cleanup();
			header_ = other.header_;
			entries_ = other.entries_;
			other.header_ = nullptr;
			other.entries_ = nullptr;
		}
		return *this;
	}

	/**
	 * @brief Destructor. Frees shared memory only if called from creator process.
	 */
	~SharedHashMap() {
		cleanup();
	}

	/**
	 * @brief Insert or update a key-value pair.
	 * @param key The key (must not be empty/sentinel)
	 * @param value The value
	 * @return true if inserted new, false if updated existing
	 * @throws std::runtime_error if map is full
	 */
	bool insert(const K& key, const V& value) {
		if (header_->size >= header_->capacity) {
			throw std::runtime_error("SharedHashMap is full");
		}

		size_t idx = findSlot(key);
		bool isNew = isEmpty(entries_[idx].key);

		entries_[idx].key = key;
		entries_[idx].value = value;

		if (isNew) {
			header_->size++;
		}
		return isNew;
	}

	/**
	 * @brief Find a value by key.
	 * @param key The key to look up
	 * @return Pointer to value if found, nullptr otherwise
	 */
	V* find(const K& key) {
		size_t idx = hash_(key) % header_->capacity;
		size_t start = idx;

		do {
			if (isEmpty(entries_[idx].key)) {
				return nullptr; // Empty slot = key not found
			}
			if (entries_[idx].key == key) {
				return &entries_[idx].value;
			}
			idx = (idx + 1) % header_->capacity;
		} while (idx != start);

		return nullptr; // Wrapped around, not found
	}

	const V* find(const K& key) const {
		return const_cast<SharedHashMap*>(this)->find(key);
	}

	/**
	 * @brief Check if key exists.
	 */
	bool contains(const K& key) const {
		return find(key) != nullptr;
	}

	/**
	 * @brief Get or insert a value.
	 * If key exists, returns reference to existing value.
	 * If key doesn't exist, inserts default value and returns reference.
	 */
	V& operator[](const K& key) {
		size_t idx = findSlot(key);
		if (isEmpty(entries_[idx].key)) {
			if (header_->size >= header_->capacity) {
				throw std::runtime_error("SharedHashMap is full");
			}
			entries_[idx].key = key;
			entries_[idx].value = V {};
			header_->size++;
		}
		return entries_[idx].value;
	}

	size_t size() const {
		return header_->size;
	}

	size_t capacity() const {
		return header_->capacity;
	}

	/**
	 * @brief Clear all entries.
	 */
	void clear() {
		std::memset(entries_, 0, header_->capacity * sizeof(Entry));
		header_->size = 0;
	}

	/**
	 * @brief Get load factor (size / capacity).
	 */
	float loadFactor() const {
		return static_cast<float>(header_->size) / static_cast<float>(header_->capacity);
	}

private:
	SharedHashMap(SharedHashMapHeader* header, Entry* entries) : header_(header), entries_(entries) {
	}

	void cleanup() {
		if (header_ != nullptr) {
			// Return memory to cache (doesn't actually free it, keeps it for reuse)
			releaseSharedHashMapMemory(header_, header_->memorySize, header_->creatorPid);
		}
		header_ = nullptr;
		entries_ = nullptr;
	}

	/**
	 * @brief Check if a key represents an empty slot.
	 * Default: zero-initialized key is empty.
	 */
	static bool isEmpty(const K& key) {
		// For Snapshot: tagIndex == 0 means empty
		// This works because memset(0) creates keys with tagIndex == 0
		// Use memset to zero the entire struct including padding bytes
		K empty;
		std::memset(&empty, 0, sizeof(K));
		return std::memcmp(&key, &empty, sizeof(K)) == 0;
	}

	/**
	 * @brief Find slot for key (either existing or first empty).
	 */
	size_t findSlot(const K& key) {
		size_t idx = hash_(key) % header_->capacity;
		size_t start = idx;

		do {
			if (isEmpty(entries_[idx].key) || entries_[idx].key == key) {
				return idx;
			}
			idx = (idx + 1) % header_->capacity;
		} while (idx != start);

		// Should never happen if we check size before insert
		throw std::runtime_error("SharedHashMap is full (findSlot)");
	}

	SharedHashMapHeader* header_;
	Entry* entries_;
	Hash hash_;
};

} // namespace nautilus::tracing
