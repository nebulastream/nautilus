#pragma once

#include <nautilus/Engine.hpp>

namespace nautilus::engine {

val<int32_t> assignment1(val<int32_t> x) {
	auto y = x;
	return y;
}

val<int32_t> assignment2(val<int32_t> x) {
	auto y = x;
	y = y + 1;
	return y;
}

val<int32_t> assignment3(val<int32_t> x) {
	auto y = x;
	y = y + 1;
	return x;
}

val<int32_t> assignment4(val<int32_t> x) {
	val<int32_t> y = 42;
	y = x + 1;
	return y;
}

val<int32_t> assignment5(val<int32_t> x) {
	val<int32_t> y = 42;
	y = x + 1;
	return x;
}

val<int32_t> assignmentSwap(val<int32_t> x) {
	auto t = x;
	x = 42;
	x = t + x;
	return x;
}


val<int8_t> int8AddExpression(val<int8_t> x) {
	val<int8_t> y = (int8_t) 2;
	return y + x;
}

val<int16_t> int16AddExpression(val<int16_t> x) {
	val<int16_t> y = (int16_t) 5;
	return x + y;
}

val<int32_t> int32AddExpression(val<int32_t> x) {
	val<int32_t> y = 5;
	return x + y;
}

val<int64_t> int64AddExpression(val<int64_t> x) {
	val<int64_t> y = (int64_t) 7;
	return x + y;
}

val<float> floatAddExpression(val<float> x) {
	val<float> y = 7.0f;
	return x + y;
}

val<double> doubleAddExpression(val<double> x) {
	val<double> y = 7.0;
	return x + y;
}

val<double> castFloatToDoubleAddExpression(val<float> x) {
	val<double> y = 7.0;
	return x + y;
}

val<int64_t> castInt8ToInt64AddExpression(val<int8_t> x) {
	val<int64_t> y = (int64_t) 7;
	return x + y;
}

val<int64_t> castInt8ToInt64AddExpression2(val<int8_t> x) {
	val<int64_t> y = (int64_t) 42;
	return y + x;
}

val<int32_t> incrementPost(val<int32_t> x) {
	auto y = x++;
	return y + x;
}

val<int32_t> incrementPre(val<int32_t> x) {
	auto y = ++x;
	return y + x;
}

val<int32_t> decrementPost(val<int32_t> x) {
	auto y = x--;
	return y + x;
}

val<int32_t> decrementPre(val<int32_t> x) {
	auto y = --x;
	return y + x;
}

val<int32_t> assignAdd(val<int32_t> x) {
	x += 5;
	return x;
}

val<int32_t> assignSub(val<int32_t> x) {
	x -= 5;
	return x;
}

val<int32_t> assignMul(val<int32_t> x) {
	x *= 5;
	return x;
}

val<int32_t> assignDiv(val<int32_t> x) {
	x /= 5;
	return x;
}

val<int32_t> assignMod(val<int32_t> x) {
	x %= 5;
	return x;
}

val<int32_t> assignAnd(val<int32_t> x) {
	x &= 5;
	return x;
}

val<int32_t> assignOr(val<int32_t> x) {
	x |= 5;
	return x;
}

val<int32_t> assignXor(val<int32_t> x) {
	x ^= 5;
	return x;
}

val<int32_t> assignShl(val<int32_t> x) {
	x <<= 5;
	return x;
}

val<int32_t> assignShr(val<int32_t> x) {
	x >>= 5;
	return x;
}
} // namespace nautilus::engine
