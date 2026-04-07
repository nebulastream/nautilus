#include "ExecutionTest.hpp"
#include "common/MemoryIntrinsicFunctions.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <cstring>

namespace nautilus::engine {

void memoryIntrinsicTest(engine::NautilusEngine& engine) {

	SECTION("memcpy basic copy") {
		auto f = engine.registerFunction(memcpyFunction);
		char src[16] = "Hello, World!";
		char dest[16] = {};
		f((void*) dest, (const void*) src, (size_t) 14);
		REQUIRE(std::strcmp(dest, "Hello, World!") == 0);
	}

	SECTION("memcpy single byte") {
		auto f = engine.registerFunction(memcpyFunction);
		char src[4] = {0x42, 0, 0, 0};
		char dest[4] = {};
		f((void*) dest, (const void*) src, (size_t) 1);
		REQUIRE(dest[0] == 0x42);
	}

	SECTION("memcpy integer array") {
		auto f = engine.registerFunction(memcpyFunction);
		int32_t src[4] = {1, 2, 3, 4};
		int32_t dest[4] = {};
		f((void*) dest, (const void*) src, (size_t) (4 * sizeof(int32_t)));
		REQUIRE(dest[0] == 1);
		REQUIRE(dest[1] == 2);
		REQUIRE(dest[2] == 3);
		REQUIRE(dest[3] == 4);
	}

	SECTION("memset zero") {
		auto f = engine.registerFunction(memsetFunction);
		char buf[17] = "AAAAAAAAAAAAAAAA";
		f((void*) buf, (int) 0, (size_t) 17);
		for (int i = 0; i < 17; i++) {
			REQUIRE(buf[i] == 0);
		}
	}

	SECTION("memset fill byte") {
		auto f = engine.registerFunction(memsetFunction);
		char buf[8] = {};
		f((void*) buf, (int) 0xAB, (size_t) 8);
		for (int i = 0; i < 8; i++) {
			REQUIRE((unsigned char) buf[i] == 0xAB);
		}
	}

	SECTION("memset partial") {
		auto f = engine.registerFunction(memsetFunction);
		char buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
		f((void*) buf, (int) 0xFF, (size_t) 4);
		for (int i = 0; i < 4; i++) {
			REQUIRE((unsigned char) buf[i] == 0xFF);
		}
		for (int i = 4; i < 8; i++) {
			REQUIRE(buf[i] == 0);
		}
	}

	SECTION("memmove non-overlapping") {
		auto f = engine.registerFunction(memmoveFunction);
		char src[16] = "Hello, World!";
		char dest[16] = {};
		f((void*) dest, (const void*) src, (size_t) 14);
		REQUIRE(std::strcmp(dest, "Hello, World!") == 0);
	}

	SECTION("memmove overlapping forward") {
		auto f = engine.registerFunction(memmoveFunction);
		// Overlapping: move [0..4] -> [2..6]
		char buf[16] = {1, 2, 3, 4, 5, 0, 0, 0};
		f((void*) (buf + 2), (const void*) buf, (size_t) 4);
		REQUIRE(buf[2] == 1);
		REQUIRE(buf[3] == 2);
		REQUIRE(buf[4] == 3);
		REQUIRE(buf[5] == 4);
	}

	SECTION("memmove overlapping backward") {
		auto f = engine.registerFunction(memmoveFunction);
		// Overlapping: move [2..6] -> [0..4]
		char buf[16] = {0, 0, 1, 2, 3, 4, 0, 0};
		f((void*) buf, (const void*) (buf + 2), (size_t) 4);
		REQUIRE(buf[0] == 1);
		REQUIRE(buf[1] == 2);
		REQUIRE(buf[2] == 3);
		REQUIRE(buf[3] == 4);
	}

	SECTION("memset zero helper") {
		auto f = engine.registerFunction(memsetZeroFunction);
		int32_t buf[4] = {1, 2, 3, 4};
		f((void*) buf, (size_t) (4 * sizeof(int32_t)));
		REQUIRE(buf[0] == 0);
		REQUIRE(buf[1] == 0);
		REQUIRE(buf[2] == 0);
		REQUIRE(buf[3] == 0);
	}
}

TEST_CASE("Memory Intrinsic Test") {
	nautilus::testing::forEachBackend([](engine::NautilusEngine& engine) { memoryIntrinsicTest(engine); });
}
} // namespace nautilus::engine
