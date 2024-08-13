#pragma once

#include <nautilus/Engine.hpp>

namespace nautilus {

class ClazzBase {
public:
	virtual int32_t add(int32_t x) = 0;
};

class Clazz : public ClazzBase {
public:
	int32_t add(int32_t x) override {
		return x + i;
	}

	int32_t addConst(int32_t x) const {
		return x + i;
	}

	int i = 42;
};

template <>
class val<Clazz*> {
public:
	val();

	val<int32_t> addConst(val<int32_t> x);
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
	return;
}

int32_t add(int32_t x, int32_t y) {
	return x + y;
}

val<int32_t> simpleDirectCall(val<int32_t> x, val<int32_t> y) {
	return invoke<>(add, x, y);
}

val<int32_t> loopDirectCall(val<int32_t> c, val<int32_t> x) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < c; i = i + 1) {
		sum = invoke<>(add, sum, x);
	}
	return sum;
}

val<int32_t> voidFuncCall(val<int32_t> x, val<int32_t> y) {
	invoke<>(voidFunc, x, y);
	return x;
}

} // namespace nautilus::engine
