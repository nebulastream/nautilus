

#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/nautilus_function.hpp>
namespace nautilus::engine {

val<int> add_indirect(val<int> a, val<int> b) {
	return a + b;
}

static auto nautilusAdd = NautilusFunction {"add", add_indirect};

val<int> nautilusFunction(val<int> a, val<int> b) {
	auto result = nautilusAdd(a, b);
	return result;
}

val<int> nautilusFunction2(val<int> a, val<int> b) {
	auto result1 = nautilusAdd(a, b);
	[[maybe_unused]] auto result2 = nautilusAdd(result1, b);
	auto result3 = nautilusAdd(a, b);
	return result3;
}

// Test: Void function (no return value)
void voidHelper(val<int*> x) {
	*x = *x + val<int>(10);
}

static auto nautilusVoidFunc = NautilusFunction {"voidHelper", voidHelper};

val<int> nautilusFunctionVoid(val<int*> a) {
	nautilusVoidFunc(a);
	return *a;
}

// Test: Function with multiple results
val<int> multiplyHelper(val<int> a, val<int> b) {
	return a * b;
}

static auto nautilusMultiply = NautilusFunction {"multiply", multiplyHelper};

val<int> nautilusFunctionMultipleResults(val<int> a, val<int> b, val<int> c) {
	auto result1 = nautilusMultiply(a, b);
	auto result2 = nautilusMultiply(b, c);
	auto result3 = nautilusMultiply(result1, result2);
	return result3;
}

// Test: Nested calls (function calling another nested function)
val<int> innerHelper(val<int> x) {
	return x * val<int>(2);
}

static auto nautilusInner = NautilusFunction {"inner", innerHelper};

val<int> outerHelper(val<int> x) {
	return nautilusInner(x) + val<int>(5);
}

static auto nautilusOuter = NautilusFunction {"outer", outerHelper};

val<int> nautilusFunctionNested(val<int> a) {
	return nautilusOuter(a);
}

// Test: Conditional with nested function call
val<int> conditionalHelper(val<int> x) {
	return x + val<int>(100);
}

static auto nautilusConditional = NautilusFunction {"conditionalHelper", conditionalHelper};

val<int> nautilusFunctionConditional(val<int> a) {
	if (a > val<int>(10)) {
		return nautilusConditional(a);
	} else {
		return a;
	}
}

// Test: Loop with nested function call
val<int> loopHelper(val<int> x) {
	return x + val<int>(1);
}

static auto nautilusLoopHelper = NautilusFunction {"loopHelper", loopHelper};

val<int> nautilusFunctionLoop(val<int> n) {
	auto sum = val<int>(0);
	for (auto i = val<int>(0); i < n; i = i + val<int>(1)) {
		sum = nautilusLoopHelper(sum);
	}
	return sum;
}

// Test: Multiple different nested functions
val<int> add10(val<int> x) {
	return x + val<int>(10);
}

val<int> multiply2(val<int> x) {
	return x * val<int>(2);
}

val<int> subtract5(val<int> x) {
	return x - val<int>(5);
}

static auto nautilusAdd10 = NautilusFunction {"add10", add10};
static auto nautilusMultiply2 = NautilusFunction {"multiply2", multiply2};
static auto nautilusSubtract5 = NautilusFunction {"subtract5", subtract5};

val<int> nautilusFunctionMultiple(val<int> a) {
	auto step1 = nautilusAdd10(a);
	auto step2 = nautilusMultiply2(step1);
	auto step3 = nautilusSubtract5(step2);
	return step3;
}

// Test: Recursive-style (same function called multiple times with different args)
val<int> recursiveHelper(val<int> x, val<int> depth) {
	if (depth <= val<int>(0)) {
		return x;
	}
	return x + val<int>(1);
}

static auto nautilusRecursive = NautilusFunction {"recursiveHelper", recursiveHelper};

val<int> nautilusFunctionRecursiveStyle(val<int> a) {
	auto result = a;
	result = nautilusRecursive(result, val<int>(3));
	result = nautilusRecursive(result, val<int>(2));
	result = nautilusRecursive(result, val<int>(1));
	return result;
}

// Test: Function with complex computation
val<int> complexComputation(val<int> a, val<int> b, val<int> c) {
	auto temp1 = a * b;
	auto temp2 = b + c;
	auto temp3 = temp1 - temp2;
	return temp3 * val<int>(2);
}

static auto nautilusComplex = NautilusFunction {"complexComputation", complexComputation};

val<int> nautilusFunctionComplex(val<int> x, val<int> y) {
	auto result1 = nautilusComplex(x, y, val<int>(5));
	auto result2 = nautilusComplex(result1, x, y);
	return result2;
}

// Test: Inline NautilusFunction with function pointer
val<int> nautilusFunctionInline(val<int> a, val<int> b) {
	auto inlineAdd = NautilusFunction {"inlineAdd", add_indirect};
	return inlineAdd(a, b);
}

// Test: Inline NautilusFunction with lambda
val<int> nautilusFunctionInlineLambda(val<int> a, val<int> b) {
	auto inlineMul = NautilusFunction {"inlineMul", [](val<int> x, val<int> y) -> val<int> {
		                                   return x * y;
	                                   }};
	return inlineMul(a, b);
}

// Test: Inline NautilusFunction with member function
struct InlineCalculator {
	val<int> sub(val<int> a, val<int> b) {
		return a - b;
	}
};

static InlineCalculator inlineCalc;

val<int> nautilusFunctionInlineMember(val<int> a, val<int> b) {
	auto inlineSub = NautilusFunction {"inlineSub", bind_instance<&InlineCalculator::sub>(inlineCalc)};
	return inlineSub(a, b);
}

// Test: Multiple inline NautilusFunctions in one function
val<int> nautilusFunctionMultipleInline(val<int> a, val<int> b) {
	auto inlineAdd = NautilusFunction {"inlineAdd2", add_indirect};
	auto inlineMul = NautilusFunction {"inlineMul2", [](val<int> x, val<int> y) -> val<int> {
		                                   return x * y;
	                                   }};
	auto sum = inlineAdd(a, b);
	auto product = inlineMul(a, b);
	return sum + product;
}

// Test: getFuncPtr() — obtain a function pointer from a NautilusFunction
// and pass it through invoke() to a runtime function that calls it.
int32_t applyFnPtr(int32_t (*fn)(int32_t, int32_t), int32_t a, int32_t b) {
	return fn(a, b);
}

val<int32_t> nautilusFunctionGetFuncPtr(val<int32_t> a, val<int32_t> b) {
	auto fnPtr = nautilusAdd.getFuncPtr();
	return invoke(applyFnPtr, fnPtr, a, b);
}

// ---------------------------------------------------------------------------
// Data-type coverage: one NautilusFunction call per val<T> specialization
// (signed/unsigned integers of every width, floating point, bool, pointer,
// and enum), so the NautilusFunction call path is exercised for all types.
// ---------------------------------------------------------------------------

// Test: NautilusFunction over val<int8_t>
val<int8_t> addInt8Helper(val<int8_t> a, val<int8_t> b) {
	return a + b;
}

static auto nautilusAddInt8 = NautilusFunction {"addInt8", addInt8Helper};

val<int8_t> nautilusFunctionInt8(val<int8_t> a, val<int8_t> b) {
	return nautilusAddInt8(a, b);
}

// Test: NautilusFunction over val<uint8_t>
val<uint8_t> mulUInt8Helper(val<uint8_t> a, val<uint8_t> b) {
	return a * b;
}

static auto nautilusMulUInt8 = NautilusFunction {"mulUInt8", mulUInt8Helper};

val<uint8_t> nautilusFunctionUInt8(val<uint8_t> a, val<uint8_t> b) {
	return nautilusMulUInt8(a, b);
}

// Test: NautilusFunction over val<int16_t>
val<int16_t> subInt16Helper(val<int16_t> a, val<int16_t> b) {
	return a - b;
}

static auto nautilusSubInt16 = NautilusFunction {"subInt16", subInt16Helper};

val<int16_t> nautilusFunctionInt16(val<int16_t> a, val<int16_t> b) {
	return nautilusSubInt16(a, b);
}

// Test: NautilusFunction over val<uint16_t>
val<uint16_t> addUInt16Helper(val<uint16_t> a, val<uint16_t> b) {
	return a + b;
}

static auto nautilusAddUInt16 = NautilusFunction {"addUInt16", addUInt16Helper};

val<uint16_t> nautilusFunctionUInt16(val<uint16_t> a, val<uint16_t> b) {
	return nautilusAddUInt16(a, b);
}

// Test: NautilusFunction over val<uint32_t>
val<uint32_t> mulUInt32Helper(val<uint32_t> a, val<uint32_t> b) {
	return a * b;
}

static auto nautilusMulUInt32 = NautilusFunction {"mulUInt32", mulUInt32Helper};

val<uint32_t> nautilusFunctionUInt32(val<uint32_t> a, val<uint32_t> b) {
	return nautilusMulUInt32(a, b);
}

// Test: NautilusFunction over val<int64_t>
val<int64_t> addInt64Helper(val<int64_t> a, val<int64_t> b) {
	return a + b;
}

static auto nautilusAddInt64 = NautilusFunction {"addInt64", addInt64Helper};

val<int64_t> nautilusFunctionInt64(val<int64_t> a, val<int64_t> b) {
	return nautilusAddInt64(a, b);
}

// Test: NautilusFunction over val<uint64_t>
val<uint64_t> mulUInt64Helper(val<uint64_t> a, val<uint64_t> b) {
	return a * b;
}

static auto nautilusMulUInt64 = NautilusFunction {"mulUInt64", mulUInt64Helper};

val<uint64_t> nautilusFunctionUInt64(val<uint64_t> a, val<uint64_t> b) {
	return nautilusMulUInt64(a, b);
}

// Test: NautilusFunction over val<float>
val<float> addFloatHelper(val<float> a, val<float> b) {
	return a + b;
}

static auto nautilusAddFloat = NautilusFunction {"addFloat", addFloatHelper};

val<float> nautilusFunctionFloat(val<float> a, val<float> b) {
	return nautilusAddFloat(a, b);
}

// Test: NautilusFunction over val<double>
val<double> mulDoubleHelper(val<double> a, val<double> b) {
	return a * b;
}

static auto nautilusMulDouble = NautilusFunction {"mulDouble", mulDoubleHelper};

val<double> nautilusFunctionDouble(val<double> a, val<double> b) {
	return nautilusMulDouble(a, b);
}

// Test: NautilusFunction over val<bool>
val<bool> andBoolHelper(val<bool> a, val<bool> b) {
	return a && b;
}

static auto nautilusAndBool = NautilusFunction {"andBool", andBoolHelper};

val<bool> nautilusFunctionBool(val<bool> a, val<bool> b) {
	return nautilusAndBool(a, b);
}

// Test: NautilusFunction reading through a val<int32_t*> pointer argument
val<int32_t> derefAddHelper(val<int32_t*> ptr, val<int32_t> addend) {
	return *ptr + addend;
}

static auto nautilusDerefAdd = NautilusFunction {"derefAdd", derefAddHelper};

val<int32_t> nautilusFunctionPtr(val<int32_t*> ptr, val<int32_t> addend) {
	return nautilusDerefAdd(ptr, addend);
}

// Test: NautilusFunction writing through a val<double*> pointer argument
void writeDoubleHelper(val<double*> ptr, val<double> value) {
	*ptr = value;
}

static auto nautilusWriteDouble = NautilusFunction {"writeDouble", writeDoubleHelper};

val<double> nautilusFunctionPtrWrite(val<double*> ptr, val<double> value) {
	nautilusWriteDouble(ptr, value);
	return *ptr;
}

// Test: NautilusFunction over an enum-typed val<T> argument
enum class NautilusFunctionColor : int32_t { RED, GREEN, BLUE };

val<int32_t> colorToCodeHelper(val<NautilusFunctionColor> color) {
	if (color == NautilusFunctionColor::RED) {
		return val<int32_t>(1);
	} else if (color == NautilusFunctionColor::GREEN) {
		return val<int32_t>(2);
	} else {
		return val<int32_t>(3);
	}
}

static auto nautilusColorToCode = NautilusFunction {"colorToCode", colorToCodeHelper};

val<int32_t> nautilusFunctionEnum(val<NautilusFunctionColor> color) {
	return nautilusColorToCode(color);
}

} // namespace nautilus::engine
