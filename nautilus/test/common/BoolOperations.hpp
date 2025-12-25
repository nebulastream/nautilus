#pragma once

#include <nautilus/Engine.hpp>

namespace nautilus::engine {

val<bool> boolNot(val<bool> x) {
	return !x;
}

val<bool> boolAnd(val<bool> a, val<bool> b) {
	return a && b;
}

val<bool> boolOr(val<bool> a, val<bool> b) {
	return a || b;
}

val<bool> boolAssignment(val<bool> x) {
	auto z = x;
	return z;
}

val<bool> boolAssignmentOr(val<bool> x, val<bool> y) {
	auto z = x;
	x = y;
	return z || y;
}

val<bool> boolConst(val<bool> x) {
	return x && true;
}

val<bool> boolEquals(val<bool> x, val<bool> z) {
	return x == z;
}

val<bool> boolNotEquals(val<bool> x, val<bool> z) {
	return x != z;
}

val<bool> boolIfElse(val<bool> x, val<bool> z) {
	if (x && z) {
		return true;
	} else {
		return false;
	}
}

val<bool> eval(val<bool> x, val<bool> z) {
	return x == z;
}

val<bool> boolNestedFunction(val<bool> x, val<bool> z) {
	if (eval(x, z)) {
		return true;
	} else {
		return false;
	}
}

template <typename T>
val<bool> operatorBool(val<T> x) {
	if (x) {
		return true;
	} else {
		return false;
	}
}

// Comparison operators with raw bool (testing NEW operators added to val_bool.hpp)
val<bool> boolEqualsMixed(val<bool> x, val<bool> y) {
	// Convert y to raw bool to test mixed operations
	bool rawY = y;
	return x == rawY;
}

val<bool> boolNotEqualsMixed(val<bool> x, val<bool> y) {
	// Convert x to raw bool to test mixed operations
	bool rawX = x;
	return rawX != y;
}

// Combined operations test using comparison and logical operators
val<bool> boolComplexOps(val<bool> a, val<bool> b, val<bool> c) {
	// Test: (a == b) && (b != c)
	auto eqResult = a == b;
	auto neqResult = b != c;
	return eqResult && neqResult;
}

// Test probability getter/setter
val<bool> boolProbabilityTest(val<bool> x) {
	x.setIsTrueProbability(0.9);
	// Just return the value to ensure probability doesn't affect logic
	return x;
}

} // namespace nautilus::engine
