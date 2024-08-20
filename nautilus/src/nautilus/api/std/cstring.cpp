
#include <cstring>
#include <nautilus/function.hpp>
#include <nautilus/std/cstring.h>
namespace nautilus {
val<void*> memcpy(val<void*> dest, val<const void*> src, val<size_t> count) {
	return invoke<>(
	    +[](void* dest, const void* src, size_t count) { return std::memcpy(dest, src, count); }, dest, src, count);
}

val<void*> memmove(val<void*> s1, val<const void*> s2, val<size_t> n) {
	return invoke<>(
	    +[](void* s1, const void* s2, size_t n) { return std::memmove(s1, s2, n); }, s1, s2, n);
}

val<char*> strcpy(val<char*> s1, val<const char*> s2) {
	return invoke<>(
	    +[](char* s1, const char* s2) { return std::strcpy(s1, s2); }, s1, s2);
}
val<char*> strncpy(val<char*> s1, const val<char*> s2, val<size_t> n) {
	return invoke<>(
	    +[](char* s1, const char* s2, size_t n) { return std::strncpy(s1, s2, n); }, s1, s2, n);
}
val<char*> strcat(val<char*> s1, const val<char*> s2) {
	return invoke<>(
	    +[](char* s1, const char* s2) { return std::strcat(s1, s2); }, s1, s2);
}
val<char*> strncat(val<char*> s1, const val<char*> s2, val<size_t> n) {
	return invoke<>(
	    +[](char* s1, const char* s2, size_t n) { return std::strncat(s1, s2, n); }, s1, s2, n);
}
val<int> memcmp(val<void*> s1, val<void*> s2, val<size_t> n) {
	return invoke<>(
	    +[](void* s1, void* s2, size_t n) { return std::memcmp(s1, s2, n); }, s1, s2, n);
}
val<int> strcmp(val<const char*> s1, val<const char*> s2) {
	return invoke<>(
	    +[](const char* s1, const char* s2) { return std::strcmp(s1, s2); }, s1, s2);
}
val<int> strcoll(val<const char*> s1, val<const char*> s2) {
	return invoke<>(
	    +[](const char* s1, const char* s2) { return std::strcoll(s1, s2); }, s1, s2);
}
val<int> strncmp(val<const char*> s1, val<const char*> s2, val<size_t> n) {
	return invoke<>(
	    +[](const char* s1, const char* s2, size_t n) { return std::strncmp(s1, s2, n); }, s1, s2, n);
}
val<size_t> strxfrm(val<char*> s1, val<const char*> s2, val<size_t> n) {
	return invoke<>(
	    +[](char* s1, const char* s2, size_t n) { return std::strxfrm(s1, s2, n); }, s1, s2, n);
}
val<const void*> memchr(val<const void*> s, val<int> c, val<size_t> n) {
	return invoke<>(
	    +[](const void* s, int c, size_t n) { return std::memchr(s, c, n); }, s, c, n);
}
val<void*> memchr(val<void*> s, val<int> c, val<size_t> n) {
	return invoke<>(
	    +[](void* s, int c, size_t n) { return std::memchr(s, c, n); }, s, c, n);
}
val<const char*> strchr(val<const char*> s, val<int> c) {
	return invoke<>(
	    +[](const char* s, int c) { return std::strchr(s, c); }, s, c);
}
val<char*> strchr(val<char*> s, val<int> c) {
	return invoke<>(
	    +[](char* s, int c) { return std::strchr(s, c); }, s, c);
}
val<size_t> strcspn(val<const char*> s1, val<const char*> s2) {
	return invoke<>(
	    +[](const char* s1, const char* s2) { return std::strcspn(s1, s2); }, s1, s2);
}
val<const char*> strpbrk(val<const char*> s1, val<const char*> s2) {
	return invoke<>(
	    +[](const char* s1, const char* s2) { return std::strpbrk(s1, s2); }, s1, s2);
}
val<char*> strpbrk(val<char*> s1, val<const char*> s2) {
	return invoke<>(
	    +[](char* s1, const char* s2) { return std::strpbrk(s1, s2); }, s1, s2);
}
val<const char*> strrchr(val<const char*> s, val<int> c) {
	return invoke<>(
	    +[](const char* s, int c) { return std::strrchr(s, c); }, s, c);
}
val<char*> strrchr(val<char*> s, val<int> c) {
	return invoke<>(
	    +[](char* s, int c) { return std::strrchr(s, c); }, s, c);
}
val<size_t> strspn(val<const char*> s1, val<const char*> s2) {
	return invoke<>(
	    +[](const char* s1, const char* s2) { return std::strspn(s1, s2); }, s1, s2);
}
val<const char*> strstr(val<const char*> s1, val<const char*> s2) {
	return invoke<>(
	    +[](const char* s1, const char* s2) { return std::strstr(s1, s2); }, s1, s2);
}
val<char*> strstr(val<char*> s1, val<const char*> s2) {
	return invoke<>(
	    +[](const char* s1, const char* s2) { return std::strstr(s1, s2); }, s1, s2);
}
val<char*> strtok(val<char*> s1, val<const char*> s2) {
	return invoke<>(
	    +[](char* s1, const char* s2) { return std::strstr(s1, s2); }, s1, s2);
}
val<void*> memset(val<void*> s, val<int> c, val<size_t> n) {
	return invoke<>(
	    +[](void* s, int c, size_t n) { return std::memset(s, c, n); }, s, c, n);
}
val<char*> strerror(val<int> errnum) {
	return invoke<>(
	    +[](int errnum) { return std::strerror(errnum); }, errnum);
}
val<size_t> strlen(val<const char*> s) {
	return invoke<>(
	    +[](const char* s) { return std::strlen(s); }, s);
}
} // namespace nautilus
