#pragma once

#include "nautilus/val_concepts.hpp"
#include <cstdint>
#include <cstdlib>
#include <iosfwd>
#include <iostream>
#include <type_traits>
#include <variant>

namespace nautilus {

enum class Type : uint8_t { v, b, i8, i16, i32, i64, ui8, ui16, ui32, ui64, f32, f64, ptr };

using ConstantLiteral =
    std::variant<bool, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double, void*>;

namespace tracing {

template <typename T>
concept is_compatible_val_type =
    is_ptr<T> || is_fundamental<T> || is_fundamental_ref<T> || is_bool<T> || is_bool_ref<T>;

template <typename T>
    requires is_ptr<std::remove_cvref_t<T>>
constexpr ConstantLiteral createConstLiteral(T&& t) noexcept {
	return ConstantLiteral((void*) t);
}

template <typename T>
    requires is_fundamental<std::remove_cvref_t<T>> || is_bool<std::remove_cvref_t<T>>
constexpr ConstantLiteral createConstLiteral(T&& t) noexcept {
	return ConstantLiteral(t);
}

/**
 * Returns the nautilus type for a C++ type.
 * @tparam T
 * @return Type
 */
template <typename T>
struct TypeResolver {};

template <is_compatible_val_type T>
struct TypeResolver<T> {
	[[nodiscard]] static constexpr Type to_type() {
		using type = std::remove_cvref_t<T>;
		if constexpr (std::is_same_v<type, bool>) {
			return Type::b;
		} else if constexpr (std::is_same_v<type, int8_t> || (std::is_same_v<type, char>) ) {
			return Type::i8;
		} else if constexpr (std::is_same_v<type, int16_t>) {
			return Type::i16;
		} else if constexpr (std::is_same_v<type, int32_t> || std::is_same_v<type, wchar_t>) {
			return Type::i32;
		} else if constexpr (std::is_same_v<type, int64_t> || std::is_same_v<type, long> ||
		                     std::is_same_v<type, long long>) {
			static_assert(sizeof(type) == sizeof(int64_t));
			return Type::i64;
		} else if constexpr (std::is_same_v<type, uint8_t>) {
			return Type::ui8;
		} else if constexpr (std::is_same_v<type, uint16_t> || std::is_same_v<type, char16_t>) {
			return Type::ui16;
		} else if constexpr (std::is_same_v<type, uint32_t> || std::is_same_v<type, char32_t>) {
			return Type::ui32;
		} else if constexpr (std::is_same_v<type, uint64_t> || std::is_same_v<type, size_t> ||
		                     std::is_same_v<type, unsigned long> || std::is_same_v<type, unsigned long long>) {
			static_assert(sizeof(type) == sizeof(uint64_t));
			return Type::ui64;
		} else if constexpr (std::is_same_v<type, float>) {
			return Type::f32;
		} else if constexpr (std::is_same_v<type, double>) {
			return Type::f64;
		} else if constexpr (std::is_pointer_v<type>) {
			return Type::ptr;
		} else if constexpr (std::is_void_v<type>) {
			return Type::v;
		} else {
#ifdef COMPILER_IS_SMART_WITH_CONSTEXPR
			static_assert(false, "unhandled type!");
#else
			std::cout << "Error: unhandled type!" << std::endl;
			std::abort();
#endif
		}
	}
};
} // namespace tracing

constexpr const char* toString(Type type) {
	switch (type) {
	case Type::v:
		return "void";
	case Type::b:
		return "bool";
	case Type::i8:
		return "i8";
	case Type::i16:
		return "i16";
	case Type::i32:
		return "i32";
	case Type::i64:
		return "i64";
	case Type::ui8:
		return "ui8";
	case Type::ui16:
		return "ui16";
	case Type::ui32:
		return "ui32";
	case Type::ui64:
		return "ui64";
	case Type::f32:
		return "f32";
	case Type::f64:
		return "f64";
	case Type::ptr:
		return "ptr";
	default:
		__builtin_unreachable();
	}
}

constexpr bool isSignedInteger(Type type) {
	switch (type) {
	case Type::i8:
	case Type::i16:
	case Type::i32:
	case Type::i64:
		return true;
	default:
		return false;
	}
}

constexpr bool isUnsignedInteger(Type type) {
	switch (type) {
	case Type::ui8:
	case Type::ui16:
	case Type::ui32:
	case Type::ui64:
		return true;
	default:
		return false;
	}
}

constexpr bool isInteger(Type type) {
	return isSignedInteger(type) || isUnsignedInteger(type);
}

constexpr bool isFloat(Type type) {
	switch (type) {
	case Type::f32:
	case Type::f64:
		return true;
	default:
		return false;
	}
}

constexpr int8_t getBitWith(Type type) {
	switch (type) {
	case Type::v:
		return -1;
	case Type::b:
	case Type::i8:
	case Type::ui8:
		return 8;
	case Type::i16:
	case Type::ui16:
		return 16;
	case Type::i32:
	case Type::ui32:
	case Type::f32:
		return 32;
	case Type::i64:
	case Type::ui64:
	case Type::f64:
	case Type::ptr:
		return 64;
	}
	return -1;
}

std::ostream& operator<<(std::ostream& os, const Type& type);

} // namespace nautilus
