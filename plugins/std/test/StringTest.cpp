
#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/std/string.h"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

void stringAppendCopy(val<char*> output) {
	val<std::string> str;
	str = str.append("test");
	str.copy(output, str.size());
}

void stringAppendOpCopy(val<char*> output) {
	val<std::string> str;
	str += output;
	str.copy(output, str.size());
}

val<std::string*> stringFromPtrReturnPtr(val<const char*> input) {
	// creates a new string object on the input
	val<std::string> str = val<std::string>(input);
	// releases the underling pointer -> now it is free and needs to be managed by the callee.
	return str.release();
}

val<std::string*> stringReturnPtr() {
	// creates a new string object on the input
	val<std::string> str = "input";
	// releases the underling pointer -> now it is free and needs to be managed by the callee.
	return str.release();
}

void runStringTest(engine::NautilusEngine& engine) {
	SECTION("stringAppendCopy") {
		auto array = std::array<char, 42>();
		auto f = engine.registerFunction(stringAppendCopy);
		f(array.data());
		auto s = std::string(array.data());
		REQUIRE(s == "test");
	}
	SECTION("stringAppendCopy") {
		auto array = std::array<char, 42>();
		auto f = engine.registerFunction(stringAppendCopy);
		f(array.data());
		auto s = std::string(array.data());
		REQUIRE(s == "test");
	}
	SECTION("stringReturnPtr") {
		auto f = engine.registerFunction(stringFromPtrReturnPtr);
		std::string source = "HelloWorld";
		auto str = f(source.c_str());
		REQUIRE(*str == source);
		delete str;
	}
	SECTION("stringReturnPtr") {
		auto f = engine.registerFunction(stringReturnPtr);
		std::string source = "input";
		auto str = f();
		REQUIRE(*str == source);
		delete str;
	}
}

TEST_CASE("StringTest - Interpreter") {
	auto engine = nautilus::testing::makeEngine("interpreter");
	runStringTest(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("StringTest - Compiler") {
	nautilus::testing::forEachBackendWithTraceMode([](engine::NautilusEngine& engine) { runStringTest(engine); });
}
#endif
} // namespace nautilus::engine
