#include "VectorFunctions.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <filesystem>
#include <string>

// Re-use the shared LLVM IR test utility from the main test suite
#include "LLVMIRTestUtil.hpp"

// Vector intrinsic plugin registration
#ifdef ENABLE_MLIR_BACKEND
#include "MLIRVectorIntrinsics.hpp"
#endif

namespace nautilus::engine {

// ============================================================================
// Helpers — test with and without native MLIR vector intrinsics
// ============================================================================

template <typename Func>
void testVectorLLVMIR(const std::string& functionName, Func func) {
	std::filesystem::path testFile(__FILE__);
	std::filesystem::path referenceIRDir = testFile.parent_path() / "reference-ir";
	nautilus::engine::testLLVMIR(functionName, func, true, referenceIRDir);
}

template <typename Func>
void testVectorLLVMIR_NoIntrinsics(const std::string& functionName, Func func) {
	std::filesystem::path testFile(__FILE__);
	std::filesystem::path referenceIRDir = testFile.parent_path() / "reference-ir-no-intrinsics";
	nautilus::engine::testLLVMIR(functionName, func, false, referenceIRDir);
}

// ============================================================================
// Register vector intrinsics once before any test case runs
// ============================================================================

struct VectorIntrinsicRegistrar {
	VectorIntrinsicRegistrar() {
#ifdef ENABLE_MLIR_BACKEND
		nautilus::compiler::mlir::RegisterMLIRVectorIntrinsicPlugin();
#endif
		// Force a fixed SIMD width so generated IR is deterministic
		// regardless of the host CPU's actual SIMD capabilities.
		// 64 bytes (AVX-512 width) matches the checked-in reference IR.
		nautilus::SetSimdWidth(64);
	}
};
static VectorIntrinsicRegistrar registrar_;

// ============================================================================
// Float arithmetic — with intrinsics
// ============================================================================

TEST_CASE("Vector LLVM IR: vectorAddFloat", "[simd][float][intrinsics]") {
	testVectorLLVMIR("vectorAddFloat", vectorAddFloat);
}

TEST_CASE("Vector LLVM IR: vectorSubFloat", "[simd][float][intrinsics]") {
	testVectorLLVMIR("vectorSubFloat", vectorSubFloat);
}

TEST_CASE("Vector LLVM IR: vectorMulFloat", "[simd][float][intrinsics]") {
	testVectorLLVMIR("vectorMulFloat", vectorMulFloat);
}

TEST_CASE("Vector LLVM IR: vectorDivFloat", "[simd][float][intrinsics]") {
	testVectorLLVMIR("vectorDivFloat", vectorDivFloat);
}

TEST_CASE("Vector LLVM IR: vectorNegFloat", "[simd][float][intrinsics]") {
	testVectorLLVMIR("vectorNegFloat", vectorNegFloat);
}

TEST_CASE("Vector LLVM IR: vectorAbsFloat", "[simd][float][intrinsics]") {
	testVectorLLVMIR("vectorAbsFloat", vectorAbsFloat);
}

// ============================================================================
// Float min/max/fma — with intrinsics
// ============================================================================

TEST_CASE("Vector LLVM IR: vectorMinFloat", "[simd][float][intrinsics]") {
	testVectorLLVMIR("vectorMinFloat", vectorMinFloat);
}

TEST_CASE("Vector LLVM IR: vectorMaxFloat", "[simd][float][intrinsics]") {
	testVectorLLVMIR("vectorMaxFloat", vectorMaxFloat);
}

TEST_CASE("Vector LLVM IR: vectorFmaFloat", "[simd][float][intrinsics]") {
	testVectorLLVMIR("vectorFmaFloat", vectorFmaFloat);
}

// ============================================================================
// Float reductions — with intrinsics
// ============================================================================

TEST_CASE("Vector LLVM IR: vectorReduceAddFloat", "[simd][float][reduction][intrinsics]") {
	testVectorLLVMIR("vectorReduceAddFloat", vectorReduceAddFloat);
}

TEST_CASE("Vector LLVM IR: vectorReduceMinFloat", "[simd][float][reduction][intrinsics]") {
	testVectorLLVMIR("vectorReduceMinFloat", vectorReduceMinFloat);
}

TEST_CASE("Vector LLVM IR: vectorReduceMaxFloat", "[simd][float][reduction][intrinsics]") {
	testVectorLLVMIR("vectorReduceMaxFloat", vectorReduceMaxFloat);
}

// ============================================================================
// Comparisons — with intrinsics
// ============================================================================

TEST_CASE("Vector LLVM IR: vectorLtFloat", "[simd][float][comparison][intrinsics]") {
	testVectorLLVMIR("vectorLtFloat", vectorLtFloat);
}

TEST_CASE("Vector LLVM IR: vectorGeFloat", "[simd][float][comparison][intrinsics]") {
	testVectorLLVMIR("vectorGeFloat", vectorGeFloat);
}

TEST_CASE("Vector LLVM IR: vectorEqFloat", "[simd][float][comparison][intrinsics]") {
	testVectorLLVMIR("vectorEqFloat", vectorEqFloat);
}

TEST_CASE("Vector LLVM IR: vectorEqInt", "[simd][int][comparison][intrinsics]") {
	testVectorLLVMIR("vectorEqInt", vectorEqInt);
}

TEST_CASE("Vector LLVM IR: vectorLtInt", "[simd][int][comparison][intrinsics]") {
	testVectorLLVMIR("vectorLtInt", vectorLtInt);
}

// ============================================================================
// Blend — with intrinsics
// ============================================================================

TEST_CASE("Vector LLVM IR: vectorBlendFloat", "[simd][float][blend][intrinsics]") {
	testVectorLLVMIR("vectorBlendFloat", vectorBlendFloat);
}

TEST_CASE("Vector LLVM IR: vectorBlendInt", "[simd][int][blend][intrinsics]") {
	testVectorLLVMIR("vectorBlendInt", vectorBlendInt);
}

// ============================================================================
// Bitwise — with intrinsics
// ============================================================================

TEST_CASE("Vector LLVM IR: vectorAndInt", "[simd][int][bitwise][intrinsics]") {
	testVectorLLVMIR("vectorAndInt", vectorAndInt);
}

TEST_CASE("Vector LLVM IR: vectorOrInt", "[simd][int][bitwise][intrinsics]") {
	testVectorLLVMIR("vectorOrInt", vectorOrInt);
}

TEST_CASE("Vector LLVM IR: vectorXorInt", "[simd][int][bitwise][intrinsics]") {
	testVectorLLVMIR("vectorXorInt", vectorXorInt);
}

// ============================================================================
// Integer arithmetic — with intrinsics
// ============================================================================

TEST_CASE("Vector LLVM IR: vectorAddInt", "[simd][int][intrinsics]") {
	testVectorLLVMIR("vectorAddInt", vectorAddInt);
}

TEST_CASE("Vector LLVM IR: vectorMulInt", "[simd][int][intrinsics]") {
	testVectorLLVMIR("vectorMulInt", vectorMulInt);
}

TEST_CASE("Vector LLVM IR: vectorReduceAddInt", "[simd][int][reduction][intrinsics]") {
	testVectorLLVMIR("vectorReduceAddInt", vectorReduceAddInt);
}

// ============================================================================
// Double — with intrinsics
// ============================================================================

TEST_CASE("Vector LLVM IR: vectorAddDouble", "[simd][double][intrinsics]") {
	testVectorLLVMIR("vectorAddDouble", vectorAddDouble);
}

// ============================================================================
// Realistic composite patterns — with intrinsics
// ============================================================================

TEST_CASE("Vector LLVM IR: vectorDotProductFloat", "[simd][float][composite][intrinsics]") {
	testVectorLLVMIR("vectorDotProductFloat", vectorDotProductFloat);
}

TEST_CASE("Vector LLVM IR: vectorDotProductInt", "[simd][int][composite][intrinsics]") {
	testVectorLLVMIR("vectorDotProductInt", vectorDotProductInt);
}

TEST_CASE("Vector LLVM IR: vectorSquaredNormFloat", "[simd][float][composite][intrinsics]") {
	testVectorLLVMIR("vectorSquaredNormFloat", vectorSquaredNormFloat);
}

TEST_CASE("Vector LLVM IR: vectorSaxpyFloat", "[simd][float][composite][intrinsics]") {
	testVectorLLVMIR("vectorSaxpyFloat", vectorSaxpyFloat);
}

TEST_CASE("Vector LLVM IR: vectorClampFloat", "[simd][float][composite][intrinsics]") {
	testVectorLLVMIR("vectorClampFloat", vectorClampFloat);
}

TEST_CASE("Vector LLVM IR: vectorDistanceSquaredFloat", "[simd][float][composite][intrinsics]") {
	testVectorLLVMIR("vectorDistanceSquaredFloat", vectorDistanceSquaredFloat);
}

TEST_CASE("Vector LLVM IR: vectorPolyEvalFloat", "[simd][float][composite][intrinsics]") {
	testVectorLLVMIR("vectorPolyEvalFloat", vectorPolyEvalFloat);
}

TEST_CASE("Vector LLVM IR: vectorSadFloat", "[simd][float][composite][intrinsics]") {
	testVectorLLVMIR("vectorSadFloat", vectorSadFloat);
}

TEST_CASE("Vector LLVM IR: vectorLerpFloat", "[simd][float][composite][intrinsics]") {
	testVectorLLVMIR("vectorLerpFloat", vectorLerpFloat);
}

TEST_CASE("Vector LLVM IR: vectorReluFloat", "[simd][float][composite][intrinsics]") {
	testVectorLLVMIR("vectorReluFloat", vectorReluFloat);
}

TEST_CASE("Vector LLVM IR: vectorMulAccFloat", "[simd][float][composite][intrinsics]") {
	testVectorLLVMIR("vectorMulAccFloat", vectorMulAccFloat);
}

TEST_CASE("Vector LLVM IR: vectorCmpBlendFloat", "[simd][float][composite][intrinsics]") {
	testVectorLLVMIR("vectorCmpBlendFloat", vectorCmpBlendFloat);
}

// ============================================================================
// Without intrinsics — scalar fallback (runtime calls)
// ============================================================================

TEST_CASE("Vector LLVM IR (no intrinsics): vectorAddFloat", "[simd][no-intrinsics]") {
	testVectorLLVMIR_NoIntrinsics("vectorAddFloat", vectorAddFloat);
}

TEST_CASE("Vector LLVM IR (no intrinsics): vectorMulFloat", "[simd][no-intrinsics]") {
	testVectorLLVMIR_NoIntrinsics("vectorMulFloat", vectorMulFloat);
}

TEST_CASE("Vector LLVM IR (no intrinsics): vectorReduceAddFloat", "[simd][no-intrinsics]") {
	testVectorLLVMIR_NoIntrinsics("vectorReduceAddFloat", vectorReduceAddFloat);
}

TEST_CASE("Vector LLVM IR (no intrinsics): vectorDotProductFloat", "[simd][no-intrinsics]") {
	testVectorLLVMIR_NoIntrinsics("vectorDotProductFloat", vectorDotProductFloat);
}

TEST_CASE("Vector LLVM IR (no intrinsics): vectorAddInt", "[simd][no-intrinsics]") {
	testVectorLLVMIR_NoIntrinsics("vectorAddInt", vectorAddInt);
}

TEST_CASE("Vector LLVM IR (no intrinsics): vectorBlendFloat", "[simd][no-intrinsics]") {
	testVectorLLVMIR_NoIntrinsics("vectorBlendFloat", vectorBlendFloat);
}

TEST_CASE("Vector LLVM IR (no intrinsics): vectorReluFloat", "[simd][no-intrinsics]") {
	testVectorLLVMIR_NoIntrinsics("vectorReluFloat", vectorReluFloat);
}

} // namespace nautilus::engine
