
#pragma once

#include "Trie.hpp"
#include <memory>
#include <ostream>
#include <vector>

namespace nautilus::tracing {

/**
 * @brief The tag address references a function on the callstack.
 */
using TagAddress = uint64_t;
using Tag = TrieNode<TagAddress>;

/**
 * @brief The tag identifies a specific executed operation in the interpreter.
 * It is represented by a list of all stack frame addresses between the operation and the execution root.
 */
class TagVector {
public:
	/**
	 * @brief Constructor to create a new tag.
	 * @param addresses
	 */
	TagVector(const std::vector<TagAddress>& addresses);

	friend std::ostream& operator<<(std::ostream& os, const TagVector& tag);

	const std::vector<TagAddress>& getAddresses();

private:
	std::vector<TagAddress> addresses;
};

} // namespace nautilus::tracing
