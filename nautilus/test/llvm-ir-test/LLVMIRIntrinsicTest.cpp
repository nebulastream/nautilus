#include "LLVMIRTestUtil.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <filesystem>
#include <string>

// Include test function headers
#include "../common/CMathIntrinsicFunctions.hpp"
#include "../common/MemoryIntrinsicFunctions.hpp"
#include "../common/ProfileFunctions.hpp"

namespace nautilus::engine {

// Import profile functions from nautilus::engine namespace
using nautilus::engine::assumeAlignedFunction;
using nautilus::engine::assumeComplexCondition;
using nautilus::engine::assumeFunction;

// Import cmath functions from nautilus::engine namespace
using nautilus::engine::acosFunction;
using nautilus::engine::asinFunction;
using nautilus::engine::atan2Function;
using nautilus::engine::atanFunction;
using nautilus::engine::ceilFunction;
using nautilus::engine::copysignFunction;
using nautilus::engine::cosFunction;
using nautilus::engine::expFunction;
using nautilus::engine::fabsFunction;
using nautilus::engine::floorFunction;
using nautilus::engine::fmaFunction;
using nautilus::engine::fmaxFunction;
using nautilus::engine::fminFunction;
using nautilus::engine::fmodFunction;
using nautilus::engine::logFunction;
using nautilus::engine::powFunction;
using nautilus::engine::sinFunction;
using nautilus::engine::sqrtFunction;
using nautilus::engine::tanFunction;

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
// Cmath Tests - Unary Operations with MLIR Intrinsics Enabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: sinFunction (MLIR intrinsics enabled)", "[cmath][sin][intrinsics]") {
	testIntrinsicLLVMIR("sinFunction_with_intrinsics", sinFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: cosFunction (MLIR intrinsics enabled)", "[cmath][cos][intrinsics]") {
	testIntrinsicLLVMIR("cosFunction_with_intrinsics", cosFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: sqrtFunction (MLIR intrinsics enabled)", "[cmath][sqrt][intrinsics]") {
	testIntrinsicLLVMIR("sqrtFunction_with_intrinsics", sqrtFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: expFunction (MLIR intrinsics enabled)", "[cmath][exp][intrinsics]") {
	testIntrinsicLLVMIR("expFunction_with_intrinsics", expFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: logFunction (MLIR intrinsics enabled)", "[cmath][log][intrinsics]") {
	testIntrinsicLLVMIR("logFunction_with_intrinsics", logFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: ceilFunction (MLIR intrinsics enabled)", "[cmath][ceil][intrinsics]") {
	testIntrinsicLLVMIR("ceilFunction_with_intrinsics", ceilFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: floorFunction (MLIR intrinsics enabled)", "[cmath][floor][intrinsics]") {
	testIntrinsicLLVMIR("floorFunction_with_intrinsics", floorFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: fabsFunction (MLIR intrinsics enabled)", "[cmath][fabs][intrinsics]") {
	testIntrinsicLLVMIR("fabsFunction_with_intrinsics", fabsFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: tanFunction (MLIR intrinsics enabled)", "[cmath][tan][intrinsics]") {
	testIntrinsicLLVMIR("tanFunction_with_intrinsics", tanFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: asinFunction (MLIR intrinsics enabled)", "[cmath][asin][intrinsics]") {
	testIntrinsicLLVMIR("asinFunction_with_intrinsics", asinFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: acosFunction (MLIR intrinsics enabled)", "[cmath][acos][intrinsics]") {
	testIntrinsicLLVMIR("acosFunction_with_intrinsics", acosFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: atanFunction (MLIR intrinsics enabled)", "[cmath][atan][intrinsics]") {
	testIntrinsicLLVMIR("atanFunction_with_intrinsics", atanFunction, true);
}

// ============================================================================
// Cmath Tests - Binary Operations with MLIR Intrinsics Enabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: powFunction (MLIR intrinsics enabled)", "[cmath][pow][intrinsics]") {
	testIntrinsicLLVMIR("powFunction_with_intrinsics", powFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: atan2Function (MLIR intrinsics enabled)", "[cmath][atan2][intrinsics]") {
	testIntrinsicLLVMIR("atan2Function_with_intrinsics", atan2Function, true);
}

TEST_CASE("LLVM IR Intrinsic Test: fminFunction (MLIR intrinsics enabled)", "[cmath][fmin][intrinsics]") {
	testIntrinsicLLVMIR("fminFunction_with_intrinsics", fminFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: fmaxFunction (MLIR intrinsics enabled)", "[cmath][fmax][intrinsics]") {
	testIntrinsicLLVMIR("fmaxFunction_with_intrinsics", fmaxFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: fmodFunction (MLIR intrinsics enabled)", "[cmath][fmod][intrinsics]") {
	testIntrinsicLLVMIR("fmodFunction_with_intrinsics", fmodFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: copysignFunction (MLIR intrinsics enabled)", "[cmath][copysign][intrinsics]") {
	testIntrinsicLLVMIR("copysignFunction_with_intrinsics", copysignFunction, true);
}

// ============================================================================
// Cmath Tests - Ternary Operations with MLIR Intrinsics Enabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: fmaFunction (MLIR intrinsics enabled)", "[cmath][fma][intrinsics]") {
	testIntrinsicLLVMIR("fmaFunction_with_intrinsics", fmaFunction, true);
}

// ============================================================================
// Cmath Tests - Unary Operations with MLIR Intrinsics Disabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: sinFunction (MLIR intrinsics disabled)", "[cmath][sin][no-intrinsics]") {
	testIntrinsicLLVMIR("sinFunction_without_intrinsics", sinFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: cosFunction (MLIR intrinsics disabled)", "[cmath][cos][no-intrinsics]") {
	testIntrinsicLLVMIR("cosFunction_without_intrinsics", cosFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: sqrtFunction (MLIR intrinsics disabled)", "[cmath][sqrt][no-intrinsics]") {
	testIntrinsicLLVMIR("sqrtFunction_without_intrinsics", sqrtFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: expFunction (MLIR intrinsics disabled)", "[cmath][exp][no-intrinsics]") {
	testIntrinsicLLVMIR("expFunction_without_intrinsics", expFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: logFunction (MLIR intrinsics disabled)", "[cmath][log][no-intrinsics]") {
	testIntrinsicLLVMIR("logFunction_without_intrinsics", logFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: ceilFunction (MLIR intrinsics disabled)", "[cmath][ceil][no-intrinsics]") {
	testIntrinsicLLVMIR("ceilFunction_without_intrinsics", ceilFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: floorFunction (MLIR intrinsics disabled)", "[cmath][floor][no-intrinsics]") {
	testIntrinsicLLVMIR("floorFunction_without_intrinsics", floorFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: fabsFunction (MLIR intrinsics disabled)", "[cmath][fabs][no-intrinsics]") {
	testIntrinsicLLVMIR("fabsFunction_without_intrinsics", fabsFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: tanFunction (MLIR intrinsics disabled)", "[cmath][tan][no-intrinsics]") {
	testIntrinsicLLVMIR("tanFunction_without_intrinsics", tanFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: asinFunction (MLIR intrinsics disabled)", "[cmath][asin][no-intrinsics]") {
	testIntrinsicLLVMIR("asinFunction_without_intrinsics", asinFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: acosFunction (MLIR intrinsics disabled)", "[cmath][acos][no-intrinsics]") {
	testIntrinsicLLVMIR("acosFunction_without_intrinsics", acosFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: atanFunction (MLIR intrinsics disabled)", "[cmath][atan][no-intrinsics]") {
	testIntrinsicLLVMIR("atanFunction_without_intrinsics", atanFunction, false);
}

// ============================================================================
// Cmath Tests - Binary Operations with MLIR Intrinsics Disabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: powFunction (MLIR intrinsics disabled)", "[cmath][pow][no-intrinsics]") {
	testIntrinsicLLVMIR("powFunction_without_intrinsics", powFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: atan2Function (MLIR intrinsics disabled)", "[cmath][atan2][no-intrinsics]") {
	testIntrinsicLLVMIR("atan2Function_without_intrinsics", atan2Function, false);
}

TEST_CASE("LLVM IR Intrinsic Test: fminFunction (MLIR intrinsics disabled)", "[cmath][fmin][no-intrinsics]") {
	testIntrinsicLLVMIR("fminFunction_without_intrinsics", fminFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: fmaxFunction (MLIR intrinsics disabled)", "[cmath][fmax][no-intrinsics]") {
	testIntrinsicLLVMIR("fmaxFunction_without_intrinsics", fmaxFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: fmodFunction (MLIR intrinsics disabled)", "[cmath][fmod][no-intrinsics]") {
	testIntrinsicLLVMIR("fmodFunction_without_intrinsics", fmodFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: copysignFunction (MLIR intrinsics disabled)", "[cmath][copysign][no-intrinsics]") {
	testIntrinsicLLVMIR("copysignFunction_without_intrinsics", copysignFunction, false);
}

// ============================================================================
// Cmath Tests - Ternary Operations with MLIR Intrinsics Disabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: fmaFunction (MLIR intrinsics disabled)", "[cmath][fma][no-intrinsics]") {
	testIntrinsicLLVMIR("fmaFunction_without_intrinsics", fmaFunction, false);
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
