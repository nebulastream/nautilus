#include "BoolOperations.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val_concepts.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

void addTest(engine::NautilusEngine& engine) {

	SECTION("boolNot") {
		auto f = engine.registerFunction(boolNot);
		REQUIRE(f(true) == false);
		REQUIRE(f(false) == true);
	}

	SECTION("boolAnd") {
		auto f = engine.registerFunction(boolAnd);
		REQUIRE(f(true, true) == true);
		REQUIRE(f(false, true) == false);
		REQUIRE(f(false, false) == false);
		REQUIRE(f(false, true) == false);
	}

	SECTION("boolOr") {
		auto f = engine.registerFunction(boolOr);
		REQUIRE(f(true, true) == true);
		REQUIRE(f(false, true) == true);
		REQUIRE(f(false, false) == false);
		REQUIRE(f(false, true) == true);
	}

	SECTION("boolAssignment") {
		auto f = engine.registerFunction(boolAssignment);
		REQUIRE(f(true) == true);
		REQUIRE(f(false) == false);
	}

	SECTION("boolAssignmentOr") {
		auto f = engine.registerFunction(boolAssignmentOr);
		REQUIRE(f(true, true) == true);
		REQUIRE(f(true, false) == true);
		REQUIRE(f(false, true) == true);
		REQUIRE(f(false, false) == false);
	}

	SECTION("boolConst") {
		auto f = engine.registerFunction(boolConst);
		REQUIRE(f(true) == true);
		REQUIRE(f(false) == false);
	}

	SECTION("boolEquals") {
		auto f = engine.registerFunction(boolEquals);
		REQUIRE(f(true, true) == true);
		REQUIRE(f(false, false) == true);
		REQUIRE(f(false, true) == false);
		REQUIRE(f(true, false) == false);
	}

	SECTION("boolNotEquals") {
		auto f = engine.registerFunction(boolNotEquals);
		REQUIRE(f(true, true) == false);
		REQUIRE(f(false, false) == false);
		REQUIRE(f(false, true) == true);
		REQUIRE(f(true, false) == true);
	}

	SECTION("boolIfElse") {
		auto f = engine.registerFunction(boolIfElse);
		REQUIRE(f(true, true) == true);
		REQUIRE(f(false, false) == false);
		REQUIRE(f(false, true) == false);
		REQUIRE(f(true, false) == false);
	}

	SECTION("boolNestedFunction") {
		auto f = engine.registerFunction(boolNestedFunction);
		REQUIRE(f(true, true) == true);
		REQUIRE(f(false, false) == true);
		REQUIRE(f(false, true) == false);
		REQUIRE(f(true, false) == false);
	}

}

TEST_CASE("Bool Interpreter Test") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	addTest(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("Bool Compiler Test") {
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
			addTest(engine);
		}
	}
}
#endif
} // namespace nautilus::engine
