
#pragma once

#include "Trie.hpp"

namespace nautilus::tracing {

/**
 * @brief The tag address references a function on the callstack.
 */
using TagAddress = uint64_t;
using Tag = TrieNode<TagAddress>;

} // namespace nautilus::tracing
