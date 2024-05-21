#pragma once

#include "nautilus/tracing/tag/Tag.hpp"
#include <cinttypes>

namespace nautilus::tracing {

struct Snapshot {
public:
	Snapshot(Tag* tag, uint64_t staticValueHash);

	Snapshot();

	Snapshot(const Snapshot& snapshot) = default;

	Snapshot& operator=(const Snapshot& snapshot) = default;

	bool operator==(const Snapshot& rhs) const;

	bool operator!=(const Snapshot& rhs) const;

private:
	friend std::hash<Snapshot>;
	uint64_t staticValueHash;
	Tag* tag;
};
} // namespace nautilus::tracing

namespace std {
template <>
struct hash<nautilus::tracing::Snapshot> {
	size_t operator()(const nautilus::tracing::Snapshot& s) const;
};
} // namespace std
