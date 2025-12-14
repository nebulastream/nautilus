#include "LLVMIRTestUtil.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <regex>
#include <string>

// Include test function headers
#include "../common/BoolOperations.hpp"
#include "../common/CastFunctions.hpp"
#include "../common/ControlFlowFunctions.hpp"
#include "../common/EnumFunction.hpp"
#include "../common/ExpressionFunctions.hpp"
#include "../common/LoopFunctions.hpp"
#include "../common/NestedIfBenchmarks.hpp"
#include "../common/PointerFunctions.hpp"
#include "../common/RunctimeCallFunctions.hpp"
#include "../common/StaticLoopFunctions.hpp"

namespace nautilus::engine {

// Import enum functions from nautilus namespace
using nautilus::callEnumClassFunction;
using nautilus::callEnumFunction;
using nautilus::enumClassFunction;
using nautilus::enumFunction;
using nautilus::getEnum;
using nautilus::handleEnum;
using nautilus::handleEnumLogLevel;
using nautilus::isEnum;

// Import pointer functions from nautilus namespace
using nautilus::addArray;
using nautilus::callMemcpy;
using nautilus::castCustomClass;
using nautilus::castPtrAndGetValue;
using nautilus::castVoidPtr;
using nautilus::customPointerAdd;
using nautilus::isNotNullptr;
using nautilus::isNullptr;
using nautilus::load;
using nautilus::loadConst;
using nautilus::passCustomClass;
using nautilus::pointerAdd;
using nautilus::pointerAddConst;
using nautilus::pointerSub;
using nautilus::pointerSubConst;
using nautilus::ptrAssignment;
using nautilus::ptrEquals;
using nautilus::ptrGreaterThan;
using nautilus::ptrGreaterThanEquals;
using nautilus::ptrLessThan;
using nautilus::ptrLessThanEquals;
using nautilus::ptrNotEquals;
using nautilus::specializeType;
using nautilus::useWrapper;

// Wrapper for testLLVMIR that uses the default reference-ir directory and enables intrinsics
template <typename Func>
void testLLVMIR(const std::string& functionName, Func func) {
	std::filesystem::path testFile(__FILE__);
	std::filesystem::path referenceIRDir = testFile.parent_path() / "reference-ir";
	nautilus::engine::testLLVMIR(functionName, func, true, referenceIRDir);
}

// ============================================================================
// Expression Tests
// ============================================================================

TEST_CASE("LLVM IR Test: int8AddExpression", "[expressions]") {
	testLLVMIR("int8AddExpression", int8AddExpression);
}

// ============================================================================
// Enum Tests
// ============================================================================

TEST_CASE("LLVM IR Test: handleEnum", "[enum][basic]") {
	testLLVMIR("handleEnum", handleEnum);
}

TEST_CASE("LLVM IR Test: handleEnumLogLevel", "[enum][basic]") {
	testLLVMIR("handleEnumLogLevel", handleEnumLogLevel);
}

TEST_CASE("LLVM IR Test: isEnum", "[enum][basic]") {
	testLLVMIR("isEnum", isEnum);
}

TEST_CASE("LLVM IR Test: getEnum", "[enum][basic]") {
	testLLVMIR("getEnum", getEnum);
}

TEST_CASE("LLVM IR Test: callEnumFunction", "[enum][invoke]") {
	testLLVMIR("callEnumFunction", callEnumFunction);
}

TEST_CASE("LLVM IR Test: callEnumClassFunction", "[enum][invoke]") {
	testLLVMIR("callEnumClassFunction", callEnumClassFunction);
}

// ============================================================================
// Boolean Operations Tests
// ============================================================================

TEST_CASE("LLVM IR Test: boolNot", "[bool]") {
	testLLVMIR("boolNot", boolNot);
}

TEST_CASE("LLVM IR Test: boolAnd", "[bool]") {
	testLLVMIR("boolAnd", boolAnd);
}

TEST_CASE("LLVM IR Test: boolOr", "[bool]") {
	testLLVMIR("boolOr", boolOr);
}

TEST_CASE("LLVM IR Test: boolEquals", "[bool]") {
	testLLVMIR("boolEquals", boolEquals);
}

TEST_CASE("LLVM IR Test: boolNotEquals", "[bool]") {
	testLLVMIR("boolNotEquals", boolNotEquals);
}

TEST_CASE("LLVM IR Test: boolAssignment", "[bool]") {
	testLLVMIR("boolAssignment", boolAssignment);
}

TEST_CASE("LLVM IR Test: boolAssignmentOr", "[bool]") {
	testLLVMIR("boolAssignmentOr", boolAssignmentOr);
}

TEST_CASE("LLVM IR Test: boolConst", "[bool]") {
	testLLVMIR("boolConst", boolConst);
}

TEST_CASE("LLVM IR Test: boolIfElse", "[bool]") {
	testLLVMIR("boolIfElse", boolIfElse);
}

TEST_CASE("LLVM IR Test: boolNestedFunction", "[bool]") {
	testLLVMIR("boolNestedFunction", boolNestedFunction);
}

// ============================================================================
// Type Cast Tests
// ============================================================================

// Implicit casts
TEST_CASE("LLVM IR Test: implicitCastExpression_int8_to_int32", "[cast][implicit]") {
	testLLVMIR("implicitCastExpression_int8_to_int32", implicitCastExpression<int8_t, int32_t>);
}

TEST_CASE("LLVM IR Test: implicitCastExpression_int32_to_int64", "[cast][implicit]") {
	testLLVMIR("implicitCastExpression_int32_to_int64", implicitCastExpression<int32_t, int64_t>);
}

TEST_CASE("LLVM IR Test: implicitCastExpression_int32_to_float", "[cast][implicit]") {
	testLLVMIR("implicitCastExpression_int32_to_float", implicitCastExpression<int32_t, float>);
}

TEST_CASE("LLVM IR Test: implicitCastExpression_float_to_double", "[cast][implicit]") {
	testLLVMIR("implicitCastExpression_float_to_double", implicitCastExpression<float, double>);
}

// Static casts
TEST_CASE("LLVM IR Test: staticCastExpression_int8_to_int32", "[cast][static]") {
	testLLVMIR("staticCastExpression_int8_to_int32", staticCastExpression<int8_t, int32_t>);
}

TEST_CASE("LLVM IR Test: staticCastExpression_int32_to_int64", "[cast][static]") {
	testLLVMIR("staticCastExpression_int32_to_int64", staticCastExpression<int32_t, int64_t>);
}

TEST_CASE("LLVM IR Test: staticCastExpression_int64_to_int32", "[cast][static]") {
	testLLVMIR("staticCastExpression_int64_to_int32", staticCastExpression<int64_t, int32_t>);
}

TEST_CASE("LLVM IR Test: staticCastExpression_int32_to_float", "[cast][static]") {
	testLLVMIR("staticCastExpression_int32_to_float", staticCastExpression<int32_t, float>);
}

TEST_CASE("LLVM IR Test: staticCastExpression_float_to_double", "[cast][static]") {
	testLLVMIR("staticCastExpression_float_to_double", staticCastExpression<float, double>);
}

TEST_CASE("LLVM IR Test: staticCastExpression_double_to_int32", "[cast][static]") {
	testLLVMIR("staticCastExpression_double_to_int32", staticCastExpression<double, int32_t>);
}

// ============================================================================
// Control Flow Tests - Basic Conditionals
// ============================================================================

TEST_CASE("LLVM IR Test: ifThenCondition", "[control-flow][basic]") {
	testLLVMIR("ifThenCondition", ifThenCondition);
}

TEST_CASE("LLVM IR Test: ifThenElseCondition", "[control-flow][basic]") {
	testLLVMIR("ifThenElseCondition", ifThenElseCondition);
}

TEST_CASE("LLVM IR Test: ifElseIfCondition", "[control-flow][basic]") {
	testLLVMIR("ifElseIfCondition", ifElseIfCondition);
}

TEST_CASE("LLVM IR Test: ifElseIfElse", "[control-flow][basic]") {
	testLLVMIR("ifElseIfElse", ifElseIfElse);
}

TEST_CASE("LLVM IR Test: ifElseIfOnly", "[control-flow][basic]") {
	testLLVMIR("ifElseIfOnly", ifElseIfOnly);
}

TEST_CASE("LLVM IR Test: conditionalReturn", "[control-flow][basic]") {
	testLLVMIR("conditionalReturn", conditionalReturn);
}

TEST_CASE("LLVM IR Test: multipleReturns", "[control-flow][basic]") {
	testLLVMIR("multipleReturns", multipleReturns);
}

TEST_CASE("LLVM IR Test: doubleIfCondition", "[control-flow][basic]") {
	testLLVMIR("doubleIfCondition", doubleIfCondition);
}

// ============================================================================
// Control Flow Tests - Logical Operators
// ============================================================================

TEST_CASE("LLVM IR Test: andCondition", "[control-flow][logical]") {
	testLLVMIR("andCondition", andCondition);
}

TEST_CASE("LLVM IR Test: orCondition", "[control-flow][logical]") {
	testLLVMIR("orCondition", orCondition);
}

TEST_CASE("LLVM IR Test: logicalAnd", "[control-flow][logical]") {
	testLLVMIR("logicalAnd", logicalAnd);
}

TEST_CASE("LLVM IR Test: logicalOr", "[control-flow][logical]") {
	testLLVMIR("logicalOr", logicalOr);
}

TEST_CASE("LLVM IR Test: logicalXOR", "[control-flow][logical]") {
	testLLVMIR("logicalXOR", logicalXOR);
}

TEST_CASE("LLVM IR Test: ifNotEqual", "[control-flow][logical]") {
	testLLVMIR("ifNotEqual", ifNotEqual);
}

TEST_CASE("LLVM IR Test: andFunction", "[control-flow][logical]") {
	testLLVMIR("andFunction", andFunction);
}

TEST_CASE("LLVM IR Test: multipleConditions", "[control-flow][logical]") {
	testLLVMIR("multipleConditions", multipleConditions);
}

TEST_CASE("LLVM IR Test: complexLogicalExpressions", "[control-flow][logical]") {
	testLLVMIR("complexLogicalExpressions", complexLogicalExpressions);
}

TEST_CASE("LLVM IR Test: shortCircuitEvaluation", "[control-flow][logical]") {
	testLLVMIR("shortCircuitEvaluation", shortCircuitEvaluation);
}

// ============================================================================
// Control Flow Tests - Nested Conditionals
// ============================================================================

TEST_CASE("LLVM IR Test: nestedIf", "[control-flow][nested]") {
	testLLVMIR("nestedIf", nestedIf);
}

TEST_CASE("LLVM IR Test: nestedIfThenElseCondition", "[control-flow][nested]") {
	testLLVMIR("nestedIfThenElseCondition", nestedIfThenElseCondition);
}

TEST_CASE("LLVM IR Test: nestedIfNoElseCondition", "[control-flow][nested]") {
	testLLVMIR("nestedIfNoElseCondition", nestedIfNoElseCondition);
}

TEST_CASE("LLVM IR Test: deeplyNestedIfElseCondition", "[control-flow][nested]") {
	testLLVMIR("deeplyNestedIfElseCondition", deeplyNestedIfElseCondition);
}

TEST_CASE("LLVM IR Test: deeplyNestedIfElseIfCondition", "[control-flow][nested]") {
	testLLVMIR("deeplyNestedIfElseIfCondition", deeplyNestedIfElseIfCondition);
}

TEST_CASE("LLVM IR Test: nestedIfElseDifferentLevels", "[control-flow][nested]") {
	testLLVMIR("nestedIfElseDifferentLevels", nestedIfElseDifferentLevels);
}

TEST_CASE("LLVM IR Test: multipleElse", "[control-flow][nested]") {
	testLLVMIR("multipleElse", multipleElse);
}

// ============================================================================
// Control Flow Tests - Complex Patterns
// ============================================================================

TEST_CASE("LLVM IR Test: ifWithTernary", "[control-flow][complex]") {
	testLLVMIR("ifWithTernary", ifWithTernary);
}

TEST_CASE("LLVM IR Test: ifWithFunctionCall", "[control-flow][complex]") {
	testLLVMIR("ifWithFunctionCall", ifWithFunctionCall);
}

TEST_CASE("LLVM IR Test: compoundAssignment", "[control-flow][complex]") {
	testLLVMIR("compoundAssignment", compoundAssignment);
}

TEST_CASE("LLVM IR Test: compoundStatements", "[control-flow][complex]") {
	testLLVMIR("compoundStatements", compoundStatements);
}

TEST_CASE("LLVM IR Test: varyingComplexity", "[control-flow][complex]") {
	testLLVMIR("varyingComplexity", varyingComplexity);
}

// ============================================================================
// Loop Tests - Basic
// ============================================================================

TEST_CASE("LLVM IR Test: sumLoop", "[loops][basic]") {
	testLLVMIR("sumLoop", sumLoop);
}

TEST_CASE("LLVM IR Test: ifInsideLoop", "[loops][basic]") {
	testLLVMIR("ifInsideLoop", ifInsideLoop);
}

TEST_CASE("LLVM IR Test: nestedSumLoop", "[loops][basic]") {
	testLLVMIR("nestedSumLoop", nestedSumLoop);
}

TEST_CASE("LLVM IR Test: ifSumLoop", "[loops][basic]") {
	testLLVMIR("ifSumLoop", ifSumLoop);
}

TEST_CASE("LLVM IR Test: ifElseSumLoop", "[loops][basic]") {
	testLLVMIR("ifElseSumLoop", ifElseSumLoop);
}

TEST_CASE("LLVM IR Test: elseOnlySumLoop", "[loops][basic]") {
	testLLVMIR("elseOnlySumLoop", elseOnlySumLoop);
}

TEST_CASE("LLVM IR Test: nestedIfSumLoop", "[loops][basic]") {
	testLLVMIR("nestedIfSumLoop", nestedIfSumLoop);
}

TEST_CASE("LLVM IR Test: nestedIfElseSumLoop", "[loops][basic]") {
	testLLVMIR("nestedIfElseSumLoop", nestedIfElseSumLoop);
}

TEST_CASE("LLVM IR Test: nestedElseOnlySumLoop", "[loops][basic]") {
	testLLVMIR("nestedElseOnlySumLoop", nestedElseOnlySumLoop);
}

// ============================================================================
// Loop Tests - Control Flow
// ============================================================================

TEST_CASE("LLVM IR Test: forBreak", "[loops][control]") {
	testLLVMIR("forBreak", forBreak);
}

TEST_CASE("LLVM IR Test: whileBreak", "[loops][control]") {
	testLLVMIR("whileBreak", whileBreak);
}

TEST_CASE("LLVM IR Test: whileContinue", "[loops][control]") {
	testLLVMIR("whileContinue", whileContinue);
}

// ============================================================================
// Loop Tests - Algorithms
// ============================================================================

TEST_CASE("LLVM IR Test: sumOfNumbers", "[loops][algorithms]") {
	testLLVMIR("sumOfNumbers", sumOfNumbers);
}

TEST_CASE("LLVM IR Test: factorial", "[loops][algorithms]") {
	testLLVMIR("factorial", factorial);
}

TEST_CASE("LLVM IR Test: reverseNumber", "[loops][algorithms]") {
	testLLVMIR("reverseNumber", reverseNumber);
}

TEST_CASE("LLVM IR Test: fibonacci", "[loops][algorithms]") {
	testLLVMIR("fibonacci", fibonacci);
}

TEST_CASE("LLVM IR Test: gcd", "[loops][algorithms]") {
	testLLVMIR("gcd", gcd);
}

TEST_CASE("LLVM IR Test: decimalToBinary", "[loops][algorithms]") {
	testLLVMIR("decimalToBinary", decimalToBinary);
}

TEST_CASE("LLVM IR Test: isPrime", "[loops][algorithms]") {
	testLLVMIR("isPrime", isPrime);
}

TEST_CASE("LLVM IR Test: collatz", "[loops][algorithms]") {
	testLLVMIR("collatz", collatz);
}

TEST_CASE("LLVM IR Test: digitSum", "[loops][algorithms]") {
	testLLVMIR("digitSum", digitSum);
}

TEST_CASE("LLVM IR Test: sumOfSquares", "[loops][algorithms]") {
	testLLVMIR("sumOfSquares", sumOfSquares);
}

TEST_CASE("LLVM IR Test: countDigits", "[loops][algorithms]") {
	testLLVMIR("countDigits", countDigits);
}

// ============================================================================
// Static Loop Tests
// ============================================================================

TEST_CASE("LLVM IR Test: staticLoop", "[loops][static]") {
	testLLVMIR("staticLoop", staticLoop);
}

TEST_CASE("LLVM IR Test: staticLoopWithIf", "[loops][static]") {
	SKIP();
	testLLVMIR("staticLoopWithIf", staticLoopWithIf);
}

TEST_CASE("LLVM IR Test: staticLoopWithDynamicLoop", "[loops][static]") {
	testLLVMIR("staticLoopWithDynamicLoop", staticLoopWithDynamicLoop);
}

TEST_CASE("LLVM IR Test: staticIterator", "[loops][static]") {
	testLLVMIR("staticIterator", staticIterator);
}

TEST_CASE("LLVM IR Test: staticConstIterator", "[loops][static]") {
	testLLVMIR("staticConstIterator", staticConstIterator);
}

TEST_CASE("LLVM IR Test: staticLoopIncrement", "[loops][static]") {
	testLLVMIR("staticLoopIncrement", staticLoopIncrement);
}

TEST_CASE("LLVM IR Test: staticWhileLoopDecrement", "[loops][static]") {
	testLLVMIR("staticWhileLoopDecrement", staticWhileLoopDecrement);
}

// ============================================================================
// Pointer Tests - Basic Operations
// ============================================================================

TEST_CASE("LLVM IR Test: load", "[pointers][basic]") {
	testLLVMIR("load", load);
}

TEST_CASE("LLVM IR Test: loadConst", "[pointers][basic]") {
	SKIP();
	testLLVMIR("loadConst", loadConst);
}

TEST_CASE("LLVM IR Test: sumArray", "[pointers][basic]") {
	testLLVMIR("sumArray", sumArray);
}

TEST_CASE("LLVM IR Test: ptrAssignment", "[pointers][basic]") {
	testLLVMIR("ptrAssignment", ptrAssignment);
}

// ============================================================================
// Pointer Tests - Arithmetic
// ============================================================================

TEST_CASE("LLVM IR Test: pointerAdd", "[pointers][arithmetic]") {
	testLLVMIR("pointerAdd", pointerAdd);
}

TEST_CASE("LLVM IR Test: pointerSub", "[pointers][arithmetic]") {
	testLLVMIR("pointerSub", pointerSub);
}

TEST_CASE("LLVM IR Test: pointerAddConst", "[pointers][arithmetic]") {
	testLLVMIR("pointerAddConst", pointerAddConst);
}

TEST_CASE("LLVM IR Test: pointerSubConst", "[pointers][arithmetic]") {
	testLLVMIR("pointerSubConst", pointerSubConst);
}

// ============================================================================
// Pointer Tests - Comparisons
// ============================================================================

TEST_CASE("LLVM IR Test: isNullptr_int32", "[pointers][comparison]") {
	testLLVMIR("isNullptr_int32", isNullptr<int32_t>);
}

TEST_CASE("LLVM IR Test: isNotNullptr_int32", "[pointers][comparison]") {
	testLLVMIR("isNotNullptr_int32", isNotNullptr<int32_t>);
}

TEST_CASE("LLVM IR Test: ptrEquals_int32", "[pointers][comparison]") {
	testLLVMIR("ptrEquals_int32", ptrEquals<int32_t>);
}

TEST_CASE("LLVM IR Test: ptrNotEquals_int32", "[pointers][comparison]") {
	testLLVMIR("ptrNotEquals_int32", ptrNotEquals<int32_t>);
}

TEST_CASE("LLVM IR Test: ptrLessThan_int32", "[pointers][comparison]") {
	testLLVMIR("ptrLessThan_int32", ptrLessThan<int32_t>);
}

TEST_CASE("LLVM IR Test: ptrLessThanEquals_int32", "[pointers][comparison]") {
	testLLVMIR("ptrLessThanEquals_int32", ptrLessThanEquals<int32_t>);
}

TEST_CASE("LLVM IR Test: ptrGreaterThan_int32", "[pointers][comparison]") {
	testLLVMIR("ptrGreaterThan_int32", ptrGreaterThan<int32_t>);
}

TEST_CASE("LLVM IR Test: ptrGreaterThanEquals_int32", "[pointers][comparison]") {
	testLLVMIR("ptrGreaterThanEquals_int32", ptrGreaterThanEquals<int32_t>);
}

// ============================================================================
// Pointer Tests - Casts and Advanced
// ============================================================================

TEST_CASE("LLVM IR Test: castVoidPtr", "[pointers][cast]") {
	testLLVMIR("castVoidPtr", castVoidPtr);
}

TEST_CASE("LLVM IR Test: castPtrAndGetValue_void_int32", "[pointers][cast]") {
	testLLVMIR("castPtrAndGetValue_void_int32", castPtrAndGetValue<void, int32_t>);
}

TEST_CASE("LLVM IR Test: customPointerAdd", "[pointers][advanced]") {
	testLLVMIR("customPointerAdd", customPointerAdd);
}

TEST_CASE("LLVM IR Test: passCustomClass", "[pointers][advanced]") {
	testLLVMIR("passCustomClass", passCustomClass);
}

TEST_CASE("LLVM IR Test: castCustomClass", "[pointers][advanced]") {
	testLLVMIR("castCustomClass", castCustomClass);
}

TEST_CASE("LLVM IR Test: specializeType", "[pointers][advanced]") {
	testLLVMIR("specializeType", specializeType);
}

TEST_CASE("LLVM IR Test: useWrapper", "[pointers][advanced]") {
	testLLVMIR("useWrapper", useWrapper);
}

// ============================================================================
// Runtime Call Tests - Basic
// ============================================================================

TEST_CASE("LLVM IR Test: useFirstArg", "[runtime-calls][basic]") {
	testLLVMIR("useFirstArg", useFirstArg);
}

TEST_CASE("LLVM IR Test: useSecondArg", "[runtime-calls][basic]") {
	testLLVMIR("useSecondArg", useSecondArg);
}

TEST_CASE("LLVM IR Test: useNoArg", "[runtime-calls][basic]") {
	testLLVMIR("useNoArg", useNoArg);
}

// ============================================================================
// Runtime Call Tests - Direct Calls
// ============================================================================

TEST_CASE("LLVM IR Test: simpleDirectCall", "[runtime-calls][direct]") {
	testLLVMIR("simpleDirectCall", simpleDirectCall);
}

TEST_CASE("LLVM IR Test: directCallWithNestedCalls", "[runtime-calls][direct]") {
	testLLVMIR("directCallWithNestedCalls", directCallWithNestedCalls);
}

TEST_CASE("LLVM IR Test: callTwoFunctions", "[runtime-calls][direct]") {
	testLLVMIR("callTwoFunctions", callTwoFunctions);
}

TEST_CASE("LLVM IR Test: loopDirectCall", "[runtime-calls][direct]") {
	testLLVMIR("loopDirectCall", loopDirectCall);
}

TEST_CASE("LLVM IR Test: loopDirectCall2", "[runtime-calls][direct]") {
	testLLVMIR("loopDirectCall2", loopDirectCall2);
}

TEST_CASE("LLVM IR Test: directCallComplexFunction", "[runtime-calls][direct]") {
	testLLVMIR("directCallComplexFunction", directCallComplexFunction);
}

TEST_CASE("LLVM IR Test: voidFuncCall", "[runtime-calls][direct]") {
	testLLVMIR("voidFuncCall", voidFuncCall);
}

TEST_CASE("LLVM IR Test: dynCastCall", "[runtime-calls][direct]") {
	testLLVMIR("dynCastCall", dynCastCall);
}

// ============================================================================
// Runtime Call Tests - Lambda and Advanced
// ============================================================================

TEST_CASE("LLVM IR Test: lambdaRuntimeFunction", "[runtime-calls][lambda]") {
	testLLVMIR("lambdaRuntimeFunction", lambdaRuntimeFunction);
}

TEST_CASE("LLVM IR Test: nestedLambdaRuntimeFunction", "[runtime-calls][lambda]") {
	testLLVMIR("nestedLambdaRuntimeFunction", nestedLambdaRuntimeFunction);
}

TEST_CASE("LLVM IR Test: callSameFunction", "[runtime-calls][advanced]") {
	testLLVMIR("callSameFunction", callSameFunction);
}

TEST_CASE("LLVM IR Test: callMemberFunction", "[runtime-calls][advanced]") {
	SKIP();
	testLLVMIR("callMemberFunction", callMemberFunction);
}

TEST_CASE("LLVM IR Test: callCountFuncCall", "[runtime-calls][advanced]") {
	testLLVMIR("callCountFuncCall", callCountFuncCall);
}

TEST_CASE("LLVM IR Test: incrementFuncCallFiveTimesWithModRef", "[runtime-calls][advanced]") {
	testLLVMIR("incrementFuncCallFiveTimesWithModRef", incrementFuncCallFiveTimesWithModRef);
}

TEST_CASE("LLVM IR Test: incrementFuncCallFiveTimesWithRef", "[runtime-calls][advanced]") {
	testLLVMIR("incrementFuncCallFiveTimesWithRef", incrementFuncCallFiveTimesWithRef);
}

// ============================================================================
// Benchmark Tests - Nested Conditionals
// ============================================================================

TEST_CASE("LLVM IR Test: nestedIf10", "[benchmarks][nested]") {
	testLLVMIR("nestedIf10", nestedIf10);
}

TEST_CASE("LLVM IR Test: nestedIf100", "[benchmarks][nested]") {
	testLLVMIR("nestedIf100", nestedIf100);
}

TEST_CASE("LLVM IR Test: chainedIf10", "[benchmarks][chained]") {
	SKIP();
	testLLVMIR("chainedIf10", chainedIf10);
}

TEST_CASE("LLVM IR Test: chainedIf100", "[benchmarks][chained]") {
	SKIP();
	testLLVMIR("chainedIf100", chainedIf100);
}

} // namespace nautilus::engine
