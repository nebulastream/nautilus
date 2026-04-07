#include "BoolOperations.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_concepts.hpp"
#include "nautilus/val_ptr.hpp"
#include <catch2/catch_all.hpp>
#include <limits>

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

	// Regression: bc backend used to leave stale upper bytes in the
	// 64-bit register slot when writing a narrow bool result, which the
	// function-return path then read as a non-zero int64_t and reported
	// as `true`. See ByteCode.hpp writeReg<>.
	SECTION("invokeReturnsFalseAfterDirtyReg") {
		auto f = engine.registerFunction(invokeReturnsFalseAfterDirtyReg);
		REQUIRE(f() == false);
	}
	SECTION("invokeReturnsTrueAfterDirtyReg") {
		auto f = engine.registerFunction(invokeReturnsTrueAfterDirtyReg);
		REQUIRE(f() == true);
	}
	SECTION("invokePointerCmpFalse") {
		auto f = engine.registerFunction(invokePointerCmpFalse);
		REQUIRE(f() == false);
	}

	// Narrow-integer return regression: each writeReg<T> path in the
	// bc backend must zero-extend into the full 64-bit register slot.
	// We test 0, -1 / max, and a typical mid value for each width.
	SECTION("invokeReturnsI8AfterDirtyReg") {
		auto f = engine.registerFunction(invokeReturnsI8AfterDirtyReg);
		REQUIRE(f(int8_t {0}) == int8_t {0});
		REQUIRE(f(int8_t {-1}) == int8_t {-1});
		REQUIRE(f(int8_t {42}) == int8_t {42});
		REQUIRE(f(std::numeric_limits<int8_t>::min()) == std::numeric_limits<int8_t>::min());
	}
	SECTION("invokeReturnsI16AfterDirtyReg") {
		auto f = engine.registerFunction(invokeReturnsI16AfterDirtyReg);
		REQUIRE(f(int16_t {0}) == int16_t {0});
		REQUIRE(f(int16_t {-1}) == int16_t {-1});
		REQUIRE(f(int16_t {1234}) == int16_t {1234});
		REQUIRE(f(std::numeric_limits<int16_t>::min()) == std::numeric_limits<int16_t>::min());
	}
	SECTION("invokeReturnsI32AfterDirtyReg") {
		auto f = engine.registerFunction(invokeReturnsI32AfterDirtyReg);
		REQUIRE(f(0) == 0);
		REQUIRE(f(-1) == -1);
		REQUIRE(f(123456) == 123456);
		REQUIRE(f(std::numeric_limits<int32_t>::min()) == std::numeric_limits<int32_t>::min());
	}
	SECTION("invokeReturnsI64AfterDirtyReg") {
		auto f = engine.registerFunction(invokeReturnsI64AfterDirtyReg);
		REQUIRE(f(int64_t {0}) == int64_t {0});
		REQUIRE(f(int64_t {-1}) == int64_t {-1});
		REQUIRE(f(int64_t {1234567890123}) == int64_t {1234567890123});
	}
	SECTION("invokeReturnsU8AfterDirtyReg") {
		auto f = engine.registerFunction(invokeReturnsU8AfterDirtyReg);
		REQUIRE(f(uint8_t {0}) == uint8_t {0});
		REQUIRE(f(uint8_t {0xFF}) == uint8_t {0xFF});
		REQUIRE(f(uint8_t {0x7F}) == uint8_t {0x7F});
	}
	SECTION("invokeReturnsU16AfterDirtyReg") {
		auto f = engine.registerFunction(invokeReturnsU16AfterDirtyReg);
		REQUIRE(f(uint16_t {0}) == uint16_t {0});
		REQUIRE(f(uint16_t {0xFFFF}) == uint16_t {0xFFFF});
		REQUIRE(f(uint16_t {0x1234}) == uint16_t {0x1234});
	}
	SECTION("invokeReturnsU32AfterDirtyReg") {
		auto f = engine.registerFunction(invokeReturnsU32AfterDirtyReg);
		REQUIRE(f(uint32_t {0}) == uint32_t {0});
		REQUIRE(f(uint32_t {0xFFFFFFFFu}) == uint32_t {0xFFFFFFFFu});
		REQUIRE(f(uint32_t {0xDEADBEEFu}) == uint32_t {0xDEADBEEFu});
	}
	SECTION("invokeReturnsU64AfterDirtyReg") {
		auto f = engine.registerFunction(invokeReturnsU64AfterDirtyReg);
		REQUIRE(f(uint64_t {0}) == uint64_t {0});
		REQUIRE(f(uint64_t {0xFFFFFFFFFFFFFFFFull}) == uint64_t {0xFFFFFFFFFFFFFFFFull});
	}

	// Cast after invoke: exercises readReg<NarrowType> on top of a slot
	// whose upper bytes were potentially polluted by a prior proxy call.
	SECTION("invokeI8ThenCastToI64") {
		auto f = engine.registerFunction(invokeI8ThenCastToI64);
		REQUIRE(f(int8_t {0}) == 0);
		REQUIRE(f(int8_t {-1}) == -1);
		REQUIRE(f(int8_t {127}) == 127);
		REQUIRE(f(int8_t {-128}) == -128);
	}
	SECTION("invokeI16ThenCastToI64") {
		auto f = engine.registerFunction(invokeI16ThenCastToI64);
		REQUIRE(f(int16_t {0}) == 0);
		REQUIRE(f(int16_t {-1}) == -1);
		REQUIRE(f(int16_t {-32768}) == -32768);
	}
	SECTION("invokeI32ThenCastToI64") {
		auto f = engine.registerFunction(invokeI32ThenCastToI64);
		REQUIRE(f(0) == 0);
		REQUIRE(f(-1) == -1);
		REQUIRE(f(std::numeric_limits<int32_t>::min()) == std::numeric_limits<int32_t>::min());
	}
	SECTION("invokeU8ThenCastToU64") {
		auto f = engine.registerFunction(invokeU8ThenCastToU64);
		REQUIRE(f(uint8_t {0}) == 0u);
		REQUIRE(f(uint8_t {0xFF}) == 0xFFu);
	}
	SECTION("invokeU16ThenCastToU64") {
		auto f = engine.registerFunction(invokeU16ThenCastToU64);
		REQUIRE(f(uint16_t {0}) == 0u);
		REQUIRE(f(uint16_t {0xFFFF}) == 0xFFFFu);
	}
	SECTION("invokeU32ThenCastToU64") {
		auto f = engine.registerFunction(invokeU32ThenCastToU64);
		REQUIRE(f(uint32_t {0}) == 0u);
		REQUIRE(f(uint32_t {0xFFFFFFFFu}) == 0xFFFFFFFFu);
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
	std::vector<std::string> traceModes = {"exceptionBasedTracing", "lazyTracing"};
	for (auto& backend : backends) {
		for (auto& traceMode : traceModes) {
			DYNAMIC_SECTION(backend + "_" + traceMode) {
				engine::Options options;
				options.setOption("engine.backend", backend);
				options.setOption("engine.traceMode", traceMode);
				auto engine = engine::NautilusEngine(options);
				boolTest(engine);
			}
		}
	}
}
#endif
} // namespace nautilus::engine
