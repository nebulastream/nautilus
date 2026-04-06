#include "LLVMIRTestUtil.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <filesystem>
#include <string>

// Include test function headers
#include "../common/MemoryIntrinsicFunctions.hpp"
#include "../common/ProfileFunctions.hpp"

namespace nautilus::engine {

// Import profile functions from nautilus::engine namespace
using nautilus::engine::assumeAlignedFunction;
using nautilus::engine::assumeComplexCondition;
using nautilus::engine::assumeFunction;

// Import memory functions from nautilus::engine namespace
using nautilus::engine::memcpyBytesFunction;
using nautilus::engine::memcpyFunction;
using nautilus::engine::memmoveFunction;
using nautilus::engine::memmoveOverlapFunction;
using nautilus::engine::memsetFunction;
using nautilus::engine::memsetZeroFunction;

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

// ============================================================================
// Memory Tests - Memory Operations with MLIR Intrinsics Enabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: memcpyFunction (MLIR intrinsics enabled)", "[memory][memcpy][intrinsics]") {
	testIntrinsicLLVMIR("memcpyFunction_with_intrinsics", memcpyFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: memmoveFunction (MLIR intrinsics enabled)", "[memory][memmove][intrinsics]") {
	testIntrinsicLLVMIR("memmoveFunction_with_intrinsics", memmoveFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: memsetFunction (MLIR intrinsics enabled)", "[memory][memset][intrinsics]") {
	testIntrinsicLLVMIR("memsetFunction_with_intrinsics", memsetFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: memcpyBytesFunction (MLIR intrinsics enabled)", "[memory][memcpy][intrinsics]") {
	testIntrinsicLLVMIR("memcpyBytesFunction_with_intrinsics", memcpyBytesFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: memmoveOverlapFunction (MLIR intrinsics enabled)", "[memory][memmove][intrinsics]") {
	testIntrinsicLLVMIR("memmoveOverlapFunction_with_intrinsics", memmoveOverlapFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: memsetZeroFunction (MLIR intrinsics enabled)", "[memory][memset][intrinsics]") {
	testIntrinsicLLVMIR("memsetZeroFunction_with_intrinsics", memsetZeroFunction, true);
}

// ============================================================================
// Memory Tests - Memory Operations with MLIR Intrinsics Disabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: memcpyFunction (MLIR intrinsics disabled)", "[memory][memcpy][no-intrinsics]") {
	testIntrinsicLLVMIR("memcpyFunction_without_intrinsics", memcpyFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: memmoveFunction (MLIR intrinsics disabled)", "[memory][memmove][no-intrinsics]") {
	testIntrinsicLLVMIR("memmoveFunction_without_intrinsics", memmoveFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: memsetFunction (MLIR intrinsics disabled)", "[memory][memset][no-intrinsics]") {
	testIntrinsicLLVMIR("memsetFunction_without_intrinsics", memsetFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: memcpyBytesFunction (MLIR intrinsics disabled)", "[memory][memcpy][no-intrinsics]") {
	testIntrinsicLLVMIR("memcpyBytesFunction_without_intrinsics", memcpyBytesFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: memmoveOverlapFunction (MLIR intrinsics disabled)",
          "[memory][memmove][no-intrinsics]") {
	testIntrinsicLLVMIR("memmoveOverlapFunction_without_intrinsics", memmoveOverlapFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: memsetZeroFunction (MLIR intrinsics disabled)", "[memory][memset][no-intrinsics]") {
	testIntrinsicLLVMIR("memsetZeroFunction_without_intrinsics", memsetZeroFunction, false);
}

} // namespace nautilus::engine
