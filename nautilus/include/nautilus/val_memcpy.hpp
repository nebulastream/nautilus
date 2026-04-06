#pragma once

#include <nautilus/val_ptr.hpp>

namespace nautilus {

/**
 * @brief Copies count bytes from src to dest.
 */
val<void*> memcpy(val<void*> dest, val<const void*> src, val<size_t> count);

/**
 * @brief Moves n bytes from s2 to s1. Memory areas may overlap.
 */
val<void*> memmove(val<void*> s1, val<const void*> s2, val<size_t> n);

/**
 * @brief Fills the first n bytes of s with the constant byte c.
 */
val<void*> memset(val<void*> s, val<int> c, val<size_t> n);

} // namespace nautilus
