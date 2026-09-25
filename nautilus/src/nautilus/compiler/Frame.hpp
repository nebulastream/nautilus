#pragma once

#include "nautilus/exceptions/RuntimeException.hpp"
#include <memory>
#include <unordered_map>
#include <utility>

namespace nautilus::compiler {

/**
 * @brief A simple frame abstraction for code generation.
 *
 * @tparam K key type
 * @tparam V value type
 * @tparam Alloc allocator for the map's nodes and buckets. Defaults to the
 *         heap; a lowering that creates one frame per block hands in a
 *         `common::ArenaAllocator` so all of them bump one arena instead.
 */
template <class K, class V, class Alloc = std::allocator<std::pair<const K, V>>>
class Frame {
public:
	using map_type = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, Alloc>;

	Frame() = default;

	explicit Frame(const Alloc& allocator) : frameMap(allocator) {
	}

	V& getValue(const K& key) {
		auto value = frameMap.find(key);
		if (value == frameMap.end()) {
			throw RuntimeException("Key " + key.toString() + " does not exists in frame.");
		}
		return value->second;
	}

	bool contains(const K& key) {
		return frameMap.contains(key);
	}

	void setValue(const K& key, const V& value) {
		frameMap.emplace(std::make_pair(key, value));
	}

	map_type& getContent() {
		return frameMap;
	}

private:
	map_type frameMap;
};

} // namespace nautilus::compiler
