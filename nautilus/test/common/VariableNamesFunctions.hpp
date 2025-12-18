#pragma once

#include <nautilus/Engine.hpp>

namespace nautilus::engine {

/**
 * @brief Test basic arithmetic with semantic variable names
 */
val<int32_t> addWithSemanticNames(val<int32_t> x) {
	val<int32_t> a(10, ::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	val<int32_t> b(20, ::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	val<int32_t> sum = a + b;
	sum.set_source_location(::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	return sum;
}

/**
 * @brief Test using VAL_LOC macro for automatic location capture
 */
val<int32_t> multiplyWithLocationMacro(val<int32_t> x) {
	val<int32_t> factor = VAL_LOC(5);
	val<int32_t> result = x * factor;
	return result;
}

/**
 * @brief Test chained operations with location info
 */
val<int32_t> chainedOperationsWithNames(val<int32_t> x) {
	val<int32_t> temp1(x + 1, ::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	val<int32_t> temp2(temp1 * 2, ::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	val<int32_t> result(temp2 - 3, ::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	return result;
}

/**
 * @brief Test comparison operations with location info
 */
val<bool> comparisonWithLocationInfo(val<int32_t> x) {
	val<int32_t> threshold(100, ::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	val<bool> is_greater = x > threshold;
	is_greater.set_source_location(::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	return is_greater;
}

/**
 * @brief Test float operations with semantic names
 */
val<float> floatOperationsWithNames(val<float> x) {
	val<float> pi(3.14159f, ::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	val<float> radius(x, ::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	val<float> area = pi * radius * radius;
	area.set_source_location(::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	return area;
}

/**
 * @brief Test boolean operations with location tracking
 */
val<bool> booleanLogicWithNames(val<int32_t> x, val<int32_t> y) {
	val<bool> check1(x > 0, ::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	val<bool> check2(y < 100, ::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	val<bool> combined = check1 && check2;
	combined.set_source_location(::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	return combined;
}

/**
 * @brief Test division with location info
 */
val<int32_t> divisionWithLocationInfo(val<int32_t> numerator) {
	val<int32_t> divisor(10, ::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	val<int32_t> quotient = numerator / divisor;
	quotient.set_source_location(::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	return quotient;
}

/**
 * @brief Test modulo operation with semantic names
 */
val<int32_t> moduloWithSemanticNames(val<int32_t> x) {
	val<int32_t> divisor(7, ::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	val<int32_t> remainder = x % divisor;
	remainder.set_source_location(::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	return remainder;
}

/**
 * @brief Test bit operations with location tracking
 */
val<int32_t> bitwiseOperationsWithNames(val<int32_t> x) {
	val<int32_t> mask(0xFF, ::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	val<int32_t> masked = x & mask;
	masked.set_source_location(::nautilus::SourceLocation(__FILE__, __LINE__, 0, __PRETTY_FUNCTION__));
	return masked;
}

} // namespace nautilus::engine
