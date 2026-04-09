// LLVM IR comparison tests for the inlining plugin. These mirror the
// non-inlined reference IR tests in `nautilus/test/llvm-ir-test/` but compile
// the same set of helpers with `mlir.inline_invoke_calls=true`. The
// reference `.ll` files under `inlining-ir/` therefore show the annotated
// helper bodies inlined into `execute` rather than external
// `declare i32 @runtimeFuncN(i32, i32)` / `tail call @runtimeFuncN` sequences.
// That divergence is the whole point of the plugin — if any of these tests
// start matching the non-inlined core reference IR, the JIT-time inliner has
// silently regressed.

#include "LLVMIRTestUtil.hpp"
#include "common/RunctimeCallFunctions.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/inline.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

namespace nautilus::engine {

using nautilus::engine::add;
using nautilus::engine::callTwoFunctions;
using nautilus::engine::directCallWithNestedCalls;
using nautilus::engine::loopDirectCall;
using nautilus::engine::loopDirectCall2;
using nautilus::engine::simpleDirectCall;
using nautilus::engine::sub;

// Static anchors so the NAUTILUS_INLINE-annotated helper functions referenced
// by the test body are actually emitted into the test binary (and thus end up
// in the InlineFunctionRegistry at program startup when the Clang pass plugin
// runs over the plugin test TU).
[[maybe_unused]] static auto* anchor_add = &add;
[[maybe_unused]] static auto* anchor_sub = &sub;

// Returns true if the Clang inlining pass plugin actually ran over the test
// binary at compile time and populated the registry with bitcode for the
// annotated helpers. On hosts where the pass is unsupported (e.g. non-Clang,
// Clang version outside the supported range, ARM), this returns false and
// the LLVM-IR-based tests skip gracefully so the reference files are never
// bootstrapped from non-inlined IR.
static bool inliningPassActive() {
	return InlineFunctionRegistry::instance().containsFunctionBitcode(reinterpret_cast<void*>(&add));
}

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

// End-to-end defense-in-depth: recompile `simpleDirectCall` with the same
// options that `testInliningLLVMIR` uses, then scan the dumped IR by hand to
// assert that the annotated `add` helper was actually folded into `execute`.
// If the reference `.ll` files ever start matching the non-inlined variants,
// this test fails loudly rather than silently rubber-stamping a regression.
TEST_CASE("Inlining LLVM IR: generated IR does not contain runtimeFunc declare", "[inlining][llvm-ir][smoke]") {
	if (!inliningPassActive()) {
		SKIP("Clang inlining pass plugin is not active on this host — cannot verify inlining.");
	}

	std::string dumpPath =
	    (std::filesystem::temp_directory_path() / "nautilus-inlining-ir-smoke-simpleDirectCall").string();
	std::filesystem::remove_all(dumpPath);
	std::filesystem::create_directories(dumpPath);

	engine::Options options;
	options.setOption("engine.backend", "mlir");
	options.setOption("dump.after_llvm_generation", true);
	options.setOption("dump.all", true);
	options.setOption("dump.console", false);
	options.setOption("dump.path", dumpPath);
	options.setOption("engine.normalizeFunctionNames", true);
	options.setOption("mlir.enableIntrinsics", true);
	options.setOption("mlir.inline_invoke_calls", true);
	options.setOption("mlir.enableMultithreading", false);

	auto engine = engine::NautilusEngine(options);
	auto function = engine.registerFunction(simpleDirectCall);
	auto generatedLLVMFile = function.getExecutable()->getGeneratedFile("after_llvm_generation");
	REQUIRE(!generatedLLVMFile.empty());

	std::ifstream in(std::string {generatedLLVMFile});
	REQUIRE(in.good());
	std::string ir((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

	// After inlining, the `add` helper body must be merged into `execute`:
	// there should be no `declare i32 @runtimeFunc0(i32, i32)` left, and no
	// `call` / `tail call` to `@runtimeFunc0`. The IR must still define
	// `execute`, and it should contain an `add` instruction — that's the
	// inlined helper body.
	REQUIRE(ir.find("define") != std::string::npos);
	REQUIRE(ir.find("@execute") != std::string::npos);
	REQUIRE(ir.find("declare i32 @runtimeFunc") == std::string::npos);
	REQUIRE(ir.find("call i32 @runtimeFunc") == std::string::npos);
	REQUIRE(ir.find("= add ") != std::string::npos);

	std::filesystem::remove_all(dumpPath);
}

} // namespace nautilus::engine
