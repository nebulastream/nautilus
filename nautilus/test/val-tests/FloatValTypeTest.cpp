
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <iostream>

namespace nautilus {

TEMPLATE_TEST_CASE("Float Val Operation Test", "[value][template]", float, double) {
	SECTION("comparison operators") {
		SECTION("==") {
			auto f1 = val<TestType>(static_cast<TestType>(42.1));
			bool x = f1 == static_cast<TestType>(42.1);
			REQUIRE(x);
		}
		SECTION("!=") {
			auto f1 = val<TestType>(static_cast<TestType>(42.1));
			REQUIRE(f1 != static_cast<TestType>(1));
		}
		SECTION(">") {
			auto f1 = val<TestType>(static_cast<TestType>(42.1));
			REQUIRE(f1 > static_cast<TestType>(1));
		}
		SECTION("<") {
			auto f1 = val<TestType>(static_cast<TestType>(42.1));
			REQUIRE(f1 < static_cast<TestType>(100));
		}
		SECTION(">=") {
			auto f1 = val<TestType>(static_cast<TestType>(42.1));
			REQUIRE(f1 >= static_cast<TestType>(42.1));
			REQUIRE(f1 >= static_cast<TestType>(1));
		}
		SECTION("<=") {
			auto f1 = val<TestType>(static_cast<TestType>(42.1));
			REQUIRE(f1 <= static_cast<TestType>(100));
			REQUIRE(f1 <= static_cast<TestType>(42.2));
		}
	}
	SECTION("arithmetic operators") {
		SECTION("+") {
			auto f1 = val<TestType>(static_cast<TestType>(42.1));
			auto f2 = val<TestType>(static_cast<TestType>(42.1));
			auto res = f1 + f2;
			REQUIRE(res == static_cast<TestType>(84.2));
		}
		SECTION("-") {
			auto f1 = val<TestType>(static_cast<TestType>(42.5));
			auto f2 = val<TestType>(static_cast<TestType>(42.2));
			auto res = f1 - f2;
			auto x = details::RawValueResolver<typename std::remove_cvref_t<decltype((res))>::raw_type>::getRawValue(res);
			REQUIRE(x == Catch::Approx(0.3));
		}
		SECTION("*") {
			auto f1 = val<TestType>(static_cast<TestType>(5.5));
			auto f2 = val<TestType>(static_cast<TestType>(5.5));
			auto res = f1 * f2;
			auto x = details::RawValueResolver<typename std::remove_cvref_t<decltype((res))>::raw_type>::getRawValue(res);
			REQUIRE(x == Catch::Approx(30.25));
		}
		SECTION("/") {
			auto f1 = val<TestType>(static_cast<TestType>(21.1));
			auto f2 = val<TestType>(static_cast<TestType>(7));
			auto res = f1 / f2;
			auto x = details::RawValueResolver<typename std::remove_cvref_t<decltype((res))>::raw_type>::getRawValue(res);
			REQUIRE(x == Catch::Approx(3.0142857143));
		}
	}
}
} // namespace nautilus
