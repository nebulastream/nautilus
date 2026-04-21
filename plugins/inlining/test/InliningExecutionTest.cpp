#include "ExecutionTest.hpp"
#include "RuntimeCallFunctions.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)
#include "InliningTestAnchors.hpp"
#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"
#include "nautilus/inline.hpp"
#endif

namespace nautilus::engine {

TEST_CASE("Inlining plugin: linkable", "[inlining][smoke]") {
	SUCCEED("nautilus-inlining plugin is built and linkable");
}

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)

// A host is expected to actually exercise the inlining pass when:
//   1. the compiler is Clang in the supported 19-21 range, AND
//   2. the host is not ARM/aarch64.
// On such hosts the pass plugin is always loaded via `nautilus_inline()`
// in the test CMakeLists, so the registry MUST be populated by the time
// the test binary starts running. On any other host we expect the
// registry to be empty. Historically we conditionally asserted "sub is
// registered IFF add is registered", which allowed a silent-skip failure
// mode where the pass ran but added nothing.
#if defined(__clang__) && __clang_major__ >= 19 && __clang_major__ < 22 && !defined(__aarch64__) && !defined(__arm__)
#define NAUTILUS_INLINING_EXPECTED_ACTIVE 1
#else
#define NAUTILUS_INLINING_EXPECTED_ACTIVE 0
#endif

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
	const bool haveAddBitcode =
	    InlineFunctionRegistry::instance().containsFunctionBitcode(reinterpret_cast<void*>(&add));
	const bool haveSubBitcode =
	    InlineFunctionRegistry::instance().containsFunctionBitcode(reinterpret_cast<void*>(&sub));
#if NAUTILUS_INLINING_EXPECTED_ACTIVE
	// On a supported toolchain both must be present — a partial registry
	// indicates the pass ran but dropped entries silently.
	REQUIRE(haveAddBitcode);
	REQUIRE(haveSubBitcode);
#else
	// On other hosts the pass didn't run, and the registry is expected to
	// be empty for these helpers. Don't require emptiness though — a user
	// could have linked in a supported-toolchain archive.
	(void) haveAddBitcode;
	(void) haveSubBitcode;
#endif
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
		REQUIRE(f(&detail::inlining_anchor_derived) == 0);
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
	auto engine = nautilus::testing::makeEngine("mlir", [](engine::Options& opts) {
		opts.setOption("engine.Compilation", true);
		opts.setOption("mlir.enableMultithreading", false);
		opts.setOption("mlir.inline_invoke_calls", true);
	});
	inlinedFunctionCallTests(engine);
}

// --------------------------------------------------------------------------
// Additional coverage tests added in the plugin hardening pass.
// --------------------------------------------------------------------------

// The `proxyCallNameOverride` hook installed by the inlining plugin must
// produce a distinct, hex-formatted name per input pointer, and must return
// nullopt for pointers whose bitcode isn't registered. This is the contract
// the MLIR backend relies on when deciding whether to route a call through
// the JIT-time inliner.
TEST_CASE("proxyCallNameOverride hook returns stable hex names", "[inlining][unit]") {
	const auto& hooks = nautilus::compiler::mlir::getLLVMBackendHooks();
	REQUIRE(static_cast<bool>(hooks.proxyCallNameOverride));

	// An on-stack object cannot be a registered function pointer, so the
	// hook must decline to rename it.
	int localStackObject = 0;
	void* unknownPtr = reinterpret_cast<void*>(&localStackObject);
	REQUIRE_FALSE(hooks.proxyCallNameOverride(unknownPtr).has_value());

#if NAUTILUS_INLINING_EXPECTED_ACTIVE
	// For a registered helper the hook must return a stable, non-empty
	// hex-formatted string that round-trips via repeated calls (i.e. the
	// name is deterministic for the same pointer — format drift between
	// call sites would break JIT inlining silently).
	void* addAddr = reinterpret_cast<void*>(&add);
	auto overriddenAdd = hooks.proxyCallNameOverride(addAddr);
	REQUIRE(overriddenAdd.has_value());
	REQUIRE(!overriddenAdd->empty());
	REQUIRE(overriddenAdd->rfind("0x", 0) == 0); // starts with "0x"
	REQUIRE(*overriddenAdd == *hooks.proxyCallNameOverride(addAddr));

	// Distinct registered functions must yield distinct names (address
	// uniqueness must survive the hex encoding).
	void* subAddr = reinterpret_cast<void*>(&sub);
	auto overriddenSub = hooks.proxyCallNameOverride(subAddr);
	REQUIRE(overriddenSub.has_value());
	REQUIRE(*overriddenAdd != *overriddenSub);
#endif
}

// The registry must distinguish "not registered" from "registered as empty
// bitcode" — previously both returned the same `static std::string empty`
// sentinel, which masked genuine miss cases. An unknown pointer must yield
// `std::nullopt`.
TEST_CASE("InlineFunctionRegistry reports misses distinctly", "[inlining][unit]") {
	// Use a stack address that will never be a registered function pointer.
	int localStackObject = 0;
	void* unknownPtr = reinterpret_cast<void*>(&localStackObject);
	const auto bitcode = InlineFunctionRegistry::instance().getBitcode(unknownPtr);
	REQUIRE_FALSE(bitcode.has_value());
	REQUIRE_FALSE(InlineFunctionRegistry::instance().containsFunctionBitcode(unknownPtr));
}

// When `mlir.inline_invoke_calls` is disabled, the engine must still be
// able to compile and execute the same traced functions — falling back to
// the opaque proxy-call path. This is the path that H2 (replacing the
// throw with a skip) protects against: even on the inlining path, any
// per-candidate failure should degrade to this same behaviour.
TEST_CASE("Inlining disabled path still executes correctly", "[inlining][unit]") {
	auto engine = nautilus::testing::makeEngine("mlir", [](engine::Options& opts) {
		opts.setOption("engine.Compilation", true);
		opts.setOption("mlir.enableMultithreading", false);
		opts.setOption("mlir.inline_invoke_calls", false);
	});
	auto f = engine.registerFunction(simpleDirectCall);
	REQUIRE(f(10, 10) == 20);
	REQUIRE(f(1, 2) == 3);
}

#endif

} // namespace nautilus::engine
