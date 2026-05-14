#pragma once

#include <nautilus/Engine.hpp>

// =============================================================================
// PATH EXPLOSION REPROS
// =============================================================================
//
// Reproducers for the symbolic-execution path-explosion phenomenon in the
// Nautilus tracer.  Each fixture in this file is a deliberate stress case
// for tracing time / trace size.  Read the introduction below before
// changing anything here.
//
// -----------------------------------------------------------------------------
// How the tracer enumerates paths
// -----------------------------------------------------------------------------
//
// Every conditional in user code lowers to a single CMP operation tagged by
// `recordSnapshot()` (`ExceptionBasedTraceContext.cpp:473`).  A tag is the
// pair `(stack_trace_hash, staticValueHash ^ aliveVars.hash())`
// (`Snapshot.hpp`).  The symbolic executor (`SymbolicExecutionContext`)
// visits each unique tag at most twice — once down the `true` arm, once
// down the `false` arm.  After the second visit, that tag is pruned
// (`SymbolicExecutionContext.cpp:35-39`).  Total iterations:
//
//     iterations  ==  Σ ( 2  for each distinct tag )
//
// Crucially, "distinct tag" is *not* "distinct source location".  Two CMPs
// sharing a source location but seen with different `aliveVars` state get
// **different tags** and are explored independently.  This is the lever that
// produces super-linear blow-up.
//
// -----------------------------------------------------------------------------
// Why some shapes blow up and others don't
// -----------------------------------------------------------------------------
//
// 1. N INDEPENDENT IFs WITH SHARED STATE — linear, O(N) returns.
//    Each CMP has one tag.  2 arms × N CMPs = 2N tag-arms; the bodies
//    merge cleanly at each CMP exit so the bottom-of-function RETURN
//    keeps its tag stable.  `chainedIf10/100/500` in
//    `NestedIfBenchmarks.hpp` are this shape; trace size scales
//    linearly in N.  See `pathExplosion_independentIfs_4` below
//    (measured: 2 RETURNs, 4 CMPs).
//
// 2. INLINED CALLEE WITH M RETURN-VALUE ORIGINS, FOLLOWED BY A CMP —
//    multiplicative, O(M).
//    A `val<T>` function with M distinct `return` expressions, when
//    invoked as a plain C++ call (not via `NautilusFunction`), folds
//    into the caller's trace.  After the inlined body, the result
//    `val<T>` has M distinct ValueRefs across the M paths.  Any later
//    CMP that depends on that result therefore sees M distinct
//    `aliveVars.hash()` snapshots and gets M distinct tags — each
//    explored twice.  A single post-call `if (r == K)` produces 2M
//    tag-arms instead of 2.  See `pathExplosion_postCallBranch_1`
//    (M=3, measured 6 RETURNs).
//
// 3. INLINED CALLEE CHAIN, EVEN WITHOUT POST-CALL BRANCHES — also
//    multiplicative, O(M^N).
//    THIS IS THE SURPRISING ONE.  You might expect that without a
//    post-call CMP, the M paths through the first callee fan straight
//    into the next callee's argument and produce only M paths
//    overall.  In practice each callee's *internal* CMPs see different
//    argument-ValueRefs across paths, so the inner CMPs get distinct
//    tags per upstream path → the explosion happens *inside* each
//    callee, before it ever reaches a return statement.
//    `pathExplosion_baseline_threeCallsNoBranch` (M=3, N=3) measures
//    27 RETURNs — exactly 3^3.
//
// 4. CHAIN of inlined calls WITH post-call branches — exponential,
//    O(M^N) × 2^N.
//    Effects (2) and (3) compound.  `pathExplosion_postCallBranch_3`
//    (M=3, N=3) measures 54 RETURNs and 65 CMPs.
//
// 5. CONSTRAINT-BLIND DEAD ARMS — root cause is the same lack of
//    path-predicate tracking that powers (2)–(4).
//    The tracer doesn't propagate constraints.  An inner CMP that is
//    logically dead under the outer path's constraints is still
//    traced both ways.  See `pathExplosion_constraintBlind_dead`
//    (measured 4 RETURNs from 2 reachable arms).
//
// 6. NAUTILUS-FUNCTION BOUNDARY — N separate traces, O(N callees).
//    A `NautilusFunction` wraps the callee in a tracing boundary;
//    every unique callee reached during tracing is enqueued in
//    `functionsToTrace` and traced independently
//    (`ExceptionBasedTraceContext.cpp:160-164`).  Depth-N call chain
//    => N traces.  Each individual trace is small, but the overhead
//    of starting fresh symbolic-execution loops compounds.  See the
//    `multipleReturnsRoot` fixture in `NautilusFunction.hpp`.
//
// -----------------------------------------------------------------------------
// Fixture taxonomy
// -----------------------------------------------------------------------------
//
//   pathExplosion_baseline_oneCall            — "no explosion" control (Tier 0)
//   pathExplosion_baseline_threeCallsNoBranch — Tier 3 (no post-call branch
//                                                still explodes inside the
//                                                callee chain)
//   pathExplosion_independentIfs*     — Tier (1): linear
//   pathExplosion_postCallBranch*     — Tier (2)/(3): multiplicative
//   pathExplosion_constraintBlind*    — Tier (4): dead-branch
//
// Path counts in comments below are *upper bounds* on the number of leaf
// return sites recorded in the raw trace dump (count `RETURN` lines in
// `nautilus/test/data/control-flow-tests/tracing/<name>.trace`).  Block
// counts and total op counts are also useful proxies.
//
// All fixtures use plain `val<T>` functions; they intentionally avoid the
// `NautilusFunction` boundary because we want the path-explosion to
// surface within a *single* trace, where the SSA pass has to clean it up.

namespace nautilus::engine {

// -----------------------------------------------------------------------------
// Tier 0: baseline — a 3-return leaf.  Used by the post-call families
// below so the path-explosion math is easy to reason about.
// -----------------------------------------------------------------------------
val<int32_t> peLeafThreeReturns(val<int32_t> v) {
	if (v == 1) {
		return 1;
	}
	if (v < 10) {
		return 42;
	}
	return v + v + 1;
}

// Baseline control: just call the leaf once, no post-call branch.
// Expected: 3 RETURNs in the raw trace (one per leaf return path),
// merged to a single RETURN by SSA.
val<int32_t> pathExplosion_baseline_oneCall(val<int32_t> v) {
	return peLeafThreeReturns(v);
}

// Tier 3 demonstration: chain three leaf calls *without* any post-call
// branch.  Naively this looks like a "no explosion" control — there is no
// CMP on the inlined return value, and two of the three leaf returns are
// statically-known constants.  In practice the trace forks O(M^N) anyway
// because each chained call's *internal* CMPs see a fresh argument
// ValueRef per upstream path, get fresh tags, and are re-explored both
// ways even on paths where the argument is a known constant.  See the
// Tier-3 entry in the intro (`How the tracer enumerates paths`, point 3)
// for the full mechanism.
//
// Expected: 27 RETURNs in the raw trace (= 3^3), 26 CMPs, 53 blocks.
// Reference: nautilus/test/data/path-explosion-tests/tracing/
// pathExplosion_baseline_threeCallsNoBranch.trace.
val<int32_t> pathExplosion_baseline_threeCallsNoBranch(val<int32_t> v) {
	val<int32_t> a = peLeafThreeReturns(v);
	val<int32_t> b = peLeafThreeReturns(a);
	val<int32_t> c = peLeafThreeReturns(b);
	return c;
}

// -----------------------------------------------------------------------------
// Tier 1: N independent ifs sharing a single result variable.  Bodies
// merge cleanly at each CMP exit, so the tag at each subsequent CMP is
// stable across paths.  Iterations scale as 2N (= linear).
//
// These are the "safe" multi-branch shape that the SSA pass and the
// optimizer handle gracefully.  Keep them here as a control: any change
// that makes these worse is a regression.
// -----------------------------------------------------------------------------
val<int32_t> pathExplosion_independentIfs_4(val<int32_t> x) {
	val<int32_t> r = 0;
	if (x > 0) {
		r = r + 1;
	}
	if (x > 1) {
		r = r + 1;
	}
	if (x > 2) {
		r = r + 1;
	}
	if (x > 3) {
		r = r + 1;
	}
	return r;
}

// -----------------------------------------------------------------------------
// Tier 2: post-call branch on the inlined return value.
//
// A single inlined call to a 3-return leaf followed by `if (r == 42)`.
// Each of the leaf's three return paths produces a different `r`
// ValueRef, so the post-call CMP gets *three* tags rather than one.
// Each tag is then explored twice (true / false).
//
// Expected: 6 RETURNs in the raw trace (2 arms × 3 inlined origins).
// Compare against `pathExplosion_baseline_oneCall` (3 RETURNs).
// -----------------------------------------------------------------------------
val<int32_t> pathExplosion_postCallBranch_1(val<int32_t> v) {
	val<int32_t> r = peLeafThreeReturns(v);
	if (r == 42) {
		return r + 1;
	}
	return r;
}

// Two chained post-call branches.  The first branch already fans the
// trace into 6 paths; the second call sees 6 distinct argument
// ValueRefs, inlines into 6 × 3 = 18 internal paths, and the second
// post-call CMP gets 18 distinct tags.
//
// Expected order of magnitude: ~3^2 = 9 distinct origins through both
// calls, ×2 arms at the final CMP, ≈18 RETURNs.  (Actual count may
// differ slightly because some upstream paths terminate early when
// inputs hit the leaf's `v == 1` short-circuit, which fixes `r` to a
// constant — and constant ValueRefs share a tag.)
val<int32_t> pathExplosion_postCallBranch_2(val<int32_t> v) {
	val<int32_t> a = peLeafThreeReturns(v);
	if (a == 42) {
		a = a + 1;
	}
	val<int32_t> b = peLeafThreeReturns(a);
	if (b == 42) {
		return b + 1;
	}
	return b;
}

// Three chained post-call branches: the worst case in this family that
// is still small enough to commit a reference for.  Expected order of
// magnitude: 3^3 ≈ 27 origins ×2 arms at the final CMP ≈ 54 RETURNs.
val<int32_t> pathExplosion_postCallBranch_3(val<int32_t> v) {
	val<int32_t> a = peLeafThreeReturns(v);
	if (a == 42) {
		a = a + 1;
	}
	val<int32_t> b = peLeafThreeReturns(a);
	if (b == 42) {
		b = b + 1;
	}
	val<int32_t> c = peLeafThreeReturns(b);
	if (c == 42) {
		return c + 1;
	}
	return c;
}

// -----------------------------------------------------------------------------
// Tier 4: constraint blindness.  The tracer does not know that the
// outer `x == 1` test implies the inner `x == 1` test on the same arm.
// Both inner CMPs are traced both ways (4 leaf arms) even though only
// 2 are reachable in practice.  This is a small but instructive
// regression on its own and the simplest demonstration of the lack of
// path-predicate tracking in symbolic execution.
//
// Expected: 4 RETURNs in the raw trace.
// -----------------------------------------------------------------------------
val<int32_t> pathExplosion_constraintBlind_dead(val<int32_t> x) {
	val<int32_t> r = 0;
	if (x == 1) {
		if (x == 1) {
			r = 10;
		} else {
			r = 11; // logically dead
		}
	} else {
		if (x == 1) {
			r = 12; // logically dead
		} else {
			r = 13;
		}
	}
	return r;
}

} // namespace nautilus::engine
