
#include "nautilus/function.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <iostream>
#include <nautilus/Engine.hpp>

namespace nautilus {

struct Funcs {
	val<int> add(val<int> a, val<int> b) {
		return a + b;
	}
};

val<int> add(val<int> a, val<int> b) {
	return a + b;
}

void d(val<int>, val<int>) {
}

TEMPLATE_TEST_CASE("Function Test", "[value][template]", double) {

	// func(a, v);

	// auto f1 = std::bind(func, std::placeholders::_1, v);
	// f1(v);
	//[[maybe_unused]]auto x = func(v, v);

	// std::invoke(func, v, v);
}
} // namespace nautilus
