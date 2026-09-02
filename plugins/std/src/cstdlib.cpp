#include <cstdlib>
#include <nautilus/function.hpp>
#include <nautilus/std/cstdlib.h>

namespace nautilus {

val<void*> malloc(val<size_t> size) noexcept {
	return invoke<>(+[](size_t size) noexcept { return std::malloc(size); }, size);
}

val<void*> aligned_alloc(val<size_t> alignment, val<size_t> size) noexcept {
	return invoke<>(
	    +[](size_t alignment, size_t size) noexcept { return std::aligned_alloc(alignment, size); }, alignment, size);
}

val<void*> calloc(val<size_t> num, val<size_t> size) noexcept {
	return invoke<>(+[](size_t num, size_t size) noexcept { return std::aligned_alloc(num, size); }, num, size);
}

val<void*> realloc(val<void*> ptr, val<size_t> new_size) noexcept {
	return invoke<>(+[](void* ptr, size_t new_size) noexcept { return std::realloc(ptr, new_size); }, ptr, new_size);
}

void free(val<void*> ptr) noexcept {
	invoke<>(+[](void* ptr) noexcept { std::free(ptr); }, ptr);
}

val<double> atof(val<const char*> str) noexcept {
	return invoke<>(+[](const char* str) noexcept { return std::atof(str); }, str);
}

val<int> atoi(val<const char*> str) noexcept {
	return invoke<>(+[](const char* str) noexcept { return std::atoi(str); }, str);
}

val<long> atol(val<const char*> str) noexcept {
	return invoke<>(+[](const char* str) noexcept { return std::atol(str); }, str);
}

val<long long> atoll(val<const char*> str) noexcept {
	return invoke<>(+[](const char* str) noexcept { return std::atoll(str); }, str);
}

} // namespace nautilus
