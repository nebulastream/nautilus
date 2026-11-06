#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <bit>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <memory>
#include <memory_resource>
#include <vector>

namespace nautilus::compiler::ir {

/**
 * @brief A map from operations to @p V for `FunctionRewriter`'s per-session
 * tables, with the part of `std::unordered_map`'s interface the rewriter uses.
 *
 * Every pass builds these tables for the whole function it rewrites, so their
 * construction is on the critical path of every IR pass. A node-based
 * `std::unordered_map` pays a modulo, a node allocation and a pointer chase
 * per insert and lookup; this table probes a flat array of (operation, index)
 * slots, linearly from a Fibonacci hash of the pointer, and deletes by
 * shifting the probe run back, so no tombstones accumulate.
 *
 * The values live apart from the slots, in a deque that only grows: a value
 * keeps its address while others are inserted, erased or rehashed, as with
 * `std::unordered_map`. An erased value stays in the deque until the table
 * goes away -- the same trade the rewriter's monotonic buffer already makes.
 * An `iterator` points at an entry: `it->first`, `it->second`, compared with
 * `end()`.
 */
template <typename V>
class OperationTable {
public:
	struct Entry {
		const Operation* first;
		V second;
	};
	using iterator = Entry*;
	using const_iterator = const Entry*;

	explicit OperationTable(std::pmr::memory_resource* resource)
	    : resource_(resource), slots_(resource), entries_(resource) {
	}

	void reserve(size_t count) {
		size_t capacity = 16;
		while (capacity * MAX_LOAD_NUMERATOR < count * MAX_LOAD_DENOMINATOR) {
			capacity *= 2;
		}
		if (capacity > slots_.size()) {
			rehash(capacity);
		}
	}

	[[nodiscard]] iterator end() {
		return nullptr;
	}
	[[nodiscard]] const_iterator end() const {
		return nullptr;
	}

	[[nodiscard]] iterator find(const Operation* key) {
		const auto slot = findSlot(key);
		return slot == NOT_FOUND ? nullptr : &entries_[slots_[slot].entry];
	}
	[[nodiscard]] const_iterator find(const Operation* key) const {
		const auto slot = findSlot(key);
		return slot == NOT_FOUND ? nullptr : &entries_[slots_[slot].entry];
	}

	[[nodiscard]] bool contains(const Operation* key) const {
		return findSlot(key) != NOT_FOUND;
	}

	V& at(const Operation* key) {
		auto it = find(key);
		if (it == nullptr) {
			throw RuntimeException("OperationTable::at: operation is not in the table");
		}
		return it->second;
	}

	V& operator[](const Operation* key) {
		if ((size_ + 1) * MAX_LOAD_DENOMINATOR > slots_.size() * MAX_LOAD_NUMERATOR) {
			rehash(slots_.empty() ? 16 : slots_.size() * 2);
		}
		size_t slot = home(key);
		while (slots_[slot].key != nullptr) {
			if (slots_[slot].key == key) {
				return entries_[slots_[slot].entry].second;
			}
			slot = (slot + 1) & mask_;
		}
		if constexpr (std::uses_allocator_v<V, std::pmr::polymorphic_allocator<std::byte>>) {
			entries_.push_back(Entry {key, V(resource_)});
		} else {
			entries_.push_back(Entry {key, V {}});
		}
		slots_[slot] = Slot {key, static_cast<uint32_t>(entries_.size() - 1)};
		++size_;
		return entries_.back().second;
	}

	bool erase(const Operation* key) {
		auto slot = findSlot(key);
		if (slot == NOT_FOUND) {
			return false;
		}
		// Backward-shift deletion: pull every later member of the probe run
		// whose home does not lie in (slot, next] into the hole.
		auto next = slot;
		while (true) {
			next = (next + 1) & mask_;
			if (slots_[next].key == nullptr) {
				break;
			}
			const auto wanted = home(slots_[next].key);
			const bool stays = slot <= next ? (slot < wanted && wanted <= next) : (slot < wanted || wanted <= next);
			if (!stays) {
				slots_[slot] = slots_[next];
				slot = next;
			}
		}
		slots_[slot] = Slot {};
		--size_;
		return true;
	}
	bool erase(iterator it) {
		return it != nullptr && erase(it->first);
	}

	[[nodiscard]] size_t size() const {
		return size_;
	}

private:
	struct Slot {
		const Operation* key = nullptr;
		uint32_t entry = 0;
	};

	static constexpr size_t NOT_FOUND = ~size_t {0};
	// Grow past a load factor of 1/2: linear probing stays short below it.
	static constexpr size_t MAX_LOAD_NUMERATOR = 1;
	static constexpr size_t MAX_LOAD_DENOMINATOR = 2;

	[[nodiscard]] size_t home(const Operation* key) const {
		// Arena objects are at least 8-byte aligned; the multiply spreads the
		// remaining bits, and the top bits index the power-of-two table.
		const auto bits = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(key)) * 0x9E3779B97F4A7C15ull;
		return static_cast<size_t>(bits >> shift_);
	}

	[[nodiscard]] size_t findSlot(const Operation* key) const {
		if (size_ == 0 || key == nullptr) {
			return NOT_FOUND;
		}
		size_t slot = home(key);
		while (slots_[slot].key != nullptr) {
			if (slots_[slot].key == key) {
				return slot;
			}
			slot = (slot + 1) & mask_;
		}
		return NOT_FOUND;
	}

	void rehash(size_t capacity) {
		std::pmr::vector<Slot> old(std::move(slots_), resource_);
		slots_ = std::pmr::vector<Slot>(capacity, resource_);
		mask_ = capacity - 1;
		shift_ = 64 - std::countr_zero(capacity);
		for (const auto& slot : old) {
			if (slot.key == nullptr) {
				continue;
			}
			auto index = home(slot.key);
			while (slots_[index].key != nullptr) {
				index = (index + 1) & mask_;
			}
			slots_[index] = slot;
		}
	}

	std::pmr::memory_resource* resource_;
	std::pmr::vector<Slot> slots_;
	std::pmr::deque<Entry> entries_;
	size_t size_ = 0;
	size_t mask_ = 0;
	int shift_ = 64;
};

} // namespace nautilus::compiler::ir
