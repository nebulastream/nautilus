
#include "nautilus/nautilus_function.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <iostream>
#include <nautilus/Engine.hpp>

namespace nautilus {

val<int> add(val<int> a, val<int> b) {
	return a + b;
}

void call_void(val<int*> a, val<int> b) {
	*a = *a + b;
}

static auto nautilusVoid = NautilusFunction {"call_void", call_void};
static auto nautilusAdd = NautilusFunction {"call_add", add};

TEST_CASE("Function Test") {
	val<int> a = 10;
	val<int> b = 20;

	auto result = nautilusAdd(a, b);
	REQUIRE(result == 30);
}

TEST_CASE("Function Void Test") {
	int a = 10;
	val<int*> aPtr = &a;
	val<int> b = 20;

	nautilusVoid(aPtr, b);
	REQUIRE(a == 30);
}

struct Calculator {
	val<int> mul(val<int> a, val<int> b) {
		return a * b;
	}
};

static Calculator calc;

// static wrapper for member function bound to instance `calc`
static auto nautilusMul = NautilusFunction {"call_mul", bind_instance<&Calculator::mul>(calc)};

TEST_CASE("Member Function Test") {
	val<int> a = 5;
	val<int> b = 7;

	auto result = nautilusMul(a, b);
	REQUIRE(result == 35);
}

TEST_CASE("getFuncPtr Test") {
	// Test that getFuncPtr() returns a valid function pointer outside of tracing
	auto funcPtr = nautilusAdd.getFuncPtr();
	// The function pointer should be callable and produce correct results
	auto result = funcPtr(val<int>(3), val<int>(4));
	REQUIRE(result == 7);
}

} // namespace nautilus
