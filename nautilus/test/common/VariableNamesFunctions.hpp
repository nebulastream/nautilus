#pragma once

#include <nautilus/Engine.hpp>

namespace nautilus::engine {

/**
 * @brief Test basic arithmetic with semantic variable names
 */
val<int32_t> addWithSemanticNames([[maybe_unused]] val<int32_t> x) {
	val<int32_t> a(10);
	val<int32_t> b(20);
	val<int32_t> sum = (a + b);
	return sum;
}

/**
 * @brief Test using macro-based value creation
 */
val<int32_t> multiplyWithLocationMacro(val<int32_t> x) {
	val<int32_t> factor(5);
	val<int32_t> result = x * factor;
	return result;
}

/**
 * @brief Test chained operations
 */
val<int32_t> chainedOperationsWithNames(val<int32_t> x) {
	val<int32_t> temp1 = x + 1;
	val<int32_t> temp2 = temp1 * 2;
	val<int32_t> result = temp2 - 3;
	return result;
}

/**
 * @brief Test comparison operations
 */
val<bool> comparisonWithLocationInfo(val<int32_t> x) {
	val<int32_t> threshold(100);
	val<bool> is_greater = x > threshold;
	return is_greater;
}

/**
 * @brief Test float operations
 */
val<float> floatOperationsWithNames(val<float> x) {
	val<float> pi(3.14159f);
	val<float> radius = x;
	val<float> area = pi * radius * radius;
	return area;
}

/**
 * @brief Test boolean operations
 */
val<bool> booleanLogicWithNames(val<int32_t> x, val<int32_t> y) {
	val<bool> check1 = x > 0;
	val<bool> check2 = y < 100;
	val<bool> combined = check1 && check2;
	return combined;
}

/**
 * @brief Test division
 */
val<int32_t> divisionWithLocationInfo(val<int32_t> numerator) {
	val<int32_t> divisor(10);
	val<int32_t> quotient = numerator / divisor;
	return quotient;
}

/**
 * @brief Test modulo operation
 */
val<int32_t> moduloWithSemanticNames(val<int32_t> x) {
	val<int32_t> divisor(7);
	val<int32_t> remainder = x % divisor;
	return remainder;
}

/**
 * @brief Test bitwise operations
 */
val<int32_t> bitwiseOperationsWithNames(val<int32_t> x) {
	val<int32_t> mask(0xFF);
	val<int32_t> masked = x & mask;
	return masked;
}

} // namespace nautilus::engine
