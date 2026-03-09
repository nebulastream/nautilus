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

	std::string generatedLLVMFilePath{generatedLLVMFile};
	std::ifstream file(generatedLLVMFilePath);
	REQUIRE(file.is_open());
	std::ostringstream ss;
	ss << file.rdbuf();
	std::string ir = ss.str();

	std::filesystem::remove_all(dumpPath);
	return ir;
}

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

} // namespace nautilus::engine
