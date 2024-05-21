
#pragma once

#include "nautilus/exceptions/RuntimeException.hpp"
#include <unordered_map>

namespace nautilus::compiler {

/**
 * @brief A simple frame abstraction for code generation.
 * @tparam K key type
 * @tparam V value type
 */
template <class K, class V>
class Frame {
public:
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

	std::unordered_map<K, V>& getContent() {
		return frameMap;
	}

private:
	std::unordered_map<K, V> frameMap;
};

} // namespace nautilus::compiler
