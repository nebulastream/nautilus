#include "LLVMIRTestUtil.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>

// Include test function headers
#include "../common/VariableNamesFunctions.hpp"

namespace nautilus::engine {

// Wrapper for testLLVMIR that uses the default reference-ir directory and enables intrinsics
template <typename Func>
void testLLVMIR(const std::string& functionName, Func func) {
	std::filesystem::path testFile(__FILE__);
	std::filesystem::path referenceIRDir = testFile.parent_path() / "reference-ir";
	nautilus::engine::testLLVMIR(functionName, func, true, referenceIRDir);
}

// ============================================================================
// Variable Names and Source Location Tests
// ============================================================================

TEST_CASE("LLVM IR Test: addWithSemanticNames", "[variable-names][basic]") {
	testLLVMIR("addWithSemanticNames", addWithSemanticNames);
}

TEST_CASE("LLVM IR Test: multiplyWithLocationMacro", "[variable-names][macro]") {
	testLLVMIR("multiplyWithLocationMacro", multiplyWithLocationMacro);
}

TEST_CASE("LLVM IR Test: chainedOperationsWithNames", "[variable-names][chained]") {
	testLLVMIR("chainedOperationsWithNames", chainedOperationsWithNames);
}

TEST_CASE("LLVM IR Test: comparisonWithLocationInfo", "[variable-names][comparison]") {
	testLLVMIR("comparisonWithLocationInfo", comparisonWithLocationInfo);
}

TEST_CASE("LLVM IR Test: floatOperationsWithNames", "[variable-names][float]") {
	testLLVMIR("floatOperationsWithNames", floatOperationsWithNames);
}

TEST_CASE("LLVM IR Test: booleanLogicWithNames", "[variable-names][boolean]") {
	testLLVMIR("booleanLogicWithNames", booleanLogicWithNames);
}

TEST_CASE("LLVM IR Test: divisionWithLocationInfo", "[variable-names][division]") {
	testLLVMIR("divisionWithLocationInfo", divisionWithLocationInfo);
}

TEST_CASE("LLVM IR Test: moduloWithSemanticNames", "[variable-names][modulo]") {
	testLLVMIR("moduloWithSemanticNames", moduloWithSemanticNames);
}

TEST_CASE("LLVM IR Test: bitwiseOperationsWithNames", "[variable-names][bitwise]") {
	testLLVMIR("bitwiseOperationsWithNames", bitwiseOperationsWithNames);
}

} // namespace nautilus::engine
