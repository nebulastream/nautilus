#include "ExecutionTest.hpp"
#include "FunctionPtrFunctions.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_func.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)

// Mirrors FunctionPtrExecutionTest.cpp from the core test tree, but drives
// the tests against the plugin-local annotated `FunctionPtrFunctions.hpp`
// and toggles `mlir.inline_invoke_calls=true` so the JIT-time inliner runs.
static void functionPtrTests(engine::NautilusEngine& engine) {
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

TEST_CASE("Function Ptr Inlining Test") {
	auto engine = nautilus::testing::makeEngine("mlir", [](engine::Options& opts) {
		opts.setOption("engine.Compilation", true);
		opts.setOption("mlir.enableMultithreading", false);
		opts.setOption("mlir.inline_invoke_calls", true);
	});
	functionPtrTests(engine);
}
#endif

} // namespace nautilus::engine
