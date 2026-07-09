#pragma once

#include "Types.hpp"
#include <cstdint>
#include <type_traits>

namespace nautilus::fuzz {

/// Number of native helper functions a Kind::Call node can target (its imm
/// is drawn modulo this).
inline constexpr int NUM_CALLEES = 9;

/// Trait: the fixed cross-domain type used by calleeMixedTypes' second
/// argument. Integer T gets a `double` second argument, floating-point T
/// gets an `int32_t` second argument, so every instantiation genuinely
/// interleaves an integer and a floating-point argument register regardless
/// of which domain the tree's own T happens to be in.
template <typename T>
struct CrossTypeTrait {
	using type = std::conditional_t<std::is_floating_point_v<T>, int32_t, double>;
};
template <typename T>
using CrossType = typename CrossTypeTrait<T>::type;

/// Trait: one domain-preserving step down in width from T (int64->int32->
/// int16->int8, unsigned mirrored, f64->f32), used by calleeNarrowReturn.
/// Bottoms out at itself (i8/f32 have no narrower same-domain type this
/// fuzzer generates). Always same-domain, so widening the result back up to
/// T at the call site is a plain, always-safe upcast -- no float clamp is
/// ever needed on the return leg.
template <typename T>
struct NarrowTypeTrait {
	using type = T;
};
template <>
struct NarrowTypeTrait<int64_t> {
	using type = int32_t;
};
template <>
struct NarrowTypeTrait<int32_t> {
	using type = int16_t;
};
template <>
struct NarrowTypeTrait<int16_t> {
	using type = int8_t;
};
template <>
struct NarrowTypeTrait<uint64_t> {
	using type = uint32_t;
};
template <>
struct NarrowTypeTrait<uint32_t> {
	using type = uint16_t;
};
template <>
struct NarrowTypeTrait<uint16_t> {
	using type = uint8_t;
};
template <>
struct NarrowTypeTrait<double> {
	using type = float;
};
template <typename T>
using NarrowType = typename NarrowTypeTrait<T>::type;

/// Convert a T-typed kid value into CrossType<T> at the native call site.
/// int->double is always a safe widen; float->int32_t is the only UB-prone
/// leg, so it goes through the shared convertClamped (Types.hpp) -- the same
/// clamp EvalNative.hpp's castThrough uses -- rather than a raw static_cast.
template <typename T>
CrossType<T> toCrossNative(T v) {
	return convertClamped<T, CrossType<T>>(v);
}

/// Pure native helpers exposed to generated programs through Kind::Call. The
/// native oracle calls them directly; the traced kernel calls the exact same
/// instantiation through nautilus::invoke(). Because both legs execute the
/// identical native code, the differential surface is exclusively the
/// backend's call lowering: argument count/arity, argument/return
/// marshalling across mixed fundamental types, narrow-integer ABI extension,
/// float register passing, void-returning calls, and pointer arguments with
/// observable side effects. Every helper must be fully defined for all
/// inputs (unsigned-wraparound integer arithmetic; IEEE 754 for floats), and
/// -- other than the pointer-domain kid of calleePtrSwap -- must not touch
/// any state besides its arguments, so replaying the same fuzzer input
/// always reproduces the same result.
template <typename T>
T calleeMix(T a, T b) {
	if constexpr (std::is_floating_point_v<T>) {
		return a * T(0.5) + b;
	} else if constexpr (std::is_same_v<T, bool>) {
		// std::make_unsigned_t<bool> is ill-formed, so the general integer
		// path below doesn't apply; logical xor is a well-defined, equally
		// exercising stand-in for the bool domain (see BOOL_KINDS).
		return a != b;
	} else {
		using U = std::make_unsigned_t<T>;
		return static_cast<T>(static_cast<U>(static_cast<U>(a) * U(3)) + static_cast<U>(b));
	}
}

template <typename T>
T calleeMin(T a, T b) {
	// NaN-safe by identity: `NaN < b` is false, so b is returned. Both the
	// oracle and the compiled kernel run this same code, so the results match
	// by construction whatever the operands are.
	return a < b ? a : b;
}

/// Arity-0 callee: no arguments at all, so there is no argument-register
/// marshalling to speak of -- this exercises pure return-value plumbing
/// through a real call (does the backend correctly produce/return a constant
/// via the ABI's return register(s)?).
template <typename T>
T calleeConstSeven() {
	if constexpr (std::is_floating_point_v<T>) {
		return T(7.5);
	} else {
		return T(7);
	}
}

/// Arity-1 callee.
template <typename T>
T calleeUnary(T x) {
	if constexpr (std::is_floating_point_v<T>) {
		return -x + T(1.5);
	} else if constexpr (std::is_same_v<T, bool>) {
		// std::make_unsigned_t<bool> is ill-formed; logical negation is the
		// bool-domain stand-in for this callee's bitwise complement.
		return !x;
	} else {
		using U = std::make_unsigned_t<T>;
		return static_cast<T>(~static_cast<U>(x));
	}
}

/// Arity-3 callee: exercises argument-register exhaustion / stack-passed
/// arguments on ABIs that run out of argument registers before the third
/// same-typed parameter.
template <typename T>
T calleeSum3(T a, T b, T c) {
	if constexpr (std::is_floating_point_v<T>) {
		return a + b + c;
	} else if constexpr (std::is_same_v<T, bool>) {
		// std::make_unsigned_t<bool> is ill-formed; logical xor-parity is a
		// well-defined, equally exercising stand-in for the bool domain.
		return (a != b) != c;
	} else {
		using U = std::make_unsigned_t<T>;
		return static_cast<T>(static_cast<U>(a) + static_cast<U>(b) + static_cast<U>(c));
	}
}

/// Mixed-argument-type callee: `a` is T, `b` is CrossType<T> (double for
/// integer T, int32_t for floating-point T) -- exactly the
/// integer/floating-point register-class interleaving that trips up SysV /
/// AAPCS64 argument classification. `b` arrives already marshalled into
/// CrossType<T> (see toCrossNative/its traced counterpart); combining it back
/// down to T is the only UB-prone leg for integer T (double -> int), so that
/// leg is clamped here with the same shared convertClamped used everywhere
/// else in this fuzzer for a float->int narrowing.
template <typename T>
T calleeMixedTypes(T a, CrossType<T> b) {
	if constexpr (std::is_floating_point_v<T>) {
		// b : int32_t -- widening it to T is always safe, no clamp needed.
		return a + static_cast<T>(b) * T(0.25);
	} else if constexpr (std::is_same_v<T, bool>) {
		// b : double -- convertClamped already handles a bool target (see
		// Types.hpp); std::make_unsigned_t<bool> below is ill-formed, so
		// combine with logical xor instead of the general integer path.
		return a != convertClamped<double, T>(b);
	} else {
		// b : double -- the only UB-prone leg of this callee.
		const T bt = convertClamped<double, T>(b);
		using U = std::make_unsigned_t<T>;
		return static_cast<T>(static_cast<U>(a) + static_cast<U>(bt));
	}
}

/// Narrow-return callee: returns NarrowType<T> (one domain-preserving step
/// narrower than T; see NarrowTypeTrait), covering the return-extension-
/// direction ABI bug class (narrow ints in particular). Narrowing a signed
/// integer via `static_cast` is well-defined in C++20 (mod 2^N), so the
/// integer leg needs no extra handling; the float leg (double -> float) is
/// always defined by IEEE 754 (may produce +-inf for out-of-range
/// magnitudes, never UB).
template <typename T>
NarrowType<T> calleeNarrowReturn(T a, T b) {
	using N = NarrowType<T>;
	if constexpr (std::is_floating_point_v<T>) {
		return static_cast<N>(a) + static_cast<N>(b);
	} else if constexpr (std::is_same_v<T, bool>) {
		// NarrowType<bool> is bool itself (NarrowTypeTrait has no narrower
		// bool-domain type); std::make_unsigned_t<bool> below is ill-formed,
		// so combine with logical xor instead of the general integer path.
		return a != b;
	} else {
		using U = std::make_unsigned_t<T>;
		const T mixed = static_cast<T>(static_cast<U>(a) * U(3) + static_cast<U>(b));
		return static_cast<N>(mixed);
	}
}

/// Void-returning callee, invoked purely for the call itself (return-value
/// marshalling for a `void` ABI return is still lowering surface worth
/// covering). Deliberately touches no state beyond its arguments -- a
/// thread_local counter would make replaying the same fuzzer input
/// non-deterministic across runs -- so the Kind::Call case treats a void
/// callee's node value as its first argument, mirroring how Kind::Store
/// evaluates to the value it just wrote rather than to anything the
/// underlying write "returns".
template <typename T>
void calleeVoidNoop(T a, T b) {
	(void) a;
	(void) b;
}

/// Pointer-argument callee with an observable side effect: reads the old
/// value at `p`, writes `v` through it, and returns the old value -- fuzzing
/// pointer marshalling through invoke() (the buffer pointer already threaded
/// through every generated kernel, see Kind::PtrBase in Ast.hpp) with a real
/// memory write, so call *ordering* relative to surrounding Load/Store is
/// under test, not just the return value. Always safe: `p` is always an
/// in-bounds pointer into the shared buffer (generatePtrNode's invariant).
template <typename T>
T calleePtrSwap(T* p, T v) {
	const T old = *p;
	*p = v;
	return old;
}

/// Structural shape of a callee, shared by the generator (Ast.hpp) and both
/// evaluators (EvalNative.hpp / EvalNautilus.hpp) so the three can never
/// disagree about how many kids a Call node has or what kind they are.
/// `arity` counts value-domain kid arguments only; `usesPointer` says an
/// additional pointer-domain kid (a bare Kind::PtrBase leaf, via
/// Ast.hpp's pushPtrBase -- deliberately not a recursive PtrAdd/PtrSub
/// offset expression the way Load/Store's is) is prepended. Kid layout
/// convention, mirroring Kind::Store:
/// when usesPointer is set, kid[0] is the pointer-domain node and the
/// `arity` value-domain kids start at kid[1]; otherwise they start at
/// kid[0].
struct CallDescriptor {
	int arity;
	bool usesPointer;
};

inline constexpr CallDescriptor calleeDescriptor(uint64_t imm) {
	switch (imm % NUM_CALLEES) {
	case 0: // mix
		return {2, false};
	case 1: // min
		return {2, false};
	case 2: // constSeven
		return {0, false};
	case 3: // unary
		return {1, false};
	case 4: // sum3
		return {3, false};
	case 5: // mixedTypes
		return {2, false};
	case 6: // narrowReturn
		return {2, false};
	case 7: // voidNoop
		return {2, false};
	default: // ptrSwap
		return {1, true};
	}
}

/// First value-domain kid index for a Call node's descriptor -- kid[0] when
/// there's no pointer-domain kid, kid[1] (right after it) otherwise. Shared
/// so the generator and both evaluators index kids identically.
inline constexpr int callValueKidStart(const CallDescriptor& d) {
	return d.usesPointer ? 1 : 0;
}

inline const char* calleeName(uint64_t imm) {
	switch (imm % NUM_CALLEES) {
	case 0:
		return "mix";
	case 1:
		return "min";
	case 2:
		return "constSeven";
	case 3:
		return "unary";
	case 4:
		return "sum3";
	case 5:
		return "mixedTypes";
	case 6:
		return "narrowReturn";
	case 7:
		return "voidNoop";
	default:
		return "ptrSwap";
	}
}

} // namespace nautilus::fuzz
