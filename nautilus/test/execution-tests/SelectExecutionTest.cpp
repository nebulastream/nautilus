#include "SelectOperations.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/select.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

void selectTest(engine::NautilusEngine& engine) {

	SECTION("selectInt32True") {
		auto f = engine.registerFunction(selectInt32True);
		REQUIRE(f(true, 42, 100) == 42);
		REQUIRE(f(false, 42, 100) == 100);
	}

	SECTION("selectInt32False") {
		auto f = engine.registerFunction(selectInt32True);
		REQUIRE(f(false, 42, 100) == 100);
	}

	SECTION("selectInt64") {
		auto f = engine.registerFunction(selectInt64True);
		REQUIRE(f(true, 1234567890123LL, 9876543210987LL) == 1234567890123LL);
		REQUIRE(f(false, 1234567890123LL, 9876543210987LL) == 9876543210987LL);
	}

	SECTION("selectUInt32") {
		auto f = engine.registerFunction(selectUInt32);
		REQUIRE(f(true, 42u, 100u) == 42u);
		REQUIRE(f(false, 42u, 100u) == 100u);
	}

	SECTION("selectUInt64") {
		auto f = engine.registerFunction(selectUInt64);
		REQUIRE(f(true, 1234567890123ULL, 9876543210987ULL) == 1234567890123ULL);
		REQUIRE(f(false, 1234567890123ULL, 9876543210987ULL) == 9876543210987ULL);
	}

	SECTION("selectFloat") {
		auto f = engine.registerFunction(selectFloat);
		REQUIRE(f(true, 3.14f, 2.71f) == 3.14f);
		REQUIRE(f(false, 3.14f, 2.71f) == 2.71f);
	}

	SECTION("selectDouble") {
		auto f = engine.registerFunction(selectDouble);
		REQUIRE(f(true, 3.14159, 2.71828) == 3.14159);
		REQUIRE(f(false, 3.14159, 2.71828) == 2.71828);
	}

	SECTION("selectBool") {
		auto f = engine.registerFunction(selectBool);
		REQUIRE(f(true, true, false) == true);
		REQUIRE(f(false, true, false) == false);
		REQUIRE(f(true, false, true) == false);
		REQUIRE(f(false, false, true) == true);
	}

	SECTION("selectInLoop") {
		auto f = engine.registerFunction(selectInLoop);
		// For count=4: i=0 (even->10), i=1 (odd->20), i=2 (even->10), i=3 (odd->20)
		// sum = 10 + 20 + 10 + 20 = 60
		REQUIRE(f(4) == 60);
		// For count=5: sum = 10 + 20 + 10 + 20 + 10 = 70
		REQUIRE(f(5) == 70);
		// For count=0: sum = 0
		REQUIRE(f(0) == 0);
	}

	SECTION("selectBasedOnComparison") {
		auto f = engine.registerFunction(selectBasedOnComparison);
		REQUIRE(f(10, 5) == 10); // max(10, 5) = 10
		REQUIRE(f(3, 7) == 7);   // max(3, 7) = 7
		REQUIRE(f(5, 5) == 5);   // max(5, 5) = 5
	}

	SECTION("nestedSelect") {
		auto f = engine.registerFunction(nestedSelect);
		REQUIRE(f(15) == 200); // x > 10 -> 200
		REQUIRE(f(8) == 100);  // x <= 10 && x > 5 -> 100
		REQUIRE(f(3) == 50);   // x <= 5 -> 50
		REQUIRE(f(11) == 200); // x > 10 -> 200
		REQUIRE(f(5) == 50);   // x <= 5 -> 50
	}

	SECTION("selectInt8") {
		auto f = engine.registerFunction(selectInt8);
		REQUIRE(f(true, static_cast<int8_t>(42), static_cast<int8_t>(100)) == static_cast<int8_t>(42));
		REQUIRE(f(false, static_cast<int8_t>(42), static_cast<int8_t>(100)) == static_cast<int8_t>(100));
	}

	SECTION("selectInt16") {
		auto f = engine.registerFunction(selectInt16);
		REQUIRE(f(true, static_cast<int16_t>(1000), static_cast<int16_t>(2000)) == static_cast<int16_t>(1000));
		REQUIRE(f(false, static_cast<int16_t>(1000), static_cast<int16_t>(2000)) == static_cast<int16_t>(2000));
	}

	SECTION("selectPointer") {
		auto f = engine.registerFunction(selectPointer);
		int32_t a = 42;
		int32_t b = 100;
		int32_t* result_true = f(true, &a, &b);
		int32_t* result_false = f(false, &a, &b);
		REQUIRE(result_true == &a);
		REQUIRE(result_false == &b);
		REQUIRE(*result_true == 42);
		REQUIRE(*result_false == 100);
	}

	SECTION("selectPointerAndDeref") {
		auto f = engine.registerFunction(selectPointerAndDeref);
		int32_t a = 42;
		int32_t b = 100;
		REQUIRE(f(true, &a, &b) == 42);
		REQUIRE(f(false, &a, &b) == 100);
	}
}

TEST_CASE("Select Interpreter Test") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	selectTest(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("Select Compiler Test") {
	std::vector<std::string> backends = {};
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	backends.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	backends.emplace_back("asmjit");
#endif
	for (auto& backend : backends) {
		DYNAMIC_SECTION(backend) {
			engine::Options options;
			options.setOption("engine.backend", backend);
			auto engine = engine::NautilusEngine(options);
			selectTest(engine);
		}
	}
}
#endif
} // namespace nautilus::engine
