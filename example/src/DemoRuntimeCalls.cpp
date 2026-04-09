// DemoRuntimeCalls.cpp — four patterns for calling functions from traced code.
//
// Pattern 1 — Opaque runtime call via invoke():
//   A plain C++ function (isPrime) that the tracer treats as a black box.
//   The backend emits a normal C ABI call at runtime.
//
// Pattern 2 — NautilusFunction cross-call:
//   One nautilus function calls another through a NautilusFunction handle.
//   Both functions are traced and compiled; the backend emits a direct
//   call between them.
//
// Pattern 3 — Recursive NautilusFunction:
//   A nautilus function calls *itself* via its own NautilusFunction handle,
//   producing a recursive compiled function.
//
// Pattern 4 — NautilusFunction pointer passed to a runtime function:
//   getFuncPtr() extracts a typed function pointer from a NautilusFunction.
//   That pointer is handed to a plain C++ callback (via invoke()) which
//   calls the compiled nautilus code from the runtime side.

#include <cstdint>
#include <iostream>
#include <nautilus/Engine.hpp>
#include <nautilus/function.hpp>
#include <nautilus/nautilus_function.hpp>
#include <nautilus/val.hpp>

using namespace nautilus;

// ============================================================================
// Pattern 1 — opaque runtime call
// ============================================================================

// Plain C++ — NOT a nautilus function. The tracer records a single CALL op.
static bool isPrime(int32_t n) {
	if (n < 2) {
		return false;
	}
	for (int32_t d = 2; d * d <= n; d++) {
		if (n % d == 0) {
			return false;
		}
	}
	return true;
}

// Traced function that delegates the primality test to the runtime.
val<int32_t> sumOfPrimesBelow(val<int32_t> n) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 2; i < n; i++) {
		if (invoke(isPrime, i)) {
			sum += i;
		}
	}
	return sum;
}

// ============================================================================
// Pattern 2 — one nautilus function calls another
// ============================================================================

// A simple helper: doubles its argument. Declared as a NautilusFunction so
// other nautilus code can call it and both sides get traced/compiled.
val<int32_t> doubleIt(val<int32_t> x) {
	return x * 2;
}
static auto nautilusDouble = NautilusFunction {"doubleIt", doubleIt};

// Calls nautilusDouble twice to quadruple. The backend compiles both
// functions and emits a direct call from quadrupleIt → doubleIt.
val<int32_t> quadrupleIt(val<int32_t> x) {
	return nautilusDouble(nautilusDouble(x));
}

// ============================================================================
// Pattern 3 — recursive NautilusFunction
// ============================================================================

// Forward-declare so the NautilusFunction object exists before the body.
val<int64_t> factorialImpl(val<int32_t> n);
static auto nautilusFactorial = NautilusFunction {"factorial", factorialImpl};

// The body calls itself through nautilusFactorial → true recursion.
val<int64_t> factorialImpl(val<int32_t> n) {
	if (n <= 1) {
		return 1;
	}
	return n * nautilusFactorial(n - 1);
}

// ============================================================================
// Pattern 4 — pass NautilusFunction pointer to a runtime callback
// ============================================================================

// Plain C++ runtime helper that receives a typed function pointer and
// calls it. The function pointer points into the compiled nautilus code.
static int32_t applyTwice(int32_t (*fn)(int32_t), int32_t x) {
	return fn(fn(x));
}

// Traced function: extracts a compiled function pointer from
// nautilusDouble via getFuncPtr(), then hands it to a runtime C function
// via invoke(). The runtime function calls back into compiled code.
val<int32_t> applyDoubleFromRuntime(val<int32_t> x) {
	auto fnPtr = nautilusDouble.getFuncPtr();
	return invoke(applyTwice, fnPtr, x);
}

// ============================================================================

int main(int, char*[]) {
	engine::Options options;
	auto engine = engine::NautilusEngine(options);
	std::cout << "Backend: " << engine.getNameOfBackend() << "\n\n";

	// Register all four entry-point functions in a single module.
	auto module = engine.createModule();
	module.registerFunction("sumOfPrimesBelow", sumOfPrimesBelow);
	module.registerFunction("quadrupleIt", quadrupleIt);
	module.registerFunction("factorial", factorialImpl);
	module.registerFunction("applyDoubleFromRuntime", applyDoubleFromRuntime);
	auto compiled = module.compile();

	auto primes = compiled.getFunction<int32_t(int32_t)>("sumOfPrimesBelow");
	auto quad = compiled.getFunction<int32_t(int32_t)>("quadrupleIt");
	auto fact = compiled.getFunction<int64_t(int32_t)>("factorial");
	auto dblRt = compiled.getFunction<int32_t(int32_t)>("applyDoubleFromRuntime");

	// Pattern 1: sum of primes below 20 = 2+3+5+7+11+13+17+19 = 77
	std::cout << "Pattern 1 — opaque runtime call (invoke):\n";
	std::cout << "  sumOfPrimesBelow(20)        = " << primes(20) << "  (expected 77)\n\n";

	// Pattern 2: quadruple(5) = double(double(5)) = double(10) = 20
	std::cout << "Pattern 2 — NautilusFunction cross-call:\n";
	std::cout << "  quadrupleIt(5)              = " << quad(5) << "  (expected 20)\n\n";

	// Pattern 3: factorial(10) = 3628800
	std::cout << "Pattern 3 — recursive NautilusFunction:\n";
	std::cout << "  factorial(10)               = " << fact(10) << "  (expected 3628800)\n\n";

	// Pattern 4: applyTwice(doubleIt, 3) = doubleIt(doubleIt(3)) = doubleIt(6) = 12
	std::cout << "Pattern 4 — NautilusFunction pointer to runtime callback:\n";
	std::cout << "  applyDoubleFromRuntime(3)   = " << dblRt(3) << "  (expected 12)\n";

	return 0;
}
