#pragma once

#include <cstdint>

namespace nautilus::nir {

/// 13-nir-spec.md §2. Integers are signless; signedness is a flag of the operation.
enum class TypeId : uint8_t {
	Void = 0,
	Bool = 1,
	I8 = 2,
	I16 = 3,
	I32 = 4,
	I64 = 5,
	F32 = 6,
	F64 = 7,
	Ptr = 8,
	// 9-15 reserved for core
	FirstExtension = 16,
};

constexpr uint32_t MAX_EXTENSION_TYPES = 240;

constexpr bool isInteger(TypeId t) {
	return t >= TypeId::I8 && t <= TypeId::I64;
}
constexpr bool isFloat(TypeId t) {
	return t == TypeId::F32 || t == TypeId::F64;
}
constexpr bool isExtension(TypeId t) {
	return static_cast<uint8_t>(t) >= static_cast<uint8_t>(TypeId::FirstExtension);
}
/// Any core type except void.
constexpr bool isScalar(TypeId t) {
	return t >= TypeId::Bool && t <= TypeId::Ptr;
}
constexpr bool isCore(TypeId t) {
	return t <= TypeId::Ptr;
}

/// Size in bytes of a core scalar type; 0 for void, reserved and extension types.
constexpr uint32_t sizeOf(TypeId t) {
	switch (t) {
	case TypeId::Bool:
	case TypeId::I8:
		return 1;
	case TypeId::I16:
		return 2;
	case TypeId::I32:
	case TypeId::F32:
		return 4;
	case TypeId::I64:
	case TypeId::F64:
	case TypeId::Ptr:
		return 8;
	default:
		return 0;
	}
}
/// Alignment equals the size for every core scalar.
constexpr uint32_t alignOf(TypeId t) {
	return sizeOf(t);
}

/// 13-nir-spec.md §4.5.1: how a narrow integer is widened where it crosses a native boundary.
enum class ExtKind : uint8_t { None = 0, Zext = 1, Sext = 2 };

/// A type word holds a type in a signature: bits 0-7 TypeId, bits 8-9 ExtKind, the rest 0.
using TypeWord = uint32_t;

constexpr TypeWord makeTypeWord(TypeId t, ExtKind k = ExtKind::None) {
	return static_cast<uint32_t>(t) | (static_cast<uint32_t>(k) << 8);
}
constexpr TypeId typeOf(TypeWord w) {
	return static_cast<TypeId>(w & 0xFF);
}
constexpr ExtKind extKindOf(TypeWord w) {
	return static_cast<ExtKind>((w >> 8) & 0x3);
}
/// The rules of §4.5.1: a kind only on bool/i8/i16/i32; bool always zext; i8/i16 always non-zero; reserved bits 0.
constexpr bool isValidTypeWord(TypeWord w) {
	if ((w >> 10) != 0) {
		return false;
	}
	auto t = typeOf(w);
	auto k = extKindOf(w);
	if (static_cast<uint8_t>(k) == 3) {
		return false;
	}
	switch (t) {
	case TypeId::Bool:
		return k == ExtKind::Zext;
	case TypeId::I8:
	case TypeId::I16:
		return k != ExtKind::None;
	case TypeId::I32:
		return true;
	default:
		return k == ExtKind::None;
	}
}

} // namespace nautilus::nir
