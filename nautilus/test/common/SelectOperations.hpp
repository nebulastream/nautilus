#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/select.hpp>

namespace nautilus::engine {

// Integer select tests
val<int32_t> selectInt32True(val<bool> condition, val<int32_t> true_value, val<int32_t> false_value) {
	return select(condition, true_value, false_value);
}

val<int64_t> selectInt64True(val<bool> condition, val<int64_t> true_value, val<int64_t> false_value) {
	return select(condition, true_value, false_value);
}

// Unsigned integer select tests
val<uint32_t> selectUInt32(val<bool> condition, val<uint32_t> true_value, val<uint32_t> false_value) {
	return select(condition, true_value, false_value);
}

val<uint64_t> selectUInt64(val<bool> condition, val<uint64_t> true_value, val<uint64_t> false_value) {
	return select(condition, true_value, false_value);
}

// Float select tests
val<float> selectFloat(val<bool> condition, val<float> true_value, val<float> false_value) {
	return select(condition, true_value, false_value);
}

val<double> selectDouble(val<bool> condition, val<double> true_value, val<double> false_value) {
	return select(condition, true_value, false_value);
}

// Boolean select test
val<bool> selectBool(val<bool> condition, val<bool> true_value, val<bool> false_value) {
	return select(condition, true_value, false_value);
}

// Complex test: select in a loop
val<int32_t> selectInLoop(val<int32_t> count) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < count; i = i + 1) {
		// Select even or odd value based on condition
		val<int32_t> value = select(i % 2 == 0, val<int32_t>(10), val<int32_t>(20));
		sum = sum + value;
	}
	return sum;
}

// Complex test: select based on comparison
val<int32_t> selectBasedOnComparison(val<int32_t> x, val<int32_t> y) {
	return select(x > y, x, y); // max(x, y)
}

// Complex test: nested select
val<int32_t> nestedSelect(val<int32_t> x) {
	val<bool> condition1 = x > 10;
	val<bool> condition2 = x > 5;
	val<int32_t> result1 = select(condition2, val<int32_t>(100), val<int32_t>(50));
	return select(condition1, val<int32_t>(200), result1);
}

// Test with small integer types
val<int8_t> selectInt8(val<bool> condition, val<int8_t> true_value, val<int8_t> false_value) {
	return select(condition, true_value, false_value);
}

val<int16_t> selectInt16(val<bool> condition, val<int16_t> true_value, val<int16_t> false_value) {
	return select(condition, true_value, false_value);
}

// Pointer select tests
val<int32_t*> selectPointer(val<bool> condition, val<int32_t*> true_value, val<int32_t*> false_value) {
	return select(condition, true_value, false_value);
}

// Test pointer select with dereferencing
val<int32_t> selectPointerAndDeref(val<bool> condition, val<int32_t*> ptr1, val<int32_t*> ptr2) {
	val<int32_t*> selected = select(condition, ptr1, ptr2);
	return *selected;
}

} // namespace nautilus::engine
