#include "LLVMIRTestUtil.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace nautilus::engine {

// Simple function defined locally to avoid ODR violations with LLVMIRTest.cpp
static val<int32_t> debugTestAddOne(val<int32_t> x) {
	return x + val<int32_t>(1);
}

static val<int32_t> debugTestMul(val<int32_t> x, val<int32_t> y) {
	return x * y;
}

static val<int32_t> debugTestIfElse(val<int32_t> x, val<int32_t> threshold) {
	if (x > threshold) {
		return x - threshold;
	} else {
		return threshold - x;
	}
}

/**
 * Compile a function with debug.enabled=true and return the generated LLVM IR as a string.
 */
template <typename Func>
std::string compileWithDebugAndGetIR(const std::string& functionName, Func func) {
	std::string dumpPath =
	    (std::filesystem::temp_directory_path() / ("nautilus-debug-ir-test-" + functionName)).string();
	std::filesystem::remove_all(dumpPath);
	std::filesystem::create_directories(dumpPath);

	engine::Options options;
	options.setOption("engine.backend", "mlir");
	options.setOption("dump.after_llvm_generation", true);
	options.setOption("dump.path", dumpPath);
	options.setOption("engine.normalizeFunctionNames", true);
	options.setOption("debug.enabled", true);

	auto engine = engine::NautilusEngine(options);
	auto function = engine.registerFunction(func);

	auto generatedLLVMFile = function.getExecutable()->getGeneratedFile("after_llvm_generation");
	REQUIRE(!generatedLLVMFile.empty());

	std::string generatedLLVMFilePath {generatedLLVMFile};
	std::ifstream file(generatedLLVMFilePath);
	REQUIRE(file.is_open());
	std::ostringstream ss;
	ss << file.rdbuf();
	std::string ir = ss.str();

	std::filesystem::remove_all(dumpPath);
	return ir;
}

/**
 * @brief Compare generated LLVM IR (with debug.enabled=true) against a reference file.
 *
 * Follows the same pattern as testLLVMIR() in LLVMIRTestUtil.hpp but enables
 * debug.enabled=true so the MLIR pipeline emits DWARF debug metadata.
 * Reference files are stored in debug-ir/ next to the other reference-ir/ files.
 * If the reference does not exist it is auto-generated on the first run.
 */
template <typename Func>
void testLLVMIRDebug(const std::string& functionName, Func func) {
	std::filesystem::path testFile(__FILE__);
	std::filesystem::path referenceIRDir = testFile.parent_path() / "debug-ir";

	std::string dumpPath =
	    (std::filesystem::temp_directory_path() / ("nautilus-debug-ir-test-" + functionName)).string();
	std::filesystem::remove_all(dumpPath);
	std::filesystem::create_directories(dumpPath);

	engine::Options options;
	options.setOption("engine.backend", "mlir");
	options.setOption("dump.after_llvm_generation", true);
	options.setOption("dump.path", dumpPath);
	options.setOption("engine.normalizeFunctionNames", true);
	options.setOption("debug.enabled", true);

	auto engine = engine::NautilusEngine(options);
	auto function = engine.registerFunction(func);

	auto generatedLLVMFile = function.getExecutable()->getGeneratedFile("after_llvm_generation");
	REQUIRE(!generatedLLVMFile.empty());

	std::string referenceIRPath = (referenceIRDir / (functionName + ".ll")).string();

	// Normalization is done in two piped sed passes so that order is correct:
	// Pass 1: strip machine-specific attributes (target-cpu, target-features, …)
	//         so that attribute groups may become empty.
	// Pass 2: delete empty attribute groups + their #N annotations, and strip
	//         #dbg_value records (LLVM 20+ syntax that llvm-diff 18/19 rejects).
	// Debug variable presence is verified separately by the pattern tests above.
	std::string pass1 = "sed -E 's/ \"target-cpu\"=\"[^\"]+\"//g; "
	                    "s/ \"target-features\"=\"[^\"]+\"//g; "
	                    "s/ \"tune-cpu\"=\"[^\"]+\"//g; "
	                    "s/ captures\\([^)]*\\)//g; "
	                    "s/, errnomem: [^,)]+//g; "
	                    "s/ range\\([^)]+\\)//g'";
	std::string pass2 = "sed -E '/^[[:space:]]*#dbg_/d; "
	                    "/^attributes #[0-9]+ = \\{ \\}$/d; "
	                    "s/ #[0-9]+([[:space:]]|$)/\\1/g'";

	auto normalize = [&](const std::string& src, const std::string& dst) {
		std::string cmd = pass1 + " " + src + " | " + pass2 + " > " + dst;
		std::system(cmd.c_str());
	};

	if (!std::filesystem::exists(referenceIRPath)) {
		std::filesystem::create_directories(referenceIRDir);
		normalize(std::string(generatedLLVMFile), referenceIRPath);
		std::filesystem::remove_all(dumpPath);
		return;
	}

	std::string tempGenFile = dumpPath + "/" + functionName + "_gen_normalized.ll";
	std::string tempRefFile = dumpPath + "/" + functionName + "_ref_normalized.ll";

	normalize(std::string(generatedLLVMFile), tempGenFile);
	normalize(referenceIRPath, tempRefFile);

	std::string llvmDiff = "llvm-diff-21";
	if (std::system("which llvm-diff-21 > /dev/null 2>&1") != 0) {
		llvmDiff = "llvm-diff";
	}
	int result = std::system((llvmDiff + " " + tempGenFile + " " + tempRefFile).c_str());

	std::filesystem::remove_all(dumpPath);
	REQUIRE(result == 0);
}

// ============================================================================
// Pattern-match tests (DWARF metadata presence)
// ============================================================================

TEST_CASE("LLVM IR Debug: debugTestAddOne contains DICompileUnit", "[debug][dwarf]") {
	auto ir = compileWithDebugAndGetIR("debugTestAddOne", debugTestAddOne);
	INFO("Generated IR:\n" << ir);
	CHECK(ir.find("DICompileUnit") != std::string::npos);
}

TEST_CASE("LLVM IR Debug: debugTestAddOne contains DISubprogram", "[debug][dwarf]") {
	auto ir = compileWithDebugAndGetIR("debugTestAddOne", debugTestAddOne);
	INFO("Generated IR:\n" << ir);
	CHECK(ir.find("DISubprogram") != std::string::npos);
}

TEST_CASE("LLVM IR Debug: debugTestAddOne contains DIFile", "[debug][dwarf]") {
	auto ir = compileWithDebugAndGetIR("debugTestAddOne", debugTestAddOne);
	INFO("Generated IR:\n" << ir);
	CHECK(ir.find("DIFile") != std::string::npos);
}

TEST_CASE("LLVM IR Debug: debugTestAddOne contains dbg metadata on instructions", "[debug][dwarf]") {
	auto ir = compileWithDebugAndGetIR("debugTestAddOne", debugTestAddOne);
	INFO("Generated IR:\n" << ir);
	// Every instruction should carry a !dbg reference
	CHECK(ir.find("!dbg") != std::string::npos);
}

TEST_CASE("LLVM IR Debug: debugTestAddOne contains debug variable records", "[debug][dwarf]") {
	auto ir = compileWithDebugAndGetIR("debugTestAddOne", debugTestAddOne);
	INFO("Generated IR:\n" << ir);
	// LLVM 20+ uses #dbg_value (new debug record format); older LLVM uses llvm.dbg.value calls.
	bool hasNewFormat = ir.find("#dbg_value") != std::string::npos;
	bool hasOldFormat = ir.find("llvm.dbg.value") != std::string::npos;
	CHECK((hasNewFormat || hasOldFormat));
}

TEST_CASE("LLVM IR Debug: debugTestAddOne contains DILocalVariable", "[debug][dwarf]") {
	auto ir = compileWithDebugAndGetIR("debugTestAddOne", debugTestAddOne);
	INFO("Generated IR:\n" << ir);
	CHECK(ir.find("DILocalVariable") != std::string::npos);
}

// ============================================================================
// Golden-file reference IR comparison tests
// ============================================================================

TEST_CASE("LLVM IR Debug reference: debugTestAddOne", "[debug][reference]") {
	testLLVMIRDebug("debugTestAddOne", debugTestAddOne);
}

TEST_CASE("LLVM IR Debug reference: debugTestMul", "[debug][reference]") {
	testLLVMIRDebug("debugTestMul", debugTestMul);
}

TEST_CASE("LLVM IR Debug reference: debugTestIfElse", "[debug][reference]") {
	testLLVMIRDebug("debugTestIfElse", debugTestIfElse);
}

} // namespace nautilus::engine
