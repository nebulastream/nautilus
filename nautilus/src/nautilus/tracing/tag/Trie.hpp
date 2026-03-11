
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
	explicit TrieNode() : content(0), parent_(nullptr) {
	}

	explicit TrieNode(T value) : content(value), parent_(nullptr) {
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
			auto& node = children.emplace_back(std::make_unique<TrieNode<T>>(value));
			node->parent_ = this;
			return node.get();
		} else {
			return found->get();
		}
	}

	/**
	 * @brief Walks parent chain to collect the full address sequence (leaf to root).
	 * Returns innermost address first, outermost last.
	 * Stops at root (parent_ == nullptr).
	 * @return std::vector<T> address chain from innermost to outermost
	 */
	std::vector<T> getAddressChain() const {
		std::vector<T> chain;
		for (auto* node = this; node->parent_ != nullptr; node = node->parent_) {
			chain.push_back(node->content);
		}
		return chain;
	}

private:
	T content;
	TrieNode<T>* parent_ = nullptr;
	std::vector<std::unique_ptr<TrieNode<T>>> children;
};
} // namespace nautilus::tracing
