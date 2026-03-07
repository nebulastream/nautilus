
#include "nautilus/static.hpp"
#include <catch2/catch_all.hpp>
#include <type_traits>

namespace nautilus {

TEMPLATE_TEST_CASE("Static Val Construction Test", "[static_val][template]", int8_t, int16_t, int32_t, int64_t, uint8_t,
                   uint16_t, uint32_t, uint64_t) {
	SECTION("default construction") {
		static_val<TestType> sv;
		// default-constructed static_val has indeterminate value, just verify it compiles
		(void) sv;
	}
	SECTION("value construction") {
		static_val<TestType> sv(static_cast<TestType>(42));
		REQUIRE(static_cast<TestType>(sv) == static_cast<TestType>(42));
	}
	SECTION("zero construction") {
		static_val<TestType> sv(static_cast<TestType>(0));
		REQUIRE(static_cast<TestType>(sv) == static_cast<TestType>(0));
	}
	SECTION("copy construction") {
		static_val<TestType> sv1(static_cast<TestType>(42));
		static_val<TestType> sv2(sv1);
		REQUIRE(static_cast<TestType>(sv2) == static_cast<TestType>(42));
	}
	SECTION("cross-type copy construction") {
		static_val<int32_t> sv1(42);
		static_val<TestType> sv2(sv1);
		REQUIRE(static_cast<TestType>(sv2) == static_cast<TestType>(42));
	}
}

TEMPLATE_TEST_CASE("Static Val Conversion Test", "[static_val][template]", int8_t, int16_t, int32_t, int64_t, uint8_t,
                   uint16_t, uint32_t, uint64_t) {
	SECTION("implicit conversion to raw type") {
		static_val<TestType> sv(static_cast<TestType>(42));
		TestType raw = sv;
		REQUIRE(raw == static_cast<TestType>(42));
	}
	SECTION("explicit cast to raw type") {
		static_val<TestType> sv(static_cast<TestType>(42));
		auto raw = static_cast<TestType>(sv);
		REQUIRE(raw == static_cast<TestType>(42));
	}
}

TEMPLATE_TEST_CASE("Static Val Assignment Test", "[static_val][template]", int8_t, int16_t, int32_t, int64_t, uint8_t,
                   uint16_t, uint32_t, uint64_t) {
	SECTION("raw value assignment") {
		static_val<TestType> sv(static_cast<TestType>(0));
		sv = static_cast<TestType>(42);
		REQUIRE(static_cast<TestType>(sv) == static_cast<TestType>(42));
	}
	SECTION("same-type copy assignment") {
		static_val<TestType> sv1(static_cast<TestType>(42));
		static_val<TestType> sv2(static_cast<TestType>(0));
		sv2 = sv1;
		REQUIRE(static_cast<TestType>(sv2) == static_cast<TestType>(42));
	}
	SECTION("cross-type assignment") {
		static_val<int32_t> sv1(99);
		static_val<TestType> sv2(static_cast<TestType>(0));
		sv2 = sv1;
		REQUIRE(static_cast<TestType>(sv2) == static_cast<TestType>(99));
	}
	SECTION("self-assignment") {
		static_val<TestType> sv(static_cast<TestType>(42));
		auto& ref = sv;
		sv = ref;
		REQUIRE(static_cast<TestType>(sv) == static_cast<TestType>(42));
	}
	SECTION("chained assignment") {
		static_val<TestType> sv1(static_cast<TestType>(0));
		static_val<TestType> sv2(static_cast<TestType>(0));
		// sv1 = (sv2 = 42) assigns 42 to sv2, returns T, then assigns to sv1
		sv1 = static_cast<TestType>(sv2 = static_cast<TestType>(42));
		REQUIRE(static_cast<TestType>(sv1) == static_cast<TestType>(42));
		REQUIRE(static_cast<TestType>(sv2) == static_cast<TestType>(42));
	}
}

TEMPLATE_TEST_CASE("Static Val Comparison Test", "[static_val][template]", int8_t, int16_t, int32_t, int64_t, uint8_t,
                   uint16_t, uint32_t, uint64_t) {
	SECTION("operator==") {
		static_val<TestType> sv(static_cast<TestType>(42));
		REQUIRE(sv == static_cast<TestType>(42));
		REQUIRE_FALSE(sv == static_cast<TestType>(0));
	}
	SECTION("operator!=") {
		static_val<TestType> sv(static_cast<TestType>(42));
		REQUIRE(sv != static_cast<TestType>(0));
		REQUIRE_FALSE(sv != static_cast<TestType>(42));
	}
	SECTION("operator<") {
		static_val<TestType> sv(static_cast<TestType>(42));
		REQUIRE(sv < static_cast<TestType>(100));
		REQUIRE_FALSE(sv < static_cast<TestType>(42));
		REQUIRE_FALSE(sv < static_cast<TestType>(1));
	}
	SECTION("operator<=") {
		static_val<TestType> sv(static_cast<TestType>(42));
		REQUIRE(sv <= static_cast<TestType>(100));
		REQUIRE(sv <= static_cast<TestType>(42));
		REQUIRE_FALSE(sv <= static_cast<TestType>(1));
	}
	SECTION("operator>") {
		static_val<TestType> sv(static_cast<TestType>(42));
		REQUIRE(sv > static_cast<TestType>(1));
		REQUIRE_FALSE(sv > static_cast<TestType>(42));
		REQUIRE_FALSE(sv > static_cast<TestType>(100));
	}
	SECTION("operator>=") {
		static_val<TestType> sv(static_cast<TestType>(42));
		REQUIRE(sv >= static_cast<TestType>(1));
		REQUIRE(sv >= static_cast<TestType>(42));
		REQUIRE_FALSE(sv >= static_cast<TestType>(100));
	}
	SECTION("comparison returns bool") {
		static_val<TestType> sv(static_cast<TestType>(42));
		auto result = (sv < static_cast<TestType>(100));
		STATIC_REQUIRE(std::is_same_v<decltype(result), bool>);
	}
}

TEMPLATE_TEST_CASE("Static Val Arithmetic Test", "[static_val][template]", int8_t, int16_t, int32_t, int64_t, uint8_t,
                   uint16_t, uint32_t, uint64_t) {
	SECTION("operator+ with raw value") {
		static_val<TestType> sv(static_cast<TestType>(10));
		auto result = sv + static_cast<TestType>(5);
		REQUIRE(static_cast<TestType>(result) == static_cast<TestType>(15));
	}
	SECTION("operator+ with static_val") {
		static_val<TestType> sv1(static_cast<TestType>(10));
		static_val<TestType> sv2(static_cast<TestType>(5));
		auto result = sv1 + static_cast<TestType>(sv2);
		REQUIRE(static_cast<TestType>(result) == static_cast<TestType>(15));
	}
	SECTION("operator+ does not modify operands") {
		static_val<TestType> sv(static_cast<TestType>(10));
		auto result = sv + static_cast<TestType>(5);
		(void) result;
		REQUIRE(static_cast<TestType>(sv) == static_cast<TestType>(10));
	}
	SECTION("operator+ with zero") {
		static_val<TestType> sv(static_cast<TestType>(42));
		auto result = sv + static_cast<TestType>(0);
		REQUIRE(static_cast<TestType>(result) == static_cast<TestType>(42));
	}
	SECTION("chained operator+") {
		static_val<TestType> sv(static_cast<TestType>(1));
		auto result = sv + static_cast<TestType>(2) + static_cast<TestType>(3);
		REQUIRE(static_cast<TestType>(result) == static_cast<TestType>(6));
	}
}

TEMPLATE_TEST_CASE("Static Val Increment/Decrement Test", "[static_val][template]", int8_t, int16_t, int32_t, int64_t,
                   uint8_t, uint16_t, uint32_t, uint64_t) {
	SECTION("pre-increment") {
		static_val<TestType> sv(static_cast<TestType>(5));
		auto& ref = ++sv;
		REQUIRE(static_cast<TestType>(sv) == static_cast<TestType>(6));
		// pre-increment returns reference to self
		REQUIRE(&ref == &sv);
	}
	SECTION("post-increment") {
		static_val<TestType> sv(static_cast<TestType>(5));
		auto old = sv++;
		REQUIRE(static_cast<TestType>(sv) == static_cast<TestType>(6));
		REQUIRE(static_cast<TestType>(old) == static_cast<TestType>(5));
	}
	SECTION("pre-decrement") {
		static_val<TestType> sv(static_cast<TestType>(5));
		auto& ref = --sv;
		REQUIRE(static_cast<TestType>(sv) == static_cast<TestType>(4));
		REQUIRE(&ref == &sv);
	}
	SECTION("post-decrement") {
		static_val<TestType> sv(static_cast<TestType>(5));
		auto old = sv--;
		REQUIRE(static_cast<TestType>(sv) == static_cast<TestType>(4));
		REQUIRE(static_cast<TestType>(old) == static_cast<TestType>(5));
	}
	SECTION("multiple increments") {
		static_val<TestType> sv(static_cast<TestType>(0));
		++sv;
		++sv;
		++sv;
		REQUIRE(static_cast<TestType>(sv) == static_cast<TestType>(3));
	}
	SECTION("multiple decrements") {
		static_val<TestType> sv(static_cast<TestType>(10));
		--sv;
		--sv;
		--sv;
		REQUIRE(static_cast<TestType>(sv) == static_cast<TestType>(7));
	}
	SECTION("increment then decrement returns to original") {
		static_val<TestType> sv(static_cast<TestType>(42));
		++sv;
		--sv;
		REQUIRE(static_cast<TestType>(sv) == static_cast<TestType>(42));
	}
}

TEMPLATE_TEST_CASE("Static Val Loop Pattern Test", "[static_val][template]", int8_t, int16_t, int32_t, int64_t, uint8_t,
                   uint16_t, uint32_t, uint64_t) {
	SECTION("for loop with addition assignment") {
		// Mimics: for (static_val<T> i = 0; i < 5; i = i + 1)
		int count = 0;
		for (static_val<TestType> i = 0; i < static_cast<TestType>(5); i = i + static_cast<TestType>(1)) {
			count++;
		}
		REQUIRE(count == 5);
	}
	SECTION("for loop with pre-increment") {
		int count = 0;
		for (static_val<TestType> i = 0; i < static_cast<TestType>(5); ++i) {
			count++;
		}
		REQUIRE(count == 5);
	}
	SECTION("for loop with post-increment") {
		int count = 0;
		for (static_val<TestType> i = 0; i < static_cast<TestType>(5); i++) {
			count++;
		}
		REQUIRE(count == 5);
	}
	SECTION("while loop with decrement") {
		static_val<TestType> count(static_cast<TestType>(10));
		TestType sum = 0;
		while (count > static_cast<TestType>(0)) {
			sum += static_cast<TestType>(count);
			count--;
		}
		// sum = 10 + 9 + ... + 1 = 55
		REQUIRE(sum == static_cast<TestType>(55));
	}
	SECTION("accumulation loop") {
		TestType result = 0;
		for (static_val<TestType> i = 1; i <= static_cast<TestType>(10); ++i) {
			result += static_cast<TestType>(i);
		}
		REQUIRE(result == static_cast<TestType>(55));
	}
	SECTION("zero-trip loop") {
		int count = 0;
		for (static_val<TestType> i = 5; i < static_cast<TestType>(5); ++i) {
			count++;
		}
		REQUIRE(count == 0);
	}
	SECTION("single iteration loop") {
		int count = 0;
		for (static_val<TestType> i = 0; i < static_cast<TestType>(1); ++i) {
			count++;
		}
		REQUIRE(count == 1);
	}
}

TEMPLATE_TEST_CASE("Static Val Edge Cases Test", "[static_val][template]", int8_t, int16_t, int32_t, int64_t, uint8_t,
                   uint16_t, uint32_t, uint64_t) {
	SECTION("max value") {
		auto max_val = std::numeric_limits<TestType>::max();
		static_val<TestType> sv(max_val);
		REQUIRE(static_cast<TestType>(sv) == max_val);
	}
	SECTION("min value") {
		auto min_val = std::numeric_limits<TestType>::min();
		static_val<TestType> sv(min_val);
		REQUIRE(static_cast<TestType>(sv) == min_val);
	}
	SECTION("reassignment overwrites") {
		static_val<TestType> sv(static_cast<TestType>(10));
		sv = static_cast<TestType>(20);
		sv = static_cast<TestType>(30);
		REQUIRE(static_cast<TestType>(sv) == static_cast<TestType>(30));
	}
	SECTION("multiple static_val in same scope") {
		static_val<TestType> a(static_cast<TestType>(1));
		static_val<TestType> b(static_cast<TestType>(2));
		static_val<TestType> c(static_cast<TestType>(3));
		REQUIRE(static_cast<TestType>(a) == static_cast<TestType>(1));
		REQUIRE(static_cast<TestType>(b) == static_cast<TestType>(2));
		REQUIRE(static_cast<TestType>(c) == static_cast<TestType>(3));
	}
	SECTION("nested scopes") {
		static_val<TestType> outer(static_cast<TestType>(10));
		{
			static_val<TestType> inner(static_cast<TestType>(20));
			REQUIRE(static_cast<TestType>(inner) == static_cast<TestType>(20));
		}
		// inner is destroyed, outer still valid
		REQUIRE(static_cast<TestType>(outer) == static_cast<TestType>(10));
	}
}

TEST_CASE("Static Val Type Constraint Test", "[static_val]") {
	SECTION("integral types are accepted") {
		STATIC_REQUIRE(std::is_constructible_v<static_val<int8_t>, int8_t>);
		STATIC_REQUIRE(std::is_constructible_v<static_val<int16_t>, int16_t>);
		STATIC_REQUIRE(std::is_constructible_v<static_val<int32_t>, int32_t>);
		STATIC_REQUIRE(std::is_constructible_v<static_val<int64_t>, int64_t>);
		STATIC_REQUIRE(std::is_constructible_v<static_val<uint8_t>, uint8_t>);
		STATIC_REQUIRE(std::is_constructible_v<static_val<uint16_t>, uint16_t>);
		STATIC_REQUIRE(std::is_constructible_v<static_val<uint32_t>, uint32_t>);
		STATIC_REQUIRE(std::is_constructible_v<static_val<uint64_t>, uint64_t>);
	}
	SECTION("raw_type alias is correct") {
		STATIC_REQUIRE(std::is_same_v<static_val<int32_t>::raw_type, int32_t>);
		STATIC_REQUIRE(std::is_same_v<static_val<uint64_t>::raw_type, uint64_t>);
	}
}

TEST_CASE("Static Val Operator Return Types Test", "[static_val]") {
	SECTION("operator+ returns raw type") {
		static_val<int32_t> sv(10);
		auto result = sv + 5;
		STATIC_REQUIRE(std::is_same_v<decltype(result), int32_t>);
	}
	SECTION("comparison operators return bool") {
		static_val<int32_t> sv(10);
		STATIC_REQUIRE(std::is_same_v<decltype(sv < 5), bool>);
		STATIC_REQUIRE(std::is_same_v<decltype(sv <= 5), bool>);
		STATIC_REQUIRE(std::is_same_v<decltype(sv > 5), bool>);
		STATIC_REQUIRE(std::is_same_v<decltype(sv >= 5), bool>);
		STATIC_REQUIRE(std::is_same_v<decltype(sv == 5), bool>);
		STATIC_REQUIRE(std::is_same_v<decltype(sv != 5), bool>);
	}
	SECTION("implicit conversion to raw type") {
		static_val<int32_t> sv(42);
		STATIC_REQUIRE(std::is_convertible_v<static_val<int32_t>, int32_t>);
		int32_t raw = sv;
		REQUIRE(raw == 42);
	}
}

} // namespace nautilus
