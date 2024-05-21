
#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>

namespace nautilus::tracing {

/**
 * @brief A basic trie data structure, which stores sequences of values.
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
		auto found = std::find_if(children.begin(), children.end(),
		                          [value](std::unique_ptr<TrieNode<T>>& x) { return x->content == value; });
		if (found == children.end()) {
			return children.emplace_back(std::make_unique<TrieNode<T>>(value)).get();
		} else {
			return found->get();
		}
	};

private:
	T content;
	std::vector<std::unique_ptr<TrieNode<T>>> children;
};
} // namespace nautilus::tracing
