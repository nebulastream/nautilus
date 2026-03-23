
#include "nautilus/Engine.hpp"
#include "nautilus/std/functional.h"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

val<size_t> hashInt32Test(val<int32_t> v) {
	return hash<int32_t>(v);
}

val<size_t> hashInt64Test(val<int64_t> v) {
	return hash<int64_t>(v);
}

val<size_t> hashDoubleTest(val<double> v) {
	return hash<double>(v);
}

val<size_t> hashCstrTest(val<const char*> s) {
	return hash_cstr(s);
}

void runFunctionalTests(engine::NautilusEngine& engine) {
	SECTION("hash_int32") {
		auto f = engine.registerFunction(hashInt32Test);
		// Hash should be deterministic
		REQUIRE(f(42) == f(42));
		REQUIRE(f(42) == std::hash<int32_t> {}(42));
		// Different values should (almost certainly) produce different hashes
		REQUIRE(f(0) != f(1));
	}
	SECTION("hash_int64") {
		auto f = engine.registerFunction(hashInt64Test);
		REQUIRE(f(42L) == std::hash<int64_t> {}(42L));
	}
	SECTION("hash_double") {
		auto f = engine.registerFunction(hashDoubleTest);
		REQUIRE(f(3.14) == std::hash<double> {}(3.14));
	}
	SECTION("hash_cstr") {
		auto f = engine.registerFunction(hashCstrTest);
		// Same string should produce same hash
		REQUIRE(f("hello") == f("hello"));
		// Different strings should (almost certainly) produce different hashes
		REQUIRE(f("hello") != f("world"));
	}
}

TEST_CASE("FunctionalTest - Interpreter") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	runFunctionalTests(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("FunctionalTest - Compiler") {
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
				runFunctionalTests(engine);
			}
		}
	}
}
#endif
} // namespace nautilus::engine
