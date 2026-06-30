#pragma once

#include "Ast.hpp"
#include "Types.hpp"
#include <array>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace nautilus::fuzz {

/**
 * @brief Independent ground-truth interpreter for an AST.
 *
 * Pure C++ scalar arithmetic with the exact, fully-defined semantics the
 * generator guarantees:
 *   - unsigned arithmetic uses plain operators (already defined wraparound);
 *     signed arithmetic (Add/Sub/Mul/Neg) is computed via the unsigned
 *     counterpart and cast back, since plain signed overflow is UB even
 *     though two's-complement representation is mandated by C++20 -- this
 *     produces the exact bit pattern every backend's two's-complement
 *     codegen actually computes, without inviting UB in the oracle,
 *   - division/modulo divisors are forced non-zero (`| T(1)`), and for
 *     signed types additionally forced away from -1 (the one divisor value
 *     for which TYPE_MIN / divisor overflows and traps via `idiv` on x86),
 *   - shift amounts are masked to [0, bit-width(T)),
 *   - the float domain needs none of the above: IEEE 754 arithmetic
 *     (including division by zero, producing +-inf/NaN) is fully defined.
 * Because every operation is well-defined, a disagreement between this
 * oracle and any Nautilus backend is an unambiguous miscompile (or tracing
 * bug).
 */
namespace detail {

template <typename T>
T wrappingAdd(T a, T b) {
	using U = std::make_unsigned_t<T>;
	return static_cast<T>(static_cast<U>(a) + static_cast<U>(b));
}

template <typename T>
T wrappingSub(T a, T b) {
	using U = std::make_unsigned_t<T>;
	return static_cast<T>(static_cast<U>(a) - static_cast<U>(b));
}

template <typename T>
T wrappingMul(T a, T b) {
	using U = std::make_unsigned_t<T>;
	return static_cast<T>(static_cast<U>(a) * static_cast<U>(b));
}

template <typename T>
T wrappingNeg(T a) {
	using U = std::make_unsigned_t<T>;
	return static_cast<T>(-static_cast<U>(a));
}

/// Force a non-zero divisor; for signed T also steer away from -1, the only
/// divisor for which TYPE_MIN / divisor overflows (and traps on x86 idiv).
template <typename T>
T safeDivisor(T r) {
	T d = static_cast<T>(r | T(1));
	if constexpr (std::is_signed_v<T>) {
		if (d == T(-1)) {
			d = T(1);
		}
	}
	return d;
}

template <typename From>
From castThroughInt(From v, TypeId target) {
	return dispatchIntType(target, [&]<typename To>() -> From { return static_cast<From>(static_cast<To>(v)); });
}

template <typename From>
From castThroughFloat(From v, TypeId target) {
	return dispatchFloatType(target, [&]<typename To>() -> From { return static_cast<From>(static_cast<To>(v)); });
}

template <typename T>
T evalNativeInt(const Ast& ast, int idx, const std::array<T, NUM_PARAMS>& args) {
	const Node& n = ast.nodes[idx];
	switch (n.kind) {
	case Kind::Const:
		return unpackImm<T>(n.imm);
	case Kind::Param:
		return args[n.imm % NUM_PARAMS];
	case Kind::Select: {
		const T c = evalNativeInt<T>(ast, n.kid[0], args);
		return c != T(0) ? evalNativeInt<T>(ast, n.kid[1], args) : evalNativeInt<T>(ast, n.kid[2], args);
	}
	case Kind::If: {
		const T c = evalNativeInt<T>(ast, n.kid[0], args);
		// Mirror the traced kernel: the else-branch is evaluated unconditionally,
		// the then-branch only when the condition holds.
		const T e = evalNativeInt<T>(ast, n.kid[2], args);
		return c != T(0) ? evalNativeInt<T>(ast, n.kid[1], args) : e;
	}
	case Kind::Neg: {
		const T v = evalNativeInt<T>(ast, n.kid[0], args);
		if constexpr (std::is_signed_v<T>) {
			return wrappingNeg<T>(v);
		} else {
			return static_cast<T>(-v);
		}
	}
	case Kind::Not:
		return static_cast<T>(~evalNativeInt<T>(ast, n.kid[0], args));
	case Kind::Cast:
		return castThroughInt<T>(evalNativeInt<T>(ast, n.kid[0], args), static_cast<TypeId>(n.imm));
	default:
		break;
	}

	const T l = evalNativeInt<T>(ast, n.kid[0], args);
	const T r = evalNativeInt<T>(ast, n.kid[1], args);
	switch (n.kind) {
	case Kind::Add:
		if constexpr (std::is_signed_v<T>) {
			return wrappingAdd<T>(l, r);
		} else {
			return static_cast<T>(l + r);
		}
	case Kind::Sub:
		if constexpr (std::is_signed_v<T>) {
			return wrappingSub<T>(l, r);
		} else {
			return static_cast<T>(l - r);
		}
	case Kind::Mul:
		if constexpr (std::is_signed_v<T>) {
			return wrappingMul<T>(l, r);
		} else {
			return static_cast<T>(l * r);
		}
	case Kind::Div:
		return static_cast<T>(l / safeDivisor<T>(r));
	case Kind::Mod:
		return static_cast<T>(l % safeDivisor<T>(r));
	case Kind::And:
		return static_cast<T>(l & r);
	case Kind::Or:
		return static_cast<T>(l | r);
	case Kind::Xor:
		return static_cast<T>(l ^ r);
	case Kind::Shl:
		return static_cast<T>(l << (r & T(sizeof(T) * 8 - 1)));
	case Kind::Shr:
		return static_cast<T>(l >> (r & T(sizeof(T) * 8 - 1)));
	case Kind::Eq:
		return l == r ? T(1) : T(0);
	case Kind::Ne:
		return l != r ? T(1) : T(0);
	case Kind::Lt:
		return l < r ? T(1) : T(0);
	case Kind::Le:
		return l <= r ? T(1) : T(0);
	case Kind::Gt:
		return l > r ? T(1) : T(0);
	case Kind::Ge:
		return l >= r ? T(1) : T(0);
	default:
		return T(0); // unreachable
	}
}

template <typename T>
T evalNativeFloat(const Ast& ast, int idx, const std::array<T, NUM_PARAMS>& args) {
	const Node& n = ast.nodes[idx];
	switch (n.kind) {
	case Kind::Const:
		return unpackImm<T>(n.imm);
	case Kind::Param:
		return args[n.imm % NUM_PARAMS];
	case Kind::Select: {
		const T c = evalNativeFloat<T>(ast, n.kid[0], args);
		return c != T(0) ? evalNativeFloat<T>(ast, n.kid[1], args) : evalNativeFloat<T>(ast, n.kid[2], args);
	}
	case Kind::If: {
		const T c = evalNativeFloat<T>(ast, n.kid[0], args);
		const T e = evalNativeFloat<T>(ast, n.kid[2], args);
		return c != T(0) ? evalNativeFloat<T>(ast, n.kid[1], args) : e;
	}
	case Kind::Neg:
		return static_cast<T>(-evalNativeFloat<T>(ast, n.kid[0], args));
	case Kind::Cast:
		return castThroughFloat<T>(evalNativeFloat<T>(ast, n.kid[0], args), static_cast<TypeId>(n.imm));
	default:
		break;
	}

	const T l = evalNativeFloat<T>(ast, n.kid[0], args);
	const T r = evalNativeFloat<T>(ast, n.kid[1], args);
	switch (n.kind) {
	case Kind::Add:
		return static_cast<T>(l + r);
	case Kind::Sub:
		return static_cast<T>(l - r);
	case Kind::Mul:
		return static_cast<T>(l * r);
	case Kind::Div:
		return static_cast<T>(l / r); // well-defined IEEE 754: produces +-inf / NaN for r == 0
	case Kind::Eq:
		return l == r ? T(1) : T(0);
	case Kind::Ne:
		return l != r ? T(1) : T(0);
	case Kind::Lt:
		return l < r ? T(1) : T(0);
	case Kind::Le:
		return l <= r ? T(1) : T(0);
	case Kind::Gt:
		return l > r ? T(1) : T(0);
	case Kind::Ge:
		return l >= r ? T(1) : T(0);
	default:
		return T(0); // unreachable
	}
}

} // namespace detail

template <typename T>
T evalNative(const Ast& ast, int idx, const std::array<T, NUM_PARAMS>& args) {
	if constexpr (std::is_floating_point_v<T>) {
		return detail::evalNativeFloat<T>(ast, idx, args);
	} else {
		return detail::evalNativeInt<T>(ast, idx, args);
	}
}

template <typename T>
T evalNative(const Ast& ast, const std::array<T, NUM_PARAMS>& args) {
	return evalNative<T>(ast, ast.root, args);
}

} // namespace nautilus::fuzz
