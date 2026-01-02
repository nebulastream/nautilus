
#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>
#include <nautilus/exceptions/RuntimeException.hpp>
#include <optional>
#include <span>
#include <stdexcept>
#include <sys/mman.h>
#include <unistd.h>

namespace nautilus::tracing {

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

template <typename K, typename V, typename Hash = std::hash<K>>
struct Map {
	struct Entry {
		K key;
		V value;
	};

	struct MapHeader {
		size_t magic = 0xDEADBEEF;
		size_t max_capacity;
		size_t capacity;
		size_t number_of_entries;

		MapHeader(size_t max_capacity, size_t capacity, size_t number_of_entries)
		    : max_capacity(max_capacity), capacity(capacity), number_of_entries(number_of_entries) {
		}
	};

	Map(const Map& other) = delete;
	Map(Map&& other) noexcept {
		this->header = other.header;
		other.header = nullptr;
	}

	Map& operator=(const Map& other) = delete;
	Map& operator=(Map&& other) noexcept {
		if (this == &other)
			return *this;

		if (this->header) {
			auto max_capacity = this->header->max_capacity;
			this->header->~MapHeader();
			munmap(this->header, max_capacity * sizeof(Entry) + sizeof(MapHeader));
		}
		this->header = other.header;
		other.header = nullptr;

		return *this;
	}

	size_t size() const {
		return this->header->number_of_entries;
	}

	size_t capacity() const {
		return this->header->max_capacity;
	}

	explicit Map(size_t max_capacity) {
		if (max_capacity == 0) {
			throw RuntimeException("Max Capacity must be greater than 0");
		}
		if ((max_capacity & (max_capacity - 1)) != 0) {
			throw RuntimeException("Max Capacity needs to be a power of 2");
		}
		size_t initial_capacity = std::min<size_t>(512, max_capacity);

		void* memory = mmap(nullptr, max_capacity * sizeof(Entry) + sizeof(MapHeader), PROT_READ | PROT_WRITE,
		                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		if (memory == MAP_FAILED) {
			throw std::runtime_error("Failed to allocate shared memory for SharedHashMap");
		}
		this->header = new (memory) MapHeader(max_capacity, initial_capacity, 0);
		msync(memory, sizeof(MapHeader), MS_SYNC);
	}

	~Map() {
		if (this->header) {
			auto max_capacity = this->header->max_capacity;
			this->header->~MapHeader();
			munmap(this->header, max_capacity * sizeof(Entry) + sizeof(MapHeader));
		}
	}

	std::span<Entry> entries() {
		return std::span(reinterpret_cast<Entry*>(header + 1), header->capacity);
	}

	int mask() {
		return header->capacity - 1;
	}

	void grow() {
		if (header->capacity * 2 > header->max_capacity) {
			return;
		}

		auto old_slots = entries();
		header->capacity *= 2;

		auto tmp = std::make_unique<Entry[]>(old_slots.size());
		::memcpy(tmp.get(), old_slots.data(), old_slots.size() * sizeof(Entry));
		::memset(old_slots.data(), 0, old_slots.size() * sizeof(Entry));
		old_slots = std::span<Entry> {tmp.get(), old_slots.size()};

		header->number_of_entries = 0;
		for (auto& [key, value] : old_slots) {
			if (key == K {}) {
				continue;
			}
			insert(key, value);
		}
	}

	bool insert(K key, V value) {
		assert(key != K {} && "Inserted null element");
		if ((100 * header->number_of_entries) / header->capacity > 90) {
			grow();
		}

		if (header->number_of_entries >= header->capacity) {
			throw RuntimeException("HashMap is full");
		}

		auto slots = entries();
		auto position = Hash {}(key) &mask();
		auto current_psl = 0;

		while (true) {
			if (slots[position].key == K {}) {
				slots[position] = {key, value};
				++header->number_of_entries;
				return true;
			}

			if (slots[position].key == key) {
				slots[position] = {key, value};
				return false;
			}

			int psl = (position - (Hash {}(slots[position].key) & mask())) & mask();
			if (psl < current_psl) {
				std::swap(key, slots[position].key);
				std::swap(value, slots[position].value);
				current_psl = psl;
			}

			position = position + 1;
			position &= mask();
			current_psl++;
		}
	}

	bool contains(const K& key) const {
		return get(key) != nullptr;
	}

	const V* get(const K& key) const {
		return const_cast<Map*>(this)->get(key);
	}

	V* get(const K& key) {
		assert(key != K {} && "Inserted null element");

		auto slots = entries();
		auto position = Hash {}(key) &mask();
		auto current_psl = 0;

		while (true) {
			if (slots[position].key == key) {
				return &slots[position].value;
			}
			if (slots[position].key == K {}) {
				return nullptr;
			}

			int entry_psl = (position - (Hash {}(slots[position].key) & mask())) & mask();

			// If we have searched longer than the occupant of this slot ever did,
			// then our key definitely isn't here (it would have been swapped).
			if (current_psl > entry_psl) {
				return nullptr;
			}

			current_psl++;

			position += 1;
			position &= mask();
		}

		__builtin_unreachable();
	}

	MapHeader* header;
};

} // namespace nautilus::tracing
