#pragma once

// The value semantics of the pure core operations (13-nir-spec.md §5): wrapping integers, IEEE floats, masked shifts,
// saturating float to integer casts. Header-only and free of module types so that the conversion's folding and the test
// evaluator use the same code and cannot disagree.

#include "nautilus/nir/Inst.hpp"
#include "nautilus/nir/Types.hpp"
#include <bit>
#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>

namespace nautilus::nir::semantics {

inline uint32_t widthOf(TypeId t) {
	switch (t) {
	case TypeId::Bool:
		return 1;
	case TypeId::I8:
		return 8;
	case TypeId::I16:
		return 16;
	case TypeId::I32:
	case TypeId::F32:
		return 32;
	default:
		return 64;
	}
}
inline uint64_t mask(TypeId t) {
	auto w = widthOf(t);
	return w == 64 ? ~0ull : ((1ull << w) - 1);
}
/// A value is kept as its zero-extended bit pattern.
inline uint64_t canon(uint64_t v, TypeId t) {
	return v & mask(t);
}
inline int64_t sext(uint64_t v, TypeId t) {
	auto w = widthOf(t);
	if (w == 64) {
		return static_cast<int64_t>(v);
	}
	uint64_t sign = 1ull << (w - 1);
	v &= mask(t);
	return static_cast<int64_t>((v ^ sign) - sign);
}
inline bool isFloatType(TypeId t) {
	return t == TypeId::F32 || t == TypeId::F64;
}
inline double asDouble(uint64_t bits, TypeId t) {
	return t == TypeId::F32 ? static_cast<double>(std::bit_cast<float>(static_cast<uint32_t>(bits)))
	                        : std::bit_cast<double>(bits);
}
inline uint64_t fromDouble(double v, TypeId t) {
	return t == TypeId::F32 ? std::bit_cast<uint32_t>(static_cast<float>(v)) : std::bit_cast<uint64_t>(v);
}

/// Division and remainder by `b`: false if they would throw (zero, or the minimum divided by -1).
inline bool divisionIsSafe(bool isUnsigned, TypeId t, uint64_t a, uint64_t b) {
	if (canon(b, t) == 0) {
		return false;
	}
	if (!isUnsigned && sext(b, t) == -1 && sext(a, t) == sext(1ull << (widthOf(t) - 1), t)) {
		return false;
	}
	return true;
}

/// Whether the divisor alone makes every division by it safe.
inline bool divisorIsAlwaysSafe(bool isUnsigned, TypeId t, uint64_t b) {
	if (canon(b, t) == 0) {
		return false;
	}
	return isUnsigned || sext(b, t) != -1;
}

/// The result of a binary core operation on constants; nullopt for an operation that is not pure, or a division that
/// would throw. `type` is the result type of the instruction (the operand type for compares is `operandType`).
inline std::optional<uint64_t> evalBinary(Op op, uint8_t flags, TypeId type, uint64_t a, uint64_t b) {
	const bool fl = isFloatType(type);
	switch (op) {
	case Op::Add:
	case Op::Sub:
	case Op::Mul: {
		if (fl) {
			if (type == TypeId::F32) {
				float x = std::bit_cast<float>(static_cast<uint32_t>(a)),
				      y = std::bit_cast<float>(static_cast<uint32_t>(b));
				float r = op == Op::Add ? x + y : op == Op::Sub ? x - y : x * y;
				return std::bit_cast<uint32_t>(r);
			}
			double x = std::bit_cast<double>(a), y = std::bit_cast<double>(b);
			double r = op == Op::Add ? x + y : op == Op::Sub ? x - y : x * y;
			return std::bit_cast<uint64_t>(r);
		}
		return canon(op == Op::Add ? a + b : op == Op::Sub ? a - b : a * b, type);
	}
	case Op::Div:
	case Op::Rem: {
		const bool rem = op == Op::Rem;
		if (fl) {
			if (type == TypeId::F32) {
				float x = std::bit_cast<float>(static_cast<uint32_t>(a)),
				      y = std::bit_cast<float>(static_cast<uint32_t>(b));
				return std::bit_cast<uint32_t>(rem ? std::fmod(x, y) : x / y);
			}
			double x = std::bit_cast<double>(a), y = std::bit_cast<double>(b);
			return std::bit_cast<uint64_t>(rem ? std::fmod(x, y) : x / y);
		}
		const bool isUnsigned = flags & 1;
		if (!divisionIsSafe(isUnsigned, type, a, b)) {
			return std::nullopt;
		}
		if (isUnsigned) {
			a = canon(a, type);
			b = canon(b, type);
			return canon(rem ? a % b : a / b, type);
		}
		int64_t sa = sext(a, type), sb = sext(b, type);
		return canon(static_cast<uint64_t>(rem ? sa % sb : sa / sb), type);
	}
	case Op::And:
		return (a & b) & 1;
	case Op::Or:
		return (a | b) & 1;
	case Op::BAnd:
		return a & b;
	case Op::BOr:
		return a | b;
	case Op::BXor:
		return a ^ b;
	case Op::Shl:
		return canon(a << (canon(b, type) % widthOf(type)), type);
	case Op::Shr: {
		unsigned amount = static_cast<unsigned>(canon(b, type) % widthOf(type));
		if (flags & 1) {
			return canon(canon(a, type) >> amount, type);
		}
		return canon(static_cast<uint64_t>(sext(a, type) >> amount), type);
	}
	default:
		return std::nullopt;
	}
}

inline std::optional<uint64_t> evalUnary(Op op, TypeId type, uint64_t a) {
	switch (op) {
	case Op::FNeg:
		return a ^ (1ull << (widthOf(type) - 1));
	case Op::Not:
		return (a & 1) ^ 1;
	case Op::BNot:
		return canon(~a, type);
	default:
		return std::nullopt;
	}
}

/// A comparison of two values of type `t` (`flags`: predicate in bits 0-2, bit 3 unsigned).
inline uint64_t evalCompare(uint8_t flags, uint64_t a, uint64_t b, TypeId t) {
	const unsigned predicate = flags & 7;
	const bool isUnsigned = flags & 8;
	if (isFloatType(t)) {
		double x = asDouble(a, t), y = asDouble(b, t);
		switch (predicate) {
		case 0:
			return x == y;
		case 1:
			return !(x == y); // a NaN operand: ne is true
		case 2:
			return x < y;
		case 3:
			return x <= y;
		case 4:
			return x > y;
		default:
			return x >= y;
		}
	}
	bool lt, eq = canon(a, t) == canon(b, t);
	if (isUnsigned || t == TypeId::Bool || t == TypeId::Ptr) {
		lt = canon(a, t) < canon(b, t);
	} else {
		lt = sext(a, t) < sext(b, t);
	}
	switch (predicate) {
	case 0:
		return eq;
	case 1:
		return !eq;
	case 2:
		return lt;
	case 3:
		return lt || eq;
	case 4:
		return !lt && !eq;
	default:
		return !lt;
	}
}

/// `kind` is the cast kind of 13 §5.5; nullopt for an invalid one.
inline std::optional<uint64_t> evalCast(uint8_t kind, uint64_t v, TypeId from, TypeId to) {
	enum : uint8_t { Sext, Zext, Trunc, SiToFp, UiToFp, FpToSi, FpToUi, FpExt, FpTrunc, Bitcast, PtrToInt, IntToPtr };
	switch (kind) {
	case Sext:
		return canon(static_cast<uint64_t>(sext(v, from)), to);
	case Zext:
	case Trunc:
	case PtrToInt:
	case IntToPtr:
	case Bitcast:
		return canon(v, to);
	case SiToFp:
		return fromDouble(static_cast<double>(sext(v, from)), to);
	case UiToFp:
		return fromDouble(static_cast<double>(canon(v, from)), to);
	case FpToSi:
	case FpToUi: {
		double d = asDouble(v, from);
		bool isSigned = kind == FpToSi;
		unsigned w = widthOf(to);
		if (std::isnan(d)) {
			return 0;
		}
		if (isSigned) {
			double lo = -std::ldexp(1.0, w - 1);
			double hi = std::ldexp(1.0, w - 1);
			if (d <= lo) {
				return canon(
				    static_cast<uint64_t>(w == 64 ? std::numeric_limits<int64_t>::min() : -(int64_t(1) << (w - 1))),
				    to);
			}
			if (d >= hi) {
				return canon(
				    static_cast<uint64_t>(w == 64 ? std::numeric_limits<int64_t>::max() : (int64_t(1) << (w - 1)) - 1),
				    to);
			}
			return canon(static_cast<uint64_t>(static_cast<int64_t>(d)), to);
		}
		if (d <= 0) {
			return 0;
		}
		if (d >= std::ldexp(1.0, w)) {
			return mask(to);
		}
		return canon(static_cast<uint64_t>(d), to);
	}
	case FpExt:
		return std::bit_cast<uint64_t>(static_cast<double>(std::bit_cast<float>(static_cast<uint32_t>(v))));
	case FpTrunc:
		return std::bit_cast<uint32_t>(static_cast<float>(std::bit_cast<double>(v)));
	}
	return std::nullopt;
}

} // namespace nautilus::nir::semantics
