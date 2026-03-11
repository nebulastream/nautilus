
#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>

namespace nautilus::tracing {

/**
 * @brief A trie node that stores sequences of values.
 * Uses an unordered_map for O(1) average child lookup instead of linear scan.
 * @tparam T
 */
template <typename T>
class TrieNode {
public:
	explicit TrieNode() : content(0) {
	}

	explicit TrieNode(T value) : content(value) {
	}

	/**
	 * @brief Appends a value to the trie and returns a ptr to the element in the node in the trie.
	 * @param value
	 * @return TrieNode<T>*
	 */
	TrieNode<T>* append(T& value) {
		auto [it, inserted] = children.try_emplace(value, nullptr);
		if (inserted) {
			it->second = std::make_unique<TrieNode<T>>(value);
		}
		return it->second.get();
	}

private:
	T content;
	std::unordered_map<T, std::unique_ptr<TrieNode<T>>> children;
};
} // namespace nautilus::tracing
