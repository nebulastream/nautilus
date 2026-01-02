#pragma once

#include "nautilus/tracing/tag/SharedTrie.hpp"
#include "nautilus/tracing/tag/Tag.hpp"
#include <cinttypes>

namespace nautilus::tracing {

struct Snapshot {
public:
	Snapshot(TrieIndex tag, uint64_t staticValueHash);

	Snapshot();

	Snapshot(const Snapshot& snapshot) = default;

	Snapshot& operator=(const Snapshot& snapshot) = default;

	bool operator==(const Snapshot& rhs) const;

	bool operator!=(const Snapshot& rhs) const;

private:
	friend std::hash<Snapshot>;
	uint64_t staticValueHash = 0;
	TrieIndex tag = 0;
};
} // namespace nautilus::tracing

namespace std {
template <>
struct hash<nautilus::tracing::Snapshot> {
	size_t operator()(const nautilus::tracing::Snapshot& s) const;
};
} // namespace std
