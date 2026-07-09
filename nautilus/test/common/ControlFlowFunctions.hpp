#pragma once

#include "nautilus/function.hpp"
#include <nautilus/Engine.hpp>
#include <nautilus/select.hpp>

namespace nautilus::engine {

val<int32_t> ifThenCondition(val<int32_t> value) {
	val<int32_t> iw = 1;
	if (value == 42) {
		iw = iw + 1;
	}
	return iw + 42;
}

val<int32_t> conditionalReturn(val<int32_t> value) {
	if (value == 42) {
		return 1;
	}
	return 120;
}

val<int32_t> ifThenElseCondition(val<int32_t> value) {
	val<int32_t> iw = val<int32_t>(1);
	if (value == 42) {
		iw = iw + 1;
	} else {
		iw = iw + 42;
	}
	return iw + 42;
}

val<int32_t> nestedIfThenElseCondition(val<int32_t> value, val<int32_t> iw) {
	if (value == 42) {
	} else {
		if (iw == 8) {
		} else {
			iw = iw + 2;
		}
	}
	return iw = iw + 2;
}

val<int32_t> nestedIfNoElseCondition(val<int32_t> value, val<int32_t> iw) {
	if (value == 42) {
		iw = iw + 4;
	} else {
		iw = iw + 9;
		if (iw == 8) {
			iw + 14;
		}
	}
	return iw = iw + 2;
}

val<int32_t> doubleIfCondition() {
	val<int32_t> iw = val<int32_t>(1);
	if (iw == 8) {
		// iw = iw + 14;
	}
	if (iw == 1) {
		iw = iw + 20;
	}
	return iw = iw + 2;
}

val<int32_t> ifElseIfCondition() {
	val<int32_t> iw = val<int32_t>(1);
	if (iw == 8) {
		iw = iw + 14;
	} else if (iw == 1) {
		iw = iw + 20;
	}
	return iw = iw + 2;
}

val<int32_t> orCondition(val<int32_t> value) {
	val<int32_t> iw = 1;
	if (value == 8 || value == 1) {
		iw = iw + 14;
	}
	return iw;
}

val<int32_t> andCondition(val<int32_t> x, val<int32_t> y) {
	val<int32_t> iw = 1;
	if (x == 8 && y == 1) {
		iw = iw + 14;
	}
	return iw;
}

val<int32_t> deeplyNestedIfElseCondition() {
	val<int32_t> iw = val<int32_t>(5);
	if (iw < 8) {
		if (iw > 6) {
			iw = iw + 10;
		} else {
			if (iw < 6) {
				if (iw == 5) {
					iw = iw + 5;
				}
			}
		}
	} else {
		iw = iw + 20;
	}
	return iw = iw + 2;
}

val<int32_t> deeplyNestedIfElseIfCondition() {
	val<int32_t> iw = val<int32_t>(5);
	if (iw < 8) {
		iw = iw + 10;
	} else {
		if (iw == 5) {
			iw = iw + 5;
		} else if (iw == 4) {
			iw = iw + 4;
		}
	}
	return iw = iw + 2;
}

val<bool> andFunction(const val<int32_t> value) {
	val<bool> equals = true;
	equals = equals && (value == (int64_t) 42);
	equals = equals && (value == (int64_t) 42);
	equals = equals && (value == (int64_t) 42);
	return equals;
}

val<int32_t> nestedIf(val<int32_t> value) {
	val<int32_t> result = 1;
	if (value < 20) {
		if (value > 10) {
			result = 2;
		} else {
			result = 3;
		}
	}
	return result;
}

val<int32_t> ifElseIfElse(val<int32_t> value) {
	val<int32_t> result;
	if (value == 0) {
		result = 10;
	} else if (value == 1) {
		result = 20;
	} else {
		result = 30;
	}
	return result;
}
val<int32_t> logicalAnd(val<int32_t> value) {
	val<int32_t> result = 0;
	if (value > 5 && value < 15) {
		result = 1;
	}
	return result;
}
val<int32_t> logicalOr(val<int32_t> value) {
	val<int32_t> result = 0;
	if (value == 10 || value == 20) {
		result = 1;
	}
	return result;
}

val<int32_t> ifNotEqual(val<int32_t> value) {
	val<int32_t> result = 1;
	if (value != 5) {
		result = 2;
	}
	return result;
}
val<int32_t> multipleConditions(val<int32_t> value) {
	val<int32_t> result = 0;
	if ((value > 0 && value < 10) || value == 20) {
		result = 1;
	}
	return result;
}
val<int32_t> ifElseIfOnly(val<int32_t> value) {
	val<int32_t> result = 0;
	if (value < 5) {
		result = 1;
	} else if (value < 10) {
		result = 2;
	}
	return result;
}
val<int32_t> compoundAssignment(val<int32_t> value) {
	val<int32_t> result = 10;
	if (value == 5) {
		result += 5;
	}
	return result;
}

val<int32_t> multipleElse(val<int32_t> value) {
	val<int32_t> result;
	if (value == 10) {
		result = 1;
	} else {
		result = 2;
		if (value == 5) {
			result = 3;
		} else {
			result = 4;
		}
	}
	return result;
}
val<int32_t> ifWithTernary(val<int32_t> value) {
	val<int32_t> result = value > 5 ? 10 : 5;
	if (value == 0) {
		result = -1;
	}
	return result;
}

val<int32_t> complexLogicalExpressions(val<int32_t> value) {
	val<int32_t> result = 0;
	if ((value > 5 && value < 10) || (value > 15 && value < 20)) {
		result = 1;
	}
	return result;
}

val<int32_t> shortCircuitEvaluation(val<int32_t> value) {
	val<int32_t> result = 0;
	if ((value != 0) && ((10 / value) > 1)) {
		result = 1;
	}
	return result;
}

val<int32_t> helperFunction(val<int32_t> x) {
	return x * 2;
}

val<int32_t> ifWithFunctionCall(val<int32_t> value) {
	val<int32_t> result = 0;
	if (helperFunction(value) > 10) {
		result = 1;
	}
	return result;
}
val<int32_t> compoundStatements(val<int32_t> value) {
	val<int32_t> result = 0;
	if (value > 5) {
		result = 1;
		result *= 2;
	}
	return result;
}
val<int32_t> varyingComplexity(val<int32_t> value) {
	val<int32_t> result;
	if (value < 5) {
		result = 1;
	} else if (value >= 5 && value <= 10) {
		result = 2;
		if (value == 7)
			result += 1;
	} else {
		result = 3;
	}
	return result;
}
val<int32_t> logicalXOR(val<int32_t> value) {
	val<int32_t> result = 0;
	auto lt = (value < 10);
	auto gt = (value > 5);
	if (lt != gt) {
		result = 1;
	}
	return result;
}
val<int32_t> nestedIfElseDifferentLevels(val<int32_t> value) {
	val<int32_t> result = 0;
	if (value > 0) {
		if (value < 5) {
			result = 1;
		} else {
			result = 2;
			if (value == 6) {
				result = 3;
			}
		}
	} else {
		result = -1;
	}
	return result;
}

val<int32_t> multipleReturns(val<int32_t> value) {
	if (value == 1) {
		return 1;
	} else if (value < 10) {
		return 42;
	}
	return value + value + 1;
}

int32_t returnValue(int32_t value) {
	return value;
}

val<int32_t> withBranchProbability(val<int32_t> value) {
	auto condition = value == 1;
	condition.setIsTrueProbability(0.9);
	if (condition) {
		return 42;
	} else {
		return invoke(returnValue, value);
	}
}

// Regression (differential fuzzer): a zero-trip loop inside an if-arm, merged
// after the if, then combined with a constant. The merge block's parameter
// reuses the SSA identifier that the untaken arm re-defines (the constant is
// traced once per path), and the taken arm's edge is emitted first, so it
// binds that identifier before the other arm's definition runs. The BC
// backend's emplace-only frame binding then silently dropped the later
// definition: the constant was written to a fresh register nobody read, the
// merge parameter's register stayed zero-initialised, and the final addition
// returned the merged value unmodified.
val<uint64_t> zeroTripLoopMergeThenAddConstant(val<uint64_t> c, val<uint64_t> p) {
	val<uint64_t> result = p;
	if (c != val<uint64_t>(static_cast<uint64_t>(0))) {
		val<uint64_t> acc = static_cast<uint64_t>(0);
		for (val<int32_t> i = 0; i < val<int32_t>(0); i = i + 1) {
			acc = acc + val<uint64_t>(static_cast<uint64_t>(1));
		}
		result = acc;
	}
	return result + val<uint64_t>(static_cast<uint64_t>(119));
}

// Every logical reference to `mem` copy-constructs through this single call
// site, mirroring the differential fuzzer's evalNautilusPtr<T> (which every
// generated `mem` leaf reaches via the same recursive-descent code
// regardless of which AST node it belongs to -- see issue #384).
val<float*> issue384_copyBasePtr(val<float*> mem) {
	return mem;
}

// Called from the SAME call site (the loop in issue384_traceFollowDesync
// below) for both comparisons, differentiated only by a native, untraced
// `bool` -- mirroring the fuzzer's generic AST evaluator dispatching on a
// runtime node kind through shared code.
val<bool> issue384_evalCmp(bool wantEq, val<float*> mem) {
	val<float*> a = issue384_copyBasePtr(mem);
	val<float*> b = issue384_copyBasePtr(mem);
	if (wantEq) {
		return a == b;
	}
	return a != b;
}

// Regression (differential fuzzer, issue #384): traceConstant/traceCopy's
// globalTagMap-collision branch (added for issue #95) folds a primary
// operation plus a same-tagged reconciliation ASSIGN into a single call, but
// LazyTraceContext::follow() only ever consumed one recorded operation per
// call when replaying that call site on a later symbolic-execution
// iteration. The two `issue384_copyBasePtr(mem)` calls below collide onto
// the same tag (identical call-stack shape, identical alive-variable set),
// tripping that reconciliation path during recording; the `if` beneath
// forces a second, FOLLOW-mode trace iteration that replays this code and
// used to desynchronize the follow() cursor, corrupting every operation
// after it in the block (a Debug assert in LazyTraceContext::follow, or
// "std::get: wrong index for variant" in Release).
val<float> issue384_traceFollowDesync(val<float*> mem, val<float> p0) {
	val<float> acc = 0.0f;
	for (int i = 0; i < 2; ++i) {
		val<bool> flag = issue384_evalCmp(i == 0, mem);
		acc = acc + select<float>(flag, val<float>(1.0f), val<float>(0.0f));
	}
	if (p0 > val<float>(0.0f)) {
		return acc + val<float>(1.0f);
	}
	return acc;
}

} // namespace nautilus::engine
