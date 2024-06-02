
#include "nautilus/Engine.hpp"
#include "nautilus/std/cmath.h"
#include "nautilus/std/cstdlib.h"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

template <typename T>
val<T> absTest(val<T> x) {
	return abs(x);
}

template <typename T>
val<T> sinTest(val<T> x) {
	return sin(x);
}

auto getInt(val<char*> str) {
	return atoi(str);
}

auto getIntStatic() {
	return atoi("42");
}

void cstdlib(engine::NautilusEngine& engine) {
	SECTION("atoi") {
		auto f = engine.registerFunction(getInt);
		REQUIRE(f("42") == 42);
		REQUIRE(f("0") == 0);
	}
	//SECTION("atoi-const") {
	//	auto f = engine.registerFunction(getIntStatic);
	//	REQUIRE(f() == 42);
	//}
}

void cmath(engine::NautilusEngine& engine) {
	SECTION("abs") {
		SECTION("float") {
			auto f = engine.registerFunction(absTest<float>);
			REQUIRE(f(1.0f) == 1.f);
			REQUIRE(f(0.f) == 0.f);
			REQUIRE(f(-1.f) == 1.f);
			REQUIRE(f(-1.f) == 1.f);
			REQUIRE(f(INFINITY) == INFINITY);
		}
		SECTION("double") {
			auto d = engine.registerFunction(absTest<double>);
			REQUIRE(d(1.0) == 1.);
			REQUIRE(d(0.) == 0.);
			REQUIRE(d(-1.) == 1.);
			REQUIRE(d(-1.) == 1.);
			REQUIRE(d((double) INFINITY) == (double) INFINITY);
		}
	}
	SECTION("sin") {
		SECTION("float") {
			auto f = engine.registerFunction(sinTest<float>);
			REQUIRE(f(1.0f) == std::sin(1.0f));
			REQUIRE(f(0.f) == std::sin(0.f));
			REQUIRE(f(-1.f) == std::sin(-1.f));
			REQUIRE(std::isnan(f(INFINITY)));
		}
		SECTION("double") {
			auto d = engine.registerFunction(sinTest<double>);
			REQUIRE(d(1.0) == std::sin(1.0));
			REQUIRE(d(0.) == std::sin(0.));
			REQUIRE(d(-1.0) == std::sin(-1.0));
			REQUIRE(std::isnan(d((double) INFINITY)));
		}
	}
}

void runAllTests(engine::NautilusEngine& engine) {
	SECTION("cstdlib") {
		cstdlib(engine);
	}
	SECTION("cmatch") {
		cmath(engine);
	}
}

TEST_CASE("STD Proxy Test - Interpreter") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	runAllTests(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("STD Proxy Test - Compiler") {
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
			runAllTests(engine);
		}
	}
}
#endif
} // namespace nautilus::engine
