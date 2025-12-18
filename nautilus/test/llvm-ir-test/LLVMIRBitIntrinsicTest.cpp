#include "LLVMIRTestUtil.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <filesystem>
#include <string>

// Include test function headers
#include "../common/BitIntrinsicFunctions.hpp"

namespace nautilus::engine {

// Import bit functions from nautilus::engine namespace
using nautilus::engine::byteswap32Function;
using nautilus::engine::byteswap64Function;
using nautilus::engine::countlZero64Function;
using nautilus::engine::countlZeroFunction;
using nautilus::engine::countrZero64Function;
using nautilus::engine::countrZeroFunction;
using nautilus::engine::popcount64Function;
using nautilus::engine::popcountFunction;
using nautilus::engine::rotl64Function;
using nautilus::engine::rotlFunction;
using nautilus::engine::rotr64Function;
using nautilus::engine::rotrFunction;

// Wrapper for testLLVMIR that uses the intrinsic-ir directory
template <typename Func>
void testBitIntrinsicLLVMIR(const std::string& functionName, Func func, bool enableIntrinsics) {
	std::filesystem::path testFile(__FILE__);
	std::filesystem::path referenceIRDir = testFile.parent_path() / "intrinsic-ir";
	nautilus::engine::testLLVMIR(functionName, func, enableIntrinsics, referenceIRDir);
}

// ============================================================================
// Bit Tests - Unary Operations with MLIR Intrinsics Enabled
// ============================================================================

TEST_CASE("LLVM IR Bit Intrinsic Test: countlZeroFunction (MLIR intrinsics enabled)",
          "[bit][countl_zero][intrinsics]") {
	testBitIntrinsicLLVMIR("countlZeroFunction_with_intrinsics", countlZeroFunction, true);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: countrZeroFunction (MLIR intrinsics enabled)",
          "[bit][countr_zero][intrinsics]") {
	testBitIntrinsicLLVMIR("countrZeroFunction_with_intrinsics", countrZeroFunction, true);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: popcountFunction (MLIR intrinsics enabled)", "[bit][popcount][intrinsics]") {
	testBitIntrinsicLLVMIR("popcountFunction_with_intrinsics", popcountFunction, true);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: countlZero64Function (MLIR intrinsics enabled)",
          "[bit][countl_zero][intrinsics]") {
	testBitIntrinsicLLVMIR("countlZero64Function_with_intrinsics", countlZero64Function, true);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: countrZero64Function (MLIR intrinsics enabled)",
          "[bit][countr_zero][intrinsics]") {
	testBitIntrinsicLLVMIR("countrZero64Function_with_intrinsics", countrZero64Function, true);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: popcount64Function (MLIR intrinsics enabled)", "[bit][popcount][intrinsics]") {
	testBitIntrinsicLLVMIR("popcount64Function_with_intrinsics", popcount64Function, true);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: byteswap32Function (MLIR intrinsics enabled)", "[bit][byteswap][intrinsics]") {
	testBitIntrinsicLLVMIR("byteswap32Function_with_intrinsics", byteswap32Function, true);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: byteswap64Function (MLIR intrinsics enabled)", "[bit][byteswap][intrinsics]") {
	testBitIntrinsicLLVMIR("byteswap64Function_with_intrinsics", byteswap64Function, true);
}

// ============================================================================
// Bit Tests - Binary Operations with MLIR Intrinsics Enabled
// ============================================================================

TEST_CASE("LLVM IR Bit Intrinsic Test: rotlFunction (MLIR intrinsics enabled)", "[bit][rotl][intrinsics]") {
	testBitIntrinsicLLVMIR("rotlFunction_with_intrinsics", rotlFunction, true);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: rotrFunction (MLIR intrinsics enabled)", "[bit][rotr][intrinsics]") {
	testBitIntrinsicLLVMIR("rotrFunction_with_intrinsics", rotrFunction, true);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: rotl64Function (MLIR intrinsics enabled)", "[bit][rotl][intrinsics]") {
	testBitIntrinsicLLVMIR("rotl64Function_with_intrinsics", rotl64Function, true);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: rotr64Function (MLIR intrinsics enabled)", "[bit][rotr][intrinsics]") {
	testBitIntrinsicLLVMIR("rotr64Function_with_intrinsics", rotr64Function, true);
}

// ============================================================================
// Bit Tests - Unary Operations with MLIR Intrinsics Disabled
// ============================================================================

TEST_CASE("LLVM IR Bit Intrinsic Test: countlZeroFunction (MLIR intrinsics disabled)",
          "[bit][countl_zero][no-intrinsics]") {
	testBitIntrinsicLLVMIR("countlZeroFunction_without_intrinsics", countlZeroFunction, false);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: countrZeroFunction (MLIR intrinsics disabled)",
          "[bit][countr_zero][no-intrinsics]") {
	testBitIntrinsicLLVMIR("countrZeroFunction_without_intrinsics", countrZeroFunction, false);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: popcountFunction (MLIR intrinsics disabled)", "[bit][popcount][no-intrinsics]") {
	testBitIntrinsicLLVMIR("popcountFunction_without_intrinsics", popcountFunction, false);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: countlZero64Function (MLIR intrinsics disabled)",
          "[bit][countl_zero][no-intrinsics]") {
	testBitIntrinsicLLVMIR("countlZero64Function_without_intrinsics", countlZero64Function, false);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: countrZero64Function (MLIR intrinsics disabled)",
          "[bit][countr_zero][no-intrinsics]") {
	testBitIntrinsicLLVMIR("countrZero64Function_without_intrinsics", countrZero64Function, false);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: popcount64Function (MLIR intrinsics disabled)",
          "[bit][popcount][no-intrinsics]") {
	testBitIntrinsicLLVMIR("popcount64Function_without_intrinsics", popcount64Function, false);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: byteswap32Function (MLIR intrinsics disabled)",
          "[bit][byteswap][no-intrinsics]") {
	testBitIntrinsicLLVMIR("byteswap32Function_without_intrinsics", byteswap32Function, false);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: byteswap64Function (MLIR intrinsics disabled)",
          "[bit][byteswap][no-intrinsics]") {
	testBitIntrinsicLLVMIR("byteswap64Function_without_intrinsics", byteswap64Function, false);
}

// ============================================================================
// Bit Tests - Binary Operations with MLIR Intrinsics Disabled
// ============================================================================

TEST_CASE("LLVM IR Bit Intrinsic Test: rotlFunction (MLIR intrinsics disabled)", "[bit][rotl][no-intrinsics]") {
	testBitIntrinsicLLVMIR("rotlFunction_without_intrinsics", rotlFunction, false);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: rotrFunction (MLIR intrinsics disabled)", "[bit][rotr][no-intrinsics]") {
	testBitIntrinsicLLVMIR("rotrFunction_without_intrinsics", rotrFunction, false);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: rotl64Function (MLIR intrinsics disabled)", "[bit][rotl][no-intrinsics]") {
	testBitIntrinsicLLVMIR("rotl64Function_without_intrinsics", rotl64Function, false);
}

TEST_CASE("LLVM IR Bit Intrinsic Test: rotr64Function (MLIR intrinsics disabled)", "[bit][rotr][no-intrinsics]") {
	testBitIntrinsicLLVMIR("rotr64Function_without_intrinsics", rotr64Function, false);
}

} // namespace nautilus::engine
