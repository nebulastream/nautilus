#pragma once

#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>
#include <type_traits>
#include <vector>

/**
 * @file data.hpp
 * @brief Constant data: lookup tables, strings and seeds that belong to the compiled function.
 *
 * Tracing a pointer to a host array (`val<const int*>(table)`) makes the module depend on that memory: it is a traced
 * address, the loads from it stay loads of unknown memory, and the module cannot be persisted. `constData` instead
 * copies the bytes into a read-only data object of the module at trace time and returns a pointer to it
 * (docs/plans/nautilus-0.4/13-nir-spec.md §4.10): the module no longer reads the host array, a load at a constant
 * index folds into a constant, and the module stays persistable.
 *
 * @code
 *   static const uint8_t table[] = {1, 2, 4, 8};
 *   val<uint8_t> lookup(val<int32_t> i) {
 *       val<const uint8_t*> t = constData(std::span<const uint8_t>(table));
 *       return t[i];
 *   }
 * @endcode
 *
 * The data is copied when the function is traced, so changing the host array later does not change the compiled code.
 * Elements must be trivially copyable and must not hold addresses (a data object has no relocations).
 */

namespace nautilus {

/// A pointer to a read-only copy of @p data in the module.
template <typename T>
    requires(std::is_trivially_copyable_v<T> && !std::is_pointer_v<T> && !std::is_member_pointer_v<T> &&
             !std::is_void_v<T>)
val<const T*> constData(std::span<const T> data) {
#ifdef ENABLE_TRACING
	if SHOULD_TRACE () {
		const auto bytes = std::as_bytes(data);
		// a data object is never empty: a zero-sized table still has an address
		auto ref = tracing::traceData(bytes, alignof(T));
		return val<const T*>(data.data(), ref);
	}
#endif
	return val<const T*>(data.data());
}

template <typename T, size_t N>
    requires std::is_trivially_copyable_v<T>
val<const T*> constData(const T (&data)[N]) {
	return constData(std::span<const T>(data, N));
}

template <typename T, size_t N>
    requires std::is_trivially_copyable_v<T>
val<const T*> constData(const std::array<T, N>& data) {
	return constData(std::span<const T>(data.data(), N));
}

template <typename T>
    requires std::is_trivially_copyable_v<T>
val<const T*> constData(const std::vector<T>& data) {
	return constData(std::span<const T>(data.data(), data.size()));
}

/// A pointer to a read-only copy of the characters of @p text followed by a terminating NUL, so that the result is a C
/// string.
inline val<const char*> constData(std::string_view text) {
#ifdef ENABLE_TRACING
	if SHOULD_TRACE () {
		std::vector<char> bytes(text.begin(), text.end());
		bytes.push_back('\0');
		auto ref = tracing::traceData(std::as_bytes(std::span<const char>(bytes)), 1);
		return val<const char*>(text.data(), ref);
	}
#endif
	return val<const char*>(text.data());
}

inline val<const char*> constData(const char* text) {
	return constData(std::string_view(text));
}

} // namespace nautilus
