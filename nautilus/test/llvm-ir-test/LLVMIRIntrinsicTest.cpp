#include "LLVMIRTestUtil.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <filesystem>
#include <string>

// Include test function headers
#include "../common/ProfileFunctions.hpp"

namespace nautilus::engine {

// Import profile functions from nautilus::engine namespace
using nautilus::engine::assumeAlignedFunction;
using nautilus::engine::assumeComplexCondition;
using nautilus::engine::assumeFunction;

// Wrapper for testLLVMIR that uses the intrinsic-ir directory
template <typename Func>
void testIntrinsicLLVMIR(const std::string& functionName, Func func, bool enableIntrinsics) {
	std::filesystem::path testFile(__FILE__);
	std::filesystem::path referenceIRDir = testFile.parent_path() / "intrinsic-ir";
	nautilus::engine::testLLVMIR(functionName, func, enableIntrinsics, referenceIRDir);
}

// ============================================================================
// Profile Tests - Assume Intrinsics with MLIR Intrinsics Enabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: assumeFunction (MLIR intrinsics enabled)", "[profile][assume][intrinsics]") {
	testIntrinsicLLVMIR("assumeFunction_with_intrinsics", assumeFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: assumeComplexCondition (MLIR intrinsics enabled)", "[profile][assume][intrinsics]") {
	testIntrinsicLLVMIR("assumeComplexCondition_with_intrinsics", assumeComplexCondition, true);
}

TEST_CASE("LLVM IR Intrinsic Test: assumeAlignedFunction (MLIR intrinsics enabled)", "[profile][assume][intrinsics]") {
	testIntrinsicLLVMIR("assumeAlignedFunction_with_intrinsics", assumeAlignedFunction, true);
}

// ============================================================================
// Profile Tests - Assume Intrinsics with MLIR Intrinsics Disabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: assumeFunction (MLIR intrinsics disabled)", "[profile][assume][no-intrinsics]") {
	testIntrinsicLLVMIR("assumeFunction_without_intrinsics", assumeFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: assumeComplexCondition (MLIR intrinsics disabled)",
          "[profile][assume][no-intrinsics]") {
	testIntrinsicLLVMIR("assumeComplexCondition_without_intrinsics", assumeComplexCondition, false);
}

TEST_CASE("LLVM IR Intrinsic Test: assumeAlignedFunction (MLIR intrinsics disabled)",
          "[profile][assume][no-intrinsics]") {
	testIntrinsicLLVMIR("assumeAlignedFunction_without_intrinsics", assumeAlignedFunction, false);
}

} // namespace nautilus::engine
