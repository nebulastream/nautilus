

#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/nautilus_function.hpp>
namespace nautilus::engine {

val<int> add_indirect(val<int> a, val<int> b) {
	return a + b;
}

static auto nautilusAdd = NautilusFunction {"add", add_indirect};

val<int> nautilusFunction(val<int> a, val<int> b) {
	auto result = nautilusAdd(a, b);
	return result;
}

val<int> nautilusFunction2(val<int> a, val<int> b) {
	auto result1 = nautilusAdd(a, b);
	auto result2 = nautilusAdd(result1, b);
	auto result3 = nautilusAdd(a, b);
	return result3;
}

// Test: Void function (no return value)
void voidHelper(val<int*> x) {
	*x = *x + val<int>(10);
}

static auto nautilusVoidFunc = NautilusFunction {"voidHelper", voidHelper};

val<int> nautilusFunctionVoid(val<int*> a) {
	nautilusVoidFunc(a);
	return *a;
}

// Test: Function with multiple results
val<int> multiplyHelper(val<int> a, val<int> b) {
	return a * b;
}

static auto nautilusMultiply = NautilusFunction {"multiply", multiplyHelper};

val<int> nautilusFunctionMultipleResults(val<int> a, val<int> b, val<int> c) {
	auto result1 = nautilusMultiply(a, b);
	auto result2 = nautilusMultiply(b, c);
	auto result3 = nautilusMultiply(result1, result2);
	return result3;
}

// Test: Nested calls (function calling another nested function)
val<int> innerHelper(val<int> x) {
	return x * val<int>(2);
}

static auto nautilusInner = NautilusFunction {"inner", innerHelper};

val<int> outerHelper(val<int> x) {
	return nautilusInner(x) + val<int>(5);
}

static auto nautilusOuter = NautilusFunction {"outer", outerHelper};

val<int> nautilusFunctionNested(val<int> a) {
	return nautilusOuter(a);
}

// Test: Conditional with nested function call
val<int> conditionalHelper(val<int> x) {
	return x + val<int>(100);
}

static auto nautilusConditional = NautilusFunction {"conditionalHelper", conditionalHelper};

val<int> nautilusFunctionConditional(val<int> a) {
	if (a > val<int>(10)) {
		return nautilusConditional(a);
	} else {
		return a;
	}
}

// Test: Loop with nested function call
val<int> loopHelper(val<int> x) {
	return x + val<int>(1);
}

static auto nautilusLoopHelper = NautilusFunction {"loopHelper", loopHelper};

val<int> nautilusFunctionLoop(val<int> n) {
	auto sum = val<int>(0);
	for (auto i = val<int>(0); i < n; i = i + val<int>(1)) {
		sum = nautilusLoopHelper(sum);
	}
	return sum;
}

// Test: Multiple different nested functions
val<int> add10(val<int> x) {
	return x + val<int>(10);
}

val<int> multiply2(val<int> x) {
	return x * val<int>(2);
}

val<int> subtract5(val<int> x) {
	return x - val<int>(5);
}

static auto nautilusAdd10 = NautilusFunction {"add10", add10};
static auto nautilusMultiply2 = NautilusFunction {"multiply2", multiply2};
static auto nautilusSubtract5 = NautilusFunction {"subtract5", subtract5};

val<int> nautilusFunctionMultiple(val<int> a) {
	auto step1 = nautilusAdd10(a);
	auto step2 = nautilusMultiply2(step1);
	auto step3 = nautilusSubtract5(step2);
	return step3;
}

// Test: Recursive-style (same function called multiple times with different args)
val<int> recursiveHelper(val<int> x, val<int> depth) {
	if (depth <= val<int>(0)) {
		return x;
	}
	return x + val<int>(1);
}

static auto nautilusRecursive = NautilusFunction {"recursiveHelper", recursiveHelper};

val<int> nautilusFunctionRecursiveStyle(val<int> a) {
	auto result = a;
	result = nautilusRecursive(result, val<int>(3));
	result = nautilusRecursive(result, val<int>(2));
	result = nautilusRecursive(result, val<int>(1));
	return result;
}

// Test: Function with complex computation
val<int> complexComputation(val<int> a, val<int> b, val<int> c) {
	auto temp1 = a * b;
	auto temp2 = b + c;
	auto temp3 = temp1 - temp2;
	return temp3 * val<int>(2);
}

static auto nautilusComplex = NautilusFunction {"complexComputation", complexComputation};

val<int> nautilusFunctionComplex(val<int> x, val<int> y) {
	auto result1 = nautilusComplex(x, y, val<int>(5));
	auto result2 = nautilusComplex(result1, x, y);
	return result2;
}

} // namespace nautilus::engine
