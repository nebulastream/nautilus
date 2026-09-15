#pragma once

#include <cstdint>
#include <nautilus/Engine.hpp>
#include <nautilus/nautilus_function.hpp>
#include <nautilus/region.hpp>
#include <nautilus/val_ptr.hpp>

/// A composite kernel built for sampling (see the perf jitdump issue's "the fixture" discussion), not
/// correctness -- every other fixture in this directory runs in nanoseconds, which a sampling profiler
/// cannot see at all. `rounds` drives only the trip count of the "hot" region, never the traced IR shape,
/// so the same compiled module cheaply exercises the hermetic jitdump-parsing test (rounds=1) and a real
/// perf sampling run (rounds large) without maintaining two fixtures that could drift apart.
///
/// Deliberately not added to test/data/ or the golden-regeneration scripts: its shape is tuned for
/// runtime, not readability, and checking in its IR dump would only create churn.
namespace nautilus::engine {

/// The "mix" region's external callee. Kept out-of-line and noinline (mirroring RunctimeCallFunctions.hpp's
/// helper1/helper2) so it lowers to a genuine CALL_EXT rather than being folded away: this is what lets a
/// profile show host-side samples landing on their own symbol, with the stack unwinding out of the JIT
/// frame into it (Finding 4 of the perf jitdump issue).
__attribute__((noinline)) inline int64_t perfNativeMix(int64_t acc, int64_t v) noexcept {
	return (acc ^ (v * 2654435761ULL)) + 0x9e3779b97f4a7c15ULL;
}

/// The "hot" region's per-element step, traced as its own Nautilus function so it lowers to a distinct
/// func.func / DISubprogram / JIT_CODE_LOAD record -- exercising cross-function attribution and proving
/// that a caller's region scopes do not leak into a callee traced outside any region() of its own.
inline val<int64_t> perfHelperBody(val<int64_t> acc, val<int64_t> v) {
	return acc + v * 3 - (v >> 1);
}

inline NautilusFunction perfHelper {"perfHelper", perfHelperBody};

/// The composite kernel itself: nested region()s (function -> outer -> hot/mix), a data-dependent
/// multi-block loop body (so per-block DILexicalBlock scoping and block-argument phi lines are exercised),
/// an internal Nautilus-to-Nautilus call, and an external invoke() call. `data` and `len` make the work
/// data-dependent -- not closed-formable by the LLVM optimizer -- and `acc` escapes via the return value,
/// so nothing here folds away or gets deleted at -O3.
inline val<int64_t> perfCompositeKernel(val<int64_t*> data, val<int32_t> len, val<int32_t> rounds) {
	val<int64_t> acc = 0;
	region("outer", [&]() {
		region("hot", [&]() {
			for (val<int32_t> r = 0; r < rounds; r = r + 1) {
				for (val<int32_t> i = 0; i < len; i = i + 1) {
					val<int64_t> v = data[i];
					if (v > 0) {
						acc = perfHelper(acc, v);
					} else {
						acc = acc - v;
					}
				}
			}
		});
		region("mix", [&]() {
			for (val<int32_t> i = 0; i < len; i = i + 1) {
				acc = invoke(perfNativeMix, acc, data[i]);
			}
		});
	});
	return acc;
}

} // namespace nautilus::engine
