#pragma once

#include "Ast.hpp"
#include "Callees.hpp"
#include "Types.hpp"
#include <array>
#include <limits>
#include <nautilus/function.hpp>
#include <nautilus/select.hpp>
#include <nautilus/static.hpp>
#include <nautilus/val.hpp>
#include <nautilus/val_enum.hpp>
#include <nautilus/val_ptr.hpp>
#include <span>
#include <type_traits>
#include <vector>

namespace nautilus::fuzz {

template <typename T>
using TracedValue = val<T>;

/// A kernel's logical value-domain arguments (the AST's Kind::Param domain),
/// sized to whatever arity the current signature shape uses -- independent of
/// the concrete kernel signature the harness compiles the AST into (which may
/// carry extra/differently-typed parameters converted into this domain at
/// the boundary, see convertClampedTraced below).
template <typename T, std::size_t N>
using TracedArgs = std::array<TracedValue<T>, N>;

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

/// Traced mirror of Types.hpp's clampFloatToInt, using a real branch (not
/// select) so the truncating cast is only ever lowered/executed for in-range
/// v. Reuses the exact same hiLimitExclusive/loLimitInclusive boundary
/// constants as the native oracle (Types.hpp) -- this boundary math must
/// never be re-derived independently.
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

/// Traced mirror of Callees.hpp's toCrossNative: marshal a T-typed kid value
/// into TracedValue<CrossType<T>> before invoke()ing calleeMixedTypes. Same
/// direction split as the native side -- int->double is always a safe widen,
/// float->int32_t is the only UB-prone leg and goes through the real-branch
/// clampFloatToIntTraced above instead of a raw cast.
template <typename T>
TracedValue<CrossType<T>> toCrossTraced(TracedValue<T> v) {
	if constexpr (std::is_floating_point_v<T>) {
		return clampFloatToIntTraced<T, CrossType<T>>(v);
	} else {
		return static_cast<TracedValue<CrossType<T>>>(v);
	}
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

/// Decodes the probability hint a bool-domain If/Select node may carry (see
/// the `if constexpr (std::is_same_v<T, bool>)` block in Ast.hpp's
/// generateNode): bit 0 of `imm` flags whether a hint is present, bits 8-15
/// hold the probability quantized to a byte. A no-op for every other kind/
/// domain, so this can be called unconditionally from evalNautilusGeneric's
/// Select/If cases regardless of T.
template <typename T>
void applyProbabilityHint(const Node& n, val<bool>& cond) {
	if constexpr (std::is_same_v<T, bool>) {
		if ((n.imm & 0x1) != 0) {
			const auto probByte = static_cast<uint8_t>((n.imm >> 8) & 0xFF);
			cond.setIsTrueProbability(static_cast<TrueProbability>(probByte) / 255.0);
		}
	}
}

/// Traced mirror of EvalNative.hpp's clampBufferIndex: same recipe as
/// clampTripCountTraced, just against BUFFER_ELEMS instead of
/// LOOP_MAX_TRIPS + 1, so a pointer built from the same raw offset lands on
/// the same buffer slot as the native oracle.
template <typename T>
val<int32_t> clampBufferIndexTraced(TracedValue<T> raw) {
	TracedValue<uint64_t> u(uint64_t(0));
	if constexpr (std::is_floating_point_v<T>) {
		u = clampFloatToIntTraced<T, uint64_t>(raw);
	} else {
		u = static_cast<TracedValue<uint64_t>>(static_cast<TracedValue<std::make_unsigned_t<T>>>(raw));
	}
	TracedValue<uint64_t> index = u % TracedValue<uint64_t>(uint64_t(BUFFER_ELEMS));
	return static_cast<val<int32_t>>(index);
}

/// Mutable per-evaluation scratch state mirroring EvalNative.hpp's
/// EvalContext: the stack of enclosing traced Loop frames (innermost last),
/// plus the base/one-past-last pointer into the shared buffer that every
/// pointer-domain node (Kind::PtrBase/PtrAdd/PtrSub) is built from.
template <typename T>
struct TracedLoopFrame {
	TracedValue<T> index;
	TracedValue<T> acc;
};

/// Traced mirror of EvalNative.hpp's LoopSignal: one frame per enclosing
/// breakable (Loop/While) loop, innermost last, holding real val<bool>
/// signals so the enclosing loop's post-body check (`if (sig.breakRequested)
/// break;` / `if (sig.continueRequested) continue;`) is itself a real traced
/// conditional break/continue -- exercising the exit-edge / back-edge merge
/// the fuzzer extension targets.
template <typename T>
struct TracedLoopSignal {
	val<bool> breakRequested;
	val<bool> continueRequested;
};

template <typename T>
struct TracedEvalContext {
	std::vector<TracedLoopFrame<T>> loopStack;
	std::vector<TracedLoopSignal<T>> signalStack;
	val<T*> basePtr;
	val<T*> lastPtr; // basePtr + (BUFFER_ELEMS - 1), precomputed once
};

template <typename T>
TracedValue<T> evalNautilusGeneric(const Ast& ast, int idx, std::span<const TracedValue<T>> args,
                                   TracedEvalContext<T>& ctx);

template <typename T>
TracedValue<T> evalNautilusCall(const Ast& ast, const Node& n, std::span<const TracedValue<T>> args,
                                TracedEvalContext<T>& ctx);

/// Traced mirror of EvalNative.hpp's evalNativePtr: evaluates a
/// pointer-domain node to a real val<T*>, using the exact same
/// operator+/operator- val<T*> arithmetic under test.
template <typename T>
val<T*> evalNautilusPtr(const Ast& ast, int idx, std::span<const TracedValue<T>> args, TracedEvalContext<T>& ctx) {
	const Node& n = ast.nodes[idx];
	switch (n.kind) {
	case Kind::PtrAdd:
		if constexpr (std::is_same_v<T, bool>) {
			// generatePtrNode<bool> always forces a PtrBase leaf (val<bool>
			// has no arithmetic to build an offset from), so this is never
			// actually reached; guarded so clampBufferIndexTraced<bool> (which
			// would need a val<bool> -> val<uint64_t> conversion that doesn't
			// exist) is never instantiated.
			__builtin_unreachable();
		} else {
			val<int32_t> off = clampBufferIndexTraced<T>(evalNautilusGeneric<T>(ast, n.kid[0], args, ctx));
			return ctx.basePtr + off;
		}
	case Kind::PtrSub:
		if constexpr (std::is_same_v<T, bool>) {
			__builtin_unreachable();
		} else {
			val<int32_t> off = clampBufferIndexTraced<T>(evalNautilusGeneric<T>(ast, n.kid[0], args, ctx));
			return ctx.lastPtr - off;
		}
	case Kind::PtrBase:
	default:
		return ctx.basePtr;
	}
}

/// Traced mirror of EvalNative.hpp's comparePtrIndices: the shared body of
/// the six Ptr* comparison kinds, producing a real val<bool> from the two
/// val<T*> operator overloads under test.
template <typename T>
val<bool> comparePtrTraced(Kind kind, val<T*> a, val<T*> b) {
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

/// Single traced evaluator for both the integer and float domains -- the
/// traced mirror of EvalNative.hpp's evalNativeGeneric, merged from the
/// former evalNautilusInt/evalNautilusFloat pair for the same reason (see
/// evalNativeGeneric's comment): every control-flow, memory, call and
/// comparison case was duplicated verbatim between the two. Only a handful
/// of operations are genuinely domain-specific (float has no bitwise/modulo
/// ops; float division needs no non-zero-divisor guard), gated via
/// `if constexpr`.
template <typename T>
TracedValue<T> evalNautilusGeneric(const Ast& ast, int idx, std::span<const TracedValue<T>> args,
                                   TracedEvalContext<T>& ctx) {
	const Node& n = ast.nodes[idx];
	switch (n.kind) {
	case Kind::Const:
		return TracedValue<T>(unpackImm<T>(n.imm));
	case Kind::Param:
		return args[n.imm % args.size()];
	case Kind::LoopIndex:
		return ctx.loopStack.back().index;
	case Kind::LoopAcc:
		return ctx.loopStack.back().acc;
	case Kind::Call:
		return evalNautilusCall<T>(ast, n, args, ctx);
	case Kind::Select: {
		TracedValue<T> c = evalNautilusGeneric<T>(ast, n.kid[0], args, ctx);
		TracedValue<T> t = evalNautilusGeneric<T>(ast, n.kid[1], args, ctx);
		TracedValue<T> f = evalNautilusGeneric<T>(ast, n.kid[2], args, ctx);
		val<bool> cond = c != TracedValue<T>(T(0));
		applyProbabilityHint<T>(n, cond);
		return select(cond, t, f);
	}
	case Kind::If: {
		TracedValue<T> c = evalNautilusGeneric<T>(ast, n.kid[0], args, ctx);
		// Else-branch first (unconditional), then conditionally overwrite with
		// the then-branch inside a real traced branch -> produces a phi/merge.
		TracedValue<T> result = evalNautilusGeneric<T>(ast, n.kid[2], args, ctx);
		val<bool> cond = c != TracedValue<T>(T(0));
		applyProbabilityHint<T>(n, cond);
		if (cond) {
			result = evalNautilusGeneric<T>(ast, n.kid[1], args, ctx);
		}
		return result;
	}
	case Kind::Loop:
		if constexpr (std::is_same_v<T, bool>) {
			// Loop isn't in BOOL_KINDS (val<bool> has no arithmetic to build a
			// trip count from); guarded so clampTripCountTraced<bool> (which
			// would need a val<bool> -> val<uint64_t> conversion that doesn't
			// exist) is never instantiated.
			__builtin_unreachable();
		} else {
			TracedValue<T> rawCount = evalNautilusGeneric<T>(ast, n.kid[0], args, ctx);
			val<int32_t> trips = clampTripCountTraced<T>(rawCount);
			TracedValue<T> acc = evalNautilusGeneric<T>(ast, n.kid[1], args, ctx);
			ctx.signalStack.push_back(TracedLoopSignal<T> {});
			for (val<int32_t> i = 0; i < trips; i = i + 1) {
				ctx.loopStack.push_back(TracedLoopFrame<T> {static_cast<TracedValue<T>>(i), acc});
				acc = evalNautilusGeneric<T>(ast, n.kid[2], args, ctx);
				ctx.loopStack.pop_back();
				TracedLoopSignal<T>& sig = ctx.signalStack.back();
				val<bool> doBreak = sig.breakRequested;
				val<bool> doContinue = sig.continueRequested;
				sig.breakRequested = val<bool>(false);
				sig.continueRequested = val<bool>(false);
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
	// Traced mirror of EvalNative.hpp's Kind::While: a real traced `for`
	// bounded by the same LOOP_MAX_TRIPS cap (never unrolled), with a real
	// traced conditional exit driven by `cond` -- exercising loop lowering
	// with a data-dependent, hard-capped exit rather than a fixed count.
	// Not in BOOL_KINDS either, but (unlike Loop) needs no gating: the trip
	// bound is a fixed LOOP_MAX_TRIPS, not a T-derived clamp, and the exit
	// condition is a plain `==`/`!=` comparison against T(0), which already
	// compiles for every domain (see Kind::If above).
	case Kind::While: {
		TracedValue<T> acc = evalNautilusGeneric<T>(ast, n.kid[0], args, ctx);
		ctx.signalStack.push_back(TracedLoopSignal<T> {});
		for (val<int32_t> trips = 0; trips < val<int32_t>(LOOP_MAX_TRIPS); trips = trips + 1) {
			ctx.loopStack.push_back(TracedLoopFrame<T> {static_cast<TracedValue<T>>(trips), acc});
			TracedValue<T> condVal = evalNautilusGeneric<T>(ast, n.kid[1], args, ctx);
			if (condVal == TracedValue<T>(T(0))) {
				ctx.loopStack.pop_back();
				break;
			}
			acc = evalNautilusGeneric<T>(ast, n.kid[2], args, ctx);
			ctx.loopStack.pop_back();
			TracedLoopSignal<T>& sig = ctx.signalStack.back();
			val<bool> doBreak = sig.breakRequested;
			val<bool> doContinue = sig.continueRequested;
			sig.breakRequested = val<bool>(false);
			sig.continueRequested = val<bool>(false);
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
	// parity, same convention as Select), and if `cond` fires (inside a real
	// traced if, see the Kind::LoopBreak comment in Ast.hpp), the innermost
	// breakable loop's real val<bool> signal is set instead.
	case Kind::LoopBreak: {
		TracedValue<T> c = evalNautilusGeneric<T>(ast, n.kid[0], args, ctx);
		if (c != TracedValue<T>(T(0))) {
			ctx.signalStack.back().breakRequested = val<bool>(true);
			return ctx.loopStack.back().acc;
		}
		return evalNautilusGeneric<T>(ast, n.kid[1], args, ctx);
	}
	// Conditional skip-to-next-iteration: same shape as LoopBreak, but sets
	// the continue signal instead.
	case Kind::LoopContinue: {
		TracedValue<T> c = evalNautilusGeneric<T>(ast, n.kid[0], args, ctx);
		if (c != TracedValue<T>(T(0))) {
			ctx.signalStack.back().continueRequested = val<bool>(true);
			return ctx.loopStack.back().acc;
		}
		return evalNautilusGeneric<T>(ast, n.kid[1], args, ctx);
	}
	case Kind::StaticLoop:
		if constexpr (std::is_same_v<T, bool>) {
			__builtin_unreachable();
		} else {
			// Plain C++ loop control over a static_val counter: the tracer
			// unrolls the body, one copy per trip, each seeing its index as a
			// constant (the static_val makes every iteration's snapshot hash
			// unique -- the same pattern as test/common/StaticLoopFunctions.hpp).
			const int64_t trips = static_cast<int64_t>(n.imm);
			TracedValue<T> acc = evalNautilusGeneric<T>(ast, n.kid[0], args, ctx);
			for (static_val<int64_t> i = 0; i < trips; ++i) {
				ctx.loopStack.push_back(
				    TracedLoopFrame<T> {TracedValue<T>(static_cast<T>(static_cast<int64_t>(i))), acc});
				acc = evalNautilusGeneric<T>(ast, n.kid[1], args, ctx);
				ctx.loopStack.pop_back();
			}
			return acc;
		}
	case Kind::Neg:
		if constexpr (std::is_same_v<T, bool>) {
			// Neg isn't in BOOL_KINDS -- val<bool> has no unary minus at all.
			__builtin_unreachable();
		} else {
			return -evalNautilusGeneric<T>(ast, n.kid[0], args, ctx);
		}
	case Kind::Not: {
		TracedValue<T> v = evalNautilusGeneric<T>(ast, n.kid[0], args, ctx);
		// Bool domain: Not means logical negation (see BOOL_KINDS in
		// Ast.hpp), using val<bool>::operator!. Integer domain only otherwise
		// (FLOAT_KINDS never generates Not); guarded so operator~ doesn't need
		// to exist for val<float>/val<double>.
		if constexpr (std::is_same_v<T, bool>) {
			return !v;
		} else if constexpr (!std::is_floating_point_v<T>) {
			return ~v;
		} else {
			__builtin_unreachable();
		}
	}
	case Kind::LNot: {
		val<bool> b = evalNautilusGeneric<T>(ast, n.kid[0], args, ctx) != TracedValue<T>(T(0));
		return select(!b, TracedValue<T>(T(1)), TracedValue<T>(T(0)));
	}
	case Kind::Cast:
		if constexpr (std::is_same_v<T, bool>) {
			// Cast isn't in BOOL_KINDS -- val<bool> never participates in the
			// Cast domain (see Types.hpp's ALL_TYPES/ROOT_TYPES comment);
			// guarded so castThroughTraced<bool> (which would need
			// conversions val<bool> doesn't provide, e.g. to/from val<T*>)
			// is never instantiated.
			__builtin_unreachable();
		} else {
			return castThroughTraced<T>(evalNautilusGeneric<T>(ast, n.kid[0], args, ctx), static_cast<TypeId>(n.imm));
		}
	case Kind::Load: {
		val<T*> p = evalNautilusPtr<T>(ast, n.kid[0], args, ctx);
		TracedValue<T> v = *p;
		return v;
	}
	case Kind::Store: {
		val<T*> p = evalNautilusPtr<T>(ast, n.kid[0], args, ctx);
		TracedValue<T> v = evalNautilusGeneric<T>(ast, n.kid[1], args, ctx);
		*p = v;
		return v;
	}
	case Kind::PtrToInt:
		if constexpr (std::is_same_v<T, bool>) {
			// PtrToInt isn't in BOOL_KINDS: val<bool*> has no pointer->integer
			// conversion (val_ptr.hpp's cast-to-arithmetic operator explicitly
			// excludes bool), so this must never be instantiated for T=bool.
			__builtin_unreachable();
		} else {
			return static_cast<TracedValue<T>>(evalNautilusPtr<T>(ast, n.kid[0], args, ctx));
		}
	case Kind::PtrEq:
	case Kind::PtrNe:
	case Kind::PtrLt:
	case Kind::PtrLe:
	case Kind::PtrGt:
	case Kind::PtrGe: {
		val<T*> a = evalNautilusPtr<T>(ast, n.kid[0], args, ctx);
		val<T*> b = evalNautilusPtr<T>(ast, n.kid[1], args, ctx);
		val<bool> cmp = comparePtrTraced<T>(n.kind, a, b);
		return select(cmp, TracedValue<T>(T(1)), TracedValue<T>(T(0)));
	}
	default:
		break;
	}

	TracedValue<T> l = evalNautilusGeneric<T>(ast, n.kid[0], args, ctx);
	TracedValue<T> r = evalNautilusGeneric<T>(ast, n.kid[1], args, ctx);
	switch (n.kind) {
	case Kind::Add:
		if constexpr (std::is_same_v<T, bool>) {
			// None of Add/Sub/Mul/Div/Mod/Shl/Shr are in BOOL_KINDS: val<bool>
			// has no arithmetic or shift operators, and the free operator+/-
			// etc. templates that satisfy is_fundamental_val<val<bool>> would
			// try to promote through val<int> (cast_value in val_details.hpp),
			// which val<bool> has no conversion to. Guarded so none of these
			// are ever instantiated for T=bool.
			__builtin_unreachable();
		} else {
			return l + r;
		}
	case Kind::Sub:
		if constexpr (std::is_same_v<T, bool>) {
			__builtin_unreachable();
		} else {
			return l - r;
		}
	case Kind::Mul:
		if constexpr (std::is_same_v<T, bool>) {
			__builtin_unreachable();
		} else {
			return l * r;
		}
	case Kind::Div:
		if constexpr (std::is_same_v<T, bool>) {
			__builtin_unreachable();
		} else if constexpr (std::is_floating_point_v<T>) {
			return l / r; // well-defined IEEE 754: produces +-inf / NaN for r == 0
		} else {
			return l / safeDivisorTraced<T>(r);
		}
	case Kind::Mod:
		if constexpr (std::is_same_v<T, bool>) {
			__builtin_unreachable();
		} else if constexpr (!std::is_floating_point_v<T>) {
			return l % safeDivisorTraced<T>(r);
		} else {
			__builtin_unreachable();
		}
	case Kind::And:
		if constexpr (!std::is_floating_point_v<T>) {
			return l & r;
		} else {
			__builtin_unreachable();
		}
	case Kind::Or:
		if constexpr (!std::is_floating_point_v<T>) {
			return l | r;
		} else {
			__builtin_unreachable();
		}
	case Kind::Xor:
		if constexpr (!std::is_floating_point_v<T>) {
			return l ^ r;
		} else {
			__builtin_unreachable();
		}
	case Kind::Shl:
		if constexpr (std::is_same_v<T, bool>) {
			__builtin_unreachable();
		} else if constexpr (!std::is_floating_point_v<T>) {
			return l << (r & TracedValue<T>(T(sizeof(T) * 8 - 1)));
		} else {
			__builtin_unreachable();
		}
	case Kind::Shr:
		if constexpr (std::is_same_v<T, bool>) {
			__builtin_unreachable();
		} else if constexpr (!std::is_floating_point_v<T>) {
			return l >> (r & TracedValue<T>(T(sizeof(T) * 8 - 1)));
		} else {
			__builtin_unreachable();
		}
	case Kind::LAnd: {
		// Real val<bool> conjunction under test. l and r are both already
		// evaluated (side-effect parity with the native oracle regardless of
		// whether && lowers short-circuiting), only the bool combine varies.
		val<bool> lb = l != TracedValue<T>(T(0));
		val<bool> rb = r != TracedValue<T>(T(0));
		return select(lb && rb, TracedValue<T>(T(1)), TracedValue<T>(T(0)));
	}
	case Kind::LOr: {
		val<bool> lb = l != TracedValue<T>(T(0));
		val<bool> rb = r != TracedValue<T>(T(0));
		return select(lb || rb, TracedValue<T>(T(1)), TracedValue<T>(T(0)));
	}
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

/// Traced mirror of EvalNative.hpp's evalNativeCall: fetch kids per the
/// selected callee's CallDescriptor (Callees.hpp) -- identical layout logic
/// to the generator and the native evaluator -- then invoke() the same
/// instantiation the native oracle calls directly, so the differential
/// surface is exactly the backend's ProxyCall/IndirectCall lowering:
/// argument count/type marshalling, narrow-integer ABI extension, float
/// register passing, void-return handling, and pointer argument marshalling.
template <typename T>
TracedValue<T> evalNautilusCall(const Ast& ast, const Node& n, std::span<const TracedValue<T>> args,
                                TracedEvalContext<T>& ctx) {
	const CallDescriptor callDesc = calleeDescriptor(n.imm);
	const int vStart = callValueKidStart(callDesc);
	TracedValue<T> v[3] = {TracedValue<T>(T(0)), TracedValue<T>(T(0)), TracedValue<T>(T(0))};
	for (int i = 0; i < callDesc.arity; ++i) {
		v[i] = evalNautilusGeneric<T>(ast, n.kid[vStart + i], args, ctx);
	}
	switch (n.imm % NUM_CALLEES) {
	case 0:
		return invoke(&calleeMix<T>, v[0], v[1]);
	case 1:
		return invoke(&calleeMin<T>, v[0], v[1]);
	case 2:
		return invoke(&calleeConstSeven<T>);
	case 3:
		return invoke(&calleeUnary<T>, v[0]);
	case 4:
		return invoke(&calleeSum3<T>, v[0], v[1], v[2]);
	case 5:
		return invoke(&calleeMixedTypes<T>, v[0], toCrossTraced<T>(v[1]));
	case 6:
		return static_cast<TracedValue<T>>(invoke(&calleeNarrowReturn<T>, v[0], v[1]));
	case 7:
		// void return: the node's value is its first value-domain kid,
		// mirroring how Kind::Store evaluates to the value it wrote.
		invoke(&calleeVoidNoop<T>, v[0], v[1]);
		return v[0];
	default: { // ptrSwap: kid[0] is the pointer-domain kid, v[0] is the value written.
		val<T*> p = evalNautilusPtr<T>(ast, n.kid[0], args, ctx);
		return invoke(&calleePtrSwap<T>, p, v[0]);
	}
	}
}

} // namespace detail

template <typename T>
TracedValue<T> evalNautilus(const Ast& ast, val<T*> basePtr, std::span<const TracedValue<T>> args) {
	detail::TracedEvalContext<T> ctx;
	ctx.basePtr = basePtr;
	ctx.lastPtr = basePtr + static_cast<int32_t>(BUFFER_ELEMS - 1);
	return detail::evalNautilusGeneric<T>(ast, ast.root, args, ctx);
}

/// Traced counterpart of Types.hpp's convertClamped: converts a val<From>
/// kernel-boundary value (an extra "mixed" parameter, or the final result on
/// its way to a narrow/differently-typed return) into a val<To>, well-defined
/// for every input. Reuses detail::clampFloatToIntTraced for the one
/// UB-prone leg (float->int) exactly like castThroughTraced does for a
/// same-domain round-trip Cast; every other direction is a plain
/// static_cast, mirroring Types.hpp::convertClamped so the native oracle and
/// the traced kernel agree on every boundary conversion by construction.
template <typename From, typename To>
TracedValue<To> convertClampedTraced(TracedValue<From> v) {
	if constexpr (std::is_floating_point_v<From> && !std::is_floating_point_v<To>) {
		return detail::clampFloatToIntTraced<From, To>(v);
	} else {
		return static_cast<TracedValue<To>>(v);
	}
}

namespace detail {

/// Traced evaluator for the enum domain (see generateFuzzEnumAst), mirroring
/// evalNativeFuzzEnumGeneric. Not an evalNautilusGeneric<T> instantiation:
/// val<enum> supports construction/copy and ==/!= (val_enum.hpp) but no
/// arithmetic/bitwise/pointer-offset operator at all, and -- unlike every
/// other domain here -- its copy constructor only takes a non-const `val<T>&`
/// (val_enum.hpp has no `const val<T>&` overload), so `args` is taken by
/// value rather than by the `const TracedArgs<T>&` every other domain uses.
///
/// `basePtr` points at FuzzEnum's *underlying* integer type, not at
/// `val<FuzzEnum>`: `val<enum*>` has no dereference operator at all
/// (val_ptr.hpp's `operator*` requires `is_arithmetic<ValType> ||
/// is_ptr<ValType>`, and enums satisfy neither), so Load/Store instead
/// exercise exactly the two conversions val<enum> does provide --
/// construction from, and conversion to, `val<underlying_type_t>` -- against
/// a buffer of that underlying type.
inline val<FuzzEnum> evalNautilusFuzzEnumGeneric(const Ast& ast, int idx, TracedArgs<FuzzEnum, ENUM_NUM_PARAMS> args,
                                                 val<std::underlying_type_t<FuzzEnum>*> basePtr) {
	using Underlying = std::underlying_type_t<FuzzEnum>;
	const Node& n = ast.nodes[idx];
	switch (n.kind) {
	case Kind::Const:
		return val<FuzzEnum>(unpackImm<FuzzEnum>(n.imm));
	case Kind::Param:
		return args[n.imm % ENUM_NUM_PARAMS];
	case Kind::Load: {
		val<Underlying> loaded = *basePtr;
		return val<FuzzEnum>(loaded);
	}
	case Kind::Store: {
		val<FuzzEnum> v = evalNautilusFuzzEnumGeneric(ast, n.kid[1], args, basePtr);
		*basePtr = static_cast<val<Underlying>>(v);
		return v;
	}
	case Kind::Eq: {
		val<FuzzEnum> l = evalNautilusFuzzEnumGeneric(ast, n.kid[0], args, basePtr);
		val<FuzzEnum> r = evalNautilusFuzzEnumGeneric(ast, n.kid[1], args, basePtr);
		// select() only supports is_arithmetic/is_pointer T (select.hpp), and
		// val<enum> is neither, so the 0/1 result is materialized via a real
		// branch instead (the same real-control-flow mechanism Kind::If uses).
		val<FuzzEnum> result = val<FuzzEnum>(FuzzEnum(0));
		if (l == r) {
			result = val<FuzzEnum>(FuzzEnum(1));
		}
		return result;
	}
	case Kind::Ne: {
		val<FuzzEnum> l = evalNautilusFuzzEnumGeneric(ast, n.kid[0], args, basePtr);
		val<FuzzEnum> r = evalNautilusFuzzEnumGeneric(ast, n.kid[1], args, basePtr);
		val<FuzzEnum> result = val<FuzzEnum>(FuzzEnum(0));
		if (l != r) {
			result = val<FuzzEnum>(FuzzEnum(1));
		}
		return result;
	}
	default:
		return val<FuzzEnum>(FuzzEnum(0)); // unreachable
	}
}

} // namespace detail

inline val<FuzzEnum> evalNautilusFuzzEnum(const Ast& ast, val<std::underlying_type_t<FuzzEnum>*> basePtr,
                                          TracedArgs<FuzzEnum, ENUM_NUM_PARAMS> args) {
	return detail::evalNautilusFuzzEnumGeneric(ast, ast.root, args, basePtr);
}

} // namespace nautilus::fuzz
