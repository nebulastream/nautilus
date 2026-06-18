#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/control_flow.hpp>
#include <nautilus/static.hpp>

// Explicit-control-flow twins of the path-explosion / loop benchmark kernels in
// NestedIfBenchmarks.hpp and LoopFunctions.hpp. They let the tracing benchmark emit
// side-by-side numbers: the implicit kernels drive the symbolic executor through
// many re-run iterations (super-linear in the branch count), whereas these explicit
// twins trace in a single pass.
namespace nautilus::engine {

// A chain of n independent explicit ifs. The host for-loop over the static_val k runs
// at trace time, emitting n explicit If constructs in sequence during the single
// trace pass. k is a static_val so each unrolled iteration gets a distinct snapshot
// hash for the body operations (just like static_iterable loops) — without it, the
// repeated body ops on the same source line would alias and corrupt the trace.
inline val<int32_t> chainedIfExplicitN(val<int32_t> x, int32_t n) {
	val<int32_t> result = 0;
	for (static_val<int32_t> k = 0; k < n; ++k) {
		If(x > val<int32_t>(static_cast<int32_t>(k)), [&]() { result = result + 1; });
	}
	return result;
}

inline val<int32_t> chainedIf10Explicit(val<int32_t> x) {
	return chainedIfExplicitN(x, 10);
}

inline val<int32_t> chainedIf100Explicit(val<int32_t> x) {
	return chainedIfExplicitN(x, 100);
}

// n nested explicit ifs (each guarded by x > k), built by host recursion at trace time.
inline void nestedIfExplicitRec(val<int32_t>& x, val<int32_t>& result, int32_t k, int32_t n) {
	if (k >= n) {
		return;
	}
	If(x > val<int32_t>(k), [&]() {
		result = result + 1;
		nestedIfExplicitRec(x, result, k + 1, n);
	});
}

inline val<int32_t> nestedIf10Explicit(val<int32_t> x) {
	val<int32_t> result = 0;
	nestedIfExplicitRec(x, result, 0, 10);
	return result;
}

// Loop twins for parity (already-cheap cases): a counted accumulation and gcd.
inline val<int32_t> sumLoopExplicitBench(val<int32_t> n) {
	val<int32_t> agg = 1;
	For(val<int32_t>(0), n, [&](val<int32_t>) { agg = agg + 10; });
	return agg;
}

inline val<int32_t> gcdExplicitBench(val<int32_t> a, val<int32_t> b) {
	While([&]() { return b != 0; },
	      [&]() {
		      val<int32_t> t = b;
		      b = a % b;
		      a = t;
	      });
	return a;
}

} // namespace nautilus::engine
