#pragma once

#include <cstdint>
#include <iosfwd>

namespace nautilus {

enum class Type : uint8_t { v, b, i8, i16, i32, i64, ui8, ui16, ui32, ui64, f32, f64, ptr };

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
