#include "ControlFlowFunctions.hpp"
#include "ExpressionFunctions.hpp"
#include "EnumFunction.h"
#include "LoopFunctions.hpp"
#include "PointerFunctions.hpp"
#include "RunctimeCallFunctions.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val_concepts.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

void addTest(engine::NautilusEngine& engine) {

	SECTION("callEnumFunction") {
		auto f = engine.registerFunction(callEnumFunction);
		REQUIRE(f(Color::BLUE) == 42);
		REQUIRE(f(Color::GREEN) == 1);
	}

	SECTION("handleEnum") {
		auto f = engine.registerFunction(handleEnum);
		REQUIRE(f(Color::BLUE) == 1);
		REQUIRE(f(Color::GREEN) == 0);
	}

	SECTION("incrementPost") {
		auto f = engine.registerFunction(incrementPost);
		REQUIRE(f(1) == 3);
		REQUIRE(f(0) == 1);
	}
	SECTION("incrementPre") {
		auto f = engine.registerFunction(incrementPre);
		REQUIRE(f(1) == 4);
		REQUIRE(f(0) == 2);
	}
	SECTION("decrementPost") {
		auto f = engine.registerFunction(decrementPost);
		REQUIRE(f(1) == 1);
		REQUIRE(f(0) == -1);
	}
	SECTION("decrementPre") {
		auto f = engine.registerFunction(decrementPre);
		REQUIRE(f(1) == 0);
		REQUIRE(f(0) == -2);
	}
	SECTION("assignAdd") {
		auto f = engine.registerFunction(assignAdd);
		REQUIRE(f(1) == 6);
		REQUIRE(f(0) == 5);
	}
	SECTION("assignSub") {
		auto f = engine.registerFunction(assignSub);
		REQUIRE(f(5) == 0);
		REQUIRE(f(1) == -4);
	}
	SECTION("assignMul") {
		auto f = engine.registerFunction(assignMul);
		REQUIRE(f(2) == 10);
		REQUIRE(f(10) == 50);
	}
	SECTION("assignDiv") {
		auto f = engine.registerFunction(assignDiv);
		REQUIRE(f(5) == 1);
		REQUIRE(f(10) == 2);
	}
	SECTION("assignMod") {
		auto f = engine.registerFunction(assignMod);
		REQUIRE(f(7) == 2);
		REQUIRE(f(5) == 0);
	}
	/*
	SECTION("assignAnd") {
	    auto f = engine.registerFunction(assignAnd);
	    REQUIRE(f(7) == 5);
	    REQUIRE(f(5) == 5);
	}
	SECTION("assignOr") {
	    auto f = engine.registerFunction(assignOr);
	    REQUIRE(f(7) == 7);
	    REQUIRE(f(5) == 5);
	} SECTION("assignXor") {
	    auto f = engine.registerFunction(assignXor);
	    REQUIRE(f(7) == 2);
	    REQUIRE(f(5) == 0);
	}SECTION("assignShl") {
	    auto f = engine.registerFunction(assignShl);
	    REQUIRE(f(7) == 224);
	    REQUIRE(f(5) == 160);
	}SECTION("assignShr") {
	    auto f = engine.registerFunction(assignShr);
	    REQUIRE(f(7) == 0);
	    REQUIRE(f(5) == 0);
	}*/
	SECTION("assignment1") {
		auto f = engine.registerFunction(assignment1);
		REQUIRE(f(1) == 1);
		REQUIRE(f(42) == 42);
	}
	SECTION("assignment2") {
		auto f = engine.registerFunction(assignment2);
		REQUIRE(f(1) == 2);
		REQUIRE(f(42) == 43);
	}
	SECTION("assignment3") {
		auto f = engine.registerFunction(assignment3);
		REQUIRE(f(1) == 1);
		REQUIRE(f(42) == 42);
	}
	SECTION("assignment4") {
		auto f = engine.registerFunction(assignment4);
		REQUIRE(f(1) == 2);
		REQUIRE(f(42) == 43);
	}
	SECTION("assignment4") {
		auto f = engine.registerFunction(assignment5);
		REQUIRE(f(1) == 1);
		REQUIRE(f(42) == 42);
	}
	SECTION("int8AddExpression") {
		auto f = engine.registerFunction(int8AddExpression);
		REQUIRE(f((int8_t) 1) == 3);
	}
	SECTION("int16AddExpression") {
		auto f = engine.registerFunction(int16AddExpression);
		REQUIRE(f((int16_t) 8) == 13);
	}
	SECTION("int32AddExpression") {
		auto f = engine.registerFunction(int32AddExpression);
		REQUIRE(f((int32_t) 8) == 13);
	}
	SECTION("int64AddExpression") {
		auto f = engine.registerFunction(int64AddExpression);
		REQUIRE(f((int64_t) 7) == 14);
		REQUIRE(f((int64_t) -7) == 0);
		REQUIRE(f((int64_t) -14) == -7);
	}
	SECTION("floatAddExpression") {
		auto f = engine.registerFunction(floatAddExpression);
		REQUIRE(f((float) 7.0) == 14);
		REQUIRE(f((float) -7) == 0);
		REQUIRE(f((float) -14) == -7);
	}
	SECTION("doubleAddExpression") {
		auto f = engine.registerFunction(doubleAddExpression);
		REQUIRE(f((double) 7.0) == 14);
		REQUIRE(f((double) -7) == 0);
		REQUIRE(f((double) -14) == -7);
	}
	SECTION("castFloatToDoubleAddExpression") {
		auto f = engine.registerFunction(castFloatToDoubleAddExpression);
		REQUIRE(f((float) 7.0) == 14);
		REQUIRE(f((float) -7) == 0);
		REQUIRE(f((float) -14) == -7);
	}
	SECTION("castInt8ToInt64AddExpression") {
		auto f = engine.registerFunction(castInt8ToInt64AddExpression);
		REQUIRE(f((int8_t) 7) == 14);
		REQUIRE(f((int8_t) -7) == 0);
		REQUIRE(f((int8_t) -14) == -7);
	}
	SECTION("castInt8ToInt64Test2") {
		auto f = engine.registerFunction(castInt8ToInt64AddExpression2);
		REQUIRE(f((int8_t) 8) == 50);
		REQUIRE(f((int8_t) -2) == 40);
	}
}

void controlFlowTest(engine::NautilusEngine& engine) {
	SECTION("ifConditionTest") {
		auto f = engine.registerFunction(ifThenCondition);
		REQUIRE(f(42) == 44);
		REQUIRE(f(1) == 43);
	}
	SECTION("ifThenElseCondition") {
		auto f = engine.registerFunction(ifThenElseCondition);
		REQUIRE(f(1) == 85);
		REQUIRE(f(42) == 44);
	}
	SECTION("nestedIfThenElseCondition") {
		auto f = engine.registerFunction(nestedIfThenElseCondition);
		REQUIRE(f(1, 1) == 5);
		REQUIRE(f(42, 1) == 3);
		REQUIRE(f(1, 8) == 10);
	}
	SECTION("nestedIfNoElseCondition") {
		auto f = engine.registerFunction(nestedIfNoElseCondition);
		REQUIRE(f(1, 1) == 12);
		REQUIRE(f(42, 1) == 7);
		REQUIRE(f(42, 10) == 16);
		REQUIRE(f(1, -1) == 10);
	}
	SECTION("deeplyNestedIfElseIfCondition") {
		auto f = engine.registerFunction(deeplyNestedIfElseIfCondition);
		REQUIRE(f() == 17);
	}
	SECTION("deeplyNestedIfElseCondition") {
		auto f = engine.registerFunction(deeplyNestedIfElseCondition);
		REQUIRE(f() == 12);
	}
	SECTION("andFunction") {
		auto f = engine.registerFunction(andFunction);
		REQUIRE(f(42) == true);
		REQUIRE(f(1) == false);
	}
	SECTION("andCondition") {
		auto f = engine.registerFunction(andCondition);
		REQUIRE(f(42, 42) == 1);
		REQUIRE(f(8, 42) == 1);
		REQUIRE(f(42, 1) == 1);
		REQUIRE(f(8, 1) == 15);
	}
	SECTION("orCondition") {
		auto f = engine.registerFunction(orCondition);
		REQUIRE(f(42) == 1);
		REQUIRE(f(8) == 15);
		REQUIRE(f(1) == 15);
	}
	SECTION("ifElseIfCondition") {
		auto f = engine.registerFunction(ifElseIfCondition);
		REQUIRE(f() == 23);
	}
	SECTION("doubleIfCondition") {
		auto f = engine.registerFunction(doubleIfCondition);
		REQUIRE(f() == 23);
	}
	SECTION("nestedIf") {
		auto f = engine.registerFunction(nestedIf);
		REQUIRE(f(25) == 1);
		REQUIRE(f(15) == 2);
		REQUIRE(f(5) == 3);
	}
	SECTION("ifElseIfElse") {
		auto f = engine.registerFunction(ifElseIfElse);
		REQUIRE(f(0) == 10);
		REQUIRE(f(1) == 20);
		REQUIRE(f(5) == 30);
		REQUIRE(f(7) == 30);
	}
	SECTION("logicalAnd") {
		auto f = engine.registerFunction(logicalAnd);
		REQUIRE(f(0) == 0);
		REQUIRE(f(5) == 0);
		REQUIRE(f(6) == 1);
		REQUIRE(f(14) == 1);
		REQUIRE(f(15) == 0);
	}
	SECTION("logicalOr") {
		auto f = engine.registerFunction(logicalOr);
		REQUIRE(f(0) == 0);
		REQUIRE(f(5) == 0);
		REQUIRE(f(10) == 1);
		REQUIRE(f(20) == 1);
		REQUIRE(f(15) == 0);
	}
	SECTION("ifNotEqual") {
		auto f = engine.registerFunction(ifNotEqual);
		REQUIRE(f(0) == 2);
		REQUIRE(f(5) == 1);
		REQUIRE(f(10) == 2);
	}
	SECTION("multipleConditions") {
		auto f = engine.registerFunction(multipleConditions);
		REQUIRE(f(0) == 0);
		REQUIRE(f(5) == 1);
		REQUIRE(f(9) == 1);
		REQUIRE(f(10) == 0);
		REQUIRE(f(11) == 0);
		REQUIRE(f(20) == 1);
	}
	SECTION("ifWithTernary") {
		auto f = engine.registerFunction(ifWithTernary);
		REQUIRE(f(1) == 5);
		REQUIRE(f(5) == 5);
		REQUIRE(f(6) == 10);
		REQUIRE(f(10) == 10);
	}
	SECTION("complexLogicalExpressions") {
		auto f = engine.registerFunction(complexLogicalExpressions);
		REQUIRE(f(1) == 0);
		REQUIRE(f(5) == 0);
		REQUIRE(f(6) == 1);
		REQUIRE(f(9) == 1);
		REQUIRE(f(10) == 0);
		REQUIRE(f(16) == 1);
		REQUIRE(f(19) == 1);
		REQUIRE(f(20) == 0);
	}
	/*SECTION("shortCircuitEvaluation") {
	    auto f = engine.registerFunction(shortCircuitEvaluation);
	    REQUIRE(f(0) == 0);
	    REQUIRE(f(10) == 0);
	    REQUIRE(f(5) == 1);
	    REQUIRE(f(4) == 1);
	}*/
	SECTION("ifWithFunctionCall") {
		auto f = engine.registerFunction(ifWithFunctionCall);
		REQUIRE(f(0) == 0);
		REQUIRE(f(2) == 0);
		REQUIRE(f(4) == 0);
		REQUIRE(f(5) == 0);
		REQUIRE(f(6) == 1);
		REQUIRE(f(10) == 1);
	}
	SECTION("compoundStatements") {
		auto f = engine.registerFunction(compoundStatements);
		REQUIRE(f(0) == 0);
		REQUIRE(f(5) == 0);
		REQUIRE(f(6) == 2);
		REQUIRE(f(10) == 2);
	}
	SECTION("varyingComplexity") {
		auto f = engine.registerFunction(varyingComplexity);
		REQUIRE(f(0) == 1);
		REQUIRE(f(4) == 1);
		REQUIRE(f(5) == 2);
		REQUIRE(f(7) == 3);
		REQUIRE(f(8) == 2);
		REQUIRE(f(10) == 2);
		REQUIRE(f(11) == 3);
	}
	SECTION("logicalXOR") {
		auto f = engine.registerFunction(logicalXOR);
		REQUIRE(f(0) == 1);
		REQUIRE(f(6) == 0);
		REQUIRE(f(7) == 0);
		REQUIRE(f(11) == 1);
	}
	SECTION("nestedIfElseDifferentLevels") {
		auto f = engine.registerFunction(nestedIfElseDifferentLevels);
		REQUIRE(f(0) == -1);
		REQUIRE(f(-42) == -1);
		REQUIRE(f(1) == 1);
		REQUIRE(f(5) == 2);
		REQUIRE(f(6) == 3);
		REQUIRE(f(7) == 2);
	}
}

void loopExecutionTest(engine::NautilusEngine& engine) {
	SECTION("sumLoop") {
		auto f = engine.registerFunction(sumLoop);
		REQUIRE(f(10) == 101);
		REQUIRE(f(0) == 1);
	}
	SECTION("nestedSumLoop") {
		auto f = engine.registerFunction(nestedSumLoop);
		REQUIRE(f(10) == 1001);
		REQUIRE(f(0) == 1);
	}
	SECTION("ifSumLoop") {
		auto f = engine.registerFunction(ifSumLoop);
		REQUIRE(f() == 51);
	}
	SECTION("ifElseSumLoop") {
		auto f = engine.registerFunction(ifElseSumLoop);
		REQUIRE(f() == 56);
	}
	SECTION("elseOnlySumLoop") {
		auto f = engine.registerFunction(elseOnlySumLoop);
		REQUIRE(f() == 1);
	}
	SECTION("nestedIfSumLoop") {
		auto f = engine.registerFunction(nestedIfSumLoop);
		REQUIRE(f() == 41);
	}
	SECTION("nestedIfElseSumLoop") {
		auto f = engine.registerFunction(nestedIfElseSumLoop);
		REQUIRE(f() == 146);
	}
	SECTION("nestedIfElseSumLoop") {
		auto f = engine.registerFunction(nestedIfElseSumLoop);
		REQUIRE(f() == 146);
	}
	SECTION("nestedElseOnlySumLoop") {
		auto f = engine.registerFunction(nestedElseOnlySumLoop);
		REQUIRE(f() == 1);
	}
	SECTION("ifInsideLoop") {
		auto f = engine.registerFunction(ifInsideLoop);
		REQUIRE(f(10) == 5);
		REQUIRE(f(0) == 0);
		REQUIRE(f(-5) == 0);
	}
	SECTION("sumOfNumbers") {
		auto f = engine.registerFunction(sumOfNumbers);
		REQUIRE(f(10) == 55);
	}
	SECTION("factorial") {
		auto f = engine.registerFunction(factorial);
		REQUIRE(f(10) == 3628800);
	}
	SECTION("factorial") {
		auto f = engine.registerFunction(reverseNumber);
		REQUIRE(f(10) == 1);
		REQUIRE(f(100) == 1);
	}
	SECTION("fibonacci") {
		auto f = engine.registerFunction(fibonacci);
		REQUIRE(f(1) == 1);
		REQUIRE(f(0) == 1);
		REQUIRE(f(5) == 5);
		REQUIRE(f(4) == 3);
		REQUIRE(f(6) == 8);
	}
	SECTION("gcd") {
		auto f = engine.registerFunction(gcd);
		REQUIRE(f(10, 0) == 10);
		REQUIRE(f(10, 2) == 2);
		REQUIRE(f(21, 3) == 3);
		REQUIRE(f(8, 512) == 8);
	}
	SECTION("decimalToBinary") {
		auto f = engine.registerFunction(decimalToBinary);
		REQUIRE(f(10) == 1010);
		REQUIRE(f(1) == 1);
		REQUIRE(f(0) == 0);
	}
	SECTION("isPrime") {
		auto f = engine.registerFunction(isPrime);
		REQUIRE(f(0) == false);
		REQUIRE(f(1) == false);
		REQUIRE(f(2) == true);
		REQUIRE(f(3) == true);
		REQUIRE(f(15) == false);
		REQUIRE(f(17) == true);
	}
	SECTION("collatz") {
		auto f = engine.registerFunction(collatz);
		REQUIRE(f(1) == 0);
		REQUIRE(f(2) == 1);
		REQUIRE(f(20) == 7);
	}
	SECTION("digitSum") {
		auto f = engine.registerFunction(digitSum);
		REQUIRE(f(1) == 1);
		REQUIRE(f(2) == 2);
		REQUIRE(f(20) == 2);
		REQUIRE(f(25) == 7);
		REQUIRE(f(64) == 10);
	}
	SECTION("sumOfSquares") {
		auto f = engine.registerFunction(sumOfSquares);
		REQUIRE(f(1) == 1);
		REQUIRE(f(2) == 5);
		REQUIRE(f(20) == 2870);
		REQUIRE(f(25) == 5525);
	}
	SECTION("countDigits") {
		auto f = engine.registerFunction(countDigits);
		REQUIRE(f(1) == 1);
		REQUIRE(f(2) == 1);
		REQUIRE(f(2870) == 4);
		REQUIRE(f(-1) == 1);
		REQUIRE(f(-2) == 1);
		REQUIRE(f(-2870) == 4);
	}
}

void functionCallExecutionTest(engine::NautilusEngine& engine) {
	SECTION("simpleDirectCall") {
		auto f = engine.registerFunction(simpleDirectCall);
		REQUIRE(f(10, 10) == 20);
		REQUIRE(f(0, 1) == 1);
	}
	SECTION("loopDirectCall") {
		auto f = engine.registerFunction(loopDirectCall);
		REQUIRE(f(10, 10) == 100);
		REQUIRE(f(1, 1) == 1);
		REQUIRE(f(0, 1) == 0);
	}
	// SECTION("voidCall") {
	//     auto f = engine.registerFunction(voidFuncCall);
	//     REQUIRE(f(10, 10) == 10);
	//     REQUIRE(f(0, 1) == 0);
	// }
}

void pointerExecutionTest(engine::NautilusEngine& engine) {
	int* values = new int[10] {1, 2, 3, 4, 5, 6, 7, 8, 9};
	// int* ptr = &values;
	SECTION("load") {
		auto f = engine.registerFunction(load);

		REQUIRE(f(values, (int32_t) 0) == 1);
		REQUIRE(f(values, (int32_t) 1) == 2);
		REQUIRE(f(values, (int32_t) 8) == 9);
	}
	SECTION("sumArray") {
		auto f = engine.registerFunction(sumArray);
		val<int> r = f(values, (int32_t) 10);
		REQUIRE(r == 45);
	}
	SECTION("sumArray2") {
		auto f = engine.registerFunction(addArray<int>);
		f(values, values, 10);
		int* ref = new int[10] {2, 4, 6, 8, 10, 12, 14, 16, 18};
		for (auto i = 0; i < 10; i++) {
			REQUIRE(values[i] == ref[i]);
		}
	}
}

void runAllTests(engine::NautilusEngine& engine) {




	SECTION("expressionTest") {
		addTest(engine);
	}
	SECTION("controlFlowTest") {
		controlFlowTest(engine);
	}
	SECTION("loopExecutionTest") {
		loopExecutionTest(engine);
	}
	SECTION("functionCallExecutionTest") {
		functionCallExecutionTest(engine);
	}
	SECTION("pointerExecutionTest") {
		pointerExecutionTest(engine);
	}


}

TEST_CASE("Engine Interpreter Test") {
	std::cout << (is_arithmetic_value<val<int>> && (is_arithmetic_value<val<int>> || convertible_to_fundamental<val<int>>)) << std::endl;
	std::cout << (is_arithmetic_value<val<int>>) << std::endl;
	std::cout << (is_arithmetic_value<val<Color>>) << std::endl;
	std::cout << std::is_arithmetic_v<typename std::remove_reference_t<val<Color>>::basic_type> << std::endl;
	std::cout << "Is int is_value? " << is_val<val<int>> << std::endl;
	std::cout << "Is int is_value? " << is_val<int> << std::endl;
	std::cout << "Is int is_value? " << is_val<int*> << std::endl;
	std::cout << "Is int is_value? " << is_traceable_value<val<int>> << std::endl;


	/*engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	runAllTests(engine);*/
}

#ifdef ENABLE_TRACING
TEST_CASE("Engine Compiler Test") {
	std::vector<std::string> backends = {};
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	backends.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	backends.emplace_back("asmjit");
#endif
	for (auto& backend : backends) {
		DYNAMIC_SECTION(backend) {
			engine::Options options;
			options.setOption("engine.backend", backend);
			auto engine = engine::NautilusEngine(options);
			runAllTests(engine);
		}
	}
}
#endif
} // namespace nautilus::engine
