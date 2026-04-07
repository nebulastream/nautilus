#include "ExecutionTest.hpp"
#include "common/BitIntrinsicFunctions.hpp"
#include "nautilus/Engine.hpp"
#include <bit>
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

void bitIntrinsicTest(engine::NautilusEngine& engine) {

	SECTION("popcount 32-bit") {
		auto f = engine.registerFunction(popcountFunction);
		REQUIRE(f((uint32_t) 0) == (uint32_t) 0);
		REQUIRE(f((uint32_t) 1) == (uint32_t) 1);
		REQUIRE(f((uint32_t) 0xFFFFFFFF) == (uint32_t) 32);
		REQUIRE(f((uint32_t) 0x0F0F0F0F) == (uint32_t) 16);
		REQUIRE(f((uint32_t) 0b10110101) == (uint32_t) 5);
		REQUIRE(f((uint32_t) 0x80000000) == (uint32_t) 1);
	}

	SECTION("popcount 64-bit") {
		auto f = engine.registerFunction(popcount64Function);
		REQUIRE(f((uint64_t) 0) == (uint64_t) 0);
		REQUIRE(f((uint64_t) 1) == (uint64_t) 1);
		REQUIRE(f((uint64_t) 0xFFFFFFFFFFFFFFFF) == (uint64_t) 64);
		REQUIRE(f((uint64_t) 0x0F0F0F0F0F0F0F0F) == (uint64_t) 32);
		REQUIRE(f((uint64_t) 0x8000000000000000) == (uint64_t) 1);
	}

	SECTION("countl_zero 32-bit") {
		auto f = engine.registerFunction(countlZeroFunction);
		REQUIRE(f((uint32_t) 0) == (uint32_t) 32);
		REQUIRE(f((uint32_t) 1) == (uint32_t) 31);
		REQUIRE(f((uint32_t) 0x80000000) == (uint32_t) 0);
		REQUIRE(f((uint32_t) 0x40000000) == (uint32_t) 1);
		REQUIRE(f((uint32_t) 0x0000FFFF) == (uint32_t) 16);
	}

	SECTION("countl_zero 64-bit") {
		auto f = engine.registerFunction(countlZero64Function);
		REQUIRE(f((uint64_t) 0) == (uint64_t) 64);
		REQUIRE(f((uint64_t) 1) == (uint64_t) 63);
		REQUIRE(f((uint64_t) 0x8000000000000000) == (uint64_t) 0);
	}

	SECTION("countr_zero 32-bit") {
		auto f = engine.registerFunction(countrZeroFunction);
		REQUIRE(f((uint32_t) 1) == (uint32_t) 0);
		REQUIRE(f((uint32_t) 2) == (uint32_t) 1);
		REQUIRE(f((uint32_t) 4) == (uint32_t) 2);
		REQUIRE(f((uint32_t) 0x80000000) == (uint32_t) 31);
		REQUIRE(f((uint32_t) 0x00000100) == (uint32_t) 8);
	}

	SECTION("countr_zero 64-bit") {
		auto f = engine.registerFunction(countrZero64Function);
		REQUIRE(f((uint64_t) 1) == (uint64_t) 0);
		REQUIRE(f((uint64_t) 2) == (uint64_t) 1);
		REQUIRE(f((uint64_t) 0x8000000000000000) == (uint64_t) 63);
	}

	SECTION("byteswap 32-bit") {
		auto f = engine.registerFunction(byteswap32Function);
		REQUIRE(f((uint32_t) 0x12345678) == (uint32_t) 0x78563412);
		REQUIRE(f((uint32_t) 0x00000001) == (uint32_t) 0x01000000);
		REQUIRE(f((uint32_t) 0xFF000000) == (uint32_t) 0x000000FF);
		REQUIRE(f((uint32_t) 0xAABBCCDD) == (uint32_t) 0xDDCCBBAA);
		// Double byteswap is identity
		REQUIRE(f(f((uint32_t) 0x12345678)) == (uint32_t) 0x12345678);
	}

	SECTION("byteswap 64-bit") {
		auto f = engine.registerFunction(byteswap64Function);
		REQUIRE(f((uint64_t) 0x0102030405060708) == (uint64_t) 0x0807060504030201);
		REQUIRE(f((uint64_t) 0xFF00000000000000) == (uint64_t) 0x00000000000000FF);
		// Double byteswap is identity
		REQUIRE(f(f((uint64_t) 0x0102030405060708)) == (uint64_t) 0x0102030405060708);
	}

	SECTION("rotl 32-bit") {
		auto f = engine.registerFunction(rotlFunction);
		REQUIRE(f((uint32_t) 0x80000000, (uint32_t) 1) == (uint32_t) 0x00000001);
		REQUIRE(f((uint32_t) 0x00000001, (uint32_t) 1) == (uint32_t) 0x00000002);
		REQUIRE(f((uint32_t) 0x12345678, (uint32_t) 0) == (uint32_t) 0x12345678);
		REQUIRE(f((uint32_t) 0x12345678, (uint32_t) 8) == (uint32_t) 0x34567812);
		REQUIRE(f((uint32_t) 0xFFFFFFFF, (uint32_t) 16) == (uint32_t) 0xFFFFFFFF);
	}

	SECTION("rotr 32-bit") {
		auto f = engine.registerFunction(rotrFunction);
		REQUIRE(f((uint32_t) 0x00000001, (uint32_t) 1) == (uint32_t) 0x80000000);
		REQUIRE(f((uint32_t) 0x80000000, (uint32_t) 1) == (uint32_t) 0x40000000);
		REQUIRE(f((uint32_t) 0x12345678, (uint32_t) 0) == (uint32_t) 0x12345678);
		REQUIRE(f((uint32_t) 0x12345678, (uint32_t) 8) == (uint32_t) 0x78123456);
		REQUIRE(f((uint32_t) 0xFFFFFFFF, (uint32_t) 16) == (uint32_t) 0xFFFFFFFF);
	}

	SECTION("rotl 64-bit") {
		auto f = engine.registerFunction(rotl64Function);
		REQUIRE(f((uint64_t) 0x8000000000000000, (uint64_t) 1) == (uint64_t) 0x0000000000000001);
		REQUIRE(f((uint64_t) 0x0000000000000001, (uint64_t) 1) == (uint64_t) 0x0000000000000002);
		REQUIRE(f((uint64_t) 0x0102030405060708, (uint64_t) 0) == (uint64_t) 0x0102030405060708);
	}

	SECTION("rotr 64-bit") {
		auto f = engine.registerFunction(rotr64Function);
		REQUIRE(f((uint64_t) 0x0000000000000001, (uint64_t) 1) == (uint64_t) 0x8000000000000000);
		REQUIRE(f((uint64_t) 0x8000000000000000, (uint64_t) 1) == (uint64_t) 0x4000000000000000);
		REQUIRE(f((uint64_t) 0x0102030405060708, (uint64_t) 0) == (uint64_t) 0x0102030405060708);
	}

	SECTION("rotl and rotr are inverses") {
		auto rotl_f = engine.registerFunction(rotlFunction);
		auto rotr_f = engine.registerFunction(rotrFunction);
		uint32_t val = 0x12345678;
		uint32_t shift = 13;
		REQUIRE(rotr_f(rotl_f(val, shift), shift) == val);
		REQUIRE(rotl_f(rotr_f(val, shift), shift) == val);
	}
}

TEST_CASE("Bit Intrinsic Test") {
	nautilus::testing::forEachBackend([](engine::NautilusEngine& engine) { bitIntrinsicTest(engine); });
}
} // namespace nautilus::engine
