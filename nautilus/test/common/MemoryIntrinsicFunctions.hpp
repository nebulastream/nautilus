#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/std/cstring.h>

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
