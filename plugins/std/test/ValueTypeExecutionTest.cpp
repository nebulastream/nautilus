
#include "ValueTypeFunctions.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

void valueExecutionTest(engine::NautilusEngine& engine) {
	SECTION("constructAndAccess") {
		auto f = engine.registerFunction(constructAndAccess);
		REQUIRE(f() == 42);
	}
	SECTION("constructSetBothFields") {
		auto f = engine.registerFunction(constructSetBothFields);
		REQUIRE(f() == 42);
	}
	SECTION("constructNonTrivialDefault") {
		auto f = engine.registerFunction(constructNonTrivialDefault);
		REQUIRE(f() == 7);
	}
	SECTION("constructAndCall") {
		auto f = engine.registerFunction(constructAndCall);
		REQUIRE(f((int32_t) 0) == 0);
		REQUIRE(f((int32_t) 5) == 10);
		REQUIRE(f((int32_t) 21) == 42);
		REQUIRE(f((int32_t) -3) == -6);
	}
	SECTION("constructWithArgs") {
		auto f = engine.registerFunction(constructWithArgs);
		REQUIRE(f((int32_t) 0, (int32_t) 0) == 0);
		REQUIRE(f((int32_t) 10, (int32_t) 32) == 42);
		REQUIRE(f((int32_t) -5, (int32_t) 5) == 0);
		REQUIRE(f((int32_t) -10, (int32_t) -10) == -20);
		REQUIRE(f((int32_t) 1, (int32_t) 0) == 1);
	}
	SECTION("copyConstruct") {
		auto f = engine.registerFunction(copyConstruct);
		REQUIRE(f() == 52);
	}
	SECTION("copyAssign") {
		auto f = engine.registerFunction(copyAssign);
		REQUIRE(f() == 42);
	}
	SECTION("copyConstructNonTrivial") {
		auto f = engine.registerFunction(copyConstructNonTrivial);
		REQUIRE(f() == 99);
	}
	SECTION("nonTrivialDestructor") {
		auto f = engine.registerFunction(nonTrivialDestructor);
		REQUIRE(f() == 42);
	}
	SECTION("modifyInLoop") {
		auto f = engine.registerFunction(modifyInLoop);
		REQUIRE(f((int32_t) 0) == 0);
		REQUIRE(f((int32_t) 1) == 1);
		REQUIRE(f((int32_t) 5) == 5);
		REQUIRE(f((int32_t) 10) == 10);
		REQUIRE(f((int32_t) 100) == 100);
	}
	SECTION("copyInLoop") {
		auto f = engine.registerFunction(copyInLoop);
		REQUIRE(f((int32_t) 0) == 0);
		REQUIRE(f((int32_t) 1) == 10);
		REQUIRE(f((int32_t) 3) == 30);
		REQUIRE(f((int32_t) 10) == 100);
	}
	SECTION("mixedAlignSetAll") {
		auto f = engine.registerFunction(mixedAlignSetAll);
		REQUIRE(f((int8_t) 1, (int32_t) 10, (int64_t) 100) == 111);
		REQUIRE(f((int8_t) -1, (int32_t) 0, (int64_t) 0) == -1);
		REQUIRE(f((int8_t) 0, (int32_t) 0, (int64_t) 0) == 0);
		REQUIRE(f((int8_t) 127, (int32_t) 1000, (int64_t) 100000) == 101127);
	}
	SECTION("mixedAlignNoClobber") {
		auto f = engine.registerFunction(mixedAlignNoClobber);
		REQUIRE(f() == 1000);
	}
	SECTION("reversePaddingAccess") {
		auto f = engine.registerFunction(reversePaddingAccess);
		REQUIRE(f((int64_t) 100, (int8_t) 10, (int16_t) 5) == 115);
		REQUIRE(f((int64_t) 0, (int8_t) 0, (int16_t) 0) == 0);
		REQUIRE(f((int64_t) -50, (int8_t) -1, (int16_t) -2) == -53);
	}
	SECTION("doubleAndByteAccess") {
		auto f = engine.registerFunction(doubleAndByteAccess);
		REQUIRE(f(3.5, (int8_t) 2) == Catch::Approx(5.5));
		REQUIRE(f(0.0, (int8_t) 0) == Catch::Approx(0.0));
		REQUIRE(f(-1.5, (int8_t) 1) == Catch::Approx(-0.5));
	}
	SECTION("mixedAlignModifyInLoop") {
		auto f = engine.registerFunction(mixedAlignModifyInLoop);
		REQUIRE(f((int32_t) 0) == 0);
		REQUIRE(f((int32_t) 1) == 111);
		REQUIRE(f((int32_t) 5) == 555);
	}
	SECTION("mixedAlignConditionalReturn") {
		auto f = engine.registerFunction(mixedAlignConditionalReturn);
		REQUIRE(f((int64_t) 10) == 13);
		REQUIRE(f((int64_t) 1) == 4);
		REQUIRE(f((int64_t) 0) == 0);
		REQUIRE(f((int64_t) -5) == -5);
	}
	SECTION("multiStructMultiLoop") {
		auto f = engine.registerFunction(multiStructMultiLoop);
		REQUIRE(f((int32_t) 0) == 0);
		REQUIRE(f((int32_t) 1) == 333);
		REQUIRE(f((int32_t) 3) == 999);
	}
	SECTION("twoStructsSameLoop") {
		auto f = engine.registerFunction(twoStructsSameLoop);
		REQUIRE(f((int32_t) 0) == 0);
		REQUIRE(f((int32_t) 1) == 30);
		REQUIRE(f((int32_t) 5) == 150);
	}
	SECTION("outerAndInnerStructLoop") {
		auto f = engine.registerFunction(outerAndInnerStructLoop);
		REQUIRE(f((int32_t) 0) == 0);
		REQUIRE(f((int32_t) 1) == 5);
		REQUIRE(f((int32_t) 10) == 50);
	}
	SECTION("multiStructConditionalLoop") {
		auto f = engine.registerFunction(multiStructConditionalLoop);
		REQUIRE(f((int32_t) 5, (int64_t) 0) == 0);
		REQUIRE(f((int32_t) 5, (int64_t) -3) == -3);
		REQUIRE(f((int32_t) 0, (int64_t) 1) == 0);
		REQUIRE(f((int32_t) 1, (int64_t) 1) == 10);
		REQUIRE(f((int32_t) 4, (int64_t) 1) == 40);
	}
	SECTION("modifyStructInLoopWithNestedCall") {
		auto f = engine.registerFunction(modifyStructInLoopWithNestedCall);
		REQUIRE(f((int32_t) 0) == 0);
		REQUIRE(f((int32_t) 1) == 1);
		REQUIRE(f((int32_t) 5) == 5);
		REQUIRE(f((int32_t) 10) == 10);
	}
	SECTION("constructStructInLoopWithNestedCall") {
		auto f = engine.registerFunction(constructStructInLoopWithNestedCall);
		REQUIRE(f((int32_t) 0) == 0);
		REQUIRE(f((int32_t) 1) == 0);
		REQUIRE(f((int32_t) 4) == 6);
		REQUIRE(f((int32_t) 5) == 10);
	}
	SECTION("multipleNestedCallsInLoop") {
		auto f = engine.registerFunction(multipleNestedCallsInLoop);
		REQUIRE(f((int32_t) 0) == 0);
		REQUIRE(f((int32_t) 1) == 100);
		REQUIRE(f((int32_t) 2) == 300);
	}
	SECTION("allocaMergeBug") {
		auto f = engine.registerFunction(allocaMergeBug);
		REQUIRE(f() == 78);
	}
	SECTION("allocaMergeInLoop") {
		auto f = engine.registerFunction(allocaMergeInLoop);
		REQUIRE(f((int32_t) 0) == 0);
		REQUIRE(f((int32_t) 1) == 78);
		REQUIRE(f((int32_t) 3) == 234);
	}
}

TEST_CASE("Value Type Interpreter Test") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	valueExecutionTest(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("Value Type Compiler Test") {
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
	std::vector<std::string> traceModes = {"exceptionBasedTracing", "lazyTracing"};
	for (auto& backend : backends) {
		for (auto& traceMode : traceModes) {
			DYNAMIC_SECTION(backend + "_" + traceMode) {
				engine::Options options;
				options.setOption("engine.backend", backend);
				options.setOption("engine.traceMode", traceMode);
				auto engine = engine::NautilusEngine(options);
				valueExecutionTest(engine);
			}
		}
	}
}
#endif

} // namespace nautilus::engine
