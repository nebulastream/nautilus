#pragma once

#include <cmath>
#include <nautilus/Engine.hpp>
#include <nautilus/val_ptr.hpp>

namespace nautilus {

class ClazzBase {
public:
	virtual int32_t add(int32_t x) = 0;
};

class Clazz : public ClazzBase {
public:
	int32_t get() {
		return i;
	}

	int32_t add(int32_t x) override {
		return x + i;
	}

	int32_t addConst(int32_t x) const {
		return x + i;
	}

	int i = 42;
};

} // namespace nautilus

namespace nautilus::engine {

void voidFunc(int32_t x, int32_t y) {
	[[maybe_unused]] auto z = x + y;
}

val<int32_t> useFirstArg(val<int32_t> x, [[maybe_unused]] val<int32_t> y) {
	return x;
}

val<int32_t> useSecondArg([[maybe_unused]] val<int32_t> x, val<int32_t> y) {
	return y;
}

val<int32_t> useNoArg([[maybe_unused]] val<int32_t> x, [[maybe_unused]] val<int32_t> y) {
	return 42;
}

void multipleVoidReturnsFunction(val<int32_t*> x) {
	val<int32_t> val = *x;
	if (val > 10) {
		*x = 1;
		return;
	}
	*x = 42;
}

int32_t add(int32_t x, int32_t y) {
	return x + y;
}

int32_t sub(int32_t x, int32_t y) {
	return x - y;
}

int32_t addAndSub(int32_t x, int32_t y) {
	return add(x, y) + sub(x, y);
}

__attribute__((noinline)) double helper1(double val) {
	return val * val + 3.14;
}

__attribute__((noinline)) double helper2(double val) {
	return std::cos(val) / (1.0 + std::abs(val));
}

double complexCalc(int32_t a, int32_t b) {
	double acc = 0.0;
	std::vector<double> data;
	data.reserve(a);
	for (int32_t i = 0; i < a; i++) {
		data.push_back(i);
	}

	for (auto& x : data) {
		double temp = helper1(x) * b;
		if (temp > 10.0)
			acc += helper2(temp);
		else
			acc -= helper2(temp);
	}
	return std::exp(acc);
}

struct Base {
	virtual ~Base() = default;
};

struct Derived : Base {
	void greet() {
		std::cout << "Hello World!\n";
	}
};

int32_t testDynamicCast(Base* b) {
	if (dynamic_cast<Derived*>(b)) {
		return 0;
	}
	return 1;
}

val<int32_t> simpleDirectCall(val<int32_t> x, val<int32_t> y) {
	return invoke(add, x, y);
}

val<int32_t> directCallWithNestedCalls(val<int32_t> x, val<int32_t> y) {
	return invoke<>(addAndSub, x, y);
}

val<int32_t> callTwoFunctions(val<int32_t> x, val<int32_t> y) {
	return invoke<>(add, x, y) + invoke<>(sub, x, y);
}

val<int32_t> loopDirectCall(val<int32_t> c, val<int32_t> x) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < c; i = i + 1) {
		sum = invoke<>(add, sum, x);
	}
	return sum;
}

inline int32_t get42() {
	return 42;
}

inline val<int32_t> loopDirectCall2(val<int32_t> x) {
	val<int32_t> sum = 0;
	val<int32_t> c = invoke(get42);
	for (val<uint64_t> i = 0; i < c; i = i + 1) {
		sum = invoke<>(add, sum, x);
	}
	return sum;
}

val<int32_t> directCallComplexFunction(val<int32_t> x, val<int32_t> y) {
	return invoke<>(complexCalc, x, y);
}

val<int32_t> dynCastCall(val<Base*> x) {
	return invoke<>(testDynamicCast, x);
}

val<int32_t> voidFuncCall(val<int32_t> x, val<int32_t> y) {
	invoke<>(voidFunc, x, y);
	return x;
}

val<int32_t> lambdaRuntimeFunction(val<int32_t> x) {
	return invoke<>(+[](int32_t x) { return x * 2; }, x);
}

val<int32_t> nestedLambdaRuntimeFunction(val<int32_t> x) {
	return invoke<>(+[](int32_t x) { return x * 2; }, invoke<>(+[](int32_t x) { return x + 2; }, x));
}

val<int32_t> callSameFunction(val<int32_t> x) {
	auto func = function(add);
	x = func(x, x);
	x = func(x, x);
	return x;
}

val<int32_t> callMemberFunction(val<Clazz*> x) {
	auto& func = memberFunc<&Clazz::get>();
	auto res = func(x);
	return res;
}

inline int32_t countFuncCall(const bool isFirstCall) {
	thread_local int32_t funcCallCount {0};
	funcCallCount = isFirstCall ? 1 : (funcCallCount + 1);
	return funcCallCount;
}

val<int32_t> callCountFuncCall(val<bool> isFirstCall) {
	return invoke(countFuncCall, isFirstCall);
}

val<int32_t> incrementFuncCallFiveTimesWithModRef(val<bool> isFirstCall) {
	constexpr FunctionAttributes funcAttr {ModRefInfo::ModRef};
	invoke(funcAttr, countFuncCall, isFirstCall);
	invoke(funcAttr, countFuncCall, nautilus::val<bool>(false));
	invoke(funcAttr, countFuncCall, nautilus::val<bool>(false));
	invoke(funcAttr, countFuncCall, nautilus::val<bool>(false));
	return invoke(funcAttr, countFuncCall, nautilus::val<bool>(false));
}

val<int32_t> incrementFuncCallFiveTimesWithRef(val<bool> isFirstCall) {
	constexpr FunctionAttributes funcAttr {ModRefInfo::Ref};
	invoke(funcAttr, countFuncCall, isFirstCall);
	invoke(funcAttr, countFuncCall, nautilus::val<bool>(false));
	invoke(funcAttr, countFuncCall, nautilus::val<bool>(false));
	invoke(funcAttr, countFuncCall, nautilus::val<bool>(false));
	return invoke(funcAttr, countFuncCall, nautilus::val<bool>(false));
}

int16_t mixI16(int16_t x, int16_t y) {
	// int promotion makes x * 3 + y well-defined; the cast wraps it back to
	// 16 bits, so the interesting outputs are ones whose 32-bit intermediate
	// (e.g. 25158 * 3 - 24951 = 50523) differs from the wrapped i16 result.
	return static_cast<int16_t>(x * 3 + y);
}

// Regression (differential fuzzer): the ABI leaves the upper bits of a narrow
// integer return value unspecified, so a backend keeping integers in
// full-width registers must re-extend a call's narrow result before using it.
// The AsmJit backends used to skip that, so a negative i16 return whose raw
// register bits were a positive 32-bit intermediate compared as positive.
val<int16_t> i16NarrowCallReturnCompare(val<int16_t> x, val<int16_t> y) {
	val<int16_t> mixed = invoke(mixI16, x, y);
	val<int16_t> result = 0;
	if (mixed < val<int16_t>(int16_t(0))) {
		result = 1;
	}
	return result;
}

int16_t minI16(int16_t x, int16_t y) {
	return x < y ? x : y;
}

// Regression (differential fuzzer): sub-32-bit integer *arguments* of a proxy
// call must be extended by the caller on several C ABIs (Darwin AArch64,
// x86-64 SysV) -- the natively compiled callee assumes that happened. The
// MLIR backend used to declare external callees without llvm.signext /
// llvm.zeroext argument attributes, so an i16 argument whose truncation LLVM
// folded away (here: from a wider value with live upper bits) reached the
// callee with garbage in the upper register bits and compared wrongly.
val<int16_t> i16NarrowCallArgCompare(val<int64_t> x, val<int16_t> y) {
	val<int16_t> truncated = static_cast<val<int16_t>>(x);
	return invoke(minI16, truncated, y);
}

int32_t addI32(int32_t x, int32_t y) {
	return x + y;
}

int32_t mulI32(int32_t x, int32_t y) {
	return x * y;
}

struct SharedDispatchCallee {
	int32_t (*fn)(int32_t, int32_t);
};

// Non-inlined, single call site: invoked once per array element from the loop
// below, so every element's Op::CALL is traced through the exact same
// call-stack shape regardless of which callee it targets -- mirroring how a
// shared recursive expression-tree evaluator (e.g. a query compiler's own
// operator dispatch) reaches unrelated invoke() calls through one call site.
__attribute__((noinline)) val<int32_t> evalSharedDispatchCallee(const SharedDispatchCallee& callee, val<int32_t> x,
                                                                val<int32_t> y) {
	return invoke(callee.fn, x, y);
}

// Regression (issue #381): Tag identity is a raw __builtin_return_address
// chain (TagRecorder.cpp), which depends only on the call-stack shape used to
// reach a traced operation -- not on which logical operation it is. Two
// Op::CALL operations reached through the same shared, non-inlined dispatch
// call site but targeting *different* native callees collide onto the same
// Tag, which ExecutionTrace::processControlFlowMerge used to treat as an
// unconditional fatal "Invalid trace. This is maybe caused by a constant
// loop." error even though there is no real control-flow re-entry.
//
// The `for (int i...)` loop below is deliberately a plain, untraced host
// loop rather than a `static_val`/`static_iterable`-driven one: `static_val`
// folds its counter into the tracer's Snapshot hash and *would* sidestep this
// specific collision, but that idiom is for loops a Nautilus *kernel* author
// writes (the StaticLoop feature -- a loop that is itself part of the traced
// program's semantics). The actual defect lives one layer further out, in
// generic interpreter/compiler infrastructure that drives Nautilus's tracing
// API from *outside* any single kernel body and has no static_val to reach
// for. This loop shape is not hypothetical: it is modeled directly on
// `evalNautilusCall`'s own argument-evaluation loop in the differential
// fuzzer (`nautilus/test/fuzz/EvalNautilus.hpp`) --
//   for (int i = 0; i < callDesc.arity; ++i) {
//       v[i] = evalNautilusGeneric<T>(ast, n.kid[vStart + i], args, ctx);
//   }
// -- which is exactly how the fuzzer's `min(0f64, 0f64)`/`mix(0u32, 0u32)`
// minimal reproductions in issue #381 collide: a Kind::Call node's own
// arguments (themselves potentially nested Kind::Call nodes targeting
// different callees) are evaluated through this one shared, plain-`int`
// loop, with no static_val anywhere in sight.
val<int32_t> sharedCallSiteDifferentCallees(val<int32_t> x, val<int32_t> y) {
	static const SharedDispatchCallee callees[] = {{addI32}, {mulI32}};
	// volatile mirrors the fuzzer's `callDesc.arity`, whose value comes from a
	// fuzzed-input-driven table lookup and is therefore genuinely opaque to
	// the optimizer: it blocks constant-propagation of the trip count so the
	// compiler cannot unroll this loop into two distinct call instructions
	// (which would trivially -- and unrealistically -- avoid the collision by
	// giving each call its own call-stack shape). The whole point is that
	// both calls into evalSharedDispatchCallee execute through the exact same
	// physical instruction, exactly as they do in the real fuzzer.
	volatile int count = 2;
	val<int32_t> acc = 0;
	for (int i = 0; i < count; i++) {
		acc = acc + evalSharedDispatchCallee(callees[i], x, y);
	}
	return acc;
}

} // namespace nautilus::engine
