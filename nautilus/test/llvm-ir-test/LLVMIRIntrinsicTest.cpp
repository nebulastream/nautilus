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
using nautilus::engine::cmathAbsFunction;
using nautilus::engine::cmathCeilFunction;
using nautilus::engine::cmathCosFunction;
using nautilus::engine::cmathCoshFunction;
using nautilus::engine::cmathExpFunction;
using nautilus::engine::cmathFabsFunction;
using nautilus::engine::cmathFloorFunction;
using nautilus::engine::cmathFmaxFunction;
using nautilus::engine::cmathFminFunction;
using nautilus::engine::cmathLogFunction;
using nautilus::engine::cmathPowFunction;
using nautilus::engine::cmathRoundFunction;
using nautilus::engine::cmathSinFunction;
using nautilus::engine::cmathSinhFunction;
using nautilus::engine::cmathSqrtFunction;
using nautilus::engine::cmathTanFunction;

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
// Profile Tests - cmath Trigonometric Functions with MLIR Intrinsics Enabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: cmathSinFunction (MLIR intrinsics enabled)", "[profile][cmath][sin][intrinsics]") {
	testIntrinsicLLVMIR("cmathSinFunction_with_intrinsics", cmathSinFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: cmathCosFunction (MLIR intrinsics enabled)", "[profile][cmath][cos][intrinsics]") {
	testIntrinsicLLVMIR("cmathCosFunction_with_intrinsics", cmathCosFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: cmathTanFunction (MLIR intrinsics enabled)", "[profile][cmath][tan][intrinsics]") {
	testIntrinsicLLVMIR("cmathTanFunction_with_intrinsics", cmathTanFunction, true);
}

// ============================================================================
// Profile Tests - cmath Exponential/Logarithmic Functions with MLIR Intrinsics Enabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: cmathExpFunction (MLIR intrinsics enabled)", "[profile][cmath][exp][intrinsics]") {
	testIntrinsicLLVMIR("cmathExpFunction_with_intrinsics", cmathExpFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: cmathLogFunction (MLIR intrinsics enabled)", "[profile][cmath][log][intrinsics]") {
	testIntrinsicLLVMIR("cmathLogFunction_with_intrinsics", cmathLogFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: cmathSqrtFunction (MLIR intrinsics enabled)", "[profile][cmath][sqrt][intrinsics]") {
	testIntrinsicLLVMIR("cmathSqrtFunction_with_intrinsics", cmathSqrtFunction, true);
}

// ============================================================================
// Profile Tests - cmath Power Functions with MLIR Intrinsics Enabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: cmathPowFunction (MLIR intrinsics enabled)", "[profile][cmath][pow][intrinsics]") {
	testIntrinsicLLVMIR("cmathPowFunction_with_intrinsics", cmathPowFunction, true);
}

// ============================================================================
// Profile Tests - cmath Rounding Functions with MLIR Intrinsics Enabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: cmathFloorFunction (MLIR intrinsics enabled)",
          "[profile][cmath][floor][intrinsics]") {
	testIntrinsicLLVMIR("cmathFloorFunction_with_intrinsics", cmathFloorFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: cmathCeilFunction (MLIR intrinsics enabled)", "[profile][cmath][ceil][intrinsics]") {
	testIntrinsicLLVMIR("cmathCeilFunction_with_intrinsics", cmathCeilFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: cmathRoundFunction (MLIR intrinsics enabled)",
          "[profile][cmath][round][intrinsics]") {
	testIntrinsicLLVMIR("cmathRoundFunction_with_intrinsics", cmathRoundFunction, true);
}

// ============================================================================
// Profile Tests - cmath Absolute Value Functions with MLIR Intrinsics Enabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: cmathAbsFunction (MLIR intrinsics enabled)", "[profile][cmath][abs][intrinsics]") {
	testIntrinsicLLVMIR("cmathAbsFunction_with_intrinsics", cmathAbsFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: cmathFabsFunction (MLIR intrinsics enabled)", "[profile][cmath][fabs][intrinsics]") {
	testIntrinsicLLVMIR("cmathFabsFunction_with_intrinsics", cmathFabsFunction, true);
}

// ============================================================================
// Profile Tests - cmath Hyperbolic Functions with MLIR Intrinsics Enabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: cmathSinhFunction (MLIR intrinsics enabled)", "[profile][cmath][sinh][intrinsics]") {
	testIntrinsicLLVMIR("cmathSinhFunction_with_intrinsics", cmathSinhFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: cmathCoshFunction (MLIR intrinsics enabled)", "[profile][cmath][cosh][intrinsics]") {
	testIntrinsicLLVMIR("cmathCoshFunction_with_intrinsics", cmathCoshFunction, true);
}

// ============================================================================
// Profile Tests - cmath Min/Max Functions with MLIR Intrinsics Enabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: cmathFmaxFunction (MLIR intrinsics enabled)", "[profile][cmath][fmax][intrinsics]") {
	testIntrinsicLLVMIR("cmathFmaxFunction_with_intrinsics", cmathFmaxFunction, true);
}

TEST_CASE("LLVM IR Intrinsic Test: cmathFminFunction (MLIR intrinsics enabled)", "[profile][cmath][fmin][intrinsics]") {
	testIntrinsicLLVMIR("cmathFminFunction_with_intrinsics", cmathFminFunction, true);
}

// ============================================================================
// Profile Tests - cmath Trigonometric Functions with MLIR Intrinsics Disabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: cmathSinFunction (MLIR intrinsics disabled)",
          "[profile][cmath][sin][no-intrinsics]") {
	testIntrinsicLLVMIR("cmathSinFunction_without_intrinsics", cmathSinFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: cmathCosFunction (MLIR intrinsics disabled)",
          "[profile][cmath][cos][no-intrinsics]") {
	testIntrinsicLLVMIR("cmathCosFunction_without_intrinsics", cmathCosFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: cmathTanFunction (MLIR intrinsics disabled)",
          "[profile][cmath][tan][no-intrinsics]") {
	testIntrinsicLLVMIR("cmathTanFunction_without_intrinsics", cmathTanFunction, false);
}

// ============================================================================
// Profile Tests - cmath Exponential/Logarithmic Functions with MLIR Intrinsics Disabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: cmathExpFunction (MLIR intrinsics disabled)",
          "[profile][cmath][exp][no-intrinsics]") {
	testIntrinsicLLVMIR("cmathExpFunction_without_intrinsics", cmathExpFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: cmathLogFunction (MLIR intrinsics disabled)",
          "[profile][cmath][log][no-intrinsics]") {
	testIntrinsicLLVMIR("cmathLogFunction_without_intrinsics", cmathLogFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: cmathSqrtFunction (MLIR intrinsics disabled)",
          "[profile][cmath][sqrt][no-intrinsics]") {
	testIntrinsicLLVMIR("cmathSqrtFunction_without_intrinsics", cmathSqrtFunction, false);
}

// ============================================================================
// Profile Tests - cmath Power Functions with MLIR Intrinsics Disabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: cmathPowFunction (MLIR intrinsics disabled)",
          "[profile][cmath][pow][no-intrinsics]") {
	testIntrinsicLLVMIR("cmathPowFunction_without_intrinsics", cmathPowFunction, false);
}

// ============================================================================
// Profile Tests - cmath Rounding Functions with MLIR Intrinsics Disabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: cmathFloorFunction (MLIR intrinsics disabled)",
          "[profile][cmath][floor][no-intrinsics]") {
	testIntrinsicLLVMIR("cmathFloorFunction_without_intrinsics", cmathFloorFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: cmathCeilFunction (MLIR intrinsics disabled)",
          "[profile][cmath][ceil][no-intrinsics]") {
	testIntrinsicLLVMIR("cmathCeilFunction_without_intrinsics", cmathCeilFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: cmathRoundFunction (MLIR intrinsics disabled)",
          "[profile][cmath][round][no-intrinsics]") {
	testIntrinsicLLVMIR("cmathRoundFunction_without_intrinsics", cmathRoundFunction, false);
}

// ============================================================================
// Profile Tests - cmath Absolute Value Functions with MLIR Intrinsics Disabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: cmathAbsFunction (MLIR intrinsics disabled)",
          "[profile][cmath][abs][no-intrinsics]") {
	testIntrinsicLLVMIR("cmathAbsFunction_without_intrinsics", cmathAbsFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: cmathFabsFunction (MLIR intrinsics disabled)",
          "[profile][cmath][fabs][no-intrinsics]") {
	testIntrinsicLLVMIR("cmathFabsFunction_without_intrinsics", cmathFabsFunction, false);
}

// ============================================================================
// Profile Tests - cmath Hyperbolic Functions with MLIR Intrinsics Disabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: cmathSinhFunction (MLIR intrinsics disabled)",
          "[profile][cmath][sinh][no-intrinsics]") {
	testIntrinsicLLVMIR("cmathSinhFunction_without_intrinsics", cmathSinhFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: cmathCoshFunction (MLIR intrinsics disabled)",
          "[profile][cmath][cosh][no-intrinsics]") {
	testIntrinsicLLVMIR("cmathCoshFunction_without_intrinsics", cmathCoshFunction, false);
}

// ============================================================================
// Profile Tests - cmath Min/Max Functions with MLIR Intrinsics Disabled
// ============================================================================

TEST_CASE("LLVM IR Intrinsic Test: cmathFmaxFunction (MLIR intrinsics disabled)",
          "[profile][cmath][fmax][no-intrinsics]") {
	testIntrinsicLLVMIR("cmathFmaxFunction_without_intrinsics", cmathFmaxFunction, false);
}

TEST_CASE("LLVM IR Intrinsic Test: cmathFminFunction (MLIR intrinsics disabled)",
          "[profile][cmath][fmin][no-intrinsics]") {
	testIntrinsicLLVMIR("cmathFminFunction_without_intrinsics", cmathFminFunction, false);
}

} // namespace nautilus::engine
