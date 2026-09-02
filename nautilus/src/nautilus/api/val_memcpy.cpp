
#include <cstring>
#include <nautilus/function.hpp>
#include <nautilus/val_memcpy.hpp>

namespace nautilus {

// The C string/memory helpers are unconditionally `noUnwind` (they cannot
// throw), but whether `std::memcpy` & co. are *declared* `noexcept` differs
// between C++ standard libraries (Apple libc++ vs GCC libstdc++). That
// declaration drove the `invoke()` overload choice, so struct copies traced
// as exception-handled on Apple but plain calls on Linux. Pass the attribute
// explicitly so the traced opcode is deterministic across toolchains.
namespace {
FunctionAttributes noUnwindAttrs() {
	FunctionAttributes attrs;
	attrs.noUnwind = true;
	return attrs;
}
} // namespace

val<void*> memcpy(val<void*> dest, val<const void*> src, val<size_t> count) {
	return invoke(noUnwindAttrs(), std::memcpy, dest, src, count);
}

val<void*> memmove(val<void*> s1, val<const void*> s2, val<size_t> n) {
	return invoke(noUnwindAttrs(), std::memmove, s1, s2, n);
}

val<void*> memset(val<void*> s, val<int> c, val<size_t> n) {
	return invoke(noUnwindAttrs(), std::memset, s, c, n);
}

} // namespace nautilus
