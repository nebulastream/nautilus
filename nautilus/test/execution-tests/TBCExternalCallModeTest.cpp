#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <nautilus/config.hpp>

// The tbc backend marshals outgoing external calls (CALL_EXT / CALL_IND)
// either through dyncall or through the built-in zero-dependency typed thunks,
// selected via the "tbc.externalCall" option ("auto" — the default — picks
// dyncall when NAUTILUS_TBC_DYNCALL is set and thunks otherwise). forEachBackend
// only exercises the default, so this test pins that every mode produces
// results identical to native evaluation across the signature shapes the thunk
// path reorders and re-extends: interleaved int/fp arguments, narrow signed
// argument/return extension, bool/float/double returns, pointer arguments, and
// signatures at and beyond the thunk caps (8 integer-class / 4 fp-class args).
//
// Kernels are local (not shared with RunctimeCallFunctions.hpp, whose helpers
// have external linkage and live in the ExecutionTest.cpp TU).
#ifdef ENABLE_TBC_BACKEND

namespace nautilus::engine {

namespace {

int32_t ecAdd(int32_t x, int32_t y) {
	return x + y;
}

bool ecIsNeg(int32_t x) {
	return x < 0;
}

float ecMulF(float x, float y) {
	return x * y;
}

int32_t ecPick(bool takeFirst, int32_t x, int32_t y) {
	return takeFirst ? x : y;
}

int64_t ecDeref(int64_t* p) {
	return *p;
}

// Interleaved integer/fp signature: the thunk reorders the three integer-class
// arguments ahead of the double/float/double tail; every value must still
// arrive in the right register.
double ecMix6(int32_t a, double b, int64_t c, float d, int8_t e, double f) {
	return static_cast<double>(a) * b + static_cast<double>(c) * static_cast<double>(d) +
	       static_cast<double>(e) * f;
}

int16_t ecMixI16(int16_t x, int16_t y) {
	// Same trap as RunctimeCallFunctions.hpp::mixI16: the 32-bit intermediate
	// differs from the wrapped i16 result, so unextended upper register bits
	// are observable.
	return static_cast<int16_t>(x * 3 + y);
}

int16_t ecMinI16(int16_t x, int16_t y) {
	return x < y ? x : y;
}

// Exactly at the caps: 8 integer-class args (args 7-8 spill to the stack on
// SysV x86-64) and 4 fp args with a mixed f32/f64 pattern.
int64_t ecSum8(int64_t a, int64_t b, int64_t c, int64_t d, int64_t e, int64_t f, int64_t g, int64_t h) {
	return a + 2 * b + 3 * c + 4 * d + 5 * e + 6 * f + 7 * g + 8 * h;
}

double ecSumF4(float a, double b, float c, double d) {
	return static_cast<double>(a) + 2.0 * b + 3.0 * static_cast<double>(c) + 4.0 * d;
}

// Beyond the caps: not thunkable, degrades per-site to dyncall (or is
// rejected at compile time when dyncall is not built).
int64_t ecSum9(int64_t a, int64_t b, int64_t c, int64_t d, int64_t e, int64_t f, int64_t g, int64_t h, int64_t i) {
	return a + 2 * b + 3 * c + 4 * d + 5 * e + 6 * f + 7 * g + 8 * h + 9 * i;
}

double ecSumF5(double a, double b, double c, double d, double e) {
	return a + 2 * b + 3 * c + 4 * d + 5 * e;
}

val<int32_t> kSimpleCall(val<int32_t> x, val<int32_t> y) {
	return invoke(ecAdd, x, y);
}

val<int32_t> kLoopCall(val<int32_t> c, val<int32_t> x) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < c; i = i + 1) {
		sum = invoke(ecAdd, sum, x);
	}
	return sum;
}

val<bool> kBoolReturn(val<int32_t> x) {
	return invoke(ecIsNeg, x);
}

val<float> kFloatCall(val<float> x, val<float> y) {
	return invoke(ecMulF, x, y);
}

val<int32_t> kBoolArg(val<bool> takeFirst, val<int32_t> x, val<int32_t> y) {
	return invoke(ecPick, takeFirst, x, y);
}

val<int64_t> kPtrArg(val<int64_t*> p) {
	return invoke(ecDeref, p);
}

val<double> kMix6(val<int32_t> a, val<double> b, val<int64_t> c, val<float> d, val<int32_t> e, val<double> f) {
	return invoke(ecMix6, a, b, c, d, static_cast<val<int8_t>>(e), f);
}

// Regression shapes from the differential fuzzer (see
// RunctimeCallFunctions.hpp): narrow signed return values and arguments must
// be re-extended across the call boundary.
val<int16_t> kNarrowReturn(val<int16_t> x, val<int16_t> y) {
	val<int16_t> mixed = invoke(ecMixI16, x, y);
	val<int16_t> result = 0;
	if (mixed < val<int16_t>(int16_t(0))) {
		result = 1;
	}
	return result;
}

val<int16_t> kNarrowArg(val<int64_t> x, val<int16_t> y) {
	val<int16_t> truncated = static_cast<val<int16_t>>(x);
	return invoke(ecMinI16, truncated, y);
}

val<int64_t> kSum8(val<int64_t> a, val<int64_t> b, val<int64_t> c, val<int64_t> d, val<int64_t> e, val<int64_t> f,
                   val<int64_t> g, val<int64_t> h) {
	return invoke(ecSum8, a, b, c, d, e, f, g, h);
}

val<double> kSumF4(val<float> a, val<double> b, val<float> c, val<double> d) {
	return invoke(ecSumF4, a, b, c, d);
}

val<int64_t> kSum9(val<int64_t> a, val<int64_t> b, val<int64_t> c, val<int64_t> d, val<int64_t> e, val<int64_t> f,
                   val<int64_t> g, val<int64_t> h, val<int64_t> i) {
	return invoke(ecSum9, a, b, c, d, e, f, g, h, i);
}

val<double> kSumF5(val<double> a, val<double> b, val<double> c, val<double> d, val<double> e) {
	return invoke(ecSumF5, a, b, c, d, e);
}

// CALL_IND with a native target: FunctionAddressOf of an external function
// flows through a register and calls back out through the external path.
val<int32_t> kFunctionPtr(val<int32_t> x) {
	auto func = function(ecAdd);
	x = func(x, x);
	x = func(x, x);
	return x;
}

engine::NautilusEngine tbcEngine(const std::string& externalCall) {
	engine::Options options;
	options.setOption("engine.backend", std::string("tbc"));
	options.setOption("tbc.externalCall", externalCall);
	return engine::NautilusEngine(options);
}

std::vector<std::string> externalCallModes() {
	std::vector<std::string> modes = {"auto", "thunks"};
#ifdef NAUTILUS_TBC_DYNCALL
	modes.emplace_back("dyncall");
#endif
	return modes;
}

} // namespace

TEST_CASE("TBC external call modes produce identical results") {
	for (const auto& mode : externalCallModes()) {
		DYNAMIC_SECTION(mode) {
			auto engine = tbcEngine(mode);

			auto simple = engine.registerFunction(kSimpleCall);
			auto loop = engine.registerFunction(kLoopCall);
			auto boolRet = engine.registerFunction(kBoolReturn);
			auto floatCall = engine.registerFunction(kFloatCall);
			auto boolArg = engine.registerFunction(kBoolArg);
			auto ptrArg = engine.registerFunction(kPtrArg);
			auto mix6 = engine.registerFunction(kMix6);
			auto narrowReturn = engine.registerFunction(kNarrowReturn);
			auto narrowArg = engine.registerFunction(kNarrowArg);
			auto fnPtr = engine.registerFunction(kFunctionPtr);

			for (int32_t x = -20; x <= 20; x += 7) {
				for (int32_t y = -15; y <= 15; y += 5) {
					REQUIRE(simple(x, y) == ecAdd(x, y));
					REQUIRE(boolArg(x > y, x, y) == ecPick(x > y, x, y));
				}
				REQUIRE(boolRet(x) == ecIsNeg(x));
				REQUIRE(fnPtr(x) == 4 * x);
			}
			REQUIRE(loop(10, 3) == 30);
			REQUIRE(loop(0, 3) == 0);
			REQUIRE(floatCall(2.5f, -4.0f) == ecMulF(2.5f, -4.0f));

			int64_t cell = 0x0123456789abcdefLL;
			REQUIRE(ptrArg(&cell) == cell);

			REQUIRE(mix6(3, 2.5, -7, 1.5f, -2, 0.25) == ecMix6(3, 2.5, -7, 1.5f, int8_t(-2), 0.25));
			REQUIRE(mix6(-1, -0.5, 1L << 40, -2.0f, 100, 8.0) == ecMix6(-1, -0.5, 1L << 40, -2.0f, int8_t(100), 8.0));

			// Narrow-int extension traps (fuzzer regressions): 25158 * 3 - 24951
			// has a positive 32-bit intermediate but a negative wrapped i16
			// result; the truncated i64 carries live upper bits.
			REQUIRE(narrowReturn(int16_t(25158), int16_t(-24951)) == 1);
			REQUIRE(narrowReturn(int16_t(3), int16_t(4)) == 0);
			REQUIRE(narrowArg(int64_t(0x7ffff0000LL + 12), int16_t(300)) ==
			        ecMinI16(int16_t(0x7ffff0000LL + 12), int16_t(300)));
			REQUIRE(narrowArg(int64_t(-70000), int16_t(-5)) == ecMinI16(int16_t(-70000), int16_t(-5)));
		}
	}
}

TEST_CASE("TBC typed thunk caps") {
	auto engine = tbcEngine("thunks");

	SECTION("signatures exactly at the caps use thunks") {
		auto sum8 = engine.registerFunction(kSum8);
		auto sumF4 = engine.registerFunction(kSumF4);
		REQUIRE(sum8(1, -2, 3, -4, 5, -6, 7, -8) == ecSum8(1, -2, 3, -4, 5, -6, 7, -8));
		REQUIRE(sumF4(1.5f, -2.25, 3.5f, 4.125) == ecSumF4(1.5f, -2.25, 3.5f, 4.125));
	}

	SECTION("signatures beyond the caps") {
#ifdef NAUTILUS_TBC_DYNCALL
		// Per-site degrade: over-cap sites marshal through dyncall.
		auto sum9 = engine.registerFunction(kSum9);
		auto sumF5 = engine.registerFunction(kSumF5);
		REQUIRE(sum9(1, -2, 3, -4, 5, -6, 7, -8, 9) == ecSum9(1, -2, 3, -4, 5, -6, 7, -8, 9));
		REQUIRE(sumF5(1.5, -2.25, 3.5, 4.125, -0.5) == ecSumF5(1.5, -2.25, 3.5, 4.125, -0.5));
#else
		// Without dyncall the backend rejects the signature when the function
		// is compiled, not at run time mid-execution.
		REQUIRE_THROWS([&] {
			auto sum9 = engine.registerFunction(kSum9);
			(void) sum9(1, -2, 3, -4, 5, -6, 7, -8, 9);
		}());
		REQUIRE_THROWS([&] {
			auto sumF5 = engine.registerFunction(kSumF5);
			(void) sumF5(1.5, -2.25, 3.5, 4.125, -0.5);
		}());
#endif
	}
}

} // namespace nautilus::engine

#endif // ENABLE_TBC_BACKEND
