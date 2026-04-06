
#include <cstring>
#include <nautilus/function.hpp>
#include <nautilus/std/cstring.h>
namespace nautilus {
val<void*> memcpy(val<void*> dest, val<const void*> src, val<size_t> count) {
	return invoke<void*, void*, const void*, size_t>(std::memcpy, dest, src, count);
}

val<void*> memmove(val<void*> s1, val<const void*> s2, val<size_t> n) {
	return invoke<void*, void*, const void*, size_t>(std::memmove, s1, s2, n);
}

val<char*> strcpy(val<char*> s1, val<const char*> s2) {
	return invoke<char*, char*, const char*>(std::strcpy, s1, s2);
}
val<char*> strncpy(val<char*> s1, const val<char*> s2, val<size_t> n) {
	return invoke<char*, char*, const char*, size_t>(std::strncpy, s1, s2, n);
}
val<char*> strcat(val<char*> s1, const val<char*> s2) {
	return invoke<char*, char*, const char*>(std::strcat, s1, s2);
}
val<char*> strncat(val<char*> s1, const val<char*> s2, val<size_t> n) {
	return invoke<char*, char*, const char*, size_t>(std::strncat, s1, s2, n);
}
val<int> memcmp(val<void*> s1, val<void*> s2, val<size_t> n) {
	return invoke<int, const void*, const void*, size_t>(std::memcmp, s1, s2, n);
}
val<int> strcmp(val<const char*> s1, val<const char*> s2) {
	return invoke<int, const char*, const char*>(std::strcmp, s1, s2);
}
val<int> strcoll(val<const char*> s1, val<const char*> s2) {
	return invoke<int, const char*, const char*>(std::strcoll, s1, s2);
}
val<int> strncmp(val<const char*> s1, val<const char*> s2, val<size_t> n) {
	return invoke<int, const char*, const char*, size_t>(std::strncmp, s1, s2, n);
}
val<size_t> strxfrm(val<char*> s1, val<const char*> s2, val<size_t> n) {
	return invoke<size_t, char*, const char*, size_t>(std::strxfrm, s1, s2, n);
}
val<const void*> memchr(val<const void*> s, val<int> c, val<size_t> n) {
	return invoke<const void*, const void*, int, size_t>(std::memchr, s, c, n);
}
val<void*> memchr(val<void*> s, val<int> c, val<size_t> n) {
	return invoke<void*, void*, int, size_t>(std::memchr, s, c, n);
}
val<const char*> strchr(val<const char*> s, val<int> c) {
	return invoke<const char*, const char*, int>(std::strchr, s, c);
}
val<char*> strchr(val<char*> s, val<int> c) {
	return invoke<char*, char*, int>(std::strchr, s, c);
}
val<size_t> strcspn(val<const char*> s1, val<const char*> s2) {
	return invoke<size_t, const char*, const char*>(std::strcspn, s1, s2);
}
val<const char*> strpbrk(val<const char*> s1, val<const char*> s2) {
	return invoke<const char*, const char*, const char*>(std::strpbrk, s1, s2);
}
val<char*> strpbrk(val<char*> s1, val<const char*> s2) {
	return invoke<char*, char*, const char*>(std::strpbrk, s1, s2);
}
val<const char*> strrchr(val<const char*> s, val<int> c) {
	return invoke<const char*, const char*, int>(std::strrchr, s, c);
}
val<char*> strrchr(val<char*> s, val<int> c) {
	return invoke<char*, char*, int>(std::strrchr, s, c);
}
val<size_t> strspn(val<const char*> s1, val<const char*> s2) {
	return invoke<size_t, const char*, const char*>(std::strspn, s1, s2);
}
val<const char*> strstr(val<const char*> s1, val<const char*> s2) {
	return invoke<const char*, const char*, const char*>(std::strstr, s1, s2);
}
val<char*> strstr(val<char*> s1, val<const char*> s2) {
	return invoke<char*, char*, const char*>(std::strstr, s1, s2);
}
val<char*> strtok(val<char*> s1, val<const char*> s2) {
	return invoke<char*, char*, const char*>(std::strtok, s1, s2);
}
val<void*> memset(val<void*> s, val<int> c, val<size_t> n) {
	return invoke<void*, void*, int, size_t>(std::memset, s, c, n);
}
val<char*> strerror(val<int> errnum) {
	return invoke<char*, int>(std::strerror, errnum);
}
val<size_t> strlen(val<const char*> s) {
	return invoke<size_t, const char*>(std::strlen, s);
}
} // namespace nautilus
