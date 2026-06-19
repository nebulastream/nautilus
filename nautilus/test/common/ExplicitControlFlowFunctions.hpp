#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/control_flow.hpp>

// Kernels exercising the explicit control-flow API (If/While/For) from
// nautilus/control_flow.hpp. Each mirrors a typical implicit-control-flow kernel
// so its results can be asserted against hand-computed expectations.
namespace nautilus::engine {

// --- Branching --------------------------------------------------------------

// if-then-else; twin of ControlFlowFunctions.hpp::ifThenElseCondition.
inline val<int32_t> ifThenElseExplicit(val<int32_t> value) {
	val<int32_t> iw = 1;
	If(value == 42, [&]() { iw = iw + 1; }, [&]() { iw = iw + 42; });
	return iw + 42;
}

// if-then with no else arm.
inline val<int32_t> ifThenExplicit(val<int32_t> value) {
	val<int32_t> r = 0;
	If(value > 0, [&]() { r = r + 10; });
	return r;
}

// Nested If inside both arms.
inline val<int32_t> nestedIfExplicit(val<int32_t> x) {
	val<int32_t> r = 0;
	If(
	    x > 0,
	    [&]() {
		    r = r + 1;
		    If(x > 10, [&]() { r = r + 10; }, [&]() { r = r + 5; });
	    },
	    [&]() { r = r - 1; });
	return r;
}

// A chain of independent ifs (the path-explosion case for the implicit path).
inline val<int32_t> chainedIfExplicit(val<int32_t> x) {
	val<int32_t> r = 0;
	If(x > 0, [&]() { r = r + 1; });
	If(x > 1, [&]() { r = r + 1; });
	If(x > 2, [&]() { r = r + 1; });
	return r;
}

// Branch arms that mutate then a single return after the merge.
inline val<int32_t> conditionalAbsExplicit(val<int32_t> x) {
	val<int32_t> r = x;
	If(x < 0, [&]() { r = val<int32_t>(0) - x; });
	return r;
}

// --- Loops ------------------------------------------------------------------

// Counted for: sum of i over [0, n).
inline val<int32_t> sumForExplicit(val<int32_t> n) {
	val<int32_t> sum = 0;
	For(val<int32_t>(0), n, [&](val<int32_t> i) { sum = sum + i; });
	return sum;
}

// Counted for accumulating a constant; twin of LoopFunctions.hpp::sumLoop.
inline val<int32_t> sumLoopExplicit(val<int32_t> n) {
	val<int32_t> agg = 1;
	For(val<int32_t>(0), n, [&](val<int32_t>) { agg = agg + 10; });
	return agg;
}

// While countdown.
inline val<int32_t> whileCountdownExplicit(val<int32_t> n) {
	val<int32_t> count = 0;
	While([&]() { return n > 0; },
	      [&]() {
		      count = count + 1;
		      n = n - 1;
	      });
	return count;
}

// gcd via while loop (data-dependent trip count, multiple loop-carried values).
inline val<int32_t> gcdExplicit(val<int32_t> a, val<int32_t> b) {
	While([&]() { return b != 0; },
	      [&]() {
		      val<int32_t> t = b;
		      b = a % b;
		      a = t;
	      });
	return a;
}

// Nested counted loops.
inline val<int32_t> nestedLoopExplicit(val<int32_t> n) {
	val<int32_t> sum = 0;
	For(val<int32_t>(0), n, [&](val<int32_t>) { For(val<int32_t>(0), n, [&](val<int32_t>) { sum = sum + 1; }); });
	return sum;
}

// If inside a loop body.
inline val<int32_t> ifInsideLoopExplicit(val<int32_t> n) {
	val<int32_t> sum = 0;
	For(val<int32_t>(0), n,
	    [&](val<int32_t> i) { If(i % 2 == 0, [&]() { sum = sum + 10; }, [&]() { sum = sum + 20; }); });
	return sum;
}

// Three-clause general For.
inline val<int32_t> generalForExplicit(val<int32_t> n) {
	val<int32_t> sum = 0;
	val<int32_t> i = 0;
	For([&]() { i = val<int32_t>(0); }, [&]() { return i < n; }, [&]() { i = i + 1; }, [&]() { sum = sum + i; });
	return sum;
}

// --- Early exit: Break / Continue -------------------------------------------

// Break out of a While loop early.
inline val<int32_t> whileBreakExplicit(val<int32_t> n) {
	val<int32_t> sum = 0;
	val<int32_t> i = 0;
	While([&]() { return i < n; },
	      [&]() {
		      If(i == 5, [&]() { Break(); });
		      sum = sum + i;
		      i = i + 1;
	      });
	return sum;
}

// Break out of a counted For loop early.
inline val<int32_t> forBreakExplicit(val<int32_t> n) {
	val<int32_t> sum = 0;
	For(val<int32_t>(0), n, [&](val<int32_t> i) {
		If(i == 5, [&]() { Break(); });
		sum = sum + i;
	});
	return sum;
}

// Continue in a For loop: the step still runs (C for-loop semantics), summing odds.
inline val<int32_t> forContinueExplicit(val<int32_t> n) {
	val<int32_t> sum = 0;
	For(val<int32_t>(0), n, [&](val<int32_t> i) {
		If(i % 2 == 0, [&]() { Continue(); });
		sum = sum + i;
	});
	return sum;
}

// Continue in a While loop: counts odd values in [0, n). The induction variable is
// advanced before the potential Continue (While's continue re-checks the condition).
inline val<int32_t> whileContinueExplicit(val<int32_t> n) {
	val<int32_t> count = 0;
	val<int32_t> i = 0;
	While([&]() { return i < n; },
	      [&]() {
		      val<int32_t> cur = i;
		      i = i + 1;
		      If(cur % 2 == 0, [&]() { Continue(); });
		      count = count + 1;
	      });
	return count;
}

// Break only escapes the innermost loop.
inline val<int32_t> nestedBreakExplicit(val<int32_t> n) {
	val<int32_t> sum = 0;
	For(val<int32_t>(0), n, [&](val<int32_t>) {
		For(val<int32_t>(0), n, [&](val<int32_t> j) {
			If(j == 2, [&]() { Break(); });
			sum = sum + 1;
		});
	});
	return sum;
}

// --- Value-yielding If ------------------------------------------------------

inline val<int32_t> maxExplicit(val<int32_t> a, val<int32_t> b) {
	return If(a > b, [&]() { return a; }, [&]() { return b; });
}

// --- Strided For ------------------------------------------------------------

inline val<int32_t> forStrideExplicit(val<int32_t> n) {
	val<int32_t> sum = 0;
	For(val<int32_t>(0), n, val<int32_t>(2), [&](val<int32_t> i) { sum = sum + i; });
	return sum;
}

// --- Mixing (must be rejected when compiled/traced) -------------------------

// Combines an explicit If with an implicit native if in the same function. This
// is unsupported: the native branch triggers symbolic re-execution, which the
// explicit primitives detect and reject with a clear RuntimeException.
inline val<int32_t> mixedExplicitAndNativeIf(val<int32_t> x) {
	val<int32_t> r = 0;
	If(x > 0, [&]() { r = r + 1; });
	if (x > 5) {
		r = r + 100;
	}
	return r;
}

// An explicit For loop whose body uses an implicit native if. The native branch
// inside the loop body triggers symbolic re-execution of the whole function, which
// re-enters the explicit loop's emitters on the second iteration — detected and
// rejected with the same clear RuntimeException.
inline val<int32_t> explicitLoopWithNativeIf(val<int32_t> n) {
	val<int32_t> sum = 0;
	For(val<int32_t>(0), n, [&](val<int32_t> i) {
		if (i % 2 == 0) {
			sum = sum + 10;
		} else {
			sum = sum + 20;
		}
	});
	return sum;
}

// A native (implicit) if placed BEFORE the explicit construct. Still rejected:
// the implicit branch is detected and the explicit construct refuses to mix.
inline val<int32_t> nativeIfThenExplicit(val<int32_t> x) {
	val<int32_t> r = 0;
	if (x > 5) {
		r = r + 100;
	}
	If(x > 0, [&]() { r = r + 1; });
	return r;
}

// An explicit While loop whose body uses an implicit native if (same rejection).
inline val<int32_t> explicitWhileWithNativeIf(val<int32_t> n) {
	val<int32_t> sum = 0;
	val<int32_t> i = 0;
	While([&]() { return i < n; },
	      [&]() {
		      if (i % 2 == 0) {
			      sum = sum + 10;
		      } else {
			      sum = sum + 20;
		      }
		      i = i + 1;
	      });
	return sum;
}

} // namespace nautilus::engine
