
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <cmath>
#include <iostream>
#include <limits>

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
			auto x =
			    details::RawValueResolver<typename std::remove_cvref_t<decltype((res))>::raw_type>::getRawValue(res);
			REQUIRE(x == Catch::Approx(0.3));
		}
		SECTION("*") {
			auto f1 = val<TestType>(static_cast<TestType>(5.5));
			auto f2 = val<TestType>(static_cast<TestType>(5.5));
			auto res = f1 * f2;
			auto x =
			    details::RawValueResolver<typename std::remove_cvref_t<decltype((res))>::raw_type>::getRawValue(res);
			REQUIRE(x == Catch::Approx(30.25));
		}
		SECTION("/") {
			auto f1 = val<TestType>(static_cast<TestType>(21.1));
			auto f2 = val<TestType>(static_cast<TestType>(7));
			auto res = f1 / f2;
			auto x =
			    details::RawValueResolver<typename std::remove_cvref_t<decltype((res))>::raw_type>::getRawValue(res);
			REQUIRE(x == Catch::Approx(3.0142857143));
		}
	}
	SECTION("special float values") {
		SECTION("positive infinity") {
			auto inf = val<TestType>(std::numeric_limits<TestType>::infinity());
			auto raw =
			    details::RawValueResolver<typename std::remove_cvref_t<decltype(inf)>::raw_type>::getRawValue(inf);
			REQUIRE(std::isinf(raw));
			REQUIRE(raw > static_cast<TestType>(0));
		}
		SECTION("negative infinity") {
			auto neg_inf = val<TestType>(-std::numeric_limits<TestType>::infinity());
			auto raw =
			    details::RawValueResolver<typename std::remove_cvref_t<decltype(neg_inf)>::raw_type>::getRawValue(
			        neg_inf);
			REQUIRE(std::isinf(raw));
			REQUIRE(raw < static_cast<TestType>(0));
		}
		SECTION("NaN identity") {
			auto nan_val = val<TestType>(std::numeric_limits<TestType>::quiet_NaN());
			auto raw =
			    details::RawValueResolver<typename std::remove_cvref_t<decltype(nan_val)>::raw_type>::getRawValue(
			        nan_val);
			REQUIRE(std::isnan(raw));
		}
		SECTION("NaN comparison is always false") {
			// NaN != NaN by IEEE 754
			auto nan_val = val<TestType>(std::numeric_limits<TestType>::quiet_NaN());
			bool eq = nan_val == nan_val;
			REQUIRE(eq == false);
		}
		SECTION("NaN != comparison is always true") {
			auto nan_val = val<TestType>(std::numeric_limits<TestType>::quiet_NaN());
			bool ne = nan_val != nan_val;
			REQUIRE(ne == true);
		}
		SECTION("negative zero") {
			auto neg_zero = val<TestType>(static_cast<TestType>(-0.0));
			auto pos_zero = val<TestType>(static_cast<TestType>(0.0));
			// IEEE 754: -0.0 == 0.0
			bool eq = neg_zero == pos_zero;
			REQUIRE(eq == true);
		}
		SECTION("infinity arithmetic") {
			auto inf = val<TestType>(std::numeric_limits<TestType>::infinity());
			auto one = val<TestType>(static_cast<TestType>(1.0));
			auto res = inf + one;
			auto raw =
			    details::RawValueResolver<typename std::remove_cvref_t<decltype(res)>::raw_type>::getRawValue(res);
			REQUIRE(std::isinf(raw));
		}
		SECTION("max finite value") {
			auto max_val = val<TestType>(std::numeric_limits<TestType>::max());
			auto raw =
			    details::RawValueResolver<typename std::remove_cvref_t<decltype(max_val)>::raw_type>::getRawValue(
			        max_val);
			REQUIRE(std::isfinite(raw));
			REQUIRE(raw == std::numeric_limits<TestType>::max());
		}
		SECTION("smallest positive normal value") {
			auto min_val = val<TestType>(std::numeric_limits<TestType>::min());
			auto raw =
			    details::RawValueResolver<typename std::remove_cvref_t<decltype(min_val)>::raw_type>::getRawValue(
			        min_val);
			REQUIRE(raw > static_cast<TestType>(0));
			REQUIRE(std::isnormal(raw));
		}
		SECTION("denormalized (subnormal) value") {
			auto denorm = val<TestType>(std::numeric_limits<TestType>::denorm_min());
			auto raw = details::RawValueResolver<typename std::remove_cvref_t<decltype(denorm)>::raw_type>::getRawValue(
			    denorm);
			REQUIRE(raw > static_cast<TestType>(0));
			REQUIRE(!std::isnormal(raw));
		}
	}
}
} // namespace nautilus
