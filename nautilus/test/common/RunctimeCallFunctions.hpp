#pragma once

#include "nautilus/inline.hpp"
#include <cmath>
#include <nautilus/Engine.hpp>
#include <nautilus/inline.hpp>
#include <nautilus/val_ptr.hpp>

namespace nautilus {

class ClazzBase {
public:
	virtual int32_t add(int32_t x) = 0;
};

class Clazz : public ClazzBase {
public:
	int32_t get() {
		return i;
	}

	int32_t add(int32_t x) override {
		return x + i;
	}

	int32_t addConst(int32_t x) const {
		return x + i;
	}

	int i = 42;
};

} // namespace nautilus

namespace nautilus::engine {

NAUT_INLINE void voidFunc(int32_t x, int32_t y) {
	[[maybe_unused]] auto z = x + y;
}

val<int32_t> useFirstArg(val<int32_t> x, [[maybe_unused]] val<int32_t> y) {
	return x;
}

val<int32_t> useSecondArg([[maybe_unused]] val<int32_t> x, val<int32_t> y) {
	return y;
}

val<int32_t> useNoArg([[maybe_unused]] val<int32_t> x, [[maybe_unused]] val<int32_t> y) {
	return 42;
}

void multipleVoidReturnsFunction(val<int32_t*> x) {
	val<int32_t> val = *x;
	if (val > 10) {
		*x = 1;
		return;
	}
	*x = 42;
}

NAUT_INLINE int32_t add(int32_t x, int32_t y) {
	return x + y;
}

NAUT_INLINE int32_t sub(int32_t x, int32_t y) {
	return x - y;
}

NAUT_INLINE int32_t addAndSub(int32_t x, int32_t y) {
	return add(x, y) + sub(x, y);
}

NAUT_INLINE __attribute__((noinline)) double helper1(double val) {
	return val * val + 3.14;
}

NAUT_INLINE __attribute__((noinline)) double helper2(double val) {
	return std::cos(val) / (1.0 + std::abs(val));
}

NAUT_INLINE double complexCalc(int32_t a, int32_t b) {
	double acc = 0.0;
	std::vector<double> data;
	data.reserve(a);
	for (int32_t i = 0; i < a; i++) {
		data.push_back(i);
	}

	for (auto& x : data) {
		double temp = helper1(x) * b;
		if (temp > 10.0)
			acc += helper2(temp);
		else
			acc -= helper2(temp);
	}
	return std::exp(acc);
}

struct Base {
	virtual ~Base() = default;
};

struct Derived : Base {
	void greet() {
		std::cout << "Hello World!\n";
	}
};

NAUT_INLINE int32_t testDynamicCast(Base* b) {
	if (dynamic_cast<Derived*>(b)) {
		return 0;
	}
	return 1;
}

val<int32_t> simpleDirectCall(val<int32_t> x, val<int32_t> y) {
	return invoke<>(add, x, y);
}

val<int32_t> directCallWithNestedCalls(val<int32_t> x, val<int32_t> y) {
	return invoke<>(addAndSub, x, y);
}

val<int32_t> callTwoFunctions(val<int32_t> x, val<int32_t> y) {
	return invoke<>(add, x, y) + invoke<>(sub, x, y);
}

val<int32_t> loopDirectCall(val<int32_t> c, val<int32_t> x) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < c; i = i + 1) {
		sum = invoke<>(add, sum, x);
	}
	return sum;
}

inline int32_t get42() {
	return 42;
}

inline val<int32_t> loopDirectCall2(val<int32_t> x) {
	val<int32_t> sum = 0;
	val<int32_t> c = invoke(get42);
	for (val<uint64_t> i = 0; i < c; i = i + 1) {
		sum = invoke<>(add, sum, x);
	}
	return sum;
}

val<int32_t> directCallComplexFunction(val<int32_t> x, val<int32_t> y) {
	return invoke<>(complexCalc, x, y);
}

val<int32_t> dynCastCall(val<Base*> x) {
	return invoke<>(testDynamicCast, x);
}

val<int32_t> voidFuncCall(val<int32_t> x, val<int32_t> y) {
	invoke<>(voidFunc, x, y);
	return x;
}

val<int32_t> lambdaRuntimeFunction(val<int32_t> x) {
	return invoke<>(+[](int32_t x) { return x * 2; }, x);
}

val<int32_t> nestedLambdaRuntimeFunction(val<int32_t> x) {
	return invoke<>(+[](int32_t x) { return x * 2; }, invoke<>(+[](int32_t x) { return x + 2; }, x));
}

val<int32_t> callSameFunction(val<int32_t> x) {
	auto func = function(add);
	x = func(x, x);
	x = func(x, x);
	return x;
}

val<int32_t> callMemberFunction(val<Clazz*> x) {
	auto& func = memberFunc(&Clazz::get);
	auto res = func(x);
	return res;
}

} // namespace nautilus::engine
