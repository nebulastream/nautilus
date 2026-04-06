
#include "nautilus/Engine.hpp"
#include "nautilus/std/vector.h"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

val<int32_t> vectorPushBackAndRead(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(x);
	vec.push_back(val<int32_t>(42));
	return vec[val<size_t>(0)] + vec[val<size_t>(1)];
}

val<size_t> vectorSize(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(x);
	vec.push_back(val<int32_t>(2));
	vec.push_back(val<int32_t>(3));
	return vec.size();
}

val<bool> vectorEmpty() {
	val<std::vector<int32_t>> vec;
	return vec.empty();
}

val<int32_t> vectorFrontBack(val<int32_t> x) {
	val<std::vector<int32_t>> vec;
	vec.push_back(val<int32_t>(10));
	vec.push_back(x);
	vec.push_back(val<int32_t>(30));
	return vec.front() + vec.back();
}

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

val<double> vectorDouble(val<double> x) {
	val<std::vector<double>> vec;
	vec.push_back(x);
	vec.push_back(val<double>(3.14));
	return vec[val<size_t>(0)] + vec[val<size_t>(1)];
}

void runVectorTest(engine::NautilusEngine& engine) {
	SECTION("vectorPushBackAndRead") {
		auto f = engine.registerFunction(vectorPushBackAndRead);
		REQUIRE(f(10) == 52);
	}
	SECTION("vectorSize") {
		auto f = engine.registerFunction(vectorSize);
		REQUIRE(f(1) == 3);
	}
	SECTION("vectorEmpty") {
		auto f = engine.registerFunction(vectorEmpty);
		REQUIRE(f() == true);
	}
	SECTION("vectorFrontBack") {
		auto f = engine.registerFunction(vectorFrontBack);
		REQUIRE(f(20) == 40);
	}
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
	SECTION("vectorClear") {
		auto f = engine.registerFunction(vectorClear);
		REQUIRE(f(7) == 99);
	}
	SECTION("vectorPopBack") {
		auto f = engine.registerFunction(vectorPopBack);
		REQUIRE(f() == 2);
	}
	SECTION("vectorDouble") {
		auto f = engine.registerFunction(vectorDouble);
		REQUIRE(f(1.0) == Catch::Approx(4.14));
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
