// LLVM IR comparison tests for the inlining plugin. These mirror the
// non-inlined reference IR tests in `nautilus/test/llvm-ir-test/` but compile
// the same set of helpers with `mlir.inline_invoke_calls=true`. The
// reference `.ll` files under `inlining-ir/` therefore show the annotated
// helper bodies inlined into `execute` rather than external
// `declare i32 @runtimeFuncN(i32, i32)` / `tail call @runtimeFuncN` sequences.
// That divergence is the whole point of the plugin — if any of these tests
// start matching the non-inlined core reference IR, the JIT-time inliner has
// silently regressed.

#include "InliningTestAnchors.hpp"
#include "LLVMIRTestUtil.hpp"
#include "common/RuntimeCallFunctions.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/inline.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <regex>
#include <string>

namespace nautilus::engine {

using nautilus::engine::add;
using nautilus::engine::callTwoFunctions;
using nautilus::engine::directCallWithNestedCalls;
using nautilus::engine::loopDirectCall;
using nautilus::engine::loopDirectCall2;
using nautilus::engine::simpleDirectCall;
using nautilus::engine::sub;

// Returns true if the Clang inlining pass plugin actually ran over the test
// binary at compile time and populated the registry with bitcode for the
// annotated helpers. On hosts where the pass is unsupported (e.g. non-Clang,
// Clang version outside the supported range, ARM), this returns false and
// the LLVM-IR-based tests skip gracefully so the reference files are never
// bootstrapped from non-inlined IR.
static bool inliningPassActive() {
	return InlineFunctionRegistry::instance().containsFunctionBitcode(reinterpret_cast<void*>(&add));
}

// RAII guard that cleans up a filesystem path on destruction. Using this
// rather than manual `remove_all` at test end ensures cleanup runs even if
// a REQUIRE aborts the test body.
namespace {
struct TempPathGuard {
	std::filesystem::path path;
	explicit TempPathGuard(std::filesystem::path p) : path(std::move(p)) {
		std::filesystem::remove_all(path);
		std::filesystem::create_directories(path);
	}
	~TempPathGuard() {
		std::error_code ec;
		std::filesystem::remove_all(path, ec);
	}
	TempPathGuard(const TempPathGuard&) = delete;
	TempPathGuard& operator=(const TempPathGuard&) = delete;
};

// Checks that the generated LLVM IR contains neither a `declare` of any
// `runtimeFuncN` (the opaque-proxy-call names the MLIR backend emits for
// unresolved helper calls) nor a call instruction targeting one. These
// regex patterns are stricter than substring matches: they tolerate
// differing return types and avoid false matches on tokens like
// `@runtimeFuncPrinterShim` that may appear in future backend changes.
void assertNoRuntimeFuncCall(const std::string& ir) {
	static const std::regex declarePattern(R"(\bdeclare\s+\S+\s+@runtimeFunc\w*)");
	static const std::regex callPattern(R"(\b(?:tail\s+)?call\s+\S+\s+@runtimeFunc\w*)");
	INFO("Unexpected @runtimeFunc reference left in IR after inlining");
	REQUIRE_FALSE(std::regex_search(ir, declarePattern));
	REQUIRE_FALSE(std::regex_search(ir, callPattern));
}

// Recompile a Nautilus-traced function with the inlining options that the
// reference-IR tests use, then read the generated LLVM IR back as a string
// so callers can run custom assertions over it. Cleans up the temp dump
// directory automatically via RAII.
template <typename Func>
std::string dumpInlinedIR(const std::string& label, Func func) {
	auto dumpPath = std::filesystem::temp_directory_path() / ("nautilus-inlining-ir-smoke-" + label);
	TempPathGuard guard(dumpPath);

	engine::Options options;
	options.setOption("engine.backend", "mlir");
	options.setOption("dump.after_llvm_generation", true);
	options.setOption("dump.all", true);
	options.setOption("dump.console", false);
	options.setOption("dump.path", dumpPath.string());
	options.setOption("engine.normalizeFunctionNames", true);
	options.setOption("mlir.enableIntrinsics", true);
	options.setOption("mlir.inline_invoke_calls", true);
	options.setOption("mlir.enableMultithreading", false);

	auto engine = engine::NautilusEngine(options);
	auto function = engine.registerFunction(func);
	auto generatedLLVMFile = function.getExecutable()->getGeneratedFile("after_llvm_generation");
	REQUIRE_FALSE(generatedLLVMFile.empty());

	std::ifstream in(std::string {generatedLLVMFile});
	REQUIRE(in.good());
	return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}
} // namespace

// Wrapper for testLLVMIR that drops the generated module into the plugin's
// own `inlining-ir/` reference directory and turns on JIT-time inlining via
// `mlir.inline_invoke_calls=true`.
template <typename Func>
void testInliningLLVMIR(const std::string& functionName, Func func) {
	if (!inliningPassActive()) {
		SKIP("Clang inlining pass plugin is not active on this host — skipping to avoid "
		     "bootstrapping non-inlined reference IR.");
	}
	std::filesystem::path testFile(__FILE__);
	std::filesystem::path referenceIRDir = testFile.parent_path() / "inlining-ir";
	nautilus::engine::testLLVMIR(functionName, func, /*enableIntrinsics=*/true, referenceIRDir,
	                             [](engine::Options& options) {
		                             options.setOption("mlir.inline_invoke_calls", true);
		                             options.setOption("mlir.enableMultithreading", false);
	                             });
}

TEST_CASE("Inlining LLVM IR: simpleDirectCall (inlined)", "[inlining][llvm-ir]") {
	testInliningLLVMIR("simpleDirectCall_inlined", simpleDirectCall);
}

TEST_CASE("Inlining LLVM IR: directCallWithNestedCalls (inlined)", "[inlining][llvm-ir]") {
	testInliningLLVMIR("directCallWithNestedCalls_inlined", directCallWithNestedCalls);
}

TEST_CASE("Inlining LLVM IR: callTwoFunctions (inlined)", "[inlining][llvm-ir]") {
	testInliningLLVMIR("callTwoFunctions_inlined", callTwoFunctions);
}

TEST_CASE("Inlining LLVM IR: loopDirectCall (inlined)", "[inlining][llvm-ir]") {
	testInliningLLVMIR("loopDirectCall_inlined", loopDirectCall);
}

TEST_CASE("Inlining LLVM IR: loopDirectCall2 (inlined)", "[inlining][llvm-ir]") {
	testInliningLLVMIR("loopDirectCall2_inlined", loopDirectCall2);
}

// End-to-end defense-in-depth: recompile each annotated helper with the
// same options that `testInliningLLVMIR` uses, then scan the dumped IR by
// hand to assert that no opaque `@runtimeFunc` proxy call survives. If the
// reference `.ll` files ever start matching the non-inlined variants, this
// test fails loudly rather than silently rubber-stamping a regression. In
// addition, for `simpleDirectCall` we assert the inlined `add` body made
// it into `execute` by looking for a raw `add` instruction.
TEST_CASE("Inlining LLVM IR: no runtimeFunc left after inlining", "[inlining][llvm-ir][smoke]") {
	if (!inliningPassActive()) {
		SKIP("Clang inlining pass plugin is not active on this host — cannot verify inlining.");
	}

	SECTION("simpleDirectCall") {
		auto ir = dumpInlinedIR("simpleDirectCall", simpleDirectCall);
		REQUIRE(ir.find("define") != std::string::npos);
		REQUIRE(ir.find("@execute") != std::string::npos);
		assertNoRuntimeFuncCall(ir);
		REQUIRE(ir.find("= add ") != std::string::npos); // inlined `add` body
	}
	SECTION("directCallWithNestedCalls") {
		assertNoRuntimeFuncCall(dumpInlinedIR("directCallWithNestedCalls", directCallWithNestedCalls));
	}
	SECTION("callTwoFunctions") {
		assertNoRuntimeFuncCall(dumpInlinedIR("callTwoFunctions", callTwoFunctions));
	}
	SECTION("loopDirectCall") {
		assertNoRuntimeFuncCall(dumpInlinedIR("loopDirectCall", loopDirectCall));
	}
	SECTION("loopDirectCall2") {
		assertNoRuntimeFuncCall(dumpInlinedIR("loopDirectCall2", loopDirectCall2));
	}
}

} // namespace nautilus::engine
