#include "BoolOperations.hpp"
#include "CastFunctions.hpp"
#include "ControlFlowFunctions.hpp"
#include "EnumFunction.hpp"
#include "ExpressionFunctions.hpp"
#include "LoopFunctions.hpp"
#include "PointerFunctions.hpp"
#include "RunctimeCallFunctions.hpp"
#include "StaticLoopFunctions.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include <catch2/catch_all.hpp>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace nautilus::log::options {

bool getLogAddresses();
void setLogAddresses(bool);

} // namespace nautilus::log::options
namespace nautilus::engine {

namespace details {
// old tracing functions for functions pointers
template <size_t... Indices, typename R, typename... FunctionArguments>
std::function<void()> createFunctionWrapper(std::index_sequence<Indices...>, R (*fnptr)(FunctionArguments...)) {
	[[maybe_unused]] std::size_t args = sizeof...(FunctionArguments);
	auto traceFunc = [=]() {
		if constexpr (std::is_void_v<R>) {
			fnptr(details::createTraceableArgument<FunctionArguments, Indices>()...);
			tracing::traceReturnOperation(Type::v, tracing::TypedValueRef());
		} else {
			auto returnValue = fnptr(details::createTraceableArgument<FunctionArguments, Indices>()...);
			auto type = tracing::TypeResolver<typename decltype(returnValue)::raw_type>::to_type();
			tracing::traceReturnOperation(type, returnValue.state);
		}
	};
	return traceFunc;
}

template <typename R, typename... FunctionArguments>
std::function<void()> createFunctionWrapper(R (*fnptr)(FunctionArguments...)) {
	return createFunctionWrapper(std::make_index_sequence<sizeof...(FunctionArguments)> {}, fnptr);
}
} // namespace details

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
	mkstemp(tmpName);
	std::ofstream tmpfile {tmpName};
	tmpfile << actual;
	std::cout << "Trace mismatch: (exp vs act) " << filePath << " " << tmpName << std::endl;
	return false;
}

void runTraceTests(const std::string& category, std::vector<std::tuple<std::string, std::function<void()>>>& tests) {
	// disable logging of addresses such that the trace is deterministic
	nautilus::log::options::setLogAddresses(false);
	for (auto& test : tests) {
		auto func = std::get<1>(test);
		auto name = std::get<0>(test);
		DYNAMIC_SECTION(name) {
			auto executionTrace = tracing::TraceContext::trace(func);
			DYNAMIC_SECTION("tracing") {
				REQUIRE(checkTestFile(executionTrace.get()->toString(), category, "tracing", name));
			}
			auto ssaCreationPhase = tracing::SSACreationPhase();
			auto afterSSA = ssaCreationPhase.apply(std::move(executionTrace));
			DYNAMIC_SECTION("after_ssa") {
				REQUIRE(checkTestFile(afterSSA.get()->toString(), category, "after_ssa", name));
			}
			DYNAMIC_SECTION("ir") {
				auto irGenerationPhase = tracing::TraceToIRConversionPhase();
				[[maybe_unused]] auto ir = irGenerationPhase.apply(std::move(afterSSA));
				REQUIRE(checkTestFile(ir.get()->toString(), category, "ir", name));
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
	    {"constructComplexReturnObject2", details::createFunctionWrapper(constructComplexReturnObject2)}

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
	    {"callTwoFunctions", details::createFunctionWrapper(callTwoFunctions)}};
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
	};
	runTraceTests("bool-tests", tests);
}

TEST_CASE("Static Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"staticLoop", details::createFunctionWrapper(staticLoop)},
	    // this test is sensitive to compiler options
	    //{"staticLoopWithIf", details::createFunctionWrapper(staticLoopWithIf)},
	    //{"staticLoopWithDynamicLoop",
	    // details::createFunctionWrapper(staticLoopWithDynamicLoop)},
	    {"staticIterator", details::createFunctionWrapper(staticIterator)},
	    {"staticConstIterator", details::createFunctionWrapper(staticConstIterator)},
	    {"staticLoopIncrement", details::createFunctionWrapper(staticLoopIncrement)},
	    {"staticWhileLoopDecrement", details::createFunctionWrapper(staticWhileLoopDecrement)}};
	runTraceTests("static-loop-tests", tests);
}

TEST_CASE("Pointer Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
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
} // namespace nautilus::engine
