
#include "nautilus/val.hpp"
#include "nautilus/val_concepts.hpp"
#include <catch2/catch_all.hpp>
#include <iostream>

namespace nautilus {

TEMPLATE_TEST_CASE("Val Concept Test", "[value][template]", int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, size_t, float, double) {
	SECTION("convertible_to_fundamental") {
		REQUIRE(convertible_to_fundamental<TestType>);
		REQUIRE(convertible_to_fundamental<const TestType>);
	}

	SECTION("is_arithmetic") {
		REQUIRE(is_arithmetic<TestType>);
		REQUIRE(is_arithmetic<const TestType>);
	}
	SECTION("is_ptr") {
		REQUIRE(!is_ptr<TestType>);
		REQUIRE(!is_ptr<const TestType>);
		REQUIRE(is_ptr<TestType*>);
		REQUIRE(is_ptr<const TestType*>);
	}
	SECTION("is_arithmetic_ptr") {
		REQUIRE(!is_arithmetic_ptr<TestType>);
		REQUIRE(!is_arithmetic_ptr<const TestType>);
		REQUIRE(is_arithmetic_ptr<TestType*>);
		REQUIRE(is_arithmetic_ptr<const TestType*>);
	}
	SECTION("is_void_ptr") {
		REQUIRE(!is_void_ptr<TestType>);
		REQUIRE(!is_void_ptr<const TestType>);
		REQUIRE(!is_void_ptr<TestType*>);
		REQUIRE(!is_void_ptr<const TestType*>);
	}
	SECTION("is_enum") {
		REQUIRE(!is_enum<TestType>);
		REQUIRE(!is_enum<const TestType>);
		REQUIRE(!is_enum<TestType*>);
		REQUIRE(!is_enum<const TestType*>);
	}
	SECTION("is_val") {
		REQUIRE(is_val<TestType>);
		REQUIRE(is_val<const TestType>);
	}
	SECTION("is_val_type") {
		REQUIRE(!is_val_type<TestType>);
		REQUIRE(is_val_type<val<TestType>>);
	}
}
} // namespace nautilus
