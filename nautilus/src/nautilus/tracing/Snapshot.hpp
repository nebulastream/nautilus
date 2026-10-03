#pragma once

#include "nautilus/tracing/tag/Tag.hpp"
#include <cinttypes>

namespace nautilus::tracing {

struct Snapshot {
public:
	Snapshot(Tag* tag, uint64_t staticValueHash, uint64_t divergence = 0);

	Snapshot();

	Snapshot(const Snapshot& snapshot) = default;

	Snapshot& operator=(const Snapshot& snapshot) = default;

	bool operator==(const Snapshot& rhs) const;

	bool operator!=(const Snapshot& rhs) const;

	[[nodiscard]] const Tag* getTag() const {
		return tag;
	}

	[[nodiscard]] uint64_t getDivergence() const {
		return divergence;
	}

	/// The same call-stack position and state, in the tag namespace of @p newDivergence.
	[[nodiscard]] Snapshot withDivergence(uint64_t newDivergence) const {
		return {tag, staticValueHash, newDivergence};
	}

private:
	friend std::hash<Snapshot>;
	uint64_t staticValueHash;
	Tag* tag;
	/// Tag namespace of the path that recorded this snapshot. Zero until the path has
	/// diverged from an already-recorded path that shares its tags (issue #487), so the
	/// tail of a diverged path cannot be merged into the other path's operations.
	uint64_t divergence;
};
} // namespace nautilus::tracing

namespace std {
template <>
struct hash<nautilus::tracing::Snapshot> {
	size_t operator()(const nautilus::tracing::Snapshot& s) const;
};
} // namespace std
