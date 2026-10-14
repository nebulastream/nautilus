#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/val_memcpy.hpp>

namespace nautilus::engine {

// ============================================================================
// Memory Intrinsic Test Functions
// ============================================================================

// Test function for memcpy intrinsic
val<void*> memcpyFunction(val<void*> dest, val<const void*> src, val<size_t> count) {
	return memcpy(dest, src, count);
}

// Test function for memmove intrinsic
val<void*> memmoveFunction(val<void*> dest, val<const void*> src, val<size_t> count) {
	return memmove(dest, src, count);
}

// Test function for memset intrinsic
val<void*> memsetFunction(val<void*> dest, val<int> value, val<size_t> count) {
	return memset(dest, value, count);
}

// Test function for memcmp
val<int> memcmpFunction(val<const void*> a, val<const void*> b, val<size_t> count) {
	return memcmp(a, b, count);
}

// memcmp of a constant length as an equality test (LLVM expands it inline)
val<bool> memcmpEq5Function(val<const void*> a, val<const void*> b) {
	return memcmp(a, b, 5) == val<int>(0);
}

// Test function for memcpy with byte arrays
val<void*> memcpyBytesFunction(val<void*> dest, val<const void*> src, val<size_t> count) {
	return memcpy(dest, src, count);
}

// Test function for memmove with overlapping regions
val<void*> memmoveOverlapFunction(val<void*> dest, val<const void*> src, val<size_t> count) {
	return memmove(dest, src, count);
}

// Test function for memset with zero
val<void*> memsetZeroFunction(val<void*> dest, val<size_t> count) {
	return memset(dest, 0, count);
}

} // namespace nautilus::engine
