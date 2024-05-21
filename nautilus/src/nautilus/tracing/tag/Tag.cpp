
#include "Tag.hpp"

namespace nautilus::tracing {
TagVector::TagVector(const std::vector<TagAddress>& addresses) : addresses(addresses) {
}

std::ostream& operator<<(std::ostream& os, const TagVector& tag) {
	os << "addresses: [";
	for (auto address : tag.addresses) {
		os << address << ";";
	}
	os << "]";
	return os;
}

const std::vector<TagAddress>& TagVector::getAddresses() {
	return addresses;
}

} // namespace nautilus::tracing
