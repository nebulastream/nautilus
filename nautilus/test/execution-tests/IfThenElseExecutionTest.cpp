#include "IfThenElseOperations.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/ifThenElse.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

void ifThenElseTest(engine::NautilusEngine& engine) {

	SECTION("ifThenElseBasic") {
		auto f = engine.registerFunction(ifThenElseBasic);
		REQUIRE(f(true, 42, 100) == 42);
		REQUIRE(f(false, 42, 100) == 100);
	}

	SECTION("ifThenElseWithComputation") {
		auto f = engine.registerFunction(ifThenElseWithComputation);
		// x > y: return x + 10
		REQUIRE(f(10, 5) == 20); // 10 + 10 = 20
		// x <= y: return y + 20
		REQUIRE(f(5, 10) == 30); // 10 + 20 = 30
		// x == y: false, so return y + 20
		REQUIRE(f(7, 7) == 27); // 7 + 20 = 27
	}

	SECTION("ifThenElseConstants") {
		auto f = engine.registerFunction(ifThenElseConstants);
		REQUIRE(f(true) == 100);
		REQUIRE(f(false) == 200);
	}

	SECTION("nestedIfThenElse") {
		auto f = engine.registerFunction(nestedIfThenElse);
		REQUIRE(f(3) == 1);  // x < 10 && x < 5 -> 1
		REQUIRE(f(7) == 2);  // x < 10 && x >= 5 -> 2
		REQUIRE(f(15) == 3); // x >= 10 -> 3
	}

	SECTION("ifThenElseFloat") {
		auto f = engine.registerFunction(ifThenElseFloat);
		REQUIRE(f(true, 3.14f, 2.71f) == 3.14f);
		REQUIRE(f(false, 3.14f, 2.71f) == 2.71f);
	}

	SECTION("ifThenElseDouble") {
		auto f = engine.registerFunction(ifThenElseDouble);
		REQUIRE(f(true, 3.14159, 2.71828) == 3.14159);
		REQUIRE(f(false, 3.14159, 2.71828) == 2.71828);
	}

	SECTION("ifThenElseBool") {
		auto f = engine.registerFunction(ifThenElseBool);
		REQUIRE(f(true, true, false) == true);
		REQUIRE(f(false, true, false) == false);
		REQUIRE(f(true, false, true) == false);
		REQUIRE(f(false, false, true) == true);
	}

	SECTION("ifThenElseMax") {
		auto f = engine.registerFunction(ifThenElseMax);
		REQUIRE(f(10, 5) == 10);  // max(10, 5) = 10
		REQUIRE(f(3, 7) == 7);    // max(3, 7) = 7
		REQUIRE(f(5, 5) == 5);    // max(5, 5) = 5
		REQUIRE(f(-5, -3) == -3); // max(-5, -3) = -3
	}

	SECTION("ifThenElseMin") {
		auto f = engine.registerFunction(ifThenElseMin);
		REQUIRE(f(10, 5) == 5);   // min(10, 5) = 5
		REQUIRE(f(3, 7) == 3);    // min(3, 7) = 3
		REQUIRE(f(5, 5) == 5);    // min(5, 5) = 5
		REQUIRE(f(-5, -3) == -5); // min(-5, -3) = -5
	}

	SECTION("ifThenElseAbs") {
		auto f = engine.registerFunction(ifThenElseAbs);
		REQUIRE(f(5) == 5);      // abs(5) = 5
		REQUIRE(f(-5) == 5);     // abs(-5) = 5
		REQUIRE(f(0) == 0);      // abs(0) = 0
		REQUIRE(f(-100) == 100); // abs(-100) = 100
	}

	SECTION("ifThenElseClamp") {
		auto f = engine.registerFunction(ifThenElseClamp);
		REQUIRE(f(5, 0, 10) == 5);   // 5 is within [0, 10]
		REQUIRE(f(-5, 0, 10) == 0);  // -5 clamped to 0
		REQUIRE(f(15, 0, 10) == 10); // 15 clamped to 10
		REQUIRE(f(0, 0, 10) == 0);   // 0 is at lower bound
		REQUIRE(f(10, 0, 10) == 10); // 10 is at upper bound
	}

	SECTION("ifThenElseCompare") {
		auto f = engine.registerFunction(ifThenElseCompare);
		REQUIRE(f(5, 5, 100) == 100); // a == b, return c
		REQUIRE(f(3, 5, 100) == 8);   // a != b, return a + b
	}

	SECTION("ifThenElseClassify") {
		auto f = engine.registerFunction(ifThenElseClassify);
		REQUIRE(f(-5) == -1); // negative -> -1
		REQUIRE(f(0) == 0);   // zero -> 0
		REQUIRE(f(5) == 1);   // positive -> 1
	}
}

TEST_CASE("IfThenElse Interpreter Test") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	ifThenElseTest(engine);
}

// TODO: Enable compiler tests once the SSA creation phase supports direct block creation
// The current SSA phase expects symbolic execution (one path per iteration) rather than
// direct block creation (all paths in one pass).
// #ifdef ENABLE_TRACING
// TEST_CASE("IfThenElse Compiler Test") {
// 	std::vector<std::string> backends = {};
// #ifdef ENABLE_MLIR_BACKEND
// 	backends.emplace_back("mlir");
// #endif
// #ifdef ENABLE_C_BACKEND
// 	backends.emplace_back("cpp");
// #endif
// #ifdef ENABLE_BC_BACKEND
// 	backends.emplace_back("bc");
// #endif
// #ifdef ENABLE_ASMJIT_BACKEND
// 	backends.emplace_back("asmjit");
// #endif
// 	for (auto& backend : backends) {
// 		DYNAMIC_SECTION(backend) {
// 			engine::Options options;
// 			options.setOption("engine.backend", backend);
// 			auto engine = engine::NautilusEngine(options);
// 			ifThenElseTest(engine);
// 		}
// 	}
// }
// #endif
} // namespace nautilus::engine
