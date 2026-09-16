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

/// Four levels of region() nesting around one hot loop, with no internal calls at all.
///
/// Region nesting deeper than two levels used to lose its qualified perf symbol entirely -- not just
/// the innermost level, but every level, leaving bare `execute` for every sample landing in the loop.
/// Four levels (rather than the three that first exposed it) pins that the encoding is recursive
/// rather than special-cased one level further out.
inline val<int64_t> perfDeepRegionKernel(val<int64_t*> data, val<int32_t> len) {
	val<int64_t> acc = 0;
	region("depth1", [&]() {
		region("depth2", [&]() {
			region("depth3", [&]() {
				region("depth4", [&]() {
					for (val<int32_t> i = 0; i < len; i = i + 1) {
						val<int64_t> v = data[i];
						acc = acc * 3 + v;
					}
				});
			});
		});
	});
	return acc;
}

/// The far end of a three-hop internal call chain: kernel -> top -> mid -> leaf.
///
/// A single hop (a kernel calling one NautilusFunction that calls nothing further) is what the rest
/// of this header already covers, and it happens to survive the debug-info pipeline; two or more hops
/// nest the MLIR inliner's `CallSiteLoc` wrappers, which is the shape that used to crash the compile
/// outright under a perf-only configuration. The loop keeps each level from folding into its caller
/// before the inliner ever sees a call.
inline val<int64_t> perfChainLeafBody(val<int64_t> x) {
	val<int64_t> acc = x;
	for (val<int32_t> i = 0; i < 4; i = i + 1) {
		acc = acc * 3 + 7;
	}
	return acc;
}

inline NautilusFunction perfChainLeaf {"perfChainLeaf", perfChainLeafBody};

inline val<int64_t> perfChainMidBody(val<int64_t> x) {
	return perfChainLeaf(x) + 2;
}

inline NautilusFunction perfChainMid {"perfChainMid", perfChainMidBody};

inline val<int64_t> perfChainTopBody(val<int64_t> x) {
	return perfChainMid(x) + 3;
}

inline NautilusFunction perfChainTop {"perfChainTop", perfChainTopBody};

inline val<int64_t> perfCallChainKernel(val<int64_t*> data, val<int32_t> len) {
	val<int64_t> acc = 0;
	for (val<int32_t> i = 0; i < len; i = i + 1) {
		val<int64_t> v = data[i];
		acc = acc + perfChainTop(v);
	}
	return acc;
}

} // namespace nautilus::engine
