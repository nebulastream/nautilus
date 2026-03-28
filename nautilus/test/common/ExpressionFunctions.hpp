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

// The result of int8_t - int8_t is int32_t
auto subInt8AndInt8(val<int8_t> x, val<int8_t> y) {
	const auto result = x - y;
	return result;
}

val<int8_t> subInt8AndInt8TruncateResult(val<int8_t> x, val<int8_t> y) {
	const auto result = x - y;
	return result;
}

auto mulInt64AndNotDefinedI64(val<int64_t> x) {
	// We want to test, if the default initialization is correct
	val<int64_t> y;
	const auto result = x * y;
	return result;
}

auto addInt8AndInt32(val<int8_t> x, val<int32_t> y) {
	const auto result = x + y;
	return result;
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

template <typename T>
val<T> shiftLeft(val<T> x, val<T> y) {
	return x << y;
}

template <typename T>
val<T> shiftRight(val<T> x, val<T> y) {
	return x >> y;
}

template <typename T>
val<T> negate(val<T> x) {
	return ~x;
}

template <typename T>
val<T> logicalNot(val<T> x) {
	return !x;
}

struct RObject {
	val<int32_t> a;
	val<int32_t> b;
};

RObject returnObject(val<int32_t> a, val<int32_t> b) {
	if (a > b) {
		return {.a = a, .b = b};
	}
	return {.a = 0, .b = 0};
}

val<int32_t> constructComplexReturnObject(val<int32_t> a, val<int32_t> b) {
	auto r = returnObject(a, b);
	auto t1 = r.a + 1;
	auto t2 = r.b + 1;
	return t1 + t2;
}

val<int32_t> constructComplexReturnObject2(val<int32_t> a, val<int32_t> b) {
	val<int32_t> t = 0;
	{ t = constructComplexReturnObject(a, b); }
	return t + 42;
}

// ============================================================================
// C++ Semantic Equivalence Test Functions
// ============================================================================

/// Tests chained prefix increment: ++(++x)
/// In C++, this is valid and should increment x twice
val<int32_t> chainedPrefixIncrement(val<int32_t> x) {
	++(++x);
	return x;
}

/// Tests chained prefix decrement: --(--x)
/// In C++, this is valid and should decrement x twice
val<int32_t> chainedPrefixDecrement(val<int32_t> x) {
	--(--x);
	return x;
}

/// Tests unary plus on a value
val<int32_t> unaryPlus(val<int32_t> x) {
	return +x;
}

/// Tests unary minus (negation) on a value
val<int32_t> unaryMinus(val<int32_t> x) {
	return -x;
}

/// Tests move assignment semantics
val<int32_t> moveAssignment(val<int32_t> x) {
	val<int32_t> y = 0;
	y = std::move(x);
	return y;
}

/// Tests that prefix increment returns a reference that can be used
val<int32_t> prefixIncrementReturnValue(val<int32_t> x) {
	// ++x should return x after increment, so (++x) + 1 = x + 2
	return (++x) + 1;
}

/// Tests that prefix decrement returns a reference that can be used
val<int32_t> prefixDecrementReturnValue(val<int32_t> x) {
	// --x should return x after decrement, so (--x) + 1 = x
	return (--x) + 1;
}

} // namespace nautilus::engine
