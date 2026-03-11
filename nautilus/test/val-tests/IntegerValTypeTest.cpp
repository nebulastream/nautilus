
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <iostream>
#include <limits>

namespace nautilus {

TEMPLATE_TEST_CASE("Integer Val Operation Test", "[value][template]", int8_t, int16_t, int32_t, int64_t, uint8_t,
                   uint16_t, uint32_t, uint64_t) {
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
			auto f1 = val<TestType>(static_cast<TestType>(13));
			auto f2 = val<TestType>(static_cast<TestType>(5));
			auto res = f1 % f2;
			REQUIRE(res == static_cast<TestType>(3));
		}
		SECTION("% zero remainder") {
			auto f1 = val<TestType>(static_cast<TestType>(15));
			auto f2 = val<TestType>(static_cast<TestType>(5));
			auto res = f1 % f2;
			REQUIRE(res == static_cast<TestType>(0));
		}
		SECTION("% dividend less than divisor") {
			auto f1 = val<TestType>(static_cast<TestType>(3));
			auto f2 = val<TestType>(static_cast<TestType>(10));
			auto res = f1 % f2;
			REQUIRE(res == static_cast<TestType>(3));
		}
		SECTION("% dividend equals divisor") {
			auto f1 = val<TestType>(static_cast<TestType>(7));
			auto f2 = val<TestType>(static_cast<TestType>(7));
			auto res = f1 % f2;
			REQUIRE(res == static_cast<TestType>(0));
		}
		SECTION("% power of two divisor") {
			auto f1 = val<TestType>(static_cast<TestType>(17));
			auto f2 = val<TestType>(static_cast<TestType>(8));
			auto res = f1 % f2;
			REQUIRE(res == static_cast<TestType>(1));
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
	SECTION("overflow and underflow behavior") {
		SECTION("max value identity") {
			auto max_val = val<TestType>(std::numeric_limits<TestType>::max());
			REQUIRE(max_val == std::numeric_limits<TestType>::max());
		}
		SECTION("min value identity") {
			auto min_val = val<TestType>(std::numeric_limits<TestType>::min());
			REQUIRE(min_val == std::numeric_limits<TestType>::min());
		}
		SECTION("unsigned overflow wraparound") {
			// Guard against integer promotion for small types (uint8/uint16 get promoted to int)
			if constexpr (std::is_unsigned_v<TestType> && sizeof(TestType) >= sizeof(int)) {
				// Unsigned overflow is well-defined: max + 1 wraps to 0
				auto max_val = val<TestType>(std::numeric_limits<TestType>::max());
				auto one = val<TestType>(static_cast<TestType>(1));
				auto res = max_val + one;
				REQUIRE(res == static_cast<TestType>(0));
			}
		}
		SECTION("unsigned underflow wraparound") {
			// Guard against integer promotion for small types (uint8/uint16 get promoted to int)
			if constexpr (std::is_unsigned_v<TestType> && sizeof(TestType) >= sizeof(int)) {
				// Unsigned underflow: 0 - 1 wraps to max
				auto zero = val<TestType>(static_cast<TestType>(0));
				auto one = val<TestType>(static_cast<TestType>(1));
				auto res = zero - one;
				REQUIRE(res == std::numeric_limits<TestType>::max());
			}
		}
		SECTION("unsigned multiply max by two") {
			// Guard against integer promotion for small types (uint8/uint16 get promoted to int)
			if constexpr (std::is_unsigned_v<TestType> && sizeof(TestType) >= sizeof(int)) {
				// (2^N - 1) * 2 mod 2^N = 2^N - 2
				auto two = val<TestType>(static_cast<TestType>(2));
				auto max_val = val<TestType>(std::numeric_limits<TestType>::max());
				auto res = max_val * two;
				REQUIRE(res == static_cast<TestType>(std::numeric_limits<TestType>::max() - 1));
			}
		}
	}
	SECTION("C++ semantic equivalence") {
		SECTION("chained prefix increment ++(++x)") {
			// C++ allows ++(++x), so val<T> should too
			auto f1 = val<TestType>(static_cast<TestType>(3));
			++(++f1);
			REQUIRE(f1 == static_cast<TestType>(5));
		}
		SECTION("chained prefix decrement --(--x)") {
			// C++ allows --(--x), so val<T> should too
			auto f1 = val<TestType>(static_cast<TestType>(5));
			--(--f1);
			REQUIRE(f1 == static_cast<TestType>(3));
		}
		SECTION("unary plus on const value") {
			// C++ allows +x on const values
			const auto f1 = val<TestType>(static_cast<TestType>(42));
			auto res = +f1;
			// Result should be promoted to int for small types
			if constexpr (sizeof(TestType) < sizeof(int)) {
				REQUIRE(res == 42);
			} else {
				REQUIRE(res == static_cast<TestType>(42));
			}
		}
		SECTION("unary minus on const value") {
			// C++ allows -x on const values
			const auto f1 = val<TestType>(static_cast<TestType>(42));
			auto res = -f1;
			// Note: for unsigned types, this wraps around
			TestType expected = static_cast<TestType>(0) - static_cast<TestType>(42);
			REQUIRE(res == expected);
		}
		SECTION("move assignment") {
			// C++ supports move assignment for arithmetic types
			auto f1 = val<TestType>(static_cast<TestType>(42));
			val<TestType> f2;
			f2 = std::move(f1);
			REQUIRE(f2 == static_cast<TestType>(42));
		}
	}
}
} // namespace nautilus
