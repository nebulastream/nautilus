
#pragma once

#include "nautilus/val.hpp"

namespace nautilus {

// Fixtures for constant-branch pruning ("engine.tracePruning"). Each condition
// below is a trace-time constant, so with pruning enabled the dead side is never
// explored and no CMP is recorded.

val<int32_t> pruneConstantTrueBranch(val<int32_t> x) {
	val<bool> flag = true;
	if (flag) {
		return x + 1;
	}
	return x + 2;
}

val<int32_t> pruneConstantFalseBranch(val<int32_t> x) {
	val<bool> flag = false;
	if (flag) {
		return x + 1;
	}
	return x + 2;
}

// Constant-ness propagates through copies and assignments.
val<int32_t> pruneAssignedConstantBranch(val<int32_t> x) {
	val<bool> flag = true;
	val<bool> copy = flag;
	if (copy) {
		return x * 2;
	}
	return x;
}

// A constant outer branch combined with a dynamic inner branch: only the inner
// CMP shows up in the trace.
val<int32_t> pruneMixedConstantAndDynamic(val<int32_t> x) {
	val<bool> flag = true;
	if (flag) {
		if (x == 5) {
			return 1;
		}
		return 2;
	}
	return 3;
}

// Overwriting a constant with a dynamic value makes the branch traceable again.
val<int32_t> pruneOverwrittenConstant(val<int32_t> x, val<bool> cond) {
	val<bool> flag = true;
	flag = cond;
	if (flag) {
		return x + 1;
	}
	return x + 2;
}

// A constant assigned inside one arm of a dynamic branch must not leak into the
// other arm: on the path that skips the assignment, the branch on @p flag is
// dynamic and must record a CMP. Regression fixture for the snapshotting tracers,
// whose per-branch state copies must include the constant-tracking map.
val<int32_t> pruneConstantSetInBranch(val<int32_t> x, val<bool> cond) {
	val<bool> flag = cond;
	if (cond) {
		flag = true;
	}
	if (flag) {
		return x + 1;
	}
	return x + 2;
}

// A constant-true loop with a traced exit still forms a proper loop: the loop
// head CMP is pruned, but the body's first repeated operation closes the loop
// via the regular control-flow merge.
val<int32_t> pruneConstantTrueLoopWithExit(val<int32_t> x) {
	val<bool> always = true;
	while (always) {
		x = x + 1;
		if (x > 10) {
			return x;
		}
	}
	return 0;
}

// A constant-true loop without any traced operation or exit cannot terminate;
// the tracer must detect it and fail instead of spinning forever.
val<int32_t> pruneConstantTrueEndlessLoop(val<int32_t> x) {
	val<bool> always = true;
	while (always) {
	}
	return x;
}

} // namespace nautilus
