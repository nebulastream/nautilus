#include "PointerFunctions.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

void pointerTest(engine::NautilusEngine& engine) {
	int* values = new int[10] {1, 2, 3, 4, 5, 6, 7, 8, 9};

	SECTION("pointerAdd") {
		auto f = engine.registerFunction(pointerAdd);
		REQUIRE(f(values, (int32_t) 0) == 1);
		REQUIRE(f(values, (int32_t) 1) == 2);
	}

	SECTION("pointerAddConst") {
		auto f = engine.registerFunction(pointerAddConst);
		REQUIRE(f(values) == 3);
	}

	SECTION("pointerSub") {
		auto f = engine.registerFunction(pointerSub);
		REQUIRE(f(values, (int32_t) 0) == 1);
		REQUIRE(f(&values[1], (int32_t) 1) == 1);
		REQUIRE(f(&values[2], (int32_t) 2) == 1);
	}

	SECTION("pointerSubConst") {
		auto f = engine.registerFunction(pointerSubConst);
		REQUIRE(f(&values[2]) == 1);
		REQUIRE(f(&values[3]) == 2);
	}

	// pointer arithmetic with different pointer element types (fixed int32_t offset)
	SECTION("pointerAdd_i8_i32") {
		int8_t data[] = {10, 20, 30, 40, 50};
		auto f = engine.registerFunction(pointerAddInt<int8_t, int32_t>);
		REQUIRE(f(data, (int32_t) 0) == 10);
		REQUIRE(f(data, (int32_t) 1) == 20);
		REQUIRE(f(data, (int32_t) 4) == 50);
	}
	SECTION("pointerAdd_i16_i32") {
		int16_t data[] = {100, 200, 300, 400, 500};
		auto f = engine.registerFunction(pointerAddInt<int16_t, int32_t>);
		REQUIRE(f(data, (int32_t) 0) == 100);
		REQUIRE(f(data, (int32_t) 1) == 200);
		REQUIRE(f(data, (int32_t) 4) == 500);
	}
	SECTION("pointerAdd_i64_i32") {
		int64_t data[] = {1000, 2000, 3000, 4000, 5000};
		auto f = engine.registerFunction(pointerAddInt<int64_t, int32_t>);
		REQUIRE(f(data, (int32_t) 0) == 1000);
		REQUIRE(f(data, (int32_t) 1) == 2000);
		REQUIRE(f(data, (int32_t) 4) == 5000);
	}

	// pointer arithmetic with different integer offset types (fixed int32_t pointer element)
	SECTION("pointerAdd_i32_i8") {
		auto f = engine.registerFunction(pointerAddInt<int32_t, int8_t>);
		REQUIRE(f(values, (int8_t) 0) == 1);
		REQUIRE(f(values, (int8_t) 1) == 2);
		REQUIRE(f(values, (int8_t) 3) == 4);
	}
	SECTION("pointerAdd_i32_i16") {
		auto f = engine.registerFunction(pointerAddInt<int32_t, int16_t>);
		REQUIRE(f(values, (int16_t) 0) == 1);
		REQUIRE(f(values, (int16_t) 1) == 2);
		REQUIRE(f(values, (int16_t) 3) == 4);
	}
	SECTION("pointerAdd_i32_i64") {
		auto f = engine.registerFunction(pointerAddInt<int32_t, int64_t>);
		REQUIRE(f(values, (int64_t) 0) == 1);
		REQUIRE(f(values, (int64_t) 1) == 2);
		REQUIRE(f(values, (int64_t) 3) == 4);
	}
	SECTION("pointerAdd_i32_ui32") {
		auto f = engine.registerFunction(pointerAddInt<int32_t, uint32_t>);
		REQUIRE(f(values, (uint32_t) 0) == 1);
		REQUIRE(f(values, (uint32_t) 1) == 2);
		REQUIRE(f(values, (uint32_t) 3) == 4);
	}
	SECTION("pointerAdd_i32_ui64") {
		auto f = engine.registerFunction(pointerAddInt<int32_t, uint64_t>);
		REQUIRE(f(values, (uint64_t) 0) == 1);
		REQUIRE(f(values, (uint64_t) 1) == 2);
		REQUIRE(f(values, (uint64_t) 3) == 4);
	}

	// pointer subtraction with different pointer element types
	SECTION("pointerSub_i8_i32") {
		int8_t data[] = {10, 20, 30, 40, 50};
		auto f = engine.registerFunction(pointerSubInt<int8_t, int32_t>);
		REQUIRE(f(data, (int32_t) 0) == 10);
		REQUIRE(f(&data[1], (int32_t) 1) == 10);
		REQUIRE(f(&data[4], (int32_t) 4) == 10);
	}
	SECTION("pointerSub_i16_i32") {
		int16_t data[] = {100, 200, 300, 400, 500};
		auto f = engine.registerFunction(pointerSubInt<int16_t, int32_t>);
		REQUIRE(f(data, (int32_t) 0) == 100);
		REQUIRE(f(&data[1], (int32_t) 1) == 100);
		REQUIRE(f(&data[4], (int32_t) 4) == 100);
	}
	SECTION("pointerSub_i64_i32") {
		int64_t data[] = {1000, 2000, 3000, 4000, 5000};
		auto f = engine.registerFunction(pointerSubInt<int64_t, int32_t>);
		REQUIRE(f(data, (int32_t) 0) == 1000);
		REQUIRE(f(&data[1], (int32_t) 1) == 1000);
		REQUIRE(f(&data[4], (int32_t) 4) == 1000);
	}

	// pointer subtraction with different offset integer types
	SECTION("pointerSub_i32_i8") {
		auto f = engine.registerFunction(pointerSubInt<int32_t, int8_t>);
		REQUIRE(f(values, (int8_t) 0) == 1);
		REQUIRE(f(&values[1], (int8_t) 1) == 1);
		REQUIRE(f(&values[3], (int8_t) 3) == 1);
	}
	SECTION("pointerSub_i32_i16") {
		auto f = engine.registerFunction(pointerSubInt<int32_t, int16_t>);
		REQUIRE(f(values, (int16_t) 0) == 1);
		REQUIRE(f(&values[1], (int16_t) 1) == 1);
		REQUIRE(f(&values[3], (int16_t) 3) == 1);
	}
	SECTION("pointerSub_i32_i64") {
		auto f = engine.registerFunction(pointerSubInt<int32_t, int64_t>);
		REQUIRE(f(values, (int64_t) 0) == 1);
		REQUIRE(f(&values[1], (int64_t) 1) == 1);
		REQUIRE(f(&values[3], (int64_t) 3) == 1);
	}

	// constant pointer arithmetic for different element types
	SECTION("pointerAddConst_i8") {
		int8_t data[] = {10, 20, 30, 40, 50};
		auto f = engine.registerFunction(pointerAddConstInt<int8_t>);
		REQUIRE(f(data) == 30);
		REQUIRE(f(&data[1]) == 40);
	}
	SECTION("pointerAddConst_i16") {
		int16_t data[] = {100, 200, 300, 400, 500};
		auto f = engine.registerFunction(pointerAddConstInt<int16_t>);
		REQUIRE(f(data) == 300);
		REQUIRE(f(&data[1]) == 400);
	}
	SECTION("pointerAddConst_i64") {
		int64_t data[] = {1000, 2000, 3000, 4000, 5000};
		auto f = engine.registerFunction(pointerAddConstInt<int64_t>);
		REQUIRE(f(data) == 3000);
		REQUIRE(f(&data[1]) == 4000);
	}
	SECTION("pointerSubConst_i8") {
		int8_t data[] = {10, 20, 30, 40, 50};
		auto f = engine.registerFunction(pointerSubConstInt<int8_t>);
		REQUIRE(f(&data[2]) == 10);
		REQUIRE(f(&data[3]) == 20);
	}
	SECTION("pointerSubConst_i16") {
		int16_t data[] = {100, 200, 300, 400, 500};
		auto f = engine.registerFunction(pointerSubConstInt<int16_t>);
		REQUIRE(f(&data[2]) == 100);
		REQUIRE(f(&data[3]) == 200);
	}
	SECTION("pointerSubConst_i64") {
		int64_t data[] = {1000, 2000, 3000, 4000, 5000};
		auto f = engine.registerFunction(pointerSubConstInt<int64_t>);
		REQUIRE(f(&data[2]) == 1000);
		REQUIRE(f(&data[3]) == 2000);
	}

	// compound assignment: += with different pointer element types
	SECTION("pointerPlusAssign_i32_i32") {
		auto f = engine.registerFunction(pointerPlusAssign<int32_t, int32_t>);
		REQUIRE(f(values, (int32_t) 0) == 1);
		REQUIRE(f(values, (int32_t) 2) == 3);
		REQUIRE(f(values, (int32_t) 4) == 5);
	}
	SECTION("pointerPlusAssign_i8_i32") {
		int8_t data[] = {10, 20, 30, 40, 50};
		auto f = engine.registerFunction(pointerPlusAssign<int8_t, int32_t>);
		REQUIRE(f(data, (int32_t) 0) == 10);
		REQUIRE(f(data, (int32_t) 2) == 30);
		REQUIRE(f(data, (int32_t) 4) == 50);
	}
	SECTION("pointerPlusAssign_i64_i32") {
		int64_t data[] = {1000, 2000, 3000, 4000, 5000};
		auto f = engine.registerFunction(pointerPlusAssign<int64_t, int32_t>);
		REQUIRE(f(data, (int32_t) 0) == 1000);
		REQUIRE(f(data, (int32_t) 2) == 3000);
		REQUIRE(f(data, (int32_t) 4) == 5000);
	}

	// compound assignment: += with different offset integer types
	SECTION("pointerPlusAssign_i32_i8") {
		auto f = engine.registerFunction(pointerPlusAssign<int32_t, int8_t>);
		REQUIRE(f(values, (int8_t) 0) == 1);
		REQUIRE(f(values, (int8_t) 2) == 3);
		REQUIRE(f(values, (int8_t) 4) == 5);
	}
	SECTION("pointerPlusAssign_i32_i64") {
		auto f = engine.registerFunction(pointerPlusAssign<int32_t, int64_t>);
		REQUIRE(f(values, (int64_t) 0) == 1);
		REQUIRE(f(values, (int64_t) 2) == 3);
		REQUIRE(f(values, (int64_t) 4) == 5);
	}

	// compound assignment: -= with different pointer element types
	SECTION("pointerMinusAssign_i32_i32") {
		auto f = engine.registerFunction(pointerMinusAssign<int32_t, int32_t>);
		REQUIRE(f(values, (int32_t) 0) == 1);
		REQUIRE(f(&values[2], (int32_t) 2) == 1);
		REQUIRE(f(&values[4], (int32_t) 4) == 1);
	}
	SECTION("pointerMinusAssign_i8_i32") {
		int8_t data[] = {10, 20, 30, 40, 50};
		auto f = engine.registerFunction(pointerMinusAssign<int8_t, int32_t>);
		REQUIRE(f(data, (int32_t) 0) == 10);
		REQUIRE(f(&data[2], (int32_t) 2) == 10);
		REQUIRE(f(&data[4], (int32_t) 4) == 10);
	}
	SECTION("pointerMinusAssign_i64_i32") {
		int64_t data[] = {1000, 2000, 3000, 4000, 5000};
		auto f = engine.registerFunction(pointerMinusAssign<int64_t, int32_t>);
		REQUIRE(f(data, (int32_t) 0) == 1000);
		REQUIRE(f(&data[2], (int32_t) 2) == 1000);
		REQUIRE(f(&data[4], (int32_t) 4) == 1000);
	}

	// compound assignment: -= with different offset integer types
	SECTION("pointerMinusAssign_i32_i8") {
		auto f = engine.registerFunction(pointerMinusAssign<int32_t, int8_t>);
		REQUIRE(f(values, (int8_t) 0) == 1);
		REQUIRE(f(&values[2], (int8_t) 2) == 1);
		REQUIRE(f(&values[4], (int8_t) 4) == 1);
	}
	SECTION("pointerMinusAssign_i32_i64") {
		auto f = engine.registerFunction(pointerMinusAssign<int32_t, int64_t>);
		REQUIRE(f(values, (int64_t) 0) == 1);
		REQUIRE(f(&values[2], (int64_t) 2) == 1);
		REQUIRE(f(&values[4], (int64_t) 4) == 1);
	}

	// pre-increment operator ++ for different pointer element types
	SECTION("pointerPreIncrement_i8") {
		int8_t data[] = {10, 20, 30, 40, 50};
		auto f = engine.registerFunction(pointerPreIncrement<int8_t>);
		REQUIRE(f(data) == 20);
		REQUIRE(f(&data[1]) == 30);
		REQUIRE(f(&data[3]) == 50);
	}
	SECTION("pointerPreIncrement_i32") {
		auto f = engine.registerFunction(pointerPreIncrement<int32_t>);
		REQUIRE(f(values) == 2);
		REQUIRE(f(&values[1]) == 3);
		REQUIRE(f(&values[3]) == 5);
	}
	SECTION("pointerPreIncrement_i64") {
		int64_t data[] = {1000, 2000, 3000, 4000, 5000};
		auto f = engine.registerFunction(pointerPreIncrement<int64_t>);
		REQUIRE(f(data) == 2000);
		REQUIRE(f(&data[1]) == 3000);
		REQUIRE(f(&data[3]) == 5000);
	}

	SECTION("pointerAddNegativeOffset") {
		auto f = engine.registerFunction(pointerAddNegativeOffset);
		// values = {1, 2, 3, 4, 5, 6, 7, 8, 9}
		// &values[3] - 1 should point to values[2] = 3
		REQUIRE(f(&values[3], (int32_t) 1) == 3);
		REQUIRE(f(&values[5], (int32_t) 2) == 4);
		REQUIRE(f(&values[4], (int32_t) 4) == 1);
	}

	SECTION("pointerLessThanAfterAdd") {
		auto f = engine.registerFunction(pointerLessThanAfterAdd);
		// base < base + positive_offset should be true
		REQUIRE(f(values, (int32_t) 1) == true);
		REQUIRE(f(values, (int32_t) 5) == true);
	}

	SECTION("pointerGreaterThanAfterAdd") {
		auto f = engine.registerFunction(pointerGreaterThanAfterAdd);
		// base + positive_offset > base should be true
		REQUIRE(f(values, (int32_t) 1) == true);
		REQUIRE(f(values, (int32_t) 3) == true);
	}

	SECTION("pointerRoundTrip") {
		auto f = engine.registerFunction(pointerRoundTrip);
		// ptr + offset - offset should give back ptr
		REQUIRE(f(values, (int32_t) 3) == 1);
		REQUIRE(f(&values[2], (int32_t) 2) == 3);
		REQUIRE(f(&values[4], (int32_t) 0) == 5);
	}

	SECTION("pointerMultiStep") {
		auto f = engine.registerFunction(pointerMultiStep);
		// ptr + 1 + 1 + 1 = ptr + 3
		REQUIRE(f(values) == 4);     // values[3] = 4
		REQUIRE(f(&values[1]) == 5); // values[4] = 5
		REQUIRE(f(&values[5]) == 9); // values[8] = 9
	}
}

TEST_CASE("Pointer Interpreter Test") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	pointerTest(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("Pointer Compiler Test") {
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
			pointerTest(engine);
		}
	}
}
#endif
} // namespace nautilus::engine
