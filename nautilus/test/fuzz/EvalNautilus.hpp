#pragma once

#include "Ast.hpp"
#include "Types.hpp"
#include <array>
#include <limits>
#include <nautilus/select.hpp>
#include <nautilus/val.hpp>
#include <type_traits>
#include <vector>

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
 *   - a Cast crossing the int/float domain boundary clamps its float->int
 *     leg via a real traced if/else-if/else chain (see castThroughTraced)
 *     rather than `select`, so the truncating cast is only ever reached
 *     (lowered/executed) for in-range inputs -- a select-based clamp would
 *     still *compute* the cast for out-of-range values, which various
 *     backends may treat as UB at the IR level even if the result is
 *     discarded,
 *   - `Loop` lowers to a real traced `for` over val<int32_t> with a mutated
 *     val<T> accumulator (the same pattern proven in
 *     test/common/LoopFunctions.hpp), so loop lowering itself is under test,
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

/// Traced mirror of EvalNative.hpp's clampFloatToInt, using a real branch
/// (not select) so the truncating cast is only ever lowered/executed for
/// in-range v. Reuses the exact same hiLimitExclusive/loLimitInclusive
/// boundary constants as the native oracle (Types.hpp) -- this boundary
/// math must never be re-derived independently.
template <typename From, typename To>
TracedValue<To> clampFloatToIntTraced(TracedValue<From> v) {
	TracedValue<To> r(To(0));
	if (v != v) { // NaN test; val<T> has no isnan(), but NaN != NaN is well-defined IEEE754
		r = TracedValue<To>(To(0));
	} else if (v < TracedValue<From>(loLimitInclusive<From, To>())) {
		r = TracedValue<To>(std::numeric_limits<To>::min());
	} else if (v >= TracedValue<From>(hiLimitExclusive<From, To>())) {
		r = TracedValue<To>(std::numeric_limits<To>::max());
	} else {
		r = static_cast<TracedValue<To>>(v);
	}
	return r;
}

template <typename From>
TracedValue<From> castThroughTraced(TracedValue<From> v, TypeId target) {
	if constexpr (std::is_floating_point_v<From>) {
		if (isFloatType(target)) {
			return dispatchFloatType(target, [&]<typename To>() -> TracedValue<From> {
				return static_cast<TracedValue<From>>(static_cast<TracedValue<To>>(v));
			});
		}
		return dispatchIntType(target, [&]<typename To>() -> TracedValue<From> {
			return static_cast<TracedValue<From>>(clampFloatToIntTraced<From, To>(v));
		});
	} else {
		if (!isFloatType(target)) {
			return dispatchIntType(target, [&]<typename To>() -> TracedValue<From> {
				return static_cast<TracedValue<From>>(static_cast<TracedValue<To>>(v));
			});
		}
		return dispatchFloatType(target, [&]<typename To>() -> TracedValue<From> {
			return clampFloatToIntTraced<To, From>(static_cast<TracedValue<To>>(v));
		});
	}
}

/// Traced mirror of EvalNative.hpp's clampTripCount: same
/// reinterpret-to-unsigned-then-modulo, narrowed to val<int32_t> only after
/// the modulo so both sides agree on the exact trip count.
template <typename T>
val<int32_t> clampTripCountTraced(TracedValue<T> raw) {
	TracedValue<uint64_t> u(uint64_t(0));
	if constexpr (std::is_floating_point_v<T>) {
		u = clampFloatToIntTraced<T, uint64_t>(raw);
	} else {
		u = static_cast<TracedValue<uint64_t>>(static_cast<TracedValue<std::make_unsigned_t<T>>>(raw));
	}
	TracedValue<uint64_t> trips = u % TracedValue<uint64_t>(uint64_t(LOOP_MAX_TRIPS + 1));
	return static_cast<val<int32_t>>(trips);
}

/// Mutable per-evaluation scratch state mirroring EvalNative.hpp's
/// EvalContext: the stack of enclosing traced Loop frames, innermost last.
template <typename T>
struct TracedLoopFrame {
	TracedValue<T> index;
	TracedValue<T> acc;
};

template <typename T>
struct TracedEvalContext {
	std::vector<TracedLoopFrame<T>> loopStack;
};

template <typename T>
TracedValue<T> evalNautilusInt(const Ast& ast, int idx, const TracedArgs<T>& args, TracedEvalContext<T>& ctx) {
	const Node& n = ast.nodes[idx];
	switch (n.kind) {
	case Kind::Const:
		return TracedValue<T>(unpackImm<T>(n.imm));
	case Kind::Param:
		return args[n.imm % NUM_PARAMS];
	case Kind::LoopIndex:
		return ctx.loopStack.back().index;
	case Kind::LoopAcc:
		return ctx.loopStack.back().acc;
	case Kind::Select: {
		TracedValue<T> c = evalNautilusInt<T>(ast, n.kid[0], args, ctx);
		TracedValue<T> t = evalNautilusInt<T>(ast, n.kid[1], args, ctx);
		TracedValue<T> f = evalNautilusInt<T>(ast, n.kid[2], args, ctx);
		return select(c != TracedValue<T>(T(0)), t, f);
	}
	case Kind::If: {
		TracedValue<T> c = evalNautilusInt<T>(ast, n.kid[0], args, ctx);
		// Else-branch first (unconditional), then conditionally overwrite with
		// the then-branch inside a real traced branch -> produces a phi/merge.
		TracedValue<T> result = evalNautilusInt<T>(ast, n.kid[2], args, ctx);
		if (c != TracedValue<T>(T(0))) {
			result = evalNautilusInt<T>(ast, n.kid[1], args, ctx);
		}
		return result;
	}
	case Kind::Loop: {
		TracedValue<T> rawCount = evalNautilusInt<T>(ast, n.kid[0], args, ctx);
		val<int32_t> trips = clampTripCountTraced<T>(rawCount);
		TracedValue<T> acc = evalNautilusInt<T>(ast, n.kid[1], args, ctx);
		for (val<int32_t> i = 0; i < trips; i = i + 1) {
			ctx.loopStack.push_back(TracedLoopFrame<T> {static_cast<TracedValue<T>>(i), acc});
			acc = evalNautilusInt<T>(ast, n.kid[2], args, ctx);
			ctx.loopStack.pop_back();
		}
		return acc;
	}
	case Kind::Neg:
		return -evalNautilusInt<T>(ast, n.kid[0], args, ctx);
	case Kind::Not:
		return ~evalNautilusInt<T>(ast, n.kid[0], args, ctx);
	case Kind::Cast:
		return castThroughTraced<T>(evalNautilusInt<T>(ast, n.kid[0], args, ctx), static_cast<TypeId>(n.imm));
	default:
		break;
	}

	TracedValue<T> l = evalNautilusInt<T>(ast, n.kid[0], args, ctx);
	TracedValue<T> r = evalNautilusInt<T>(ast, n.kid[1], args, ctx);
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
TracedValue<T> evalNautilusFloat(const Ast& ast, int idx, const TracedArgs<T>& args, TracedEvalContext<T>& ctx) {
	const Node& n = ast.nodes[idx];
	switch (n.kind) {
	case Kind::Const:
		return TracedValue<T>(unpackImm<T>(n.imm));
	case Kind::Param:
		return args[n.imm % NUM_PARAMS];
	case Kind::LoopIndex:
		return ctx.loopStack.back().index;
	case Kind::LoopAcc:
		return ctx.loopStack.back().acc;
	case Kind::Select: {
		TracedValue<T> c = evalNautilusFloat<T>(ast, n.kid[0], args, ctx);
		TracedValue<T> t = evalNautilusFloat<T>(ast, n.kid[1], args, ctx);
		TracedValue<T> f = evalNautilusFloat<T>(ast, n.kid[2], args, ctx);
		return select(c != TracedValue<T>(T(0)), t, f);
	}
	case Kind::If: {
		TracedValue<T> c = evalNautilusFloat<T>(ast, n.kid[0], args, ctx);
		TracedValue<T> result = evalNautilusFloat<T>(ast, n.kid[2], args, ctx);
		if (c != TracedValue<T>(T(0))) {
			result = evalNautilusFloat<T>(ast, n.kid[1], args, ctx);
		}
		return result;
	}
	case Kind::Loop: {
		TracedValue<T> rawCount = evalNautilusFloat<T>(ast, n.kid[0], args, ctx);
		val<int32_t> trips = clampTripCountTraced<T>(rawCount);
		TracedValue<T> acc = evalNautilusFloat<T>(ast, n.kid[1], args, ctx);
		for (val<int32_t> i = 0; i < trips; i = i + 1) {
			ctx.loopStack.push_back(TracedLoopFrame<T> {static_cast<TracedValue<T>>(i), acc});
			acc = evalNautilusFloat<T>(ast, n.kid[2], args, ctx);
			ctx.loopStack.pop_back();
		}
		return acc;
	}
	case Kind::Neg:
		return -evalNautilusFloat<T>(ast, n.kid[0], args, ctx);
	case Kind::Cast:
		return castThroughTraced<T>(evalNautilusFloat<T>(ast, n.kid[0], args, ctx), static_cast<TypeId>(n.imm));
	default:
		break;
	}

	TracedValue<T> l = evalNautilusFloat<T>(ast, n.kid[0], args, ctx);
	TracedValue<T> r = evalNautilusFloat<T>(ast, n.kid[1], args, ctx);
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
TracedValue<T> evalNautilus(const Ast& ast, const TracedArgs<T>& args) {
	detail::TracedEvalContext<T> ctx;
	if constexpr (std::is_floating_point_v<T>) {
		return detail::evalNautilusFloat<T>(ast, ast.root, args, ctx);
	} else {
		return detail::evalNautilusInt<T>(ast, ast.root, args, ctx);
	}
}

} // namespace nautilus::fuzz
