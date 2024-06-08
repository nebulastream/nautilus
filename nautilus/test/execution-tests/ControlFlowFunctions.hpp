#pragma once

#include <nautilus/Engine.hpp>

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
val<int32_t> ifInsideLoop(val<int32_t> value) {
	val<int32_t> result = 0;
	for (val<int32_t> i = 0; i < value; i = i + 1) {
		if (i % 2 == 0) {
			result += 1;
		}
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

} // namespace nautilus::engine
