#pragma once

#include <cstdint>

namespace nautilus::testing {
/// Heap allocations made by the process so far (global operator new, all threads).
uint64_t allocations();
} // namespace nautilus::testing
