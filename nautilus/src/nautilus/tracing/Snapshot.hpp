#pragma once

#include "nautilus/tracing/tag/Tag.hpp"
#include <cinttypes>

namespace nautilus::tracing {

struct Snapshot {
public:
	Snapshot(Tag* tag, uint64_t staticValueHash, uint64_t aliveHash = 0);

	Snapshot();

	Snapshot(const Snapshot& snapshot) = default;

	Snapshot& operator=(const Snapshot& snapshot) = default;

	bool operator==(const Snapshot& rhs) const;

	bool operator!=(const Snapshot& rhs) const;

	[[nodiscard]] const Tag* getTag() const {
		return tag;
	}

	[[nodiscard]] uint64_t getAliveHash() const {
		return aliveHash;
	}

private:
	friend std::hash<Snapshot>;
	uint64_t staticValueHash;
	Tag* tag;
	uint64_t aliveHash;
};
} // namespace nautilus::tracing

namespace std {
template <>
struct hash<nautilus::tracing::Snapshot> {
	size_t operator()(const nautilus::tracing::Snapshot& s) const;
};
} // namespace std
