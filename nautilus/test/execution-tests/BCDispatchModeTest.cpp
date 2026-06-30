#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>

// The bytecode interpreter offers three dispatch strategies selected via the
// "bc.dispatch" option: "call" (indirect-call table, the default), "switch"
// (inlined switch) and "threaded" (computed-goto). forEachBackend only ever
// exercises the default, so this test pins that all three modes produce
// identical results across a range of inputs and opcode families.
#ifdef ENABLE_BC_BACKEND

namespace nautilus::engine {

namespace {

val<int64_t> bcArith(val<int64_t> a, val<int64_t> b) {
	return (a + b) * (a - b) + (a * b) - (b + 7);
}

val<int64_t> bcFib(val<int64_t> n) {
	val<int64_t> a = 0;
	val<int64_t> b = 1;
	for (val<int64_t> i = 0; i < n; i = i + 1) {
		val<int64_t> t = a + b;
		a = b;
		b = t;
	}
	return a;
}

val<int64_t> bcSumSquares(val<int32_t> n) {
	val<int64_t> sum = 0;
	for (val<int32_t> i = 0; i < n; i++) {
		val<int64_t> v = i;
		sum += v * v;
	}
	return sum;
}

val<int64_t> bcBranchy(val<int64_t> x) {
	val<int64_t> r = 0;
	if (x > 10) {
		r = x * 2;
	} else if (x < -5) {
		r = x - 100;
	} else {
		r = x + 1;
	}
	return r;
}

val<int32_t> bcCasts(val<int64_t> x) {
	val<int32_t> a = static_cast<val<int32_t>>(x);
	val<float> f = static_cast<val<float>>(a);
	val<double> d = static_cast<val<double>>(f);
	val<int64_t> back = static_cast<val<int64_t>>(d);
	return static_cast<val<int32_t>>(back + a);
}

val<int64_t> bcModBit(val<int64_t> a, val<int64_t> b) {
	val<int64_t> m = a % (b + 1);
	return m + (a & b) + (a | b) + (a ^ b);
}

engine::NautilusEngine bcEngine(const std::string& dispatch) {
	engine::Options options;
	options.setOption("engine.backend", std::string("bc"));
	options.setOption("bc.dispatch", dispatch);
	return engine::NautilusEngine(options);
}

} // namespace

TEST_CASE("BC dispatch modes produce identical results") {
	auto call = bcEngine("call");
	for (const auto& mode : {std::string("switch"), std::string("threaded")}) {
		DYNAMIC_SECTION(mode) {
			auto alt = bcEngine(mode);

			auto cArith = call.registerFunction(bcArith);
			auto aArith = alt.registerFunction(bcArith);
			auto cFib = call.registerFunction(bcFib);
			auto aFib = alt.registerFunction(bcFib);
			auto cSum = call.registerFunction(bcSumSquares);
			auto aSum = alt.registerFunction(bcSumSquares);
			auto cBranch = call.registerFunction(bcBranchy);
			auto aBranch = alt.registerFunction(bcBranchy);
			auto cCast = call.registerFunction(bcCasts);
			auto aCast = alt.registerFunction(bcCasts);
			auto cMod = call.registerFunction(bcModBit);
			auto aMod = alt.registerFunction(bcModBit);

			for (int64_t a = -20; a <= 20; a += 3) {
				REQUIRE(aFib(a) == cFib(a));
				REQUIRE(aBranch(a) == cBranch(a));
				REQUIRE(aCast(a * 1000003) == cCast(a * 1000003));
				for (int64_t b = -20; b <= 20; b += 5) {
					REQUIRE(aArith(a, b) == cArith(a, b));
					REQUIRE(aMod(a, b) == cMod(a, b));
				}
			}
			for (int32_t n = 0; n <= 5000; n += 777) {
				REQUIRE(aSum(n) == cSum(n));
			}
		}
	}
}

} // namespace nautilus::engine

#endif // ENABLE_BC_BACKEND
