
#include "nautilus/Engine.hpp"
#include "nautilus/std/vector.h"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

// --- Element access ---

val<int32_t> vectorPushBackAndRead(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(x);
	vec.push_back(val<int32_t>(42));
	return vec[val<size_t>(0)] + vec[val<size_t>(1)];
}

val<int32_t> vectorAt(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(10));
	vec.push_back(x);
	vec.push_back(val<int32_t>(30));
	return vec.at(val<size_t>(1));
}

val<int32_t> vectorFrontBack(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(10));
	vec.push_back(x);
	vec.push_back(val<int32_t>(30));
	return vec.front() + vec.back();
}

val<int32_t> vectorDataPtr(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(x);
	vec.push_back(val<int32_t>(50));
	vec.push_back(val<int32_t>(60));
	auto ptr = vec.data();
	return ptr[val<size_t>(0)] + ptr[val<size_t>(1)] + ptr[val<size_t>(2)];
}

// --- Capacity ---

val<bool> vectorEmpty() {
	val<std::vector<int32_t>> vec;
	return vec.empty();
}

val<size_t> vectorNotEmptyAfterPush() {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(1));
	return vec.size();
}

val<bool> vectorEmptyAfterClear() {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(1));
	vec.push_back(val<int32_t>(2));
	vec.clear();
	return vec.empty();
}

val<size_t> vectorSize(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(x);
	vec.push_back(val<int32_t>(2));
	vec.push_back(val<int32_t>(3));
	return vec.size();
}

val<bool> vectorReserveCapacity() {
	val<std::vector<int32_t>> vec;
	vec.reserve(val<size_t>(100));
	auto cap = vec.capacity();
	// capacity should be at least 100 after reserve
	return cap >= val<size_t>(100);
}

// --- Modifiers ---

val<int32_t> vectorClear(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(x);
	vec.clear();
	vec.push_back(val<int32_t>(99));
	return vec[val<size_t>(0)];
}

val<int32_t> vectorPopBack() {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(1));
	vec.push_back(val<int32_t>(2));
	vec.push_back(val<int32_t>(3));
	vec.pop_back();
	return vec.back();
}

val<size_t> vectorResizeGrow() {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(1));
	vec.resize(val<size_t>(5));
	return vec.size();
}

val<int32_t> vectorResizeGrowReadsDefault() {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(42));
	vec.resize(val<size_t>(3));
	// new elements should be value-initialized (0 for int32_t)
	return vec[val<size_t>(1)] + vec[val<size_t>(2)];
}

val<size_t> vectorResizeShrink() {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(1));
	vec.push_back(val<int32_t>(2));
	vec.push_back(val<int32_t>(3));
	vec.push_back(val<int32_t>(4));
	vec.resize(val<size_t>(2));
	return vec.size();
}

val<int32_t> vectorResizeWithValue(val<int32_t> fill) {
	val<std::vector<int32_t>> vec;
	vec.resize(val<size_t>(3), fill);
	return vec[val<size_t>(0)] + vec[val<size_t>(1)] + vec[val<size_t>(2)];
}

// --- Ownership & interop ---

val<std::vector<int32_t>*> vectorReturnPtr(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(x);
	vec.push_back(val<int32_t>(100));
	return vec.release();
}

val<int32_t> vectorFromPtr(val<std::vector<int32_t>*> vec_ptr) {
	val<std::vector<int32_t>> vec(vec_ptr);
	auto result = vec[val<size_t>(0)] + vec[val<size_t>(1)];
	// release ownership since the caller owns the vector
	vec.release();
	return result;
}

// --- Assignment ---

val<int32_t> vectorAssign(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(x);
	vec.push_back(val<int32_t>(20));
	val<std::vector<int32_t>> vec2;
	vec2 = vec;
	auto result = vec2[val<size_t>(0)] + vec2[val<size_t>(1)];
	// release one since assignment shares the underlying pointer
	vec.release();
	return result;
}

// --- Different element types ---

val<double> vectorDouble(val<double> x) {
	val<std::vector<double>> vec;
	vec.push_back(x);
	vec.push_back(val<double>(3.14));
	return vec[val<size_t>(0)] + vec[val<size_t>(1)];
}

val<int64_t> vectorInt64(val<int64_t> x) {
	val<std::vector<int64_t>> vec;
	vec.push_back(x);
	vec.push_back(val<int64_t>(1000000000000LL));
	return vec[val<size_t>(0)] + vec[val<size_t>(1)];
}

val<float> vectorFloat(val<float> x) {
	val<std::vector<float>> vec;
	vec.push_back(x);
	vec.push_back(val<float>(2.5f));
	return vec.front() + vec.back();
}

val<int8_t> vectorInt8(val<int8_t> x) {
	val<std::vector<int8_t>> vec;
	vec.push_back(x);
	vec.push_back(val<int8_t>(10));
	return vec[val<size_t>(0)] + vec[val<size_t>(1)];
}

// --- Combined operations ---

val<int32_t> vectorBuildAndSum(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.reserve(val<size_t>(4));
	vec.push_back(x);
	vec.push_back(x + val<int32_t>(1));
	vec.push_back(x + val<int32_t>(2));
	vec.push_back(x + val<int32_t>(3));
	return vec[val<size_t>(0)] + vec[val<size_t>(1)] + vec[val<size_t>(2)] + vec[val<size_t>(3)];
}

val<size_t> vectorPopToEmpty() {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(1));
	vec.push_back(val<int32_t>(2));
	vec.pop_back();
	vec.pop_back();
	return vec.size();
}

val<int32_t> vectorClearAndRefill(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(100));
	vec.push_back(val<int32_t>(200));
	vec.clear();
	vec.push_back(x);
	return vec.front();
}

void runVectorTest(engine::NautilusEngine& engine) {
	// Element access
	SECTION("vectorPushBackAndRead") {
		auto f = engine.registerFunction(vectorPushBackAndRead);
		REQUIRE(f(10) == 52);
	}
	SECTION("vectorAt") {
		auto f = engine.registerFunction(vectorAt);
		REQUIRE(f(77) == 77);
	}
	SECTION("vectorFrontBack") {
		auto f = engine.registerFunction(vectorFrontBack);
		REQUIRE(f(20) == 40);
	}
	SECTION("vectorDataPtr") {
		auto f = engine.registerFunction(vectorDataPtr);
		REQUIRE(f(5) == 115);
	}

	// Capacity
	SECTION("vectorEmpty") {
		auto f = engine.registerFunction(vectorEmpty);
		REQUIRE(f() == true);
	}
	SECTION("vectorNotEmptyAfterPush") {
		auto f = engine.registerFunction(vectorNotEmptyAfterPush);
		REQUIRE(f() == 1);
	}
	SECTION("vectorEmptyAfterClear") {
		auto f = engine.registerFunction(vectorEmptyAfterClear);
		REQUIRE(f() == true);
	}
	SECTION("vectorSize") {
		auto f = engine.registerFunction(vectorSize);
		REQUIRE(f(1) == 3);
	}
	SECTION("vectorReserveCapacity") {
		auto f = engine.registerFunction(vectorReserveCapacity);
		REQUIRE(f() == true);
	}

	// Modifiers
	SECTION("vectorClear") {
		auto f = engine.registerFunction(vectorClear);
		REQUIRE(f(7) == 99);
	}
	SECTION("vectorPopBack") {
		auto f = engine.registerFunction(vectorPopBack);
		REQUIRE(f() == 2);
	}
	SECTION("vectorResizeGrow") {
		auto f = engine.registerFunction(vectorResizeGrow);
		REQUIRE(f() == 5);
	}
	SECTION("vectorResizeGrowReadsDefault") {
		auto f = engine.registerFunction(vectorResizeGrowReadsDefault);
		REQUIRE(f() == 0);
	}
	SECTION("vectorResizeShrink") {
		auto f = engine.registerFunction(vectorResizeShrink);
		REQUIRE(f() == 2);
	}
	SECTION("vectorResizeWithValue") {
		auto f = engine.registerFunction(vectorResizeWithValue);
		REQUIRE(f(7) == 21);
	}

	// Ownership & interop
	SECTION("vectorReturnPtr") {
		auto f = engine.registerFunction(vectorReturnPtr);
		auto* vec = f(42);
		REQUIRE(vec->size() == 2);
		REQUIRE((*vec)[0] == 42);
		REQUIRE((*vec)[1] == 100);
		delete vec;
	}
	SECTION("vectorFromPtr") {
		auto f = engine.registerFunction(vectorFromPtr);
		auto vec = std::vector<int32_t> {5, 15};
		REQUIRE(f(&vec) == 20);
	}

	// Assignment
	SECTION("vectorAssign") {
		auto f = engine.registerFunction(vectorAssign);
		REQUIRE(f(10) == 30);
	}

	// Different element types
	SECTION("vectorDouble") {
		auto f = engine.registerFunction(vectorDouble);
		REQUIRE(f(1.0) == Catch::Approx(4.14));
	}
	SECTION("vectorInt64") {
		auto f = engine.registerFunction(vectorInt64);
		REQUIRE(f(42LL) == 1000000000042LL);
	}
	SECTION("vectorFloat") {
		auto f = engine.registerFunction(vectorFloat);
		REQUIRE(f(1.5f) == Catch::Approx(4.0f));
	}
	SECTION("vectorInt8") {
		auto f = engine.registerFunction(vectorInt8);
		REQUIRE(f(static_cast<int8_t>(5)) == 15);
	}

	// Combined operations
	SECTION("vectorBuildAndSum") {
		auto f = engine.registerFunction(vectorBuildAndSum);
		// x=10: 10+11+12+13 = 46
		REQUIRE(f(10) == 46);
	}
	SECTION("vectorPopToEmpty") {
		auto f = engine.registerFunction(vectorPopToEmpty);
		REQUIRE(f() == 0);
	}
	SECTION("vectorClearAndRefill") {
		auto f = engine.registerFunction(vectorClearAndRefill);
		REQUIRE(f(55) == 55);
	}
}

TEST_CASE("VectorTest - Interpreter") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	runVectorTest(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("VectorTest - Compiler") {
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
				runVectorTest(engine);
			}
		}
	}
}
#endif
} // namespace nautilus::engine
