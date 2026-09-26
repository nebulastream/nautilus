
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

template <typename LHS, typename RHS>
concept canMultiply = requires(LHS l, RHS r) { l * r; };

template <typename LHS, typename RHS>
concept canAdd = requires(LHS l, RHS r) { l + r; };

template <typename LHS, typename RHS>
concept canSubtract = requires(LHS l, RHS r) { l - r; };

template <typename LHS, typename RHS>
concept canCompare = requires(LHS l, RHS r) { l == r; };

template <typename LHS, typename RHS>
concept canIndex = requires(LHS l, RHS r) { l[r]; };

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

	SECTION("const and rvalue pointer arithmetic (gh-502)") {
		const val<int*> base = val<int*>(values);
		static_val<uint64_t> two = 2;
		STATIC_REQUIRE(std::is_same_v<decltype(base + two), val<int*>>);
		STATIC_REQUIRE(std::is_same_v<decltype(base + uint64_t {2}), val<int*>>);
		STATIC_REQUIRE(std::is_same_v<decltype(base - uint64_t {2}), val<int*>>);
		STATIC_REQUIRE(std::is_same_v<decltype((base + 1) + 1), val<int*>>);
		REQUIRE(base + two == val<int*>(&values[2]));
		REQUIRE(base + uint64_t {2} == val<int*>(&values[2]));
		REQUIRE((base + 1) + 1 == val<int*>(&values[2]));
		REQUIRE(val<int*>(&values[3]) - uint64_t {2} == val<int*>(&values[1]));
	}

	SECTION("pointer arithmetic follows the built-in pointer rules") {
		val<int*> p = val<int*>(&values[4]);
		const val<int*> cp = p;
		static_val<uint64_t> two = 2;
		STATIC_REQUIRE(std::is_same_v<decltype(2 + p), val<int*>>);
		STATIC_REQUIRE(std::is_same_v<decltype(p - cp), val<std::ptrdiff_t>>);
		STATIC_REQUIRE(std::is_same_v<decltype(p++), val<int*>>);
		STATIC_REQUIRE(std::is_same_v<decltype(+p), val<int*>>);
		REQUIRE(2 + p == val<int*>(&values[6]));
		REQUIRE(two + cp == val<int*>(&values[6]));
		REQUIRE(cp - two == val<int*>(&values[2]));
		REQUIRE(val<int*>(&values[6]) - cp == 2);
		REQUIRE(cp - val<int*>(&values[6]) == -2);
		REQUIRE(val<const int*>(&values[6]) - cp == 2);
		val<int> atTwo = cp[two];
		val<int> atZero = *cp;
		val<int> atMinusOne = cp[int8_t {-1}];
		REQUIRE(atTwo == 7);
		REQUIRE(atZero == 5);
		REQUIRE(atMinusOne == 4);

		auto q = p;
		q += two;
		REQUIRE(q == val<int*>(&values[6]));
		q -= two;
		REQUIRE(q == p);
		REQUIRE(q++ == p);
		REQUIRE(q == val<int*>(&values[5]));
		REQUIRE(q-- == val<int*>(&values[5]));
		REQUIRE(q == p);
		REQUIRE(--q == val<int*>(&values[3]));
	}

	SECTION("operations built-in pointers reject are rejected (gh-502)") {
		STATIC_REQUIRE(!is_fundamental_val<val<int*>>);
		STATIC_REQUIRE(!is_integral_val<val<int*>>);
		STATIC_REQUIRE(!canMultiply<val<int*>, int>);
		STATIC_REQUIRE(!canAdd<val<int*>, val<int*>>);
		STATIC_REQUIRE(!canAdd<val<int*>, double>);
		STATIC_REQUIRE(!canAdd<val<int*>, val<double>>);
		STATIC_REQUIRE(!canAdd<val<void*>, int>);
		STATIC_REQUIRE(!canAdd<val<double>, val<int*>>);
		STATIC_REQUIRE(!canSubtract<val<int*>, val<double*>>);
		STATIC_REQUIRE(!canSubtract<val<int>, val<int*>>);
		STATIC_REQUIRE(!canIndex<val<int*>, val<int*>>);
		STATIC_REQUIRE(!canIndex<val<int*>, double>);
	}

	SECTION("pointers to compatible types compare via their composite pointer type") {
		auto p = val<int*>(&values[1]);
		auto cp = val<const int*>(&values[1]);
		auto vp = val<void*>(&values[2]);
		REQUIRE(p == cp);
		REQUIRE(cp == p);
		REQUIRE(p != vp);
		REQUIRE(cp < val<const int*>(&values[2]));
		REQUIRE(p <= cp);
		REQUIRE(val<const int*>(&values[3]) > p);
		STATIC_REQUIRE(!canCompare<val<int*>, val<double*>>);
		STATIC_REQUIRE(!canCompare<val<int*>, val<int>>);
	}

	SECTION("conversions are implicit only where built-in pointer conversions are") {
		STATIC_REQUIRE(std::is_convertible_v<val<int*>, val<void*>>);
		STATIC_REQUIRE(std::is_convertible_v<val<int*>, val<const int*>>);
		STATIC_REQUIRE(!std::is_convertible_v<val<const int*>, val<int*>>);
		STATIC_REQUIRE(!std::is_convertible_v<val<void*>, val<int*>>);
		STATIC_REQUIRE(!std::is_convertible_v<val<int*>, val<double*>>);
		STATIC_REQUIRE(!std::is_convertible_v<val<int*>, val<uint64_t>>);
		STATIC_REQUIRE(!std::is_convertible_v<val<void*>, val<uint64_t>>);
		// ... but can still be requested explicitly.
		STATIC_REQUIRE(std::is_constructible_v<val<int*>, val<void*>>);
		STATIC_REQUIRE(std::is_constructible_v<val<double*>, val<int*>>);
		STATIC_REQUIRE(std::is_constructible_v<val<uint64_t>, val<int*>>);
		auto p = val<int*>(values);
		REQUIRE(static_cast<val<int*>>(static_cast<val<void*>>(p)) == p);
		REQUIRE(static_cast<val<uintptr_t>>(p) == reinterpret_cast<uintptr_t>(values));
	}

	SECTION("Uninit") {
		auto f1 = val<int*>(nullptr);
		REQUIRE(f1 == nullptr);
		val<int8_t*> f2;
		REQUIRE(f2 == nullptr);
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
