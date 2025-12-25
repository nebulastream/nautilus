#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/ifThenElse.hpp>

namespace nautilus::engine {

// Basic ifThenElse with integer result
val<int32_t> ifThenElseBasic(val<bool> condition, val<int32_t> true_value, val<int32_t> false_value) {
	return ifThenElse(condition, [&]() { return true_value; }, [&]() { return false_value; });
}

// ifThenElse with computation in branches
val<int32_t> ifThenElseWithComputation(val<int32_t> x, val<int32_t> y) {
	return ifThenElse(x > y, [&]() { return x + val<int32_t>(10); }, [&]() { return y + val<int32_t>(20); });
}

// ifThenElse with constant values
val<int32_t> ifThenElseConstants(val<bool> condition) {
	return ifThenElse(condition, []() { return val<int32_t>(100); }, []() { return val<int32_t>(200); });
}

// Nested ifThenElse
val<int32_t> nestedIfThenElse(val<int32_t> x) {
	return ifThenElse(
	    x < val<int32_t>(10),
	    [&]() {
		    return ifThenElse(x < val<int32_t>(5), []() { return val<int32_t>(1); }, []() { return val<int32_t>(2); });
	    },
	    []() { return val<int32_t>(3); });
}

// ifThenElse with float type
val<float> ifThenElseFloat(val<bool> condition, val<float> true_value, val<float> false_value) {
	return ifThenElse(condition, [&]() { return true_value; }, [&]() { return false_value; });
}

// ifThenElse with double type
val<double> ifThenElseDouble(val<bool> condition, val<double> true_value, val<double> false_value) {
	return ifThenElse(condition, [&]() { return true_value; }, [&]() { return false_value; });
}

// ifThenElse with boolean result
val<bool> ifThenElseBool(val<bool> condition, val<bool> true_value, val<bool> false_value) {
	return ifThenElse(condition, [&]() { return true_value; }, [&]() { return false_value; });
}

// ifThenElse max function (common use case)
val<int32_t> ifThenElseMax(val<int32_t> a, val<int32_t> b) {
	return ifThenElse(a > b, [&]() { return a; }, [&]() { return b; });
}

// ifThenElse min function
val<int32_t> ifThenElseMin(val<int32_t> a, val<int32_t> b) {
	return ifThenElse(a < b, [&]() { return a; }, [&]() { return b; });
}

// ifThenElse absolute value
val<int32_t> ifThenElseAbs(val<int32_t> x) {
	return ifThenElse(x >= val<int32_t>(0), [&]() { return x; }, [&]() { return val<int32_t>(0) - x; });
}

// ifThenElse clamp function
val<int32_t> ifThenElseClamp(val<int32_t> x, val<int32_t> lo, val<int32_t> hi) {
	return ifThenElse(
	    x < lo, [&]() { return lo; }, [&]() { return ifThenElse(x > hi, [&]() { return hi; }, [&]() { return x; }); });
}

// void ifThenElse with side effects (modifying a value)
// Note: The lambdas must explicitly return void by not returning the assignment result
void ifThenElseVoidSideEffect(val<bool> condition, val<int32_t>& result) {
	ifThenElse(
	    condition, [&]() -> void { result = result + val<int32_t>(1); },
	    [&]() -> void { result = result + val<int32_t>(2); });
}

// ifThenElse with comparison result
val<int32_t> ifThenElseCompare(val<int32_t> a, val<int32_t> b, val<int32_t> c) {
	auto cond = (a == b);
	return ifThenElse(cond, [&]() { return c; }, [&]() { return a + b; });
}

// Chain of ifThenElse (classify a number)
val<int32_t> ifThenElseClassify(val<int32_t> x) {
	return ifThenElse(
	    x < val<int32_t>(0), []() { return val<int32_t>(-1); },
	    [&]() {
		    return ifThenElse(x > val<int32_t>(0), []() { return val<int32_t>(1); }, []() { return val<int32_t>(0); });
	    });
}

} // namespace nautilus::engine
