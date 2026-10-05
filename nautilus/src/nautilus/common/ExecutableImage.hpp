#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace nautilus::common {

struct ExecutableImageLocation {
	std::string buildId;
	uint64_t loadOffset;

	bool operator==(const ExecutableImageLocation&) const = default;
};

std::optional<ExecutableImageLocation> locateExecutableAddress(const void* address);
void* resolveExecutableAddress(const ExecutableImageLocation& location);
std::string stableExecutableAddressName(const void* address);

} // namespace nautilus::common
