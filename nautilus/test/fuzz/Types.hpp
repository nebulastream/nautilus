#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>

namespace nautilus::fuzz {

/// Every concrete C++ type the fuzzer can generate a kernel over. Picked once
/// per fuzzer input (the very first byte) and held fixed for the rest of
/// generation/evaluation -- each generated program is monomorphic in one of
/// these types, mirroring how the original uint64_t-only fuzzer worked.
/// `Bool`/`Enum` are additional first-class domains alongside the original
/// ten (see `BOOL_KINDS` in Ast.hpp and `generateFuzzEnumAst`); they are
/// deliberately excluded from `ALL_TYPES` below since that array also serves
/// as the `Cast` target list for the original ten, and neither `val<bool>`
/// nor `val<enum>` participates in `Cast` (see Ast.hpp/EvalNautilus.hpp).
enum class TypeId : uint8_t { I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, Bool, Enum };

/// A small, fixed enum with a fixed underlying type (required so every bit
/// pattern of that underlying type is a valid value, letting Const leaves be
/// generated the same way as any other domain's -- see generateFuzzEnumAst).
enum class FuzzEnum : int32_t { A = 0, B = 1, C = 2, D = 3 };

/// Number of value-domain parameters the enum domain's kernels take. Unlike
/// every other domain, enum doesn't participate in the arity{1,2,3,4}/mixed/
/// narrowReturn/voidReturn signature-shape space (Harness.hpp's checkOneEnum
/// keeps the harness's original fixed shape), so this stays a plain constant
/// rather than a per-call parameter.
inline constexpr uint32_t ENUM_NUM_PARAMS = 3;

inline constexpr TypeId ALL_TYPES[] = {TypeId::I8,  TypeId::I16, TypeId::I32, TypeId::I64, TypeId::U8,
                                       TypeId::U16, TypeId::U32, TypeId::U64, TypeId::F32, TypeId::F64};

/// Every top-level domain the fuzzer picks from the first input byte:
/// ALL_TYPES plus the bool/enum domains. Kept separate from ALL_TYPES because
/// the latter doubles as the Cast target list (see above).
inline constexpr TypeId ROOT_TYPES[] = {TypeId::I8,  TypeId::I16, TypeId::I32, TypeId::I64, TypeId::U8,   TypeId::U16,
                                        TypeId::U32, TypeId::U64, TypeId::F32, TypeId::F64, TypeId::Bool, TypeId::Enum};

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
	case TypeId::Bool:
		return "bool";
	case TypeId::Enum:
		return "enum";
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
/// Delegates to dispatchIntType/dispatchFloatType rather than re-enumerating
/// all ten types itself, so the type -> concrete-type mapping lives in
/// exactly one place per domain.
template <typename Fn>
auto dispatchAnyType(TypeId target, Fn&& fn) {
	if (isFloatType(target)) {
		return dispatchFloatType(target, std::forward<Fn>(fn));
	}
	return dispatchIntType(target, std::forward<Fn>(fn));
}

/// Top-level dispatch across all twelve root domains (the original ten plus
/// bool). Excludes TypeId::Enum -- unlike bool, the enum domain doesn't reuse
/// generateNode<T>/evalNativeGeneric<T>/evalNautilusGeneric<T> (val<enum> is
/// missing too many of the operators those templates assume; see
/// generateFuzzEnumAst/evalNativeFuzzEnum/evalNautilusFuzzEnum), so its
/// caller (Harness.hpp's checkOne) handles TypeId::Enum directly instead of
/// going through this generic `fn.template operator()<T>()` dispatch.
template <typename Fn>
auto dispatchRootType(TypeId target, Fn&& fn) {
	if (target == TypeId::Bool) {
		return fn.template operator()<bool>();
	}
	return dispatchAnyType(target, std::forward<Fn>(fn));
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
	} else if constexpr (std::is_same_v<T, bool>) {
		return "bool";
	} else if constexpr (std::is_enum_v<T>) {
		return "enum";
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
	if constexpr (std::is_enum_v<T>) {
		return std::to_string(static_cast<std::underlying_type_t<T>>(v));
	} else if constexpr (std::is_floating_point_v<T>) {
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

/// Elementwise valuesEqual over a whole buffer -- used to diff a void-return
/// (Store-only) kernel's final buffer contents against the native oracle's,
/// since std::array::operator== would report two agreeing NaNs as a mismatch.
template <typename T, size_t M>
bool arraysEqual(const std::array<T, M>& a, const std::array<T, M>& b) {
	for (size_t i = 0; i < M; ++i) {
		if (!valuesEqual<T>(a[i], b[i])) {
			return false;
		}
	}
	return true;
}

/// Smallest `From` value that is already out of range for `To` (i.e. the
/// first representative for which a float->int cast is UB). Deliberately
/// compared against a power-of-two boundary rather than
/// `static_cast<From>(numeric_limits<To>::max())`: for wide integer types
/// the latter is not exactly representable in `float`/`double` and rounds
/// *up* past the true max (e.g. `(double)INT64_MAX` rounds to `2^63`), which
/// would let an out-of-range value slip past a naive `<=` check. `2^bits` is
/// always exactly representable in IEEE-754 binary float for every
/// `(From, To)` combination used by this fuzzer (bits <= 64). Shared by
/// EvalNative.hpp (native oracle) and EvalNautilus.hpp (traced kernel) so
/// the float<->int cast clamp is sound identically on both sides -- this
/// boundary math must never be re-derived independently in two places.
template <typename From, typename To>
constexpr From hiLimitExclusive() {
	constexpr int bits = sizeof(To) * 8 - (std::is_signed_v<To> ? 1 : 0);
	return static_cast<From>(std::ldexp(From(1), bits));
}

/// Largest representable-as-`From` lower bound for `To`: `To`'s minimum is
/// always a power of two (or zero for unsigned types), so it is exactly
/// representable in `From` and needs no power-of-two-boundary trick.
template <typename From, typename To>
constexpr From loLimitInclusive() {
	if constexpr (std::is_signed_v<To>) {
		return static_cast<From>(std::numeric_limits<To>::min());
	} else {
		return From(0);
	}
}

/// Convert a native `From` value to `To`, well-defined for every input.
/// Every direction except float->int is a plain (always-safe) static_cast;
/// float->int is the only UB-prone leg, so it alone is range-clamped via the
/// hiLimitExclusive/loLimitInclusive boundary above (NaN -> 0, out-of-range
/// -> To's min/max) -- the exact same recipe EvalNative.hpp's castThrough
/// uses for a same-domain round-trip Cast, reused here for one-way
/// conversions at a kernel signature boundary (parameter marshalling from a
/// "mixed" secondary type, or a narrow/void return) and for Kind::Call's
/// cross-type argument/return marshalling (Callees.hpp) -- this boundary
/// math must never be re-derived independently in more than one place.
template <typename From, typename To>
To convertClamped(From v) {
	if constexpr (std::is_floating_point_v<From> && !std::is_floating_point_v<To>) {
		if (std::isnan(v)) {
			return To(0);
		}
		if (v < loLimitInclusive<From, To>()) {
			return std::numeric_limits<To>::min();
		}
		if (v >= hiLimitExclusive<From, To>()) {
			return std::numeric_limits<To>::max();
		}
		return static_cast<To>(v);
	} else {
		return static_cast<To>(v);
	}
}

/// Diagnostic-only formatting of a fixed-size array (the shared pointer
/// buffer) for failure reports -- shared by the scalar-return and
/// void/buffer-diffing signature shapes alike.
template <typename T, size_t M>
std::string formatArray(const std::array<T, M>& arr) {
	std::string out = "[";
	for (size_t i = 0; i < M; ++i) {
		if (i != 0) {
			out += ", ";
		}
		out += formatValue<T>(arr[i]);
	}
	out += "]";
	return out;
}

} // namespace nautilus::fuzz
