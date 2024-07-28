
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <iostream>

namespace nautilus {

TEMPLATE_TEST_CASE("Integer Val Operation Test", "[value][template]", int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t) {
	SECTION("comparison operators") {
		SECTION("==") {
			auto f1 = val<TestType>(static_cast<TestType>(42));
			val<TestType> f2 = f1;
			REQUIRE(f2 == static_cast<TestType>(42));
		}
		SECTION("!=") {
			auto f1 = val<TestType>(static_cast<TestType>(42));
			REQUIRE(f1 != static_cast<TestType>(1));
		}
		SECTION(">") {
			auto f1 = val<TestType>(static_cast<TestType>(42));
			REQUIRE(f1 > static_cast<TestType>(1));
		}
		SECTION("<") {
			auto f1 = val<TestType>(static_cast<TestType>(42));
			REQUIRE(f1 < static_cast<TestType>(100));
		}
		SECTION(">=") {
			auto f1 = val<TestType>(static_cast<TestType>(42));
			REQUIRE(f1 >= static_cast<TestType>(42));
			REQUIRE(f1 >= static_cast<TestType>(1));
		}
		SECTION("<=") {
			auto f1 = val<TestType>(static_cast<TestType>(42));
			REQUIRE(f1 <= static_cast<TestType>(100));
			REQUIRE(f1 <= static_cast<TestType>(42));
		}
	}
	SECTION("bitwise operators") {
		SECTION("&") {
			auto f1 = val<TestType>(static_cast<TestType>(42));
			auto f2 = val<TestType>(static_cast<TestType>(42));
			auto res = f1 & f2;
			REQUIRE(res == static_cast<TestType>(42));
		}
		SECTION("|") {
			auto f1 = val<TestType>(static_cast<TestType>(42));
			auto f2 = val<TestType>(static_cast<TestType>(42));
			auto res = f1 | f2;
			REQUIRE(res == static_cast<TestType>(42));
		}
		SECTION("^") {
			auto f1 = val<TestType>(static_cast<TestType>(42));
			auto f2 = val<TestType>(static_cast<TestType>(42));
			auto res = f1 ^ f2;
			REQUIRE(res == static_cast<TestType>(0));
		}
		SECTION("~") {
			// auto f1 = val<TestType>(static_cast<TestType>(42));
			// auto res = ~f1;
			// REQUIRE(res == static_cast<TestType>(84));
		}
		SECTION("<<") {
			auto f1 = val<TestType>(static_cast<TestType>(42));
			auto f2 = val<TestType>(static_cast<TestType>(2));
			auto res = f1 << f2;
			auto ref = static_cast<TestType>(42) << static_cast<TestType>(2);
			bool t = res == ref;
			REQUIRE(t);
		}
		SECTION(">>") {
			auto f1 = val<TestType>(static_cast<TestType>(42));
			auto f2 = val<TestType>(static_cast<TestType>(2));
			auto res = f1 >> f2;
			auto ref = static_cast<TestType>(42) >> static_cast<TestType>(2);
			REQUIRE(res == ref);
		}
	}
	SECTION("arithmetic operators") {
		SECTION("+") {
			auto f1 = val<TestType>(static_cast<TestType>(42));
			//  auto f2 = val<TestType>(static_cast<TestType>(42));
			auto res = f1 + static_cast<TestType>(42);
			REQUIRE(res == static_cast<TestType>(84));
		}
		SECTION("-") {
			auto f1 = val<TestType>(static_cast<TestType>(42));
			auto f2 = val<TestType>(static_cast<TestType>(42));
			auto res = f1 - f2;
			REQUIRE(res == static_cast<TestType>(0));
		}
		SECTION("*") {
			auto f1 = val<TestType>(static_cast<TestType>(5));
			auto f2 = val<TestType>(static_cast<TestType>(5));
			auto res = f1 * f2;
			REQUIRE(res == static_cast<TestType>(25));
		}
		SECTION("/") {
			auto f1 = val<TestType>(static_cast<TestType>(21));
			auto f2 = val<TestType>(static_cast<TestType>(7));
			auto res = f1 / f2;
			REQUIRE(res == static_cast<TestType>(3));
		}
		SECTION("%") {
			SKIP("Not Implemented");
			auto f1 = val<TestType>(static_cast<TestType>(13));
			auto f2 = val<TestType>(static_cast<TestType>(5));
			auto res = f1 % f2;
			REQUIRE(res == static_cast<TestType>(3));
		}
		/*SECTION("++") {

		    auto f1 = val<TestType>(static_cast<TestType>(3));
		    auto res = f1++;
		    REQUIRE(res == static_cast<TestType>(3));
		    REQUIRE(f1 == static_cast<TestType>(4));
		    auto res2 = ++f1;
		    REQUIRE(res2 == static_cast<TestType>(5));
		    REQUIRE(f1 == static_cast<TestType>(5));
		}*/
		SECTION("--") {
			auto f1 = val<TestType>(static_cast<TestType>(3));
			auto res = f1--;
			REQUIRE(res == static_cast<TestType>(3));
			REQUIRE(f1 == static_cast<TestType>(2));
			auto res2 = --f1;
			REQUIRE(res2 == static_cast<TestType>(1));
			REQUIRE(f1 == static_cast<TestType>(1));
		}
		SECTION("+=") {
			auto f1 = val<TestType>(static_cast<TestType>(3));
			f1 += static_cast<TestType>(3);
			REQUIRE(f1 == static_cast<TestType>(6));
		}
	}
}
} // namespace nautilus
