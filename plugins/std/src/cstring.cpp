
#include <cstring>
#include <nautilus/function.hpp>
#include <nautilus/std/cstring.h>
namespace nautilus {

val<char*> strcpy(const val<char*>& s1, const val<const char*>& s2) noexcept {
	return invoke<char*, char*, const char*>(std::strcpy, s1, s2);
}
val<char*> strncpy(const val<char*>& s1, const val<char*> s2, const val<size_t>& n) noexcept {
	return invoke<char*, char*, const char*, size_t>(std::strncpy, s1, s2, n);
}
val<char*> strcat(const val<char*>& s1, const val<char*> s2) noexcept {
	return invoke<char*, char*, const char*>(std::strcat, s1, s2);
}
val<char*> strncat(const val<char*>& s1, const val<char*> s2, const val<size_t>& n) noexcept {
	return invoke<char*, char*, const char*, size_t>(std::strncat, s1, s2, n);
}
val<int> memcmp(const val<void*>& s1, const val<void*>& s2, const val<size_t>& n) noexcept {
	return invoke<int, const void*, const void*, size_t>(std::memcmp, s1, s2, n);
}
val<int> strcmp(const val<const char*>& s1, const val<const char*>& s2) noexcept {
	return invoke<int, const char*, const char*>(std::strcmp, s1, s2);
}
val<int> strcoll(const val<const char*>& s1, const val<const char*>& s2) noexcept {
	return invoke<int, const char*, const char*>(std::strcoll, s1, s2);
}
val<int> strncmp(const val<const char*>& s1, const val<const char*>& s2, const val<size_t>& n) noexcept {
	return invoke<int, const char*, const char*, size_t>(std::strncmp, s1, s2, n);
}
val<size_t> strxfrm(const val<char*>& s1, const val<const char*>& s2, const val<size_t>& n) noexcept {
	return invoke<size_t, char*, const char*, size_t>(std::strxfrm, s1, s2, n);
}
val<const void*> memchr(const val<const void*>& s, const val<int>& c, const val<size_t>& n) noexcept {
	return invoke<const void*, const void*, int, size_t>(std::memchr, s, c, n);
}
val<void*> memchr(const val<void*>& s, const val<int>& c, const val<size_t>& n) noexcept {
	return invoke<void*, void*, int, size_t>(std::memchr, s, c, n);
}
val<const char*> strchr(const val<const char*>& s, const val<int>& c) noexcept {
	return invoke<const char*, const char*, int>(std::strchr, s, c);
}
val<char*> strchr(const val<char*>& s, const val<int>& c) noexcept {
	return invoke<char*, char*, int>(std::strchr, s, c);
}
val<size_t> strcspn(const val<const char*>& s1, const val<const char*>& s2) noexcept {
	return invoke<size_t, const char*, const char*>(std::strcspn, s1, s2);
}
val<const char*> strpbrk(const val<const char*>& s1, const val<const char*>& s2) noexcept {
	return invoke<const char*, const char*, const char*>(std::strpbrk, s1, s2);
}
val<char*> strpbrk(const val<char*>& s1, const val<const char*>& s2) noexcept {
	return invoke<char*, char*, const char*>(std::strpbrk, s1, s2);
}
val<const char*> strrchr(const val<const char*>& s, const val<int>& c) noexcept {
	return invoke<const char*, const char*, int>(std::strrchr, s, c);
}
val<char*> strrchr(const val<char*>& s, const val<int>& c) noexcept {
	return invoke<char*, char*, int>(std::strrchr, s, c);
}
val<size_t> strspn(const val<const char*>& s1, const val<const char*>& s2) noexcept {
	return invoke<size_t, const char*, const char*>(std::strspn, s1, s2);
}
val<const char*> strstr(const val<const char*>& s1, const val<const char*>& s2) noexcept {
	return invoke<const char*, const char*, const char*>(std::strstr, s1, s2);
}
val<char*> strstr(const val<char*>& s1, const val<const char*>& s2) noexcept {
	return invoke<char*, char*, const char*>(std::strstr, s1, s2);
}
val<char*> strtok(const val<char*>& s1, const val<const char*>& s2) noexcept {
	return invoke<char*, char*, const char*>(std::strtok, s1, s2);
}
val<char*> strerror(const val<int>& errnum) noexcept {
	return invoke<char*, int>(std::strerror, errnum);
}
val<size_t> strlen(const val<const char*>& s) noexcept {
	return invoke<size_t, const char*>(std::strlen, s);
}
} // namespace nautilus
