
#pragma once

#include "nautilus/common/Arena.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>

namespace nautilus::tracing {

/**
 * @brief A basic trie data structure, which stores sequences of values.
 *
 * Nodes and child-pointer storage are allocated from a caller-supplied Arena rather than
 * individually via `make_unique`, so appending to the trie amortises to a bump-pointer
 * allocation instead of a `malloc` per node. Callers must pass the same Arena that backs the
 * data structure(s) the resulting node pointers end up referenced by (e.g. an ExecutionTrace's
 * Operations), so that node addresses -- which are handed out and compared/hashed by identity --
 * stay valid for as long as those references are needed, instead of only for the lifetime of the
 * trie's owner.
 *
 * @tparam T
 */
template <typename T>
class TrieNode {
public:
	explicit TrieNode() : content(0), parent(nullptr) {
	}

	explicit TrieNode(T value, TrieNode<T>* parent = nullptr) : content(value), parent(parent) {
	}

	/**
	 * @brief Appends a value to the trie and returns a ptr to the element in the node in the trie.
	 * @param value
	 * @param arena Arena to allocate the new node (and child-storage growth) from.
	 * @return TrieNode<T>*
	 */
	TrieNode<T>* append(T& value, common::Arena& arena) {
		if (table != nullptr) {
			return appendHashed(value, arena);
		}
		for (size_t i = 0; i < childCount; i++) {
			if (children[i]->content == value) {
				return children[i];
			}
		}
		auto* child = arena.createUnmanaged<TrieNode<T>>(value, this);
		if (childCount == MAX_LINEAR_CHILDREN) {
			// A node with many children (a function with many distinct call sites below one return address) would scan
			// them all on every tag: move them into an open-addressing table.
			buildTable(arena);
			insert(child);
			++childCount;
			return child;
		}
		if (childCount == childCapacity) {
			const size_t newCapacity = childCapacity == 0 ? 4 : childCapacity * 2;
			auto** newChildren =
			    static_cast<TrieNode<T>**>(arena.allocate(newCapacity * sizeof(TrieNode<T>*), alignof(TrieNode<T>*)));
			for (size_t i = 0; i < childCount; i++) {
				newChildren[i] = children[i];
			}
			children = newChildren;
			childCapacity = newCapacity;
		}
		children[childCount++] = child;
		return child;
	}

	[[nodiscard]] const T& getContent() const noexcept {
		return content;
	}

	/// Returns the parent node, or nullptr if this is the (sentinel-content) root.
	[[nodiscard]] const TrieNode<T>* getParent() const noexcept {
		return parent;
	}

private:
	/// Up to this many children are searched linearly; a node with more uses a hash table.
	static constexpr size_t MAX_LINEAR_CHILDREN = 8;

	static size_t hashOf(const T& value) noexcept {
		const uint64_t h = static_cast<uint64_t>(std::hash<T> {}(value)) * 0x9E3779B97F4A7C15ull;
		return static_cast<size_t>(h ^ (h >> 32));
	}

	TrieNode<T>* appendHashed(T& value, common::Arena& arena) {
		for (size_t slot = hashOf(value) & tableMask;; slot = (slot + 1) & tableMask) {
			auto* entry = table[slot];
			if (entry == nullptr) {
				break;
			}
			if (entry->content == value) {
				return entry;
			}
		}
		if ((childCount + 1) * 2 > tableMask + 1) {
			growTable(arena);
		}
		auto* child = arena.createUnmanaged<TrieNode<T>>(value, this);
		insert(child);
		++childCount;
		return child;
	}

	void insert(TrieNode<T>* child) {
		size_t slot = hashOf(child->content) & tableMask;
		while (table[slot] != nullptr) {
			slot = (slot + 1) & tableMask;
		}
		table[slot] = child;
	}

	void allocateTable(size_t capacity, common::Arena& arena) {
		table = static_cast<TrieNode<T>**>(arena.allocate(capacity * sizeof(TrieNode<T>*), alignof(TrieNode<T>*)));
		std::memset(static_cast<void*>(table), 0, capacity * sizeof(TrieNode<T>*));
		tableMask = capacity - 1;
	}

	void buildTable(common::Arena& arena) {
		auto** linear = children;
		allocateTable(4 * MAX_LINEAR_CHILDREN, arena);
		for (size_t i = 0; i < childCount; i++) {
			insert(linear[i]);
		}
		children = nullptr;
		childCapacity = 0;
	}

	void growTable(common::Arena& arena) {
		auto** old = table;
		const size_t oldCapacity = tableMask + 1;
		allocateTable(oldCapacity * 2, arena);
		for (size_t i = 0; i < oldCapacity; i++) {
			if (old[i] != nullptr) {
				insert(old[i]);
			}
		}
	}

	T content;
	// Arena-owned array of child pointers; never owns heap memory itself, so TrieNode stays
	// trivially destructible and can be allocated via Arena::createUnmanaged.
	TrieNode<T>** children = nullptr;
	size_t childCount = 0;
	size_t childCapacity = 0;
	// Open-addressing table of the children once there are more than MAX_LINEAR_CHILDREN; null before that.
	TrieNode<T>** table = nullptr;
	size_t tableMask = 0;
	TrieNode<T>* parent;
};
} // namespace nautilus::tracing
