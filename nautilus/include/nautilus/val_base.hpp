#pragma once

// Forwarding header: val_base has been renamed to any_val.
// Include the new header and provide a deprecated typedef.
#include "nautilus/any_val.hpp"

namespace nautilus {
/// @deprecated Use any_val instead.
using val_base = any_val;
} // namespace nautilus
