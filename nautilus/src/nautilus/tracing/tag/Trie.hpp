
#pragma once

#include "nautilus/common/Arena.hpp"
#include <cstddef>
#include <cstdint>

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
		for (size_t i = 0; i < childCount; i++) {
			if (children[i]->content == value) {
				return children[i];
			}
		}
		auto* child = arena.createUnmanaged<TrieNode<T>>(value, this);
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
	T content;
	// Arena-owned array of child pointers; never owns heap memory itself, so TrieNode stays
	// trivially destructible and can be allocated via Arena::createUnmanaged.
	TrieNode<T>** children = nullptr;
	size_t childCount = 0;
	size_t childCapacity = 0;
	TrieNode<T>* parent;
};
} // namespace nautilus::tracing
