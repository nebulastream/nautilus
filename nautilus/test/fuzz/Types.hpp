#pragma once

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <type_traits>

namespace nautilus::fuzz {

/// Every concrete C++ type the fuzzer can generate a kernel over. Picked once
/// per fuzzer input (the very first byte) and held fixed for the rest of
/// generation/evaluation -- each generated program is monomorphic in one of
/// these ten types, mirroring how the original uint64_t-only fuzzer worked.
enum class TypeId : uint8_t { I8, I16, I32, I64, U8, U16, U32, U64, F32, F64 };

inline constexpr TypeId ALL_TYPES[] = {TypeId::I8, TypeId::I16, TypeId::I32, TypeId::I64, TypeId::U8,
                                       TypeId::U16, TypeId::U32, TypeId::U64, TypeId::F32, TypeId::F64};

/// The eight integer TypeIds, used to pick Cast targets for the integer
/// domain (int<->float casts are intentionally out of scope -- casting an
/// out-of-range float to an integer type is undefined behavior in C++ and
/// would need explicit range clamping to fuzz safely).
inline constexpr TypeId INT_TYPE_IDS[] = {TypeId::I8, TypeId::I16, TypeId::I32, TypeId::I64,
                                          TypeId::U8, TypeId::U16, TypeId::U32, TypeId::U64};

/// The two float TypeIds, used to pick Cast targets for the float domain.
inline constexpr TypeId FLOAT_TYPE_IDS[] = {TypeId::F32, TypeId::F64};

inline bool isFloatType(TypeId t) {
	return t == TypeId::F32 || t == TypeId::F64;
}

inline const char* typeName(TypeId t) {
	switch (t) {
	case TypeId::I8:
		return "i8";
	case TypeId::I16:
		return "i16";
	case TypeId::I32:
		return "i32";
	case TypeId::I64:
		return "i64";
	case TypeId::U8:
		return "u8";
	case TypeId::U16:
		return "u16";
	case TypeId::U32:
		return "u32";
	case TypeId::U64:
		return "u64";
	case TypeId::F32:
		return "f32";
	case TypeId::F64:
		return "f64";
	}
	return "?";
}

/// Pack a value's raw bytes into a uint64_t so Node (shared, untemplated
/// across all ten type instantiations) can store any of them in one
/// uint64_t `imm` field. Uses memcpy rather than std::bit_cast because
/// bit_cast requires equal source/destination sizes, which doesn't hold for
/// e.g. int8_t -> uint64_t.
template <typename T>
uint64_t packImm(T v) {
	uint64_t buf = 0;
	std::memcpy(&buf, &v, sizeof(T));
	return buf;
}

template <typename T>
T unpackImm(uint64_t buf) {
	T v {};
	std::memcpy(&v, &buf, sizeof(T));
	return v;
}

/// Runtime TypeId -> compile-time type dispatch, restricted to the integer
/// domain. Invokes `fn.template operator()<To>()` for the concrete type `To`
/// matching `target`. Used to evaluate Cast nodes without hand-writing an
/// eight-way switch at every call site.
template <typename Fn>
auto dispatchIntType(TypeId target, Fn&& fn) {
	switch (target) {
	case TypeId::I8:
		return fn.template operator()<int8_t>();
	case TypeId::I16:
		return fn.template operator()<int16_t>();
	case TypeId::I32:
		return fn.template operator()<int32_t>();
	case TypeId::I64:
		return fn.template operator()<int64_t>();
	case TypeId::U8:
		return fn.template operator()<uint8_t>();
	case TypeId::U16:
		return fn.template operator()<uint16_t>();
	case TypeId::U32:
		return fn.template operator()<uint32_t>();
	case TypeId::U64:
		return fn.template operator()<uint64_t>();
	default:
		__builtin_unreachable();
	}
}

/// Same as dispatchIntType, restricted to the float domain (float, double).
template <typename Fn>
auto dispatchFloatType(TypeId target, Fn&& fn) {
	switch (target) {
	case TypeId::F32:
		return fn.template operator()<float>();
	case TypeId::F64:
		return fn.template operator()<double>();
	default:
		__builtin_unreachable();
	}
}

/// Top-level dispatch across all ten types, used once per fuzzer input to
/// pick which monomorphic instantiation of the generator/evaluator to run.
template <typename Fn>
auto dispatchAnyType(TypeId target, Fn&& fn) {
	switch (target) {
	case TypeId::I8:
		return fn.template operator()<int8_t>();
	case TypeId::I16:
		return fn.template operator()<int16_t>();
	case TypeId::I32:
		return fn.template operator()<int32_t>();
	case TypeId::I64:
		return fn.template operator()<int64_t>();
	case TypeId::U8:
		return fn.template operator()<uint8_t>();
	case TypeId::U16:
		return fn.template operator()<uint16_t>();
	case TypeId::U32:
		return fn.template operator()<uint32_t>();
	case TypeId::U64:
		return fn.template operator()<uint64_t>();
	case TypeId::F32:
		return fn.template operator()<float>();
	case TypeId::F64:
		return fn.template operator()<double>();
	}
	__builtin_unreachable();
}

/// Compile-time type -> short suffix, the mirror of typeName(TypeId) for use
/// in templated contexts (e.g. annotating a printed Const literal).
template <typename T>
constexpr const char* typeSuffix() {
	if constexpr (std::is_same_v<T, int8_t>) {
		return "i8";
	} else if constexpr (std::is_same_v<T, int16_t>) {
		return "i16";
	} else if constexpr (std::is_same_v<T, int32_t>) {
		return "i32";
	} else if constexpr (std::is_same_v<T, int64_t>) {
		return "i64";
	} else if constexpr (std::is_same_v<T, uint8_t>) {
		return "u8";
	} else if constexpr (std::is_same_v<T, uint16_t>) {
		return "u16";
	} else if constexpr (std::is_same_v<T, uint32_t>) {
		return "u32";
	} else if constexpr (std::is_same_v<T, uint64_t>) {
		return "u64";
	} else if constexpr (std::is_same_v<T, float>) {
		return "f32";
	} else if constexpr (std::is_same_v<T, double>) {
		return "f64";
	} else {
		static_assert(!sizeof(T), "unsupported fuzz value type");
	}
}

/// Diagnostic-only formatting for failure reports. 1-byte integer types are
/// widened before formatting so they print numerically rather than risking
/// char-like interpretation. Floats use enough significant digits (17) to
/// round-trip a double exactly (9 suffices for float; 17 is a safe superset
/// for both).
template <typename T>
std::string formatValue(T v) {
	if constexpr (std::is_floating_point_v<T>) {
		char buf[64];
		std::snprintf(buf, sizeof(buf), "%.17g", static_cast<double>(v));
		return buf;
	} else if constexpr (sizeof(T) == 1) {
		if constexpr (std::is_signed_v<T>) {
			return std::to_string(static_cast<int>(v));
		} else {
			return std::to_string(static_cast<unsigned int>(v));
		}
	} else {
		return std::to_string(v);
	}
}

/// Oracle-vs-backend equality. Exact for integers; NaN-aware for floats so
/// that two NaN results (a routine, expected outcome of generated float
/// programs) aren't reported as a mismatch.
template <typename T>
bool valuesEqual(T a, T b) {
	if constexpr (std::is_floating_point_v<T>) {
		return a == b || (std::isnan(a) && std::isnan(b));
	} else {
		return a == b;
	}
}

} // namespace nautilus::fuzz
