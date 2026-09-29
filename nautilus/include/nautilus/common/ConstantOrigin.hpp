#pragma once

#include <cstdint>

namespace nautilus {
enum class ConstantOrigin : uint8_t { Unspecified, CacheInvariant };
enum class ConstantOriginTracking : uint8_t { Disabled, Enabled };
} // namespace nautilus
