#include "catch2/catch_test_macros.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/nautilus_function.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

// --- Test functions ---

val<int32_t> addWithConstant(val<int32_t> a, val<int32_t> b) {
	return a + b;
}

// Specialize on arg 0 (first argument)
static auto specAddArg0 =
    SpecializableNautilusFunction<decltype(&addWithConstant), 0> {"specAdd_arg0", addWithConstant};

// Wrapper that calls the specializable function from within a traced context
val<int32_t> callSpecAddArg0(val<int32_t> a, val<int32_t> b) {
	return specAddArg0(a, b);
}

// Three-argument function with specialization on args 0 and 2
val<int32_t> threeArgFunc(val<int32_t> a, val<int32_t> b, val<int32_t> c) {
	return (a + b) * c;
}

static auto specThreeArgs =
    SpecializableNautilusFunction<decltype(&threeArgFunc), 0, 2> {"specThreeArgs", threeArgFunc};

val<int32_t> callSpecThreeArgs(val<int32_t> a, val<int32_t> b, val<int32_t> c) {
	return specThreeArgs(a, b, c);
}

// Lambda-based specializable function
static auto specLambda = SpecializableNautilusFunction<std::function<val<int32_t>(val<int32_t>, val<int32_t>)>, 1> {
    "specLambda", std::function<val<int32_t>(val<int32_t>, val<int32_t>)>(
                      [](val<int32_t> x, val<int32_t> y) -> val<int32_t> { return x * y; })};

val<int32_t> callSpecLambda(val<int32_t> a, val<int32_t> b) {
	return specLambda(a, b);
}

// --- Test cases ---

void specializationExecutionTests(engine::NautilusEngine& engine) {
	SECTION("specializeOnArg0") {
		// Calls callSpecAddArg0 which internally calls specAddArg0.
		// When traced, arg0 is constant (whatever value is passed).
		auto f = engine.registerFunction(callSpecAddArg0);
		REQUIRE(f(10, 5) == 15);
		REQUIRE(f(10, 3) == 13);
		REQUIRE(f(10, 0) == 10);
		REQUIRE(f(10, -1) == 9);
	}

	SECTION("specializeOnMultipleArgs") {
		// threeArgFunc: (a + b) * c, specialized on args 0 and 2
		auto f = engine.registerFunction(callSpecThreeArgs);
		REQUIRE(f(2, 3, 4) == 20); // (2+3)*4
		REQUIRE(f(2, 5, 4) == 28); // (2+5)*4
		REQUIRE(f(2, 0, 4) == 8);  // (2+0)*4
	}

	SECTION("specializeWithLambda") {
		// Lambda: x * y, specialized on arg 1
		auto f = engine.registerFunction(callSpecLambda);
		REQUIRE(f(3, 7) == 21);
		REQUIRE(f(5, 7) == 35);
		REQUIRE(f(0, 7) == 0);
	}

	SECTION("differentConstantValues") {
		// Test that different constant values produce correct results
		auto f = engine.registerFunction(callSpecAddArg0);
		REQUIRE(f(42, 1) == 43);
		REQUIRE(f(100, 1) == 101);
	}
}

TEST_CASE("SpecializableNautilusFunction Interpretation Test") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	specializationExecutionTests(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("SpecializableNautilusFunction Compiled Execution Test") {
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
	std::vector<std::string> traceModes = {"exceptionBasedTracing", "lazyTracing"};
	for (auto& backend : backends) {
		for (auto& traceMode : traceModes) {
			DYNAMIC_SECTION(backend + "_" + traceMode) {
				engine::Options options;
				options.setOption("engine.backend", backend);
				options.setOption("engine.traceMode", traceMode);
				if (backend == "mlir") {
					options.setOption("engine.Compilation", true);
					options.setOption("mlir.enableMultithreading", false);
				}
				auto engine = engine::NautilusEngine(options);
				specializationExecutionTests(engine);
			}
		}
	}
}
#endif

} // namespace nautilus::engine
