#pragma once

#include <nautilus/Engine.hpp>
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

void voidFunc(int32_t x, int32_t y) {
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

int32_t add(int32_t x, int32_t y) {
	return x + y;
}

int32_t sub(int32_t x, int32_t y) {
	return x - y;
}

val<int32_t> simpleDirectCall(val<int32_t> x, val<int32_t> y) {
	return invoke<>(add, x, y);
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
