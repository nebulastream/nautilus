#include "ControlFlowFunctions.hpp"
#include "ExpressionFunctions.hpp"
#include "LoopFunctions.hpp"
#include "PointerFunctions.hpp"
#include "RunctimeCallFunctions.hpp"
#include "StaticLoopFunctions.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/config.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace nautilus::engine {

template <typename T>
void writeTestFile(T* trace, std::string group, std::string name) {
	// Create an ofstream object for writing to a file
	std::ofstream file(std::string(TEST_DATA_FOLDER) + "/" + group + "/" + name + ".trace");
	// Check if the file is open
	if (file.is_open()) {
		// Write the string to the file
		file << *trace;
		// Close the file
		file.close();
	} else {
		// Error message if the file couldn't be opened
		std::cerr << "Unable to open file";
	}
}

template <typename T>
bool checkTestFile(T* trace, std::string group, const std::string& name) {
	if (trace == nullptr) {
		std::cerr << "Trace pointer is null\n";
		return false;
	}

	auto groupDir = std::string(TEST_DATA_FOLDER) + "/" + group + "/";
	if (!std::filesystem::exists(groupDir)) {
		std::filesystem::create_directories(groupDir);
	}

	// Check if the file exists
	std::string filePath = groupDir + name + ".trace";
	if (!std::filesystem::exists(filePath)) {
		std::cerr << "File does not exist: " << filePath << "\n";
		writeTestFile(trace, group, name);
		return true;
	}

	std::ifstream file(filePath);
	std::stringstream targetString;
	targetString << *trace; // Ensure this operation is valid

	if (!file.is_open()) {
		std::cerr << "Unable to open file\n";
		writeTestFile(trace, group, name);
		return false;
	}

	std::string fileLine, stringLine;
	bool differencesFound = false;
	int lineNumber = 1;

	while (true) {
		auto& fileLineExists = std::getline(file, fileLine);
		auto& stringLineExists = std::getline(targetString, stringLine);

		if (!(bool) fileLineExists || !(bool) stringLineExists) {
			// Handle case where lines are exhausted in either stream
			if (fileLineExists) {
				differencesFound = true;
				std::cout << "Extra line in file at line " << lineNumber << ": " << fileLine << "\n";
			}
			if (stringLineExists) {
				differencesFound = true;
				std::cout << "Extra line in string at line " << lineNumber << ": " << stringLine << "\n";
			}
			break;
		}

		if (fileLine != stringLine) {
			differencesFound = true;
			std::cout << "Difference in line " << lineNumber << ":\n"
			          << "File: " << fileLine << "\n"
			          << "String: " << stringLine << "\n";
		}
		lineNumber++;
	}

	file.close();

	if (differencesFound) {
		std::cout << "Got Trace" << std::endl;
		std::cout << *trace << std::endl;
	}
	return !differencesFound;
}

void runTraceTests(std::vector<std::tuple<std::string, std::function<void()>>>& tests) {
	for (auto& test : tests) {
		auto func = std::get<1>(test);
		auto name = std::get<0>(test);
		auto executionTrace = tracing::TraceContext::trace(func);
		DYNAMIC_SECTION(name) {
			DYNAMIC_SECTION("tracing") {
				REQUIRE(checkTestFile(executionTrace.get(), "tracing", name));
			}
			auto ssaCreationPhase = tracing::SSACreationPhase();
			auto afterSSA = ssaCreationPhase.apply(std::move(executionTrace));
			DYNAMIC_SECTION("after_ssa") {
				REQUIRE(checkTestFile<tracing::ExecutionTrace>(afterSSA.get(), "after_ssa", name));
			}
			DYNAMIC_SECTION("ir") {
				auto irGenerationPhase = tracing::TraceToIRConversionPhase();
				[[maybe_unused]] auto ir = irGenerationPhase.apply(std::move(afterSSA));
				REQUIRE(checkTestFile<compiler::ir::IRGraph>(ir.get(), "ir", name));
			}
		}
	}
}

TEST_CASE("Expression Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"assignShr", details::createFunctionWrapper(assignShr)},
	    {"assignShl", details::createFunctionWrapper(assignShl)},
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
	    {"castInt8ToInt64AddExpression2", details::createFunctionWrapper(castInt8ToInt64AddExpression2)}};
	runTraceTests(tests);
}
TEST_CASE("Control-flow Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"ifThenCondition", details::createFunctionWrapper(ifThenCondition)},
	    {"conditionalReturn", details::createFunctionWrapper(conditionalReturn)},
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
	    {"ifWithTernary", details::createFunctionWrapper(ifWithTernary)},
	    {"complexLogicalExpressions", details::createFunctionWrapper(complexLogicalExpressions)},
	    {"shortCircuitEvaluation", details::createFunctionWrapper(shortCircuitEvaluation)},
	    {"ifWithFunctionCall", details::createFunctionWrapper(ifWithFunctionCall)},
	    {"compoundStatements", details::createFunctionWrapper(compoundStatements)},
	    {"varyingComplexity", details::createFunctionWrapper(varyingComplexity)},
	    {"logicalXOR", details::createFunctionWrapper(logicalXOR)},
	    {"nestedIfElseDifferentLevels", details::createFunctionWrapper(nestedIfElseDifferentLevels)},
	};
	runTraceTests(tests);
}

TEST_CASE("Loop Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"castVoidPtr", details::createFunctionWrapper(castVoidPtr)},
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
	    {"sumArray", details::createFunctionWrapper(sumArray)},
	    {"addArrayInt32", details::createFunctionWrapper(addArray<int32_t>)},
	    {"simpleDirectCall", details::createFunctionWrapper(simpleDirectCall)},
	    {"loopDirectCall", details::createFunctionWrapper(loopDirectCall)},
	    {"passCustomStruct", details::createFunctionWrapper(passCustomStruct)},
	    {"specializeType", details::createFunctionWrapper(specializeType)},
	    {"useWrapper", details::createFunctionWrapper(useWrapper)},
	    {"voidFuncCall", details::createFunctionWrapper(voidFuncCall)}};
	runTraceTests(tests);
}

TEST_CASE("Static Trace Test") {
	auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
	    {"staticLoop", details::createFunctionWrapper(staticLoop)},
	    // {"staticLoopWithIf", details::createFunctionWrapper(staticLoopWithIf)},
	    {"staticLoopWithDynamicLoop", details::createFunctionWrapper(staticLoopWithDynamicLoop)},
	    // {"staticIterator", details::createFunctionWrapper(staticIterator)},
	    {"staticLoopIncrement", details::createFunctionWrapper(staticLoopIncrement)},
	    {"staticWhileLoopDecrement", details::createFunctionWrapper(staticWhileLoopDecrement)}};
	runTraceTests(tests);
}
} // namespace nautilus::engine
