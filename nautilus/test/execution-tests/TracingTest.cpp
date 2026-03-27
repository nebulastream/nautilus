#include "BitIntrinsicFunctions.hpp"
#include "BoolOperations.hpp"
#include "CMathIntrinsicFunctions.hpp"
#include "CastFunctions.hpp"
#include "ControlFlowFunctions.hpp"
#include "EnumFunction.hpp"
#include "ExpressionFunctions.hpp"
#include "FunctionPtrFunctions.hpp"
#include "LoopFunctions.hpp"
#include "MemoryIntrinsicFunctions.hpp"
#include "NautilusFunction.hpp"
#include "NestedIfBenchmarks.hpp"
#include "PointerFunctions.hpp"
#include "ProfileFunctions.hpp"
#include "RunctimeCallFunctions.hpp"
#include "SelectOperations.hpp"
#include "StaticLoopFunctions.hpp"
#include "ValueTypeFunctions.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/SSAVerifier.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include <catch2/catch_all.hpp>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>

namespace nautilus::log::options {

bool getLogAddresses();
void setLogAddresses(bool);

} // namespace nautilus::log::options
namespace nautilus::engine {

bool checkTestFile(std::string actual, const std::string category, const std::string group, const std::string& name) {
	auto groupDir = std::string(TEST_DATA_FOLDER) + category + "/" + group + "/";
	if (!std::filesystem::exists(groupDir)) {
		std::filesystem::create_directories(groupDir);
	}

	// Check if the file exists
	std::string filePath = groupDir + name + ".trace";
	if (!std::filesystem::exists(filePath)) {
		std::cerr << "File does not exist: " << filePath << " Initializing with current trace. Please Rerun.\n";
		std::ofstream file {filePath};
		file << actual;
		// fail here so that CI fails if testdata is missing.
		return false;
	}

	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Unable to open file " << filePath << std::endl;
		return false;
	}

	std::stringstream expect;
	expect << file.rdbuf();

	if (expect.str() == actual) {
		return true;
	}

	char tmpName[] = "/tmp/actual_trace_XXXXXX";
	int tmpFd = mkstemp(tmpName);
	close(tmpFd);
	std::ofstream tmpfile {tmpName};
	tmpfile << actual;
	std::cerr << "Trace mismatch: (exp vs act) " << filePath << " " << tmpName << std::endl;
	std::cerr << "=== Expected (" << filePath << ") ===\n"
	          << expect.str() << "\n=== Actual ===\n"
	          << actual << "\n=== End ===\n";
	return false;
}

using TraceFn = std::unique_ptr<tracing::TraceModule> (*)(std::list<compiler::CompilableFunction>&,
                                                          const engine::Options&);

static auto traceContexts = std::vector<std::tuple<std::string, TraceFn>> {
    {"ExceptionBasedTraceContext", tracing::ExceptionBasedTraceContext::Trace},
    {"LazyTraceContext", tracing::LazyTraceContext::Trace},
};

void runTraceTests(const std::string& category, std::vector<std::tuple<std::string, std::function<void()>>>& tests) {
	// disable logging of addresses such that the trace is deterministic
	nautilus::log::options::setLogAddresses(false);
	for (auto& [ctxName, traceFn] : traceContexts) {
		DYNAMIC_SECTION(ctxName) {
			for (auto& [name, func] : tests) {
				DYNAMIC_SECTION(name) {
					auto rootFunction = compiler::CompilableFunction("execute", func);
					std::list<compiler::CompilableFunction> functionsToTrace;
					functionsToTrace.push_back(rootFunction);

					// Trace all functions (initially just "execute", but may include nested functions)

					auto executionTrace = traceFn(functionsToTrace, engine::Options());
					DYNAMIC_SECTION("tracing") {
						REQUIRE(checkTestFile(executionTrace.get()->toString(), category, "tracing", name));
					}
					auto ssaCreationPhase = tracing::SSACreationPhase();
					auto afterSSA =
					    ssaCreationPhase.apply(std::shared_ptr<tracing::TraceModule>(std::move(executionTrace)));
					DYNAMIC_SECTION("after_ssa") {
						REQUIRE(checkTestFile(afterSSA.get()->toString(), category, "after_ssa", name));
					}
					DYNAMIC_SECTION("ssa_verify") {
						for (const auto& fnName : afterSSA->getFunctionNames()) {
							auto ssaResult = tracing::VerifySSA(*afterSSA->getFunction(fnName));
							if (!ssaResult.valid) {
								for (const auto& error : ssaResult.errors) {
									FAIL(error);
								}
							}
						}
					}
					DYNAMIC_SECTION("ir") {
						auto irGenerationPhase = tracing::TraceToIRConversionPhase();
						[[maybe_unused]] auto ir = irGenerationPhase.apply(std::move(afterSSA));
						REQUIRE(checkTestFile(ir.get()->toString(), category, "ir", name));
					}
				}
			}
		}
	}
}

TEST_CASE("Expression Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"assignment1", details::createFunctionWrapper(assignment1)},
	    {"assignment2", details::createFunctionWrapper(assignment2)},
	    {"assignment3", details::createFunctionWrapper(assignment3)},
	    {"assignment4", details::createFunctionWrapper(assignment4)},
	    {"assignment5", details::createFunctionWrapper(assignment5)},
	    {"int8AddExpression", details::createFunctionWrapper(int8AddExpression)},
	    {"int16AddExpression", details::createFunctionWrapper(int16AddExpression)},
	    {"int32AddExpression", details::createFunctionWrapper(int32AddExpression)},
	    {"int64AddExpression", details::createFunctionWrapper(int64AddExpression)},
	    {"floatAddExpression", details::createFunctionWrapper(floatAddExpression)},
	    {"doubleAddExpression", details::createFunctionWrapper(doubleAddExpression)},
	    {"castFloatToDoubleAddExpression", details::createFunctionWrapper(castFloatToDoubleAddExpression)},
	    {"castInt8ToInt64AddExpression", details::createFunctionWrapper(castInt8ToInt64AddExpression)},
	    {"castInt8ToInt64AddExpression2", details::createFunctionWrapper(castInt8ToInt64AddExpression2)},
	    {"incrementPost", details::createFunctionWrapper(incrementPost)},
	    {"incrementPre", details::createFunctionWrapper(incrementPre)},
	    {"decrementPost", details::createFunctionWrapper(decrementPost)},
	    {"decrementPre", details::createFunctionWrapper(decrementPre)},
	    {"assignAdd", details::createFunctionWrapper(assignAdd)},
	    {"assignSub", details::createFunctionWrapper(assignSub)},
	    {"assignMul", details::createFunctionWrapper(assignMul)},
	    {"assignDiv", details::createFunctionWrapper(assignDiv)},
	    {"assignMod", details::createFunctionWrapper(assignMod)},
	    {"assignAnd", details::createFunctionWrapper(assignAnd)},
	    {"assignOr", details::createFunctionWrapper(assignOr)},
	    {"assignXor", details::createFunctionWrapper(assignXor)},
	    {"assignShl", details::createFunctionWrapper(assignShl)},
	    {"assignShr", details::createFunctionWrapper(assignShr)},
	    {"shiftLeft_i8", details::createFunctionWrapper(shiftLeft<int8_t>)},
	    {"shiftRight_i8", details::createFunctionWrapper(shiftRight<int8_t>)},
	    {"negate_i8", details::createFunctionWrapper(negate<int8_t>)},
	    {"logicalNot_bool", details::createFunctionWrapper(logicalNot<bool>)},
	    {"mulInt64AndNotDefinedI64", details::createFunctionWrapper(mulInt64AndNotDefinedI64)},
	    {"subInt8AndInt8", details::createFunctionWrapper(subInt8AndInt8)},
	    {"addInt8AndInt32", details::createFunctionWrapper(addInt8AndInt32)},
	    // previously missing: C++ semantic tests
	    {"subInt8AndInt8TruncateResult", details::createFunctionWrapper(subInt8AndInt8TruncateResult)},
	    {"chainedPrefixIncrement", details::createFunctionWrapper(chainedPrefixIncrement)},
	    {"chainedPrefixDecrement", details::createFunctionWrapper(chainedPrefixDecrement)},
	    {"unaryPlus", details::createFunctionWrapper(unaryPlus)},
	    {"unaryMinus", details::createFunctionWrapper(unaryMinus)},
	    {"moveAssignment", details::createFunctionWrapper(moveAssignment)},
	    {"prefixIncrementReturnValue", details::createFunctionWrapper(prefixIncrementReturnValue)},
	    {"prefixDecrementReturnValue", details::createFunctionWrapper(prefixDecrementReturnValue)},
	    // wider type coverage for shift/negate
	    {"shiftLeft_i32", details::createFunctionWrapper(shiftLeft<int32_t>)},
	    {"shiftRight_i32", details::createFunctionWrapper(shiftRight<int32_t>)},
	    {"shiftLeft_i64", details::createFunctionWrapper(shiftLeft<int64_t>)},
	    {"shiftRight_i64", details::createFunctionWrapper(shiftRight<int64_t>)},
	    {"negate_i32", details::createFunctionWrapper(negate<int32_t>)},
	    {"negate_i64", details::createFunctionWrapper(negate<int64_t>)},
	    // standalone arithmetic operators
	    {"int32SubExpression", details::createFunctionWrapper(int32SubExpression)},
	    {"int32MulExpression", details::createFunctionWrapper(int32MulExpression)},
	    {"int32DivExpression", details::createFunctionWrapper(int32DivExpression)},
	    {"int32ModExpression", details::createFunctionWrapper(int32ModExpression)},
	    // bitwise operators
	    {"bitwiseAnd", details::createFunctionWrapper(bitwiseAnd)},
	    {"bitwiseOr", details::createFunctionWrapper(bitwiseOr)},
	    {"bitwiseXor", details::createFunctionWrapper(bitwiseXor)},
	    // comparison operators
	    {"cmpEqual", details::createFunctionWrapper(cmpEqual)},
	    {"cmpNotEqual", details::createFunctionWrapper(cmpNotEqual)},
	    {"cmpLessThan", details::createFunctionWrapper(cmpLessThan)},
	    {"cmpLessThanOrEqual", details::createFunctionWrapper(cmpLessThanOrEqual)},
	    {"cmpGreaterThan", details::createFunctionWrapper(cmpGreaterThan)},
	    {"cmpGreaterThanOrEqual", details::createFunctionWrapper(cmpGreaterThanOrEqual)},
	    // unsigned arithmetic
	    {"uint32AddExpression", details::createFunctionWrapper(uint32AddExpression)},
	    {"uint32SubExpression", details::createFunctionWrapper(uint32SubExpression)},
	    {"uint32MulExpression", details::createFunctionWrapper(uint32MulExpression)},
	    {"uint32DivExpression", details::createFunctionWrapper(uint32DivExpression)},
	    {"uint64AddExpression", details::createFunctionWrapper(uint64AddExpression)},
	    // multi-operation expressions
	    {"complexArithmetic", details::createFunctionWrapper(complexArithmetic)},
	    {"multipleAssignments", details::createFunctionWrapper(multipleAssignments)},
	    {"expressionWithConstants", details::createFunctionWrapper(expressionWithConstants)},
	};
	runTraceTests("expression-tests", tests);
}
TEST_CASE("Control-flow Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"ifThenCondition", details::createFunctionWrapper(ifThenCondition)},
	    {"multipleVoidReturnsFunction", details::createFunctionWrapper(multipleVoidReturnsFunction)},
	    //{"conditionalReturn", details::createFunctionWrapper(conditionalReturn)},
	    //{"multipleReturns", details::createFunctionWrapper(multipleReturns)},
	    {"ifThenElseCondition", details::createFunctionWrapper(ifThenElseCondition)},
	    {"nestedIfThenElseCondition", details::createFunctionWrapper(nestedIfThenElseCondition)},
	    {"nestedIfNoElseCondition", details::createFunctionWrapper(nestedIfNoElseCondition)},
	    {"doubleIfCondition", details::createFunctionWrapper(doubleIfCondition)},
	    {"ifElseIfCondition", details::createFunctionWrapper(ifElseIfCondition)},
	    {"orCondition", details::createFunctionWrapper(orCondition)},
	    {"andCondition", details::createFunctionWrapper(andCondition)},
	    {"deeplyNestedIfElseCondition", details::createFunctionWrapper(deeplyNestedIfElseCondition)},
	    {"deeplyNestedIfElseIfCondition", details::createFunctionWrapper(deeplyNestedIfElseIfCondition)},
	    {"andFunction", details::createFunctionWrapper(andFunction)},
	    {"nestedIf", details::createFunctionWrapper(nestedIf)},
	    {"ifElseIfElse", details::createFunctionWrapper(ifElseIfElse)},
	    {"logicalAnd", details::createFunctionWrapper(logicalAnd)},
	    {"logicalOr", details::createFunctionWrapper(logicalOr)},
	    {"ifNotEqual", details::createFunctionWrapper(ifNotEqual)},
	    {"multipleConditions", details::createFunctionWrapper(multipleConditions)},
	    {"ifElseIfOnly", details::createFunctionWrapper(ifElseIfOnly)},
	    {"compoundAssignment", details::createFunctionWrapper(compoundAssignment)},
	    {"multipleElse", details::createFunctionWrapper(multipleElse)},
	    {"ifWithTernary", details::createFunctionWrapper(ifWithTernary)},
	    {"complexLogicalExpressions", details::createFunctionWrapper(complexLogicalExpressions)},
	    {"shortCircuitEvaluation", details::createFunctionWrapper(shortCircuitEvaluation)},
	    {"ifWithFunctionCall", details::createFunctionWrapper(ifWithFunctionCall)},
	    {"compoundStatements", details::createFunctionWrapper(compoundStatements)},
	    {"varyingComplexity", details::createFunctionWrapper(varyingComplexity)},
	    {"logicalXOR", details::createFunctionWrapper(logicalXOR)},
	    {"nestedIfElseDifferentLevels", details::createFunctionWrapper(nestedIfElseDifferentLevels)},
	    {"constructComplexReturnObject", details::createFunctionWrapper(constructComplexReturnObject)},
	    {"constructComplexReturnObject2", details::createFunctionWrapper(constructComplexReturnObject2)},
	    {"nestedIf10", details::createFunctionWrapper(nestedIf10)},
	    {"nestedIf100", details::createFunctionWrapper(nestedIf100)},
	    {"chainedIf10", details::createFunctionWrapper(chainedIf10)},
	    {"chainedIf100", details::createFunctionWrapper(chainedIf100)},
	    {"chainedIf500", details::createFunctionWrapper(chainedIf500)},
	    // previously missing control flow test
	    {"withBranchProbability", details::createFunctionWrapper(withBranchProbability)},
	};
	runTraceTests("control-flow-tests", tests);
}

TEST_CASE("Loop Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"ifInsideLoop", details::createFunctionWrapper(ifInsideLoop)},
	    {"sumLoop", details::createFunctionWrapper(sumLoop)},
	    {"nestedSumLoop", details::createFunctionWrapper(nestedSumLoop)},
	    {"ifSumLoop", details::createFunctionWrapper(ifSumLoop)},
	    {"ifElseSumLoop", details::createFunctionWrapper(ifElseSumLoop)},
	    {"elseOnlySumLoop", details::createFunctionWrapper(elseOnlySumLoop)},
	    {"nestedIfSumLoop", details::createFunctionWrapper(nestedIfSumLoop)},
	    {"nestedIfElseSumLoop", details::createFunctionWrapper(nestedIfElseSumLoop)},
	    {"nestedElseOnlySumLoop", details::createFunctionWrapper(nestedElseOnlySumLoop)},
	    {"sumOfNumbers", details::createFunctionWrapper(sumOfNumbers)},
	    {"factorial", details::createFunctionWrapper(factorial)},
	    {"reverseNumber", details::createFunctionWrapper(reverseNumber)},
	    {"fibonacci", details::createFunctionWrapper(fibonacci)},
	    {"gcd", details::createFunctionWrapper(gcd)},
	    {"decimalToBinary", details::createFunctionWrapper(decimalToBinary)},
	    {"isPrime", details::createFunctionWrapper(isPrime)},
	    {"collatz", details::createFunctionWrapper(collatz)},
	    {"digitSum", details::createFunctionWrapper(digitSum)},
	    {"sumOfSquares", details::createFunctionWrapper(sumOfSquares)},
	    {"countDigits", details::createFunctionWrapper(countDigits)},
	    {"forBreak", details::createFunctionWrapper(forBreak)},
	    {"whileBreak", details::createFunctionWrapper(whileBreak)},
	    {"whileContinue", details::createFunctionWrapper(whileContinue)},
	    // new loop pattern tests
	    {"forContinue", details::createFunctionWrapper(forContinue)},
	    {"whileMultiCondition", details::createFunctionWrapper(whileMultiCondition)},
	    {"nestedLoopBreak", details::createFunctionWrapper(nestedLoopBreak)},
	    {"loopCounterModification", details::createFunctionWrapper(loopCounterModification)},
	    {"doWhileMinOnce", details::createFunctionWrapper(doWhileMinOnce)},
	    {"power", details::createFunctionWrapper(power)},
	};
	runTraceTests("loop-tests", tests);
}

TEST_CASE("Runtime Call Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"simpleDirectCall", details::createFunctionWrapper(simpleDirectCall)},
	    {"loopDirectCall", details::createFunctionWrapper(loopDirectCall)},
	    {"useFirstArg", details::createFunctionWrapper(useFirstArg)},
	    {"useSecondArg", details::createFunctionWrapper(useSecondArg)},
	    {"useNoArg", details::createFunctionWrapper(useNoArg)},
	    {"lambdaRuntimeFunction", details::createFunctionWrapper(lambdaRuntimeFunction)},
	    {"nestedLambdaRuntimeFunction", details::createFunctionWrapper(nestedLambdaRuntimeFunction)},
	    {"callSameFunction", details::createFunctionWrapper(callSameFunction)},
	    {"voidFuncCall", details::createFunctionWrapper(voidFuncCall)},
	    {"callTwoFunctions", details::createFunctionWrapper(callTwoFunctions)},
	    // previously missing runtime call tests
	    {"directCallWithNestedCalls", details::createFunctionWrapper(directCallWithNestedCalls)},
	    {"loopDirectCall2", details::createFunctionWrapper(loopDirectCall2)},
	    {"directCallComplexFunction", details::createFunctionWrapper(directCallComplexFunction)},
	    {"callCountFuncCall", details::createFunctionWrapper(callCountFuncCall)},
	    {"callMemberFunction", details::createFunctionWrapper(callMemberFunction)},
	    {"incrementFuncCallFiveTimesWithModRef", details::createFunctionWrapper(incrementFuncCallFiveTimesWithModRef)},
	    {"incrementFuncCallFiveTimesWithRef", details::createFunctionWrapper(incrementFuncCallFiveTimesWithRef)},
	};
	runTraceTests("runtime-call-tests", tests);
}

TEST_CASE("Enum Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"handleEnum", details::createFunctionWrapper(handleEnum)},
	    {"isEnum", details::createFunctionWrapper(isEnum)},
	    {"getEnum", details::createFunctionWrapper(getEnum)},
	    {"callEnumFunction", details::createFunctionWrapper(callEnumFunction)},
	    {"callEnumClassFunction", details::createFunctionWrapper(callEnumClassFunction)},
	    {"handleEnumLogLevel", details::createFunctionWrapper(handleEnumLogLevel)},
	};
	runTraceTests("enum-tests", tests);
}

TEST_CASE("Bool Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"boolNot", details::createFunctionWrapper(boolNot)},
	    {"boolAnd", details::createFunctionWrapper(boolAnd)},
	    {"boolOr", details::createFunctionWrapper(boolOr)},
	    {"boolAssignment", details::createFunctionWrapper(boolAssignment)},
	    {"boolAssignmentOr", details::createFunctionWrapper(boolAssignmentOr)},
	    {"boolConst", details::createFunctionWrapper(boolConst)},
	    {"boolEquals", details::createFunctionWrapper(boolEquals)},
	    {"boolNotEquals", details::createFunctionWrapper(boolNotEquals)},
	    {"boolIfElse", details::createFunctionWrapper(boolIfElse)},
	    {"boolNestedFunction", details::createFunctionWrapper(boolNestedFunction)},
	    // previously missing bool tests
	    {"boolComplexOps", details::createFunctionWrapper(boolComplexOps)},
	    {"boolProbabilityTest", details::createFunctionWrapper(boolProbabilityTest)},
	};
	runTraceTests("bool-tests", tests);
}

TEST_CASE("Static Trace Test") {

	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
// these tests are sensitive to compiler options and not supported on ARM
#ifndef __aarch64__
	    {"staticLoop", details::createFunctionWrapper(staticLoop)},
	    {"staticLoopWithDynamicLoop", details::createFunctionWrapper(staticLoopWithDynamicLoop)},
	    {"staticLoopWithDynamicLoopPostIncrement",
	     details::createFunctionWrapper(staticLoopWithDynamicLoopPostIncrement)},
	    {"staticLoopWithDynamicLoopPreIncrement",
	     details::createFunctionWrapper(staticLoopWithDynamicLoopPreIncrement)},
	    {"staticLoopWithDynamicLoopNotEqual", details::createFunctionWrapper(staticLoopWithDynamicLoopNotEqual)},
#endif
	    {"staticLoopWithIf", details::createFunctionWrapper(staticLoopWithIf)},
	    {"staticIterator", details::createFunctionWrapper(staticIterator)},
	    {"staticConstIterator", details::createFunctionWrapper(staticConstIterator)},
	    {"staticLoopIncrement", details::createFunctionWrapper(staticLoopIncrement)},
	    {"staticWhileLoopDecrement", details::createFunctionWrapper(staticWhileLoopDecrement)},
	    {"staticPreIncrement", details::createFunctionWrapper(staticPreIncrement)},
	    {"staticPreDecrement", details::createFunctionWrapper(staticPreDecrement)},
	    {"staticNestedLoop", details::createFunctionWrapper(staticNestedLoop)},
	    {"staticLoopMultipleAccumulators", details::createFunctionWrapper(staticLoopMultipleAccumulators)},
	    {"staticLoopSingleIteration", details::createFunctionWrapper(staticLoopSingleIteration)},
	    {"staticCountdown", details::createFunctionWrapper(staticCountdown)},
	    {"staticLoopWithArg", details::createFunctionWrapper(staticLoopWithArg)},
	    {"staticSequentialLoops", details::createFunctionWrapper(staticSequentialLoops)},
	    {"staticLoopAccumulateCounter", details::createFunctionWrapper(staticLoopAccumulateCounter)},
	    {"staticIteratorSum", details::createFunctionWrapper(staticIteratorSum)},
	    {"staticRawArrayIterator", details::createFunctionWrapper(staticRawArrayIterator)},
	    {"staticStdArrayIterator", details::createFunctionWrapper(staticStdArrayIterator)},
	    {"staticEnumerateWeightedSum", details::createFunctionWrapper(staticEnumerateWeightedSum)},
	    {"staticEnumerateSum", details::createFunctionWrapper(staticEnumerateSum)}};
	runTraceTests("static-loop-tests", tests);
}

TEST_CASE("Value Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    // default constructor
	    {"constructAndAccess", details::createFunctionWrapper(constructAndAccess)},
	    {"constructSetBothFields", details::createFunctionWrapper(constructSetBothFields)},
	    {"constructNonTrivialDefault", details::createFunctionWrapper(constructNonTrivialDefault)},
	    // parameterised constructor
	    {"constructWithArgs", details::createFunctionWrapper(constructWithArgs)},
	    // copy constructor and copy assignment
	    {"copyConstruct", details::createFunctionWrapper(copyConstruct)},
	    {"copyAssign", details::createFunctionWrapper(copyAssign)},
	    {"copyConstructNonTrivial", details::createFunctionWrapper(copyConstructNonTrivial)},
	    // destructor
	    {"nonTrivialDestructor", details::createFunctionWrapper(nonTrivialDestructor)},
	    // loops
	    {"modifyInLoop", details::createFunctionWrapper(modifyInLoop)},
	    {"copyInLoop", details::createFunctionWrapper(copyInLoop)},
	    {"constructAndCall", details::createFunctionWrapper(constructAndCall)},

	};
	runTraceTests("value-tracing-tests", tests);
}

TEST_CASE("Pointer Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"getField", details::createFunctionWrapper(getField)},
	    {"setFieldConst", details::createFunctionWrapper(setFieldConst)},
	    {"setFieldIndirect", details::createFunctionWrapper(setFieldIndirect)},
	    {"customPointerAdd", details::createFunctionWrapper(customPointerAdd)},
	    {"pointerAdd", details::createFunctionWrapper(pointerAdd)},
	    {"pointerAddConst", details::createFunctionWrapper(pointerAddConst)},
	    {"pointerSub", details::createFunctionWrapper(pointerSub)},
	    {"pointerSubConst", details::createFunctionWrapper(pointerSubConst)},
	    {"ptrAssignment", details::createFunctionWrapper(ptrAssignment)},
	    {"load", details::createFunctionWrapper(load)},
	    {"loadConst", details::createFunctionWrapper(loadConst)},
	    {"castVoidPtr", details::createFunctionWrapper(castVoidPtr)},
	    {"isNullptr_i8", details::createFunctionWrapper(isNullptr<int8_t>)},
	    {"ptrEquals_i8", details::createFunctionWrapper(ptrEquals<int8_t>)},
	    {"ptrNotEquals_i8", details::createFunctionWrapper(ptrNotEquals<int8_t>)},
	    {"ptrLessThan_i8", details::createFunctionWrapper(ptrLessThan<int8_t>)},
	    {"ptrLessThanEquals_i8", details::createFunctionWrapper(ptrLessThanEquals<int8_t>)},
	    {"ptrGreaterThan_i8", details::createFunctionWrapper(ptrGreaterThan<int8_t>)},
	    {"ptrGreaterThanEquals_i8", details::createFunctionWrapper(ptrGreaterThanEquals<int8_t>)},
	    {"castPtrAndGetValue_i8", details::createFunctionWrapper(castPtrAndGetValue<int8_t, int16_t>)},
	    {"sumArray", details::createFunctionWrapper(sumArray)},
	    {"addArray_i8", details::createFunctionWrapper(addArray<int8_t>)},
	    {"callMemcpy", details::createFunctionWrapper(callMemcpy)},
	    {"passCustomClass", details::createFunctionWrapper(passCustomClass)},
	    {"castCustomClass", details::createFunctionWrapper(castCustomClass)},
	    {"specializeType", details::createFunctionWrapper(specializeType)},
	    {"useWrapper", details::createFunctionWrapper(useWrapper)},
	    // pointer arithmetic with different pointer element types (fixed int32_t offset)
	    {"pointerAdd_i8_i32", details::createFunctionWrapper(pointerAddInt<int8_t, int32_t>)},
	    {"pointerAdd_i16_i32", details::createFunctionWrapper(pointerAddInt<int16_t, int32_t>)},
	    {"pointerAdd_i64_i32", details::createFunctionWrapper(pointerAddInt<int64_t, int32_t>)},
	    // pointer arithmetic with different integer offset types (fixed int32_t pointer element)
	    {"pointerAdd_i32_i8", details::createFunctionWrapper(pointerAddInt<int32_t, int8_t>)},
	    {"pointerAdd_i32_i16", details::createFunctionWrapper(pointerAddInt<int32_t, int16_t>)},
	    {"pointerAdd_i32_i64", details::createFunctionWrapper(pointerAddInt<int32_t, int64_t>)},
	    {"pointerAdd_i32_ui32", details::createFunctionWrapper(pointerAddInt<int32_t, uint32_t>)},
	    {"pointerAdd_i32_ui64", details::createFunctionWrapper(pointerAddInt<int32_t, uint64_t>)},
	    // pointer subtraction with different types
	    {"pointerSub_i8_i32", details::createFunctionWrapper(pointerSubInt<int8_t, int32_t>)},
	    {"pointerSub_i16_i32", details::createFunctionWrapper(pointerSubInt<int16_t, int32_t>)},
	    {"pointerSub_i64_i32", details::createFunctionWrapper(pointerSubInt<int64_t, int32_t>)},
	    {"pointerSub_i32_i8", details::createFunctionWrapper(pointerSubInt<int32_t, int8_t>)},
	    {"pointerSub_i32_i16", details::createFunctionWrapper(pointerSubInt<int32_t, int16_t>)},
	    {"pointerSub_i32_i64", details::createFunctionWrapper(pointerSubInt<int32_t, int64_t>)},
	    // constant pointer arithmetic for different element types
	    {"pointerAddConst_i8", details::createFunctionWrapper(pointerAddConstInt<int8_t>)},
	    {"pointerAddConst_i16", details::createFunctionWrapper(pointerAddConstInt<int16_t>)},
	    {"pointerAddConst_i64", details::createFunctionWrapper(pointerAddConstInt<int64_t>)},
	    {"pointerSubConst_i8", details::createFunctionWrapper(pointerSubConstInt<int8_t>)},
	    {"pointerSubConst_i16", details::createFunctionWrapper(pointerSubConstInt<int16_t>)},
	    {"pointerSubConst_i64", details::createFunctionWrapper(pointerSubConstInt<int64_t>)},
	    // compound assignment operators: +=, -=, ++
	    {"pointerPlusAssign_i32_i32", details::createFunctionWrapper(pointerPlusAssign<int32_t, int32_t>)},
	    {"pointerPlusAssign_i8_i32", details::createFunctionWrapper(pointerPlusAssign<int8_t, int32_t>)},
	    {"pointerPlusAssign_i64_i32", details::createFunctionWrapper(pointerPlusAssign<int64_t, int32_t>)},
	    {"pointerPlusAssign_i32_i8", details::createFunctionWrapper(pointerPlusAssign<int32_t, int8_t>)},
	    {"pointerPlusAssign_i32_i64", details::createFunctionWrapper(pointerPlusAssign<int32_t, int64_t>)},
	    {"pointerMinusAssign_i32_i32", details::createFunctionWrapper(pointerMinusAssign<int32_t, int32_t>)},
	    {"pointerMinusAssign_i8_i32", details::createFunctionWrapper(pointerMinusAssign<int8_t, int32_t>)},
	    {"pointerMinusAssign_i64_i32", details::createFunctionWrapper(pointerMinusAssign<int64_t, int32_t>)},
	    {"pointerMinusAssign_i32_i8", details::createFunctionWrapper(pointerMinusAssign<int32_t, int8_t>)},
	    {"pointerMinusAssign_i32_i64", details::createFunctionWrapper(pointerMinusAssign<int32_t, int64_t>)},
	    {"pointerPreIncrement_i8", details::createFunctionWrapper(pointerPreIncrement<int8_t>)},
	    {"pointerPreIncrement_i32", details::createFunctionWrapper(pointerPreIncrement<int32_t>)},
	    {"pointerPreIncrement_i64", details::createFunctionWrapper(pointerPreIncrement<int64_t>)},
	    // previously missing pointer tests
	    {"isNotNullptr_i8", details::createFunctionWrapper(isNotNullptr<int8_t>)},
	    {"pointerAddNegativeOffset", details::createFunctionWrapper(pointerAddNegativeOffset)},
	    {"pointerRoundTrip", details::createFunctionWrapper(pointerRoundTrip)},
	    {"pointerMultiStep", details::createFunctionWrapper(pointerMultiStep)},
	    {"pointerLessThanAfterAdd", details::createFunctionWrapper(pointerLessThanAfterAdd)},
	    {"pointerGreaterThanAfterAdd", details::createFunctionWrapper(pointerGreaterThanAfterAdd)},
	};
	runTraceTests("pointer-tests", tests);
}

TEST_CASE("Cast Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"staticCast_i8_i8", details::createFunctionWrapper(staticCastExpression<int8_t, int8_t>)},
	    {"staticCast_i8_i16", details::createFunctionWrapper(staticCastExpression<int8_t, int16_t>)},
	    {"staticCast_i8_i32", details::createFunctionWrapper(staticCastExpression<int8_t, int32_t>)},
	    {"staticCast_i8_i64", details::createFunctionWrapper(staticCastExpression<int8_t, int64_t>)},
	    {"staticCast_i8_ui8", details::createFunctionWrapper(staticCastExpression<int8_t, uint8_t>)},
	    {"staticCast_i8_ui16", details::createFunctionWrapper(staticCastExpression<int8_t, uint16_t>)},
	    {"staticCast_i8_ui32", details::createFunctionWrapper(staticCastExpression<int8_t, uint32_t>)},
	    {"staticCast_i8_ui64", details::createFunctionWrapper(staticCastExpression<int8_t, uint64_t>)},
	    {"staticCast_i16_i8", details::createFunctionWrapper(staticCastExpression<int16_t, int8_t>)},
	    // i16 casts
	    {"staticCast_i16_i16", details::createFunctionWrapper(staticCastExpression<int16_t, int16_t>)},
	    {"staticCast_i16_i32", details::createFunctionWrapper(staticCastExpression<int16_t, int32_t>)},
	    {"staticCast_i16_i64", details::createFunctionWrapper(staticCastExpression<int16_t, int64_t>)},
	    {"staticCast_i16_ui8", details::createFunctionWrapper(staticCastExpression<int16_t, uint8_t>)},
	    {"staticCast_i16_ui16", details::createFunctionWrapper(staticCastExpression<int16_t, uint16_t>)},
	    {"staticCast_i16_ui32", details::createFunctionWrapper(staticCastExpression<int16_t, uint32_t>)},
	    {"staticCast_i16_ui64", details::createFunctionWrapper(staticCastExpression<int16_t, uint64_t>)},
	    // i32 casts
	    {"staticCast_i32_i8", details::createFunctionWrapper(staticCastExpression<int32_t, int8_t>)},
	    {"staticCast_i32_i16", details::createFunctionWrapper(staticCastExpression<int32_t, int16_t>)},
	    {"staticCast_i32_i32", details::createFunctionWrapper(staticCastExpression<int32_t, int32_t>)},
	    {"staticCast_i32_i64", details::createFunctionWrapper(staticCastExpression<int32_t, int64_t>)},
	    {"staticCast_i32_ui8", details::createFunctionWrapper(staticCastExpression<int32_t, uint8_t>)},
	    {"staticCast_i32_ui16", details::createFunctionWrapper(staticCastExpression<int32_t, uint16_t>)},
	    {"staticCast_i32_ui32", details::createFunctionWrapper(staticCastExpression<int32_t, uint32_t>)},
	    {"staticCast_i32_ui64", details::createFunctionWrapper(staticCastExpression<int32_t, uint64_t>)},
	    // i64 casts
	    {"staticCast_i64_i8", details::createFunctionWrapper(staticCastExpression<int64_t, int8_t>)},
	    {"staticCast_i64_i16", details::createFunctionWrapper(staticCastExpression<int64_t, int16_t>)},
	    {"staticCast_i64_i32", details::createFunctionWrapper(staticCastExpression<int64_t, int32_t>)},
	    {"staticCast_i64_i64", details::createFunctionWrapper(staticCastExpression<int64_t, int64_t>)},
	    {"staticCast_i64_ui8", details::createFunctionWrapper(staticCastExpression<int64_t, uint8_t>)},
	    {"staticCast_i64_ui16", details::createFunctionWrapper(staticCastExpression<int64_t, uint16_t>)},
	    {"staticCast_i64_ui32", details::createFunctionWrapper(staticCastExpression<int64_t, uint32_t>)},
	    {"staticCast_i64_ui64", details::createFunctionWrapper(staticCastExpression<int64_t, uint64_t>)},
	    // ui8 casts
	    {"staticCast_ui8_i8", details::createFunctionWrapper(staticCastExpression<uint8_t, int8_t>)},
	    {"staticCast_ui8_i16", details::createFunctionWrapper(staticCastExpression<uint8_t, int16_t>)},
	    {"staticCast_ui8_i32", details::createFunctionWrapper(staticCastExpression<uint8_t, int32_t>)},
	    {"staticCast_ui8_i64", details::createFunctionWrapper(staticCastExpression<uint8_t, int64_t>)},
	    {"staticCast_ui8_ui8", details::createFunctionWrapper(staticCastExpression<uint8_t, uint8_t>)},
	    {"staticCast_ui8_ui16", details::createFunctionWrapper(staticCastExpression<uint8_t, uint16_t>)},
	    {"staticCast_ui8_ui32", details::createFunctionWrapper(staticCastExpression<uint8_t, uint32_t>)},
	    {"staticCast_ui8_ui64", details::createFunctionWrapper(staticCastExpression<uint8_t, uint64_t>)},
	    // i16 casts
	    {"staticCast_ui16_i8", details::createFunctionWrapper(staticCastExpression<uint16_t, int8_t>)},
	    {"staticCast_ui16_i16", details::createFunctionWrapper(staticCastExpression<uint16_t, int16_t>)},
	    {"staticCast_ui16_i32", details::createFunctionWrapper(staticCastExpression<uint16_t, int32_t>)},
	    {"staticCast_ui16_i64", details::createFunctionWrapper(staticCastExpression<uint16_t, int64_t>)},
	    {"staticCast_ui16_ui8", details::createFunctionWrapper(staticCastExpression<uint16_t, uint8_t>)},
	    {"staticCast_ui16_ui16", details::createFunctionWrapper(staticCastExpression<uint16_t, uint16_t>)},
	    {"staticCast_ui16_ui32", details::createFunctionWrapper(staticCastExpression<uint16_t, uint32_t>)},
	    {"staticCast_ui16_ui64", details::createFunctionWrapper(staticCastExpression<uint16_t, uint64_t>)},
	    // ui32 casts
	    {"staticCast_ui32_i8", details::createFunctionWrapper(staticCastExpression<uint32_t, int8_t>)},
	    {"staticCast_ui32_i16", details::createFunctionWrapper(staticCastExpression<uint32_t, int16_t>)},
	    {"staticCast_ui32_i32", details::createFunctionWrapper(staticCastExpression<uint32_t, int32_t>)},
	    {"staticCast_ui32_i64", details::createFunctionWrapper(staticCastExpression<uint32_t, int64_t>)},
	    {"staticCast_ui32_ui8", details::createFunctionWrapper(staticCastExpression<uint32_t, uint8_t>)},
	    {"staticCast_ui32_ui16", details::createFunctionWrapper(staticCastExpression<uint32_t, uint16_t>)},
	    {"staticCast_ui32_ui32", details::createFunctionWrapper(staticCastExpression<uint32_t, uint32_t>)},
	    {"staticCast_ui32_ui64", details::createFunctionWrapper(staticCastExpression<uint32_t, uint64_t>)},
	    // i64 casts
	    {"staticCast_ui64_i8", details::createFunctionWrapper(staticCastExpression<uint64_t, int8_t>)},
	    {"staticCast_ui64_i16", details::createFunctionWrapper(staticCastExpression<uint64_t, int16_t>)},
	    {"staticCast_ui64_i32", details::createFunctionWrapper(staticCastExpression<uint64_t, int32_t>)},
	    {"staticCast_ui64_i64", details::createFunctionWrapper(staticCastExpression<uint64_t, int64_t>)},
	    {"staticCast_ui64_ui8", details::createFunctionWrapper(staticCastExpression<uint64_t, uint8_t>)},
	    {"staticCast_ui64_ui16", details::createFunctionWrapper(staticCastExpression<uint64_t, uint16_t>)},
	    {"staticCast_ui64_ui32", details::createFunctionWrapper(staticCastExpression<uint64_t, uint32_t>)},
	    {"staticCast_ui64_ui64", details::createFunctionWrapper(staticCastExpression<uint64_t, uint64_t>)},
	    // float casts
	    {"staticCast_f_i8", details::createFunctionWrapper(staticCastExpression<float, int8_t>)},
	    {"staticCast_f_i16", details::createFunctionWrapper(staticCastExpression<float, int16_t>)},
	    {"staticCast_f_i32", details::createFunctionWrapper(staticCastExpression<float, int32_t>)},
	    {"staticCast_f_i64", details::createFunctionWrapper(staticCastExpression<float, int64_t>)},
	    {"staticCast_f_ui8", details::createFunctionWrapper(staticCastExpression<float, uint8_t>)},
	    {"staticCast_f_ui16", details::createFunctionWrapper(staticCastExpression<float, uint16_t>)},
	    {"staticCast_f_ui32", details::createFunctionWrapper(staticCastExpression<float, uint32_t>)},
	    {"staticCast_f_ui64", details::createFunctionWrapper(staticCastExpression<float, uint64_t>)},
	    // double casts
	    {"staticCast_d_i8", details::createFunctionWrapper(staticCastExpression<double, int8_t>)},
	    {"staticCast_d_i16", details::createFunctionWrapper(staticCastExpression<double, int16_t>)},
	    {"staticCast_d_i32", details::createFunctionWrapper(staticCastExpression<double, int32_t>)},
	    {"staticCast_d_i64", details::createFunctionWrapper(staticCastExpression<double, int64_t>)},
	    {"staticCast_d_ui8", details::createFunctionWrapper(staticCastExpression<double, uint8_t>)},
	    {"staticCast_d_ui16", details::createFunctionWrapper(staticCastExpression<double, uint16_t>)},
	    {"staticCast_d_ui32", details::createFunctionWrapper(staticCastExpression<double, uint32_t>)},
	    {"staticCast_d_ui64", details::createFunctionWrapper(staticCastExpression<double, uint64_t>)},

	    {"implicitCast_i8_i8", details::createFunctionWrapper(implicitCastExpression<int8_t, int8_t>)},
	    {"implicitCast_i8_i16", details::createFunctionWrapper(implicitCastExpression<int8_t, int16_t>)},
	    {"implicitCast_i8_i32", details::createFunctionWrapper(implicitCastExpression<int8_t, int32_t>)},
	    {"implicitCast_i8_i64", details::createFunctionWrapper(implicitCastExpression<int8_t, int64_t>)},
	    {"implicitCast_i8_ui8", details::createFunctionWrapper(implicitCastExpression<int8_t, uint8_t>)},
	    {"implicitCast_i8_ui16", details::createFunctionWrapper(implicitCastExpression<int8_t, uint16_t>)},
	    {"implicitCast_i8_ui32", details::createFunctionWrapper(implicitCastExpression<int8_t, uint32_t>)},
	    {"implicitCast_i8_ui64", details::createFunctionWrapper(implicitCastExpression<int8_t, uint64_t>)},
	    {"implicitCast_i16_i8", details::createFunctionWrapper(implicitCastExpression<int16_t, int8_t>)},
	    // i16 casts
	    {"implicitCast_i16_i16", details::createFunctionWrapper(implicitCastExpression<int16_t, int16_t>)},
	    {"implicitCast_i16_i32", details::createFunctionWrapper(implicitCastExpression<int16_t, int32_t>)},
	    {"implicitCast_i16_i64", details::createFunctionWrapper(implicitCastExpression<int16_t, int64_t>)},
	    {"implicitCast_i16_ui8", details::createFunctionWrapper(implicitCastExpression<int16_t, uint8_t>)},
	    {"implicitCast_i16_ui16", details::createFunctionWrapper(implicitCastExpression<int16_t, uint16_t>)},
	    {"implicitCast_i16_ui32", details::createFunctionWrapper(implicitCastExpression<int16_t, uint32_t>)},
	    {"implicitCast_i16_ui64", details::createFunctionWrapper(implicitCastExpression<int16_t, uint64_t>)},
	    // i32 casts
	    {"implicitCast_i32_i8", details::createFunctionWrapper(implicitCastExpression<int32_t, int8_t>)},
	    {"implicitCast_i32_i16", details::createFunctionWrapper(implicitCastExpression<int32_t, int16_t>)},
	    {"implicitCast_i32_i32", details::createFunctionWrapper(implicitCastExpression<int32_t, int32_t>)},
	    {"implicitCast_i32_i64", details::createFunctionWrapper(implicitCastExpression<int32_t, int64_t>)},
	    {"implicitCast_i32_ui8", details::createFunctionWrapper(implicitCastExpression<int32_t, uint8_t>)},
	    {"implicitCast_i32_ui16", details::createFunctionWrapper(implicitCastExpression<int32_t, uint16_t>)},
	    {"implicitCast_i32_ui32", details::createFunctionWrapper(implicitCastExpression<int32_t, uint32_t>)},
	    {"implicitCast_i32_ui64", details::createFunctionWrapper(implicitCastExpression<int32_t, uint64_t>)},
	    // i64 casts
	    {"implicitCast_i64_i8", details::createFunctionWrapper(implicitCastExpression<int64_t, int8_t>)},
	    {"implicitCast_i64_i16", details::createFunctionWrapper(implicitCastExpression<int64_t, int16_t>)},
	    {"implicitCast_i64_i32", details::createFunctionWrapper(implicitCastExpression<int64_t, int32_t>)},
	    {"implicitCast_i64_i64", details::createFunctionWrapper(implicitCastExpression<int64_t, int64_t>)},
	    {"implicitCast_i64_ui8", details::createFunctionWrapper(implicitCastExpression<int64_t, uint8_t>)},
	    {"implicitCast_i64_ui16", details::createFunctionWrapper(implicitCastExpression<int64_t, uint16_t>)},
	    {"implicitCast_i64_ui32", details::createFunctionWrapper(implicitCastExpression<int64_t, uint32_t>)},
	    {"implicitCast_i64_ui64", details::createFunctionWrapper(implicitCastExpression<int64_t, uint64_t>)},
	    // ui8 casts
	    {"implicitCast_ui8_i8", details::createFunctionWrapper(implicitCastExpression<uint8_t, int8_t>)},
	    {"implicitCast_ui8_i16", details::createFunctionWrapper(implicitCastExpression<uint8_t, int16_t>)},
	    {"implicitCast_ui8_i32", details::createFunctionWrapper(implicitCastExpression<uint8_t, int32_t>)},
	    {"implicitCast_ui8_i64", details::createFunctionWrapper(implicitCastExpression<uint8_t, int64_t>)},
	    {"implicitCast_ui8_ui8", details::createFunctionWrapper(implicitCastExpression<uint8_t, uint8_t>)},
	    {"implicitCast_ui8_ui16", details::createFunctionWrapper(implicitCastExpression<uint8_t, uint16_t>)},
	    {"implicitCast_ui8_ui32", details::createFunctionWrapper(implicitCastExpression<uint8_t, uint32_t>)},
	    {"implicitCast_ui8_ui64", details::createFunctionWrapper(implicitCastExpression<uint8_t, uint64_t>)},
	    // i16 casts
	    {"implicitCast_ui16_i8", details::createFunctionWrapper(implicitCastExpression<uint16_t, int8_t>)},
	    {"implicitCast_ui16_i16", details::createFunctionWrapper(implicitCastExpression<uint16_t, int16_t>)},
	    {"implicitCast_ui16_i32", details::createFunctionWrapper(implicitCastExpression<uint16_t, int32_t>)},
	    {"implicitCast_ui16_i64", details::createFunctionWrapper(implicitCastExpression<uint16_t, int64_t>)},
	    {"implicitCast_ui16_ui8", details::createFunctionWrapper(implicitCastExpression<uint16_t, uint8_t>)},
	    {"implicitCast_ui16_ui16", details::createFunctionWrapper(implicitCastExpression<uint16_t, uint16_t>)},
	    {"implicitCast_ui16_ui32", details::createFunctionWrapper(implicitCastExpression<uint16_t, uint32_t>)},
	    {"implicitCast_ui16_ui64", details::createFunctionWrapper(implicitCastExpression<uint16_t, uint64_t>)},
	    // ui32 casts
	    {"implicitCast_ui32_i8", details::createFunctionWrapper(implicitCastExpression<uint32_t, int8_t>)},
	    {"implicitCast_ui32_i16", details::createFunctionWrapper(implicitCastExpression<uint32_t, int16_t>)},
	    {"implicitCast_ui32_i32", details::createFunctionWrapper(implicitCastExpression<uint32_t, int32_t>)},
	    {"implicitCast_ui32_i64", details::createFunctionWrapper(implicitCastExpression<uint32_t, int64_t>)},
	    {"implicitCast_ui32_ui8", details::createFunctionWrapper(implicitCastExpression<uint32_t, uint8_t>)},
	    {"implicitCast_ui32_ui16", details::createFunctionWrapper(implicitCastExpression<uint32_t, uint16_t>)},
	    {"implicitCast_ui32_ui32", details::createFunctionWrapper(implicitCastExpression<uint32_t, uint32_t>)},
	    {"implicitCast_ui32_ui64", details::createFunctionWrapper(implicitCastExpression<uint32_t, uint64_t>)},
	    // i64 casts
	    {"implicitCast_ui64_i8", details::createFunctionWrapper(implicitCastExpression<uint64_t, int8_t>)},
	    {"implicitCast_ui64_i16", details::createFunctionWrapper(implicitCastExpression<uint64_t, int16_t>)},
	    {"implicitCast_ui64_i32", details::createFunctionWrapper(implicitCastExpression<uint64_t, int32_t>)},
	    {"implicitCast_ui64_i64", details::createFunctionWrapper(implicitCastExpression<uint64_t, int64_t>)},
	    {"implicitCast_ui64_ui8", details::createFunctionWrapper(implicitCastExpression<uint64_t, uint8_t>)},
	    {"implicitCast_ui64_ui16", details::createFunctionWrapper(implicitCastExpression<uint64_t, uint16_t>)},
	    {"implicitCast_ui64_ui32", details::createFunctionWrapper(implicitCastExpression<uint64_t, uint32_t>)},
	    {"implicitCast_ui64_ui64", details::createFunctionWrapper(implicitCastExpression<uint64_t, uint64_t>)},
	    // float casts
	    {"implicitCast_f_i8", details::createFunctionWrapper(implicitCastExpression<float, int8_t>)},
	    {"implicitCast_f_i16", details::createFunctionWrapper(implicitCastExpression<float, int16_t>)},
	    {"implicitCast_f_i32", details::createFunctionWrapper(implicitCastExpression<float, int32_t>)},
	    {"implicitCast_f_i64", details::createFunctionWrapper(implicitCastExpression<float, int64_t>)},
	    {"implicitCast_f_ui8", details::createFunctionWrapper(implicitCastExpression<float, uint8_t>)},
	    {"implicitCast_f_ui16", details::createFunctionWrapper(implicitCastExpression<float, uint16_t>)},
	    {"implicitCast_f_ui32", details::createFunctionWrapper(implicitCastExpression<float, uint32_t>)},
	    {"implicitCast_f_ui64", details::createFunctionWrapper(implicitCastExpression<float, uint64_t>)},
	    // double casts
	    {"implicitCast_d_i8", details::createFunctionWrapper(implicitCastExpression<double, int8_t>)},
	    {"implicitCast_d_i16", details::createFunctionWrapper(implicitCastExpression<double, int16_t>)},
	    {"implicitCast_d_i32", details::createFunctionWrapper(implicitCastExpression<double, int32_t>)},
	    {"implicitCast_d_i64", details::createFunctionWrapper(implicitCastExpression<double, int64_t>)},
	    {"implicitCast_d_ui8", details::createFunctionWrapper(implicitCastExpression<double, uint8_t>)},
	    {"implicitCast_d_ui16", details::createFunctionWrapper(implicitCastExpression<double, uint16_t>)},
	    {"implicitCast_d_ui32", details::createFunctionWrapper(implicitCastExpression<double, uint32_t>)},
	    {"implicitCast_d_ui64", details::createFunctionWrapper(implicitCastExpression<double, uint64_t>)},
	};
	runTraceTests("cast-tests", tests);
}

void store_missing_downcast(val<unsigned int*> v) {
	*v = (val<long>) -1;
}

TEST_CASE("Regressions") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"store_mising_downcast-gh_#90", details::createFunctionWrapper(store_missing_downcast)}};
	runTraceTests("regressions", tests);
}

TEST_CASE("Select Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"selectInt32True", details::createFunctionWrapper(selectInt32True)},
	    {"selectInt64True", details::createFunctionWrapper(selectInt64True)},
	    {"selectUInt32", details::createFunctionWrapper(selectUInt32)},
	    {"selectUInt64", details::createFunctionWrapper(selectUInt64)},
	    {"selectFloat", details::createFunctionWrapper(selectFloat)},
	    {"selectDouble", details::createFunctionWrapper(selectDouble)},
	    {"selectBool", details::createFunctionWrapper(selectBool)},
	    {"selectInt8", details::createFunctionWrapper(selectInt8)},
	    {"selectInt16", details::createFunctionWrapper(selectInt16)},
	    {"selectInLoop", details::createFunctionWrapper(selectInLoop)},
	    {"selectBasedOnComparison", details::createFunctionWrapper(selectBasedOnComparison)},
	    {"nestedSelect", details::createFunctionWrapper(nestedSelect)},
	    {"selectPointer", details::createFunctionWrapper(selectPointer)},
	    {"selectPointerAndDeref", details::createFunctionWrapper(selectPointerAndDeref)},
	};
	runTraceTests("expression-tests", tests);
}

TEST_CASE("Function Ptr Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"fnPtrIsNull", details::createFunctionWrapper(fnPtrIsNull)},
	    {"fnPtrNotNull", details::createFunctionWrapper(fnPtrNotNull)},
	    {"fnPtrEquals", details::createFunctionWrapper(fnPtrEquals)},
	    {"fnPtrNotEquals", details::createFunctionWrapper(fnPtrNotEquals)},
	    {"callThroughFnPtr", details::createFunctionWrapper(callThroughFnPtr)},
	    {"callVoidFnPtr", details::createFunctionWrapper(callVoidFnPtr)},
	    {"fnPtrToVoidPtr", details::createFunctionWrapper(fnPtrToVoidPtr)},
	    {"fnPtrAsTypedArg", details::createFunctionWrapper(fnPtrAsTypedArg)},
	    {"fnPtrCopyAndCall", details::createFunctionWrapper(fnPtrCopyAndCall)},
	    {"fnPtrAssignAndCall", details::createFunctionWrapper(fnPtrAssignAndCall)},
	    {"selectFnPtr", details::createFunctionWrapper(selectFnPtr)},
	    {"fnPtrInLoop", details::createFunctionWrapper(fnPtrInLoop)},
	    {"fnPtrNullBranch", details::createFunctionWrapper(fnPtrNullBranch)},
	    {"fnPtrFromLambda", details::createFunctionWrapper(fnPtrFromLambda)},
	    {"fnPtrRoundtripVoidPtr", details::createFunctionWrapper(fnPtrRoundtripVoidPtr)},
	    {"fnPtrInlineConst", details::createFunctionWrapper(fnPtrInlineConst)},
	    {"fnPtrInlineConstUnary", details::createFunctionWrapper(fnPtrInlineConstUnary)},
	};
	runTraceTests("function-ptr-tests", tests);
}

<<<<<<< HEAD
TEST_CASE("Nautilus Function Call Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"nautilusFunction", details::createFunctionWrapper(nautilusFunction)},
	    {"nautilusFunction2", details::createFunctionWrapper(nautilusFunction2)},
	    {"nautilusFunctionVoid", details::createFunctionWrapper(nautilusFunctionVoid)},
	    {"nautilusFunctionMultipleResults", details::createFunctionWrapper(nautilusFunctionMultipleResults)},
	    {"nautilusFunctionNested", details::createFunctionWrapper(nautilusFunctionNested)},
	    {"nautilusFunctionConditional", details::createFunctionWrapper(nautilusFunctionConditional)},
	    {"nautilusFunctionLoop", details::createFunctionWrapper(nautilusFunctionLoop)},
	    {"nautilusFunctionMultiple", details::createFunctionWrapper(nautilusFunctionMultiple)},
	    {"nautilusFunctionRecursiveStyle", details::createFunctionWrapper(nautilusFunctionRecursiveStyle)},
	    {"nautilusFunctionComplex", details::createFunctionWrapper(nautilusFunctionComplex)},
	    {"nautilusFunctionInline", details::createFunctionWrapper(nautilusFunctionInline)},
	    {"nautilusFunctionInlineLambda", details::createFunctionWrapper(nautilusFunctionInlineLambda)},
	    {"nautilusFunctionInlineMember", details::createFunctionWrapper(nautilusFunctionInlineMember)},
	    {"nautilusFunctionMultipleInline", details::createFunctionWrapper(nautilusFunctionMultipleInline)},
	    {"nautilusFunctionGetFuncPtr", details::createFunctionWrapper(nautilusFunctionGetFuncPtr)},
	};
	runTraceTests("nautilus-function-call-tests", tests);
}
=======
TEST_CASE("Bit Intrinsic Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"countlZeroFunction", details::createFunctionWrapper(countlZeroFunction)},
	    {"countrZeroFunction", details::createFunctionWrapper(countrZeroFunction)},
	    {"popcountFunction", details::createFunctionWrapper(popcountFunction)},
	    {"countlZero64Function", details::createFunctionWrapper(countlZero64Function)},
	    {"countrZero64Function", details::createFunctionWrapper(countrZero64Function)},
	    {"popcount64Function", details::createFunctionWrapper(popcount64Function)},
	    {"byteswap32Function", details::createFunctionWrapper(byteswap32Function)},
	    {"byteswap64Function", details::createFunctionWrapper(byteswap64Function)},
	    {"rotlFunction", details::createFunctionWrapper(rotlFunction)},
	    {"rotrFunction", details::createFunctionWrapper(rotrFunction)},
	    {"rotl64Function", details::createFunctionWrapper(rotl64Function)},
	    {"rotr64Function", details::createFunctionWrapper(rotr64Function)},
	};
	runTraceTests("bit-intrinsic-tests", tests);
}

TEST_CASE("CMath Intrinsic Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    // unary operations
	    {"sinFunction", details::createFunctionWrapper(sinFunction)},
	    {"cosFunction", details::createFunctionWrapper(cosFunction)},
	    {"sqrtFunction", details::createFunctionWrapper(sqrtFunction)},
	    {"expFunction", details::createFunctionWrapper(expFunction)},
	    {"logFunction", details::createFunctionWrapper(logFunction)},
	    {"ceilFunction", details::createFunctionWrapper(ceilFunction)},
	    {"floorFunction", details::createFunctionWrapper(floorFunction)},
	    {"fabsFunction", details::createFunctionWrapper(fabsFunction)},
	    {"tanFunction", details::createFunctionWrapper(tanFunction)},
	    {"asinFunction", details::createFunctionWrapper(asinFunction)},
	    {"acosFunction", details::createFunctionWrapper(acosFunction)},
	    {"atanFunction", details::createFunctionWrapper(atanFunction)},
	    // binary operations
	    {"powFunction", details::createFunctionWrapper(powFunction)},
	    {"atan2Function", details::createFunctionWrapper(atan2Function)},
	    {"fminFunction", details::createFunctionWrapper(fminFunction)},
	    {"fmaxFunction", details::createFunctionWrapper(fmaxFunction)},
	    {"fmodFunction", details::createFunctionWrapper(fmodFunction)},
	    {"copysignFunction", details::createFunctionWrapper(copysignFunction)},
	    // ternary operation
	    {"fmaFunction", details::createFunctionWrapper(fmaFunction)},
	};
	runTraceTests("cmath-intrinsic-tests", tests);
}

TEST_CASE("Memory Intrinsic Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"memcpyFunction", details::createFunctionWrapper(memcpyFunction)},
	    {"memmoveFunction", details::createFunctionWrapper(memmoveFunction)},
	    {"memsetFunction", details::createFunctionWrapper(memsetFunction)},
	    {"memcpyBytesFunction", details::createFunctionWrapper(memcpyBytesFunction)},
	    {"memmoveOverlapFunction", details::createFunctionWrapper(memmoveOverlapFunction)},
	    {"memsetZeroFunction", details::createFunctionWrapper(memsetZeroFunction)},
	};
	runTraceTests("memory-intrinsic-tests", tests);
}

TEST_CASE("Profile Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"assumeFunction", details::createFunctionWrapper(assumeFunction)},
	    {"assumeComplexCondition", details::createFunctionWrapper(assumeComplexCondition)},
	    {"assumeAlignedFunction", details::createFunctionWrapper(assumeAlignedFunction)},
	};
	runTraceTests("profile-tests", tests);
}

TEST_CASE("Cross-Type Arithmetic Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    // multiplication across types
	    {"mulAWithB_i8_i32_i32", details::createFunctionWrapper(mulAWithB<int8_t, int32_t, int32_t>)},
	    {"mulAWithB_i32_i64_i64", details::createFunctionWrapper(mulAWithB<int32_t, int64_t, int64_t>)},
	    {"mulAWithB_i16_i32_i32", details::createFunctionWrapper(mulAWithB<int16_t, int32_t, int32_t>)},
	    {"mulAWithB_f_d_d", details::createFunctionWrapper(mulAWithB<float, double, double>)},
	    // addition across types
	    {"addAWithB_i8_i32_i32", details::createFunctionWrapper(addAWithB<int8_t, int32_t, int32_t>)},
	    {"addAWithB_i32_i64_i64", details::createFunctionWrapper(addAWithB<int32_t, int64_t, int64_t>)},
	    {"addAWithB_f_d_d", details::createFunctionWrapper(addAWithB<float, double, double>)},
	    // subtraction across types
	    {"subAWithB_i8_i32_i32", details::createFunctionWrapper(subAWithB<int8_t, int32_t, int32_t>)},
	    {"subAWithB_i32_i64_i64", details::createFunctionWrapper(subAWithB<int32_t, int64_t, int64_t>)},
	    {"subAWithB_f_d_d", details::createFunctionWrapper(subAWithB<float, double, double>)},
	    // division across types
	    {"divAWithB_i8_i32_i32", details::createFunctionWrapper(divAWithB<int8_t, int32_t, int32_t>)},
	    {"divAWithB_i32_i64_i64", details::createFunctionWrapper(divAWithB<int32_t, int64_t, int64_t>)},
	    {"divAWithB_f_d_d", details::createFunctionWrapper(divAWithB<float, double, double>)},
	};
	runTraceTests("cross-type-arithmetic-tests", tests);
}

>>>>>>> 07cfaa9 (Add trace tests for intrinsics, cross-type arithmetic, and remaining gaps)
} // namespace nautilus::engine
