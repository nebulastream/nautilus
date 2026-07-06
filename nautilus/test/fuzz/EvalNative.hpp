#pragma once

#include "Ast.hpp"
#include "Callees.hpp"
#include "Types.hpp"
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <span>
#include <type_traits>
#include <vector>

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
 *   - float->int casts (directly, or as one leg of a Cast node routed
 *     through the other domain) clamp out-of-range/NaN inputs to a defined
 *     result instead of invoking C++'s float-to-int UB; int->float casts
 *     need no such handling (precision loss only, never UB),
 *   - Loop trip counts are clamped to [0, LOOP_MAX_TRIPS] via the same
 *     unsigned-reinterpret-then-modulo on both the native and traced side,
 *   - the float domain otherwise needs none of the above: IEEE 754
 *     arithmetic (including division by zero, producing +-inf/NaN) is
 *     fully defined.
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

/// The only UB-prone leg of any cast crossing the int/float domain: clamp a
/// float into To's representable range before truncating. Thin wrapper
/// around Types.hpp's convertClamped (shared with EvalNautilus.hpp and the
/// harness's kernel-boundary conversions so the boundary math can't drift
/// between them). NaN maps to 0; out-of-range maps to To's min/max.
template <typename From, typename To>
To clampFloatToInt(From v) {
	return convertClamped<From, To>(v);
}

/// Unified "cast through" dispatch: (From)(To)v for any target TypeId,
/// same-domain or crossing int<->float. Exactly one leg of a
/// domain-crossing round trip is float->int (the only UB-prone direction);
/// that leg always goes through clampFloatToInt, the other leg is a plain
/// (always-safe) static_cast.
template <typename From>
From castThrough(From v, TypeId target) {
	if constexpr (std::is_floating_point_v<From>) {
		if (isFloatType(target)) {
			return dispatchFloatType(target,
			                         [&]<typename To>() -> From { return static_cast<From>(static_cast<To>(v)); });
		}
		// From=float, target=int: clamp v into the integer range, then cast
		// that integer back up to From (always safe, no clamp needed).
		return dispatchIntType(target,
		                       [&]<typename To>() -> From { return static_cast<From>(clampFloatToInt<From, To>(v)); });
	} else {
		if (!isFloatType(target)) {
			return dispatchIntType(target,
			                       [&]<typename To>() -> From { return static_cast<From>(static_cast<To>(v)); });
		}
		// From=int, target=float: int->float is safe; the *return* leg
		// (float back down to the integer From) is what needs clamping.
		return dispatchFloatType(target,
		                         [&]<typename To>() -> From { return clampFloatToInt<To, From>(static_cast<To>(v)); });
	}
}

/// Reduce a raw loop-count value of type T to a small, defined trip count in
/// [0, LOOP_MAX_TRIPS]. Reinterprets to the unsigned-width representation
/// first (well-defined bit-pattern reinterpret for ints; clamp-through-u64
/// for floats) so the final modulo is taken in the same domain on both the
/// native and traced sides, then narrows only at the very end.
template <typename T>
int clampTripCount(T raw) {
	uint64_t u;
	if constexpr (std::is_floating_point_v<T>) {
		u = clampFloatToInt<T, uint64_t>(raw);
	} else {
		u = static_cast<uint64_t>(static_cast<std::make_unsigned_t<T>>(raw));
	}
	return static_cast<int>(u % static_cast<uint64_t>(LOOP_MAX_TRIPS + 1));
}

/// Reduce a raw pointer-offset value of type T to a defined index in
/// [0, BUFFER_ELEMS), via the identical reinterpret-then-modulo recipe as
/// clampTripCount, just against BUFFER_ELEMS instead of LOOP_MAX_TRIPS + 1.
/// Shared verbatim (mod constant aside) with clampBufferIndexTraced in
/// EvalNautilus.hpp so a pointer built from the same raw offset always lands
/// on the same buffer slot on both the native and traced sides.
template <typename T>
int clampBufferIndex(T raw) {
	uint64_t u;
	if constexpr (std::is_floating_point_v<T>) {
		u = clampFloatToInt<T, uint64_t>(raw);
	} else {
		u = static_cast<uint64_t>(static_cast<std::make_unsigned_t<T>>(raw));
	}
	return static_cast<int>(u % static_cast<uint64_t>(BUFFER_ELEMS));
}

/// Mutable per-evaluation scratch state, separate from the read-only kernel
/// `args`: the stack of enclosing Loop frames, innermost last. LoopIndex /
/// LoopAcc always read `loopStack.back()` -- nesting is plain shadowing, the
/// generator guarantees the stack is non-empty wherever they appear.
template <typename T>
struct LoopFrame {
	T index;
	T acc;
};

/// One signal frame per enclosing *breakable* (Loop/While, never StaticLoop --
/// see generateNode's breakDepth comment in Ast.hpp) loop, innermost last.
/// Kind::LoopBreak/LoopContinue set a flag on `signalStack.back()`; the
/// enclosing Loop/While case checks and clears it right after each iteration's
/// body finishes. A dedicated stack (rather than one flat pair of flags) is
/// what keeps a signal generated inside a nested breakable loop from leaking
/// into an outer loop's own check -- each loop only ever sees its own frame.
struct LoopSignal {
	bool breakRequested = false;
	bool continueRequested = false;
};

/// The shared buffer every generated program's pointer-domain nodes index
/// into (see Kind::PtrBase in Ast.hpp). A raw, non-owning pointer: the
/// harness owns the storage and resets its contents between the native-oracle
/// run and each backend run so Store side effects from one run never leak
/// into the next.
template <typename T>
struct EvalContext {
	std::vector<LoopFrame<T>> loopStack;
	std::vector<LoopSignal> signalStack;
	std::array<T, BUFFER_ELEMS>* memory = nullptr;
};

template <typename T>
T evalNativeGeneric(const Ast& ast, int idx, std::span<const T> args, EvalContext<T>& ctx);

/// Evaluate a pointer-domain node (Kind::PtrBase/PtrAdd/PtrSub, see Ast.hpp)
/// to a buffer index. Always in [0, BUFFER_ELEMS) by construction --
/// generatePtrNode never nests, so there is no "current pointer" state to
/// track, just a single offset off of index 0 or BUFFER_ELEMS - 1.
template <typename T>
int evalNativePtr(const Ast& ast, int idx, std::span<const T> args, EvalContext<T>& ctx) {
	const Node& n = ast.nodes[idx];
	switch (n.kind) {
	case Kind::PtrAdd:
		return clampBufferIndex<T>(evalNativeGeneric<T>(ast, n.kid[0], args, ctx));
	case Kind::PtrSub:
		return (BUFFER_ELEMS - 1) - clampBufferIndex<T>(evalNativeGeneric<T>(ast, n.kid[0], args, ctx));
	case Kind::PtrBase:
	default:
		return 0;
	}
}

/// Shared body of the six Ptr* comparison kinds: both native evaluators
/// (formerly evalNativeInt/evalNativeFloat, now a single evalNativeGeneric)
/// reduced their pointer-domain kids to plain buffer indices and compared
/// those, so the comparison itself never depended on T -- pulled out once
/// here instead of appearing twice.
inline bool comparePtrIndices(Kind kind, int a, int b) {
	switch (kind) {
	case Kind::PtrEq:
		return a == b;
	case Kind::PtrNe:
		return a != b;
	case Kind::PtrLt:
		return a < b;
	case Kind::PtrLe:
		return a <= b;
	case Kind::PtrGt:
		return a > b;
	default: // Kind::PtrGe
		return a >= b;
	}
}

/// Single evaluator for both the integer and float domains. The two used to
/// be separate functions (evalNativeInt/evalNativeFloat) that duplicated
/// every control-flow, memory, call and comparison case verbatim -- the
/// exact kind of duplication that let the Select short-circuit bug (see
/// README.md) need fixing in two places instead of one. The domain still
/// matters for a handful of operations (float has no bitwise/modulo ops;
/// signed integer arithmetic needs the wraparound helpers; float division
/// needs no non-zero-divisor guard), so those cases branch internally via
/// `if constexpr` -- the same pattern already used by safeDivisor/castThrough
/// above -- rather than being split back into two functions.
template <typename T>
T evalNativeGeneric(const Ast& ast, int idx, std::span<const T> args, EvalContext<T>& ctx) {
	const Node& n = ast.nodes[idx];
	switch (n.kind) {
	case Kind::Const:
		return unpackImm<T>(n.imm);
	case Kind::Param:
		return args[n.imm % args.size()];
	case Kind::LoopIndex:
		return ctx.loopStack.back().index;
	case Kind::LoopAcc:
		return ctx.loopStack.back().acc;
	case Kind::Select: {
		// Both branches are evaluated unconditionally, matching the traced
		// kernel's `select(cond, t, f)` (a data mux, not real branching -- see
		// EvalNautilus.hpp). This must hold even though a plain C++ ternary
		// would short-circuit: now that Store can appear inside t/f, only
		// evaluating one side here would silently skip the other's memory
		// side effect while the traced kernel still performs it, breaking
		// oracle/traced parity for any program that stores inside a Select
		// branch.
		const T c = evalNativeGeneric<T>(ast, n.kid[0], args, ctx);
		const T t = evalNativeGeneric<T>(ast, n.kid[1], args, ctx);
		const T f = evalNativeGeneric<T>(ast, n.kid[2], args, ctx);
		return c != T(0) ? t : f;
	}
	case Kind::If: {
		const T c = evalNativeGeneric<T>(ast, n.kid[0], args, ctx);
		// Mirror the traced kernel: the else-branch is evaluated unconditionally,
		// the then-branch only when the condition holds.
		const T e = evalNativeGeneric<T>(ast, n.kid[2], args, ctx);
		return c != T(0) ? evalNativeGeneric<T>(ast, n.kid[1], args, ctx) : e;
	}
	case Kind::Loop: {
		const T rawCount = evalNativeGeneric<T>(ast, n.kid[0], args, ctx);
		const int trips = clampTripCount<T>(rawCount);
		T acc = evalNativeGeneric<T>(ast, n.kid[1], args, ctx);
		ctx.signalStack.push_back(LoopSignal {});
		for (int i = 0; i < trips; ++i) {
			ctx.loopStack.push_back(LoopFrame<T> {static_cast<T>(i), acc});
			acc = evalNativeGeneric<T>(ast, n.kid[2], args, ctx);
			ctx.loopStack.pop_back();
			LoopSignal& sig = ctx.signalStack.back();
			const bool doBreak = sig.breakRequested;
			const bool doContinue = sig.continueRequested;
			sig.breakRequested = false;
			sig.continueRequested = false;
			if (doBreak) {
				break;
			}
			if (doContinue) {
				continue;
			}
		}
		ctx.signalStack.pop_back();
		return acc;
	}
	// Runtime-computed, unclamped-condition fold (see Kind::While in
	// Ast.hpp): the trip cap bounds the *for* below regardless of what cond
	// evaluates to, so termination never depends on the data-dependent
	// condition -- only how many of the (up to) LOOP_MAX_TRIPS iterations
	// actually run does.
	case Kind::While: {
		T acc = evalNativeGeneric<T>(ast, n.kid[0], args, ctx);
		ctx.signalStack.push_back(LoopSignal {});
		for (int trips = 0; trips < LOOP_MAX_TRIPS; ++trips) {
			ctx.loopStack.push_back(LoopFrame<T> {static_cast<T>(trips), acc});
			const T condVal = evalNativeGeneric<T>(ast, n.kid[1], args, ctx);
			if (condVal == T(0)) {
				ctx.loopStack.pop_back();
				break;
			}
			acc = evalNativeGeneric<T>(ast, n.kid[2], args, ctx);
			ctx.loopStack.pop_back();
			LoopSignal& sig = ctx.signalStack.back();
			const bool doBreak = sig.breakRequested;
			const bool doContinue = sig.continueRequested;
			sig.breakRequested = false;
			sig.continueRequested = false;
			if (doBreak) {
				break;
			}
			if (doContinue) {
				continue;
			}
		}
		ctx.signalStack.pop_back();
		return acc;
	}
	// Conditional early exit: `value` is always evaluated (side-effect
	// parity, same convention as Select), and if `cond` fires, the innermost
	// breakable loop's signal is set instead of returning `value`.
	case Kind::LoopBreak: {
		const T c = evalNativeGeneric<T>(ast, n.kid[0], args, ctx);
		if (c != T(0)) {
			ctx.signalStack.back().breakRequested = true;
			return ctx.loopStack.back().acc;
		}
		return evalNativeGeneric<T>(ast, n.kid[1], args, ctx);
	}
	// Conditional skip-to-next-iteration: same shape as LoopBreak, but
	// signals continue instead of break.
	case Kind::LoopContinue: {
		const T c = evalNativeGeneric<T>(ast, n.kid[0], args, ctx);
		if (c != T(0)) {
			ctx.signalStack.back().continueRequested = true;
			return ctx.loopStack.back().acc;
		}
		return evalNativeGeneric<T>(ast, n.kid[1], args, ctx);
	}
	case Kind::StaticLoop: {
		const int trips = static_cast<int>(n.imm);
		T acc = evalNativeGeneric<T>(ast, n.kid[0], args, ctx);
		for (int i = 0; i < trips; ++i) {
			ctx.loopStack.push_back(LoopFrame<T> {static_cast<T>(i), acc});
			acc = evalNativeGeneric<T>(ast, n.kid[1], args, ctx);
			ctx.loopStack.pop_back();
		}
		return acc;
	}
	case Kind::Neg: {
		const T v = evalNativeGeneric<T>(ast, n.kid[0], args, ctx);
		if constexpr (std::is_floating_point_v<T>) {
			return static_cast<T>(-v);
		} else if constexpr (std::is_signed_v<T>) {
			return wrappingNeg<T>(v);
		} else {
			return static_cast<T>(-v);
		}
	}
	case Kind::Not:
		// Integer domain only (FLOAT_KINDS never generates Not); guarded so the
		// bitwise-complement expression doesn't need to compile for float T.
		if constexpr (!std::is_floating_point_v<T>) {
			return static_cast<T>(~evalNativeGeneric<T>(ast, n.kid[0], args, ctx));
		} else {
			__builtin_unreachable();
		}
	case Kind::LNot:
		return evalNativeGeneric<T>(ast, n.kid[0], args, ctx) == T(0) ? T(1) : T(0);
	case Kind::Cast:
		return castThrough<T>(evalNativeGeneric<T>(ast, n.kid[0], args, ctx), static_cast<TypeId>(n.imm));
	case Kind::Load: {
		const int i = evalNativePtr<T>(ast, n.kid[0], args, ctx);
		return (*ctx.memory)[static_cast<size_t>(i)];
	}
	case Kind::Store: {
		const int i = evalNativePtr<T>(ast, n.kid[0], args, ctx);
		const T v = evalNativeGeneric<T>(ast, n.kid[1], args, ctx);
		(*ctx.memory)[static_cast<size_t>(i)] = v;
		return v;
	}
	case Kind::PtrToInt: {
		const int i = evalNativePtr<T>(ast, n.kid[0], args, ctx);
		return static_cast<T>(reinterpret_cast<uintptr_t>(&(*ctx.memory)[static_cast<size_t>(i)]));
	}
	case Kind::PtrEq:
	case Kind::PtrNe:
	case Kind::PtrLt:
	case Kind::PtrLe:
	case Kind::PtrGt:
	case Kind::PtrGe: {
		const int a = evalNativePtr<T>(ast, n.kid[0], args, ctx);
		const int b = evalNativePtr<T>(ast, n.kid[1], args, ctx);
		return comparePtrIndices(n.kind, a, b) ? T(1) : T(0);
	}
	default:
		break;
	}

	const T l = evalNativeGeneric<T>(ast, n.kid[0], args, ctx);
	const T r = evalNativeGeneric<T>(ast, n.kid[1], args, ctx);
	switch (n.kind) {
	case Kind::Add:
		if constexpr (std::is_floating_point_v<T>) {
			return static_cast<T>(l + r);
		} else if constexpr (std::is_signed_v<T>) {
			return wrappingAdd<T>(l, r);
		} else {
			return static_cast<T>(l + r);
		}
	case Kind::Sub:
		if constexpr (std::is_floating_point_v<T>) {
			return static_cast<T>(l - r);
		} else if constexpr (std::is_signed_v<T>) {
			return wrappingSub<T>(l, r);
		} else {
			return static_cast<T>(l - r);
		}
	case Kind::Mul:
		if constexpr (std::is_floating_point_v<T>) {
			return static_cast<T>(l * r);
		} else if constexpr (std::is_signed_v<T>) {
			return wrappingMul<T>(l, r);
		} else {
			return static_cast<T>(l * r);
		}
	case Kind::Div:
		if constexpr (std::is_floating_point_v<T>) {
			return static_cast<T>(l / r); // well-defined IEEE 754: produces +-inf / NaN for r == 0
		} else {
			return static_cast<T>(l / safeDivisor<T>(r));
		}
	case Kind::Mod:
		if constexpr (!std::is_floating_point_v<T>) {
			return static_cast<T>(l % safeDivisor<T>(r));
		} else {
			__builtin_unreachable();
		}
	case Kind::And:
		if constexpr (!std::is_floating_point_v<T>) {
			return static_cast<T>(l & r);
		} else {
			__builtin_unreachable();
		}
	case Kind::Or:
		if constexpr (!std::is_floating_point_v<T>) {
			return static_cast<T>(l | r);
		} else {
			__builtin_unreachable();
		}
	case Kind::Xor:
		if constexpr (!std::is_floating_point_v<T>) {
			return static_cast<T>(l ^ r);
		} else {
			__builtin_unreachable();
		}
	case Kind::Shl:
		if constexpr (!std::is_floating_point_v<T>) {
			return static_cast<T>(l << (r & T(sizeof(T) * 8 - 1)));
		} else {
			__builtin_unreachable();
		}
	case Kind::Shr:
		if constexpr (!std::is_floating_point_v<T>) {
			return static_cast<T>(l >> (r & T(sizeof(T) * 8 - 1)));
		} else {
			__builtin_unreachable();
		}
	// LAnd/LOr sit in this binary tail so l and r are both already evaluated
	// unconditionally -- deliberate, see the Kind::LAnd comment in Ast.hpp.
	case Kind::LAnd:
		return (l != T(0)) && (r != T(0)) ? T(1) : T(0);
	case Kind::LOr:
		return (l != T(0)) || (r != T(0)) ? T(1) : T(0);
	case Kind::Call:
		// Direct call of the same instantiation the traced kernel invoke()s.
		return n.imm % NUM_CALLEES == 0 ? calleeMix<T>(l, r) : calleeMin<T>(l, r);
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
T evalNative(const Ast& ast, std::span<const T> args, std::array<T, BUFFER_ELEMS>& memory) {
	detail::EvalContext<T> ctx;
	ctx.memory = &memory;
	return detail::evalNativeGeneric<T>(ast, ast.root, args, ctx);
}

} // namespace nautilus::fuzz
