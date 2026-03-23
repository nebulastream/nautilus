#pragma once

#include "nautilus/function.hpp"
#include "nautilus/val.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>

namespace nautilus {

// Hash wrappers for common types used in database operations (hash joins, group-by).
// Each type needs its own extern "C" wrapper because std::hash is a template
// without a stable function pointer.

extern "C" {
inline size_t hash_int8_impl(int8_t v) {
	return std::hash<int8_t> {}(v);
}
inline size_t hash_int16_impl(int16_t v) {
	return std::hash<int16_t> {}(v);
}
inline size_t hash_int32_impl(int32_t v) {
	return std::hash<int32_t> {}(v);
}
inline size_t hash_int64_impl(int64_t v) {
	return std::hash<int64_t> {}(v);
}
inline size_t hash_uint8_impl(uint8_t v) {
	return std::hash<uint8_t> {}(v);
}
inline size_t hash_uint16_impl(uint16_t v) {
	return std::hash<uint16_t> {}(v);
}
inline size_t hash_uint32_impl(uint32_t v) {
	return std::hash<uint32_t> {}(v);
}
inline size_t hash_uint64_impl(uint64_t v) {
	return std::hash<uint64_t> {}(v);
}
inline size_t hash_float_impl(float v) {
	return std::hash<float> {}(v);
}
inline size_t hash_double_impl(double v) {
	return std::hash<double> {}(v);
}
}

template <typename T>
    requires std::is_arithmetic_v<T>
val<size_t> hash(val<T> value);

template <>
inline val<size_t> hash<int8_t>(val<int8_t> value) {
	return invoke<size_t, int8_t>(hash_int8_impl, value);
}
template <>
inline val<size_t> hash<int16_t>(val<int16_t> value) {
	return invoke<size_t, int16_t>(hash_int16_impl, value);
}
template <>
inline val<size_t> hash<int32_t>(val<int32_t> value) {
	return invoke<size_t, int32_t>(hash_int32_impl, value);
}
template <>
inline val<size_t> hash<int64_t>(val<int64_t> value) {
	return invoke<size_t, int64_t>(hash_int64_impl, value);
}
template <>
inline val<size_t> hash<uint8_t>(val<uint8_t> value) {
	return invoke<size_t, uint8_t>(hash_uint8_impl, value);
}
template <>
inline val<size_t> hash<uint16_t>(val<uint16_t> value) {
	return invoke<size_t, uint16_t>(hash_uint16_impl, value);
}
template <>
inline val<size_t> hash<uint32_t>(val<uint32_t> value) {
	return invoke<size_t, uint32_t>(hash_uint32_impl, value);
}
template <>
inline val<size_t> hash<uint64_t>(val<uint64_t> value) {
	return invoke<size_t, uint64_t>(hash_uint64_impl, value);
}
template <>
inline val<size_t> hash<float>(val<float> value) {
	return invoke<size_t, float>(hash_float_impl, value);
}
template <>
inline val<size_t> hash<double>(val<double> value) {
	return invoke<size_t, double>(hash_double_impl, value);
}

// Hash for C-strings (useful for string key lookups)
inline val<size_t> hash_cstr(val<const char*> str) {
	return invoke(
	    +[](const char* s) -> size_t {
		    // FNV-1a hash for null-terminated strings
		    size_t h = 14695981039346656037ULL;
		    while (*s) {
			    h ^= static_cast<size_t>(*s);
			    h *= 1099511628211ULL;
			    ++s;
		    }
		    return h;
	    },
	    str);
}

} // namespace nautilus
