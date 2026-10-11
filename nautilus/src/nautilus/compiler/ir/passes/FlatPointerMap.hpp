#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace nautilus::compiler::ir {

/**
 * @brief A hash map from IR objects (operations, blocks) to values, for the
 * short-lived tables the IR passes and analyses build per run.
 *
 * Open addressing with linear probing over one array: no per-entry
 * allocation, and a lookup touches one or two cache lines where a
 * `std::unordered_map` chases a bucket and a node. Keys are pointers and
 * never null (null marks a free slot). Erasing shifts the following run of
 * the probe sequence back instead of leaving a tombstone, so lookups stay
 * short however many entries come and go. No iteration order is defined, so
 * none is offered.
 */
template <typename K, typename V>
class FlatPointerMap {
public:
	explicit FlatPointerMap(size_t expected = 0) {
		reserve(expected);
	}

	void reserve(size_t expected) {
		size_t capacity = 16;
		while (capacity < expected * 2) {
			capacity *= 2;
		}
		if (capacity > slots_.size()) {
			rehash(capacity);
		}
	}

	[[nodiscard]] V* find(const K* key) {
		const size_t slot = slotOf(key);
		return slots_[slot].key == key ? &slots_[slot].value : nullptr;
	}

	[[nodiscard]] const V* find(const K* key) const {
		const size_t slot = slotOf(key);
		return slots_[slot].key == key ? &slots_[slot].value : nullptr;
	}

	[[nodiscard]] bool contains(const K* key) const {
		return find(key) != nullptr;
	}

	/// The value for @p key, value-initialized and inserted if absent; the
	/// flag tells which.
	std::pair<V*, bool> tryEmplace(const K* key) {
		if ((size_ + 1) * 2 > slots_.size()) {
			rehash(slots_.size() * 2);
		}
		const size_t slot = slotOf(key);
		if (slots_[slot].key == key) {
			return {&slots_[slot].value, false};
		}
		slots_[slot].key = key;
		slots_[slot].value = V {};
		++size_;
		return {&slots_[slot].value, true};
	}

	V& operator[](const K* key) {
		return *tryEmplace(key).first;
	}

	bool erase(const K* key) {
		size_t hole = slotOf(key);
		if (slots_[hole].key != key) {
			return false;
		}
		// Backward-shift deletion: move later entries of the same probe run
		// into the hole unless that would put them before their home slot.
		for (size_t next = (hole + 1) & mask(); slots_[next].key != nullptr; next = (next + 1) & mask()) {
			const size_t home = homeOf(slots_[next].key);
			const bool homeInHoleToNext = hole <= next ? (hole < home && home <= next) : (hole < home || home <= next);
			if (!homeInHoleToNext) {
				slots_[hole] = std::move(slots_[next]);
				hole = next;
			}
		}
		slots_[hole].key = nullptr;
		slots_[hole].value = V {};
		--size_;
		return true;
	}

	[[nodiscard]] size_t size() const {
		return size_;
	}

private:
	struct Slot {
		const K* key = nullptr;
		V value {};
	};

	[[nodiscard]] size_t mask() const {
		return slots_.size() - 1;
	}

	[[nodiscard]] size_t homeOf(const K* key) const {
		// Heap objects are at least 8-byte aligned: drop the always-zero bits,
		// spread the rest with a multiply and fold the high half back down
		// before masking.
		const auto hash = (reinterpret_cast<uintptr_t>(key) >> 3) * 0x9E3779B97F4A7C15ull;
		return static_cast<size_t>(hash ^ (hash >> 32)) & mask();
	}

	/// The slot holding @p key, or the free slot where it would go.
	[[nodiscard]] size_t slotOf(const K* key) const {
		size_t slot = homeOf(key);
		while (slots_[slot].key != nullptr && slots_[slot].key != key) {
			slot = (slot + 1) & mask();
		}
		return slot;
	}

	void rehash(size_t capacity) {
		std::vector<Slot> old(capacity);
		old.swap(slots_);
		for (auto& slot : old) {
			if (slot.key != nullptr) {
				slots_[slotOf(slot.key)] = std::move(slot);
			}
		}
	}

	std::vector<Slot> slots_;
	size_t size_ = 0;
};

/// The key-only counterpart of `FlatPointerMap`.
template <typename K>
class FlatPointerSet {
public:
	explicit FlatPointerSet(size_t expected = 0) : map_(expected) {
	}

	/// True if @p key was not yet in the set.
	bool insert(const K* key) {
		return map_.tryEmplace(key).second;
	}
	[[nodiscard]] bool contains(const K* key) const {
		return map_.contains(key);
	}
	bool erase(const K* key) {
		return map_.erase(key);
	}
	[[nodiscard]] size_t size() const {
		return map_.size();
	}

private:
	struct Empty {};
	FlatPointerMap<K, Empty> map_;
};

} // namespace nautilus::compiler::ir
