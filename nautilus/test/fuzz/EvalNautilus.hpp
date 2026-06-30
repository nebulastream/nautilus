#pragma once

#include "Ast.hpp"
#include "Types.hpp"
#include <array>
#include <limits>
#include <nautilus/select.hpp>
#include <nautilus/val.hpp>
#include <type_traits>

namespace nautilus::fuzz {

template <typename T>
using TracedValue = val<T>;

template <typename T>
using TracedArgs = std::array<TracedValue<T>, NUM_PARAMS>;

/**
 * @brief Realize an AST as traced Nautilus operations.
 *
 * Walks the exact same tree as evalNative, but emits val<T> operations so the
 * function can be traced, optimized and lowered through every backend. Each
 * construct is the well-defined counterpart of the native oracle:
 *   - comparisons become `select(cond, 1, 0)` (an IR compare + select),
 *   - `If` uses real traced control flow so branch lowering is exercised,
 *   - divisors are forced non-zero (and, for signed types, away from -1 --
 *     see EvalNative.hpp's safeDivisor for why) and shift amounts masked,
 *     exactly as native,
 *   - unlike the native oracle, signed arithmetic needs no extra wraparound
 *     handling here: producing the correct two's-complement result for
 *     `+ - * unary-` is exactly the behavior under test.
 */
namespace detail {

template <typename T>
TracedValue<T> safeDivisorTraced(TracedValue<T> r) {
	TracedValue<T> d = r | TracedValue<T>(T(1));
	if constexpr (std::is_signed_v<T>) {
		d = select(d == TracedValue<T>(T(-1)), TracedValue<T>(T(1)), d);
	}
	return d;
}

template <typename From>
TracedValue<From> castThroughIntTraced(TracedValue<From> v, TypeId target) {
	return dispatchIntType(target, [&]<typename To>() -> TracedValue<From> {
		return static_cast<TracedValue<From>>(static_cast<TracedValue<To>>(v));
	});
}

template <typename From>
TracedValue<From> castThroughFloatTraced(TracedValue<From> v, TypeId target) {
	return dispatchFloatType(target, [&]<typename To>() -> TracedValue<From> {
		return static_cast<TracedValue<From>>(static_cast<TracedValue<To>>(v));
	});
}

template <typename T>
TracedValue<T> evalNautilusInt(const Ast& ast, int idx, const TracedArgs<T>& args) {
	const Node& n = ast.nodes[idx];
	switch (n.kind) {
	case Kind::Const:
		return TracedValue<T>(unpackImm<T>(n.imm));
	case Kind::Param:
		return args[n.imm % NUM_PARAMS];
	case Kind::Select: {
		TracedValue<T> c = evalNautilusInt<T>(ast, n.kid[0], args);
		TracedValue<T> t = evalNautilusInt<T>(ast, n.kid[1], args);
		TracedValue<T> f = evalNautilusInt<T>(ast, n.kid[2], args);
		return select(c != TracedValue<T>(T(0)), t, f);
	}
	case Kind::If: {
		TracedValue<T> c = evalNautilusInt<T>(ast, n.kid[0], args);
		// Else-branch first (unconditional), then conditionally overwrite with
		// the then-branch inside a real traced branch -> produces a phi/merge.
		TracedValue<T> result = evalNautilusInt<T>(ast, n.kid[2], args);
		if (c != TracedValue<T>(T(0))) {
			result = evalNautilusInt<T>(ast, n.kid[1], args);
		}
		return result;
	}
	case Kind::Neg:
		return -evalNautilusInt<T>(ast, n.kid[0], args);
	case Kind::Not:
		return ~evalNautilusInt<T>(ast, n.kid[0], args);
	case Kind::Cast:
		return castThroughIntTraced<T>(evalNautilusInt<T>(ast, n.kid[0], args), static_cast<TypeId>(n.imm));
	default:
		break;
	}

	TracedValue<T> l = evalNautilusInt<T>(ast, n.kid[0], args);
	TracedValue<T> r = evalNautilusInt<T>(ast, n.kid[1], args);
	switch (n.kind) {
	case Kind::Add:
		return l + r;
	case Kind::Sub:
		return l - r;
	case Kind::Mul:
		return l * r;
	case Kind::Div:
		return l / safeDivisorTraced<T>(r);
	case Kind::Mod:
		return l % safeDivisorTraced<T>(r);
	case Kind::And:
		return l & r;
	case Kind::Or:
		return l | r;
	case Kind::Xor:
		return l ^ r;
	case Kind::Shl:
		return l << (r & TracedValue<T>(T(sizeof(T) * 8 - 1)));
	case Kind::Shr:
		return l >> (r & TracedValue<T>(T(sizeof(T) * 8 - 1)));
	case Kind::Eq:
		return select(l == r, TracedValue<T>(T(1)), TracedValue<T>(T(0)));
	case Kind::Ne:
		return select(l != r, TracedValue<T>(T(1)), TracedValue<T>(T(0)));
	case Kind::Lt:
		return select(l < r, TracedValue<T>(T(1)), TracedValue<T>(T(0)));
	case Kind::Le:
		return select(l <= r, TracedValue<T>(T(1)), TracedValue<T>(T(0)));
	case Kind::Gt:
		return select(l > r, TracedValue<T>(T(1)), TracedValue<T>(T(0)));
	case Kind::Ge:
		return select(l >= r, TracedValue<T>(T(1)), TracedValue<T>(T(0)));
	default:
		return TracedValue<T>(T(0)); // unreachable
	}
}

template <typename T>
TracedValue<T> evalNautilusFloat(const Ast& ast, int idx, const TracedArgs<T>& args) {
	const Node& n = ast.nodes[idx];
	switch (n.kind) {
	case Kind::Const:
		return TracedValue<T>(unpackImm<T>(n.imm));
	case Kind::Param:
		return args[n.imm % NUM_PARAMS];
	case Kind::Select: {
		TracedValue<T> c = evalNautilusFloat<T>(ast, n.kid[0], args);
		TracedValue<T> t = evalNautilusFloat<T>(ast, n.kid[1], args);
		TracedValue<T> f = evalNautilusFloat<T>(ast, n.kid[2], args);
		return select(c != TracedValue<T>(T(0)), t, f);
	}
	case Kind::If: {
		TracedValue<T> c = evalNautilusFloat<T>(ast, n.kid[0], args);
		TracedValue<T> result = evalNautilusFloat<T>(ast, n.kid[2], args);
		if (c != TracedValue<T>(T(0))) {
			result = evalNautilusFloat<T>(ast, n.kid[1], args);
		}
		return result;
	}
	case Kind::Neg:
		return -evalNautilusFloat<T>(ast, n.kid[0], args);
	case Kind::Cast:
		return castThroughFloatTraced<T>(evalNautilusFloat<T>(ast, n.kid[0], args), static_cast<TypeId>(n.imm));
	default:
		break;
	}

	TracedValue<T> l = evalNautilusFloat<T>(ast, n.kid[0], args);
	TracedValue<T> r = evalNautilusFloat<T>(ast, n.kid[1], args);
	switch (n.kind) {
	case Kind::Add:
		return l + r;
	case Kind::Sub:
		return l - r;
	case Kind::Mul:
		return l * r;
	case Kind::Div:
		return l / r; // well-defined IEEE 754: produces +-inf / NaN for r == 0
	case Kind::Eq:
		return select(l == r, TracedValue<T>(T(1)), TracedValue<T>(T(0)));
	case Kind::Ne:
		return select(l != r, TracedValue<T>(T(1)), TracedValue<T>(T(0)));
	case Kind::Lt:
		return select(l < r, TracedValue<T>(T(1)), TracedValue<T>(T(0)));
	case Kind::Le:
		return select(l <= r, TracedValue<T>(T(1)), TracedValue<T>(T(0)));
	case Kind::Gt:
		return select(l > r, TracedValue<T>(T(1)), TracedValue<T>(T(0)));
	case Kind::Ge:
		return select(l >= r, TracedValue<T>(T(1)), TracedValue<T>(T(0)));
	default:
		return TracedValue<T>(T(0)); // unreachable
	}
}

} // namespace detail

template <typename T>
TracedValue<T> evalNautilus(const Ast& ast, int idx, const TracedArgs<T>& args) {
	if constexpr (std::is_floating_point_v<T>) {
		return detail::evalNautilusFloat<T>(ast, idx, args);
	} else {
		return detail::evalNautilusInt<T>(ast, idx, args);
	}
}

template <typename T>
TracedValue<T> evalNautilus(const Ast& ast, const TracedArgs<T>& args) {
	return evalNautilus<T>(ast, ast.root, args);
}

} // namespace nautilus::fuzz
