#include "FunctionPtrFunctions.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_func.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

void functionPtrTests(engine::NautilusEngine& engine) {
	SECTION("fnPtrIsNull-null") {
		auto f = engine.registerFunction(fnPtrIsNull);
		REQUIRE(f(nullptr) == true);
	}

	SECTION("fnPtrIsNull-nonNull") {
		auto f = engine.registerFunction(fnPtrIsNull);
		REQUIRE(f(addFn) == false);
	}

	SECTION("fnPtrNotNull-null") {
		auto f = engine.registerFunction(fnPtrNotNull);
		REQUIRE(f(nullptr) == false);
	}

	SECTION("fnPtrNotNull-nonNull") {
		auto f = engine.registerFunction(fnPtrNotNull);
		REQUIRE(f(addFn) == true);
	}

	SECTION("fnPtrEquals-same") {
		auto f = engine.registerFunction(fnPtrEquals);
		REQUIRE(f(addFn, addFn) == true);
		REQUIRE(f(mulFn, mulFn) == true);
	}

	SECTION("fnPtrEquals-different") {
		auto f = engine.registerFunction(fnPtrEquals);
		REQUIRE(f(addFn, mulFn) == false);
	}

	SECTION("fnPtrNotEquals-different") {
		auto f = engine.registerFunction(fnPtrNotEquals);
		REQUIRE(f(addFn, mulFn) == true);
		REQUIRE(f(nullptr, addFn) == true);
	}

	SECTION("fnPtrNotEquals-same") {
		auto f = engine.registerFunction(fnPtrNotEquals);
		REQUIRE(f(addFn, addFn) == false);
	}

	SECTION("callThroughFnPtr-add") {
		auto f = engine.registerFunction(callThroughFnPtr);
		REQUIRE(f(addFn, (int32_t) 3, (int32_t) 4) == 7);
		REQUIRE(f(addFn, (int32_t) 0, (int32_t) 0) == 0);
		REQUIRE(f(addFn, (int32_t) -1, (int32_t) 1) == 0);
	}

	SECTION("callThroughFnPtr-mul") {
		auto f = engine.registerFunction(callThroughFnPtr);
		REQUIRE(f(mulFn, (int32_t) 3, (int32_t) 4) == 12);
		REQUIRE(f(mulFn, (int32_t) 0, (int32_t) 5) == 0);
		REQUIRE(f(mulFn, (int32_t) -2, (int32_t) 3) == -6);
	}

	SECTION("callVoidFnPtr") {
		auto f = engine.registerFunction(callVoidFnPtr);
		// returns first argument unchanged
		REQUIRE(f(voidFn, (int32_t) 7, (int32_t) 8) == 7);
		REQUIRE(f(voidFn, (int32_t) 0, (int32_t) 99) == 0);
	}

	SECTION("fnPtrToVoidPtr") {
		auto f = engine.registerFunction(fnPtrToVoidPtr);
		REQUIRE(f(addFn, (int32_t) 42) == 42);
		REQUIRE(f(mulFn, (int32_t) 5) == 5);
	}

	SECTION("fnPtrAsTypedArg") {
		auto f = engine.registerFunction(fnPtrAsTypedArg);
		REQUIRE(f(addFn, (int32_t) 10, (int32_t) 20) == 30);
		REQUIRE(f(mulFn, (int32_t) 3, (int32_t) 7) == 21);
	}

	SECTION("fnPtrCopyAndCall") {
		auto f = engine.registerFunction(fnPtrCopyAndCall);
		REQUIRE(f(addFn, (int32_t) 5, (int32_t) 6) == 11);
		REQUIRE(f(mulFn, (int32_t) 4, (int32_t) 3) == 12);
	}

	SECTION("fnPtrAssignAndCall") {
		auto f = engine.registerFunction(fnPtrAssignAndCall);
		// second fn ptr wins
		REQUIRE(f(addFn, mulFn, (int32_t) 3, (int32_t) 4) == 12);
		REQUIRE(f(mulFn, addFn, (int32_t) 3, (int32_t) 4) == 7);
	}

	SECTION("selectFnPtr-add") {
		auto f = engine.registerFunction(selectFnPtr);
		REQUIRE(f(true, (int32_t) 3, (int32_t) 4) == 7);
	}

	SECTION("selectFnPtr-mul") {
		auto f = engine.registerFunction(selectFnPtr);
		REQUIRE(f(false, (int32_t) 3, (int32_t) 4) == 12);
	}

	SECTION("fnPtrInLoop") {
		auto f = engine.registerFunction(fnPtrInLoop);
		// sum = 0 + x + x + x + x  for n=4 iterations using add
		REQUIRE(f(addFn, (int32_t) 5, (int32_t) 4) == 20);
		REQUIRE(f(addFn, (int32_t) 1, (int32_t) 10) == 10);
		REQUIRE(f(addFn, (int32_t) 0, (int32_t) 100) == 0);
	}

	SECTION("fnPtrNullBranch-null") {
		auto f = engine.registerFunction(fnPtrNullBranch);
		REQUIRE(f(nullptr, (int32_t) 5, (int32_t) 6) == 0);
	}

	SECTION("fnPtrNullBranch-nonNull") {
		auto f = engine.registerFunction(fnPtrNullBranch);
		REQUIRE(f(addFn, (int32_t) 5, (int32_t) 6) == 11);
		REQUIRE(f(mulFn, (int32_t) 3, (int32_t) 3) == 9);
	}

	SECTION("fnPtrFromLambda") {
		auto f = engine.registerFunction(fnPtrFromLambda);
		REQUIRE(f((int32_t) 4) == 12);
		REQUIRE(f((int32_t) 0) == 0);
		REQUIRE(f((int32_t) -2) == -6);
	}

	SECTION("fnPtrRoundtripVoidPtr") {
		auto f = engine.registerFunction(fnPtrRoundtripVoidPtr);
		REQUIRE(f(addFn, (int32_t) 5, (int32_t) 7) == 12);
		REQUIRE(f(mulFn, (int32_t) 4, (int32_t) 3) == 12);
	}

	SECTION("fnPtrInlineConst") {
		auto f = engine.registerFunction(fnPtrInlineConst);
		REQUIRE(f((int32_t) 3, (int32_t) 4) == 7);
		REQUIRE(f((int32_t) -1, (int32_t) 1) == 0);
		REQUIRE(f((int32_t) 0, (int32_t) 0) == 0);
	}

	SECTION("fnPtrInlineConstUnary") {
		auto f = engine.registerFunction(fnPtrInlineConstUnary);
		REQUIRE(f((int32_t) 5) == -5);
		REQUIRE(f((int32_t) 0) == 0);
		REQUIRE(f((int32_t) -3) == 3);
	}
}

TEST_CASE("Function Ptr Interpreter Test") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	functionPtrTests(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("Function Ptr Compiler Test") {
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
				if (backend == "mlir") {
					options.setOption("engine.Compilation", true);
					options.setOption("mlir.enableMultithreading", false);
					options.setOption("mlir.inline_invoke_calls", true);
				}
				auto engine = engine::NautilusEngine(options);
				functionPtrTests(engine);
			}
		}
	}
}
#endif

} // namespace nautilus::engine
