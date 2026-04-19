#include "ExecutionTest.hpp"
#include "RunctimeCallFunctions.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)
#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"
#include "nautilus/inline.hpp"
#endif

namespace nautilus::engine {

TEST_CASE("Inlining plugin: linkable", "[inlining][smoke]") {
	SUCCEED("nautilus-inlining plugin is built and linkable");
}

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)

// Static anchors so the NAUTILUS_INLINE-annotated helper functions referenced
// by the test body are actually emitted into the test binary (and thus end
// up in the InlineFunctionRegistry at program startup when the Clang pass
// plugin runs over the plugin test TU).
[[maybe_unused]] static auto* anchor_add = &add;
[[maybe_unused]] static auto* anchor_sub = &sub;
[[maybe_unused]] static Derived d;

// Defense-in-depth smoke test: if inlining is actually active, the Clang
// pass plugin registered bitcode for the annotated helpers at program load
// time and the file-scope InliningPluginRegistrar installed closures into
// the core LLVMBackendHooks. Verify both.
TEST_CASE("Inlining plugin installs backend hooks", "[inlining][smoke]") {
	const auto& hooks = nautilus::compiler::mlir::getLLVMBackendHooks();
	REQUIRE(static_cast<bool>(hooks.preOptModuleTransform));
	REQUIRE(static_cast<bool>(hooks.jitSymbolContributor));
	REQUIRE(static_cast<bool>(hooks.proxyCallNameOverride));
}

TEST_CASE("InlineFunctionRegistry contains annotated helpers", "[inlining][smoke]") {
	// Reference the helpers so they are definitely instantiated in this TU
	// and the Clang pass finds them during compilation of the test binary.
	REQUIRE(add(1, 2) == 3);
	REQUIRE(sub(5, 2) == 3);
	// When the pass ran, it populated the registry with bitcode for these
	// helpers — look them up by address.
	const bool haveAddBitcode =
	    InlineFunctionRegistry::instance().containsFunctionBitcode(reinterpret_cast<void*>(&add));
	const bool haveSubBitcode =
	    InlineFunctionRegistry::instance().containsFunctionBitcode(reinterpret_cast<void*>(&sub));
	// These must be true on hosts where the pass is supported (Clang 19 on
	// non-ARM). On other hosts, the test binary is still built and run but
	// the registry will be empty. Only assert strictly when we know inlining
	// is active.
	if (haveAddBitcode) {
		REQUIRE(haveSubBitcode);
	}
}

// Drives the annotated helper functions through the engine, exercising the
// full JIT-time inlining path against the plugin's own copies of the common
// RunctimeCallFunctions.hpp helpers (which carry NAUTILUS_INLINE).
static void inlinedFunctionCallTests(engine::NautilusEngine& engine) {
	SECTION("simpleDirectCall") {
		auto f = engine.registerFunction(simpleDirectCall);
		REQUIRE(f(10, 10) == 20);
		REQUIRE(f(0, 1) == 1);
	}

	SECTION("dynCastCall") {
		auto f = engine.registerFunction(dynCastCall);
		REQUIRE(f(&d) == 0);
	}

	SECTION("directCallWithNestedCalls") {
		auto f = engine.registerFunction(directCallWithNestedCalls);
		REQUIRE(f(10, 10) == 20);
		REQUIRE(f(0, 1) == 0);
	}

	SECTION("directCallComplexFunction") {
		auto f = engine.registerFunction(directCallComplexFunction);
		REQUIRE(f(10, 10) == 0);
		REQUIRE(f(100, 100) == 1);
	}

	SECTION("twoDistinctFunctionCalls") {
		auto f = engine.registerFunction(callTwoFunctions);
		REQUIRE(f(10, 10) == 20);
		REQUIRE(f(0, 1) == 0);
	}

	SECTION("loopDirectCall") {
		auto f = engine.registerFunction(loopDirectCall);
		REQUIRE(f(10, 10) == 100);
		REQUIRE(f(1, 1) == 1);
		REQUIRE(f(0, 1) == 0);
	}

	SECTION("loopDirectCall2") {
		auto f = engine.registerFunction(loopDirectCall2);
		REQUIRE(f(10) == 420);
		REQUIRE(f(1) == 42);
		REQUIRE(f(0) == 0);
	}

	SECTION("voidCall") {
		auto f = engine.registerFunction(voidFuncCall);
		REQUIRE_NOTHROW(f(10, 10));
	}

	SECTION("callMemberFunction") {
		auto f = engine.registerFunction(callMemberFunction);
		auto c = Clazz();
		auto res = f(&c);
		REQUIRE(res == 42);
	}

	SECTION("countFuncCall") {
		auto f = engine.registerFunction(callCountFuncCall);
		REQUIRE(f(true) == 1);
		REQUIRE(f(false) == 2);
	}
}

TEST_CASE("Inlined Function Call Test") {
	// Cycle the constant-folding tracer off/on so we catch tracer-
	// induced interactions with the inlining plugin. See the comment
	// in AssumeExecutionTest.cpp for the broader rationale.
	for (bool ctrEnabled : nautilus::testing::constantTracerModes()) {
		DYNAMIC_SECTION((ctrEnabled ? "ctrOn" : "ctrOff")) {
			nautilus::testing::ConstantTracerFlagGuard guard(ctrEnabled);
			auto engine = nautilus::testing::makeEngine("mlir", [](engine::Options& opts) {
				opts.setOption("engine.Compilation", true);
				opts.setOption("mlir.enableMultithreading", false);
				opts.setOption("mlir.inline_invoke_calls", true);
			});
			inlinedFunctionCallTests(engine);
		}
	}
}

#endif

} // namespace nautilus::engine
