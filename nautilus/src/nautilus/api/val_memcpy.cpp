
#include <cstring>
#include <nautilus/function.hpp>
#include <nautilus/val_memcpy.hpp>

namespace nautilus {

val<void*> memcpy(val<void*> dest, val<const void*> src, val<size_t> count) {
	return invoke<void*, void*, const void*, size_t>(std::memcpy, dest, src, count);
}

val<void*> memmove(val<void*> s1, val<const void*> s2, val<size_t> n) {
	return invoke<void*, void*, const void*, size_t>(std::memmove, s1, s2, n);
}

val<void*> memset(val<void*> s, val<int> c, val<size_t> n) {
	return invoke<void*, void*, int, size_t>(std::memset, s, c, n);
}

} // namespace nautilus
