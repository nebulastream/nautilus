#include "BoolOperations.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_concepts.hpp"
#include "nautilus/val_ptr.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

void boolTest(engine::NautilusEngine& engine) {

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

	SECTION("operatorBool") {
		auto f = engine.registerFunction(operatorBool<bool>);
		REQUIRE(f(true) == true);
		REQUIRE(f(false) == false);
	}

	SECTION("operatorBoolPtr") {
		auto f = engine.registerFunction(operatorBool<int*>);
		int* x = nullptr;
		int tmp = 1234;
		int* y = &tmp;
		REQUIRE(f(x) == false);
		REQUIRE(f(y) == true);
	}

	SECTION("boolEqualsMixed") {
		auto f = engine.registerFunction(boolEqualsMixed);
		REQUIRE(f(true, true) == true);
		REQUIRE(f(false, false) == true);
		REQUIRE(f(true, false) == false);
		REQUIRE(f(false, true) == false);
	}

	SECTION("boolNotEqualsMixed") {
		auto f = engine.registerFunction(boolNotEqualsMixed);
		REQUIRE(f(true, true) == false);
		REQUIRE(f(false, false) == false);
		REQUIRE(f(true, false) == true);
		REQUIRE(f(false, true) == true);
	}

	SECTION("boolComplexOps") {
		auto f = engine.registerFunction(boolComplexOps);
		// (a == b) && (b != c)
		REQUIRE(f(true, true, false) == true);    // (T == T) && (T != F) = T && T = T
		REQUIRE(f(true, true, true) == false);    // (T == T) && (T != T) = T && F = F
		REQUIRE(f(true, false, true) == false);   // (T == F) && (F != T) = F && T = F
		REQUIRE(f(false, false, true) == true);   // (F == F) && (F != T) = T && T = T
		REQUIRE(f(false, false, false) == false); // (F == F) && (F != F) = T && F = F
	}

	SECTION("boolProbabilityTest") {
		auto f = engine.registerFunction(boolProbabilityTest);
		REQUIRE(f(true) == true);
		REQUIRE(f(false) == false);
	}
}

TEST_CASE("Bool Interpreter Test") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	boolTest(engine);
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
			boolTest(engine);
		}
	}
}
#endif
} // namespace nautilus::engine
