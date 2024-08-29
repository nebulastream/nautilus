#include "nautilus/tracing/Snapshot.hpp"

namespace nautilus::tracing {

Snapshot::Snapshot(Tag* tag, uint64_t staticValueHash) : staticValueHash(staticValueHash), tag(tag) {
}

Snapshot::Snapshot() : staticValueHash(), tag() {}

bool Snapshot::operator==(const nautilus::tracing::Snapshot& rhs) const {
	return staticValueHash == rhs.staticValueHash && tag == rhs.tag;
}

bool Snapshot::operator!=(const nautilus::tracing::Snapshot& rhs) const {
	return !(rhs == *this);
}
} // namespace nautilus::tracing

namespace std {

size_t hash<nautilus::tracing::Snapshot>::operator()(const nautilus::tracing::Snapshot& s) const {
	// Start with a hash of the first member using std::hash
	size_t result = hash<uint64_t>()(s.staticValueHash);

	// Combine it with the hash of the second member
	// Shift and xor approach to combine the hash values
	result ^= hash<const nautilus::tracing::Tag*>()(s.tag) + 0x9e3779b9 + (result << 6) + (result >> 2);
	return result;
}
} // namespace std
