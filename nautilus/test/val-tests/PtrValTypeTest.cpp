
#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"
#include <catch2/catch_all.hpp>
#include <iostream>

namespace nautilus {

struct Y {
	int x;
};

struct X {
	int x;
	int y;
	Y h;

	auto add(int z) {
		return x * z;
	}
};

TEST_CASE("Ptr Val Test") {
	int values[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	SECTION("==") {
		auto f1 = val<int*>(values);
		auto f2 = val<int*>(values);
		REQUIRE(f2 == f1);
	}
	SECTION("!=") {
		auto f1 = val<int*>(values);
		auto f2 = val<int*>(&values[1]);
		REQUIRE(f2 != f1);
	}
	SECTION(">") {
		auto f1 = val<int*>(values);
		auto f2 = val<int*>(&values[1]);
		REQUIRE(f2 > f1);
	}
	SECTION(">=") {
		auto f1 = val<int*>(values);
		auto f2 = val<int*>(&values[1]);
		REQUIRE(f2 >= f1);
	}
	SECTION("<") {
		auto f1 = val<int*>(values);
		auto f2 = val<int*>(&values[1]);
		REQUIRE(f1 < f2);
	}
	SECTION("<=") {
		auto f1 = val<int*>(values);
		auto f2 = val<int*>(&values[1]);
		REQUIRE(f1 <= f2);
	}
	SECTION("+") {
		auto f1 = val<int*>(values);
		auto f2 = val<int*>(values);
		REQUIRE(f1 == f2);
	}
	SECTION("*") {
		auto f1 = val<int*>(values);
		val<int> value1 = *f1;
		REQUIRE(value1 == 1);
		(*f1) = 42;
		value1 = *f1;
		REQUIRE(value1 == 42);
	}
	SECTION("[]") {
		auto f1 = val<int*>(values);
		val<int> v = f1[0];
		REQUIRE(v == 1);
		val<int> v2 = f1[1];
		REQUIRE(v2 == 2);
		val<int> v3 = f1[2];
		REQUIRE(v3 == 3);
	}
}

/*

TEMPLATE_TEST_CASE("Ptr Val Operation Test", "[value][template]", int8_t) {
    SECTION("comparison operators") {
        TestType value = 42;
        [[maybe_unused]] X xValue = X{1, 42};

        [[maybe_unused]]  val<X *> ptrX = &xValue;
        [[maybe_unused]]  val<int &> res = ptrX.get(&X::x);
        res = 66;

        [[maybe_unused]]  auto resF = ptrX.invoke(&X::add, 42);

        [[maybe_unused]]  auto cptr = &xValue;
        //auto opajsd = __builtin_offsetof(X, &X::y)
        [[maybe_unused]] auto diffx = OFFSET_OF(&X::x);
        [[maybe_unused]] auto diffy = OFFSET_OF(&X::y);
        [[maybe_unused]] auto diffh = OFFSET_OF(&X::h);


        val<int8_t *> ptr = &value;


        [[maybe_unused]] auto c1 = ptr + 64;
        auto o = val<size_t>(64);
        [[maybe_unused]] val<int8_t *> c2_offset = ptr + o;
        [[maybe_unused]] auto c2 = (val<int64_t *>) c2_offset;

        [[maybe_unused]] int8_t *p1 = &value;
        [[maybe_unused]] int64_t *p2 = (int64_t *) p1;


    }
    */

} // namespace nautilus
