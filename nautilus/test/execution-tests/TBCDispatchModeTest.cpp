#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>

// The tbc interpreter offers three dispatch skins selected via the
// "tbc.dispatch" option: "tailcall" (musttail threading, Clang), "goto"
// (computed goto, GCC/Clang) and "switch" (portable). "auto" — the default —
// picks the strongest one the build supports, and unavailable requests
// silently degrade. forEachBackend only exercises the default, so this test
// pins that every skin and every lowering-option combination produce results
// identical to the plain switch/no-optimization reference across a range of
// inputs and opcode families.
#ifdef ENABLE_TBC_BACKEND

namespace nautilus::engine {

namespace {

val<int64_t> tbcArith(val<int64_t> a, val<int64_t> b) {
	return (a + b) * (a - b) + (a * b) - (b + 7);
}

val<int64_t> tbcFib(val<int64_t> n) {
	val<int64_t> a = 0;
	val<int64_t> b = 1;
	for (val<int64_t> i = 0; i < n; i = i + 1) {
		val<int64_t> t = a + b;
		a = b;
		b = t;
	}
	return a;
}

val<int64_t> tbcSumSquares(val<int32_t> n) {
	val<int64_t> sum = 0;
	for (val<int32_t> i = 0; i < n; i++) {
		val<int64_t> v = i;
		sum += v * v;
	}
	return sum;
}

val<int64_t> tbcBranchy(val<int64_t> x) {
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

val<int32_t> tbcCasts(val<int64_t> x) {
	val<int32_t> a = static_cast<val<int32_t>>(x);
	val<float> f = static_cast<val<float>>(a);
	val<double> d = static_cast<val<double>>(f);
	val<int64_t> back = static_cast<val<int64_t>>(d);
	return static_cast<val<int32_t>>(back + a);
}

val<int64_t> tbcModBit(val<int64_t> a, val<int64_t> b) {
	val<int64_t> m = a % (b + 1);
	return m + (a & b) + (a | b) + (a ^ b);
}

engine::NautilusEngine tbcEngine(const std::string& dispatch, bool superinstructions, bool immediates,
                                 bool coalescing) {
	engine::Options options;
	options.setOption("engine.backend", std::string("tbc"));
	options.setOption("tbc.dispatch", dispatch);
	options.setOption("tbc.superinstructions", superinstructions);
	options.setOption("tbc.immediates", immediates);
	options.setOption("tbc.coalescing", coalescing);
	return engine::NautilusEngine(options);
}

struct Variant {
	std::string dispatch;
	bool super;
	bool imm;
	bool coalesce;
	std::string name() const {
		return dispatch + (super ? "_superinstructions" : "") + (imm ? "_immediates" : "") +
		       (coalesce ? "_coalescing" : "");
	}
};

} // namespace

TEST_CASE("TBC dispatch modes produce identical results") {
	// Reference: portable switch skin with every lowering optimization off.
	auto reference = tbcEngine("switch", false, false, false);
	// Every (dispatch, superinstructions, immediates, coalescing) combination
	// must match the reference. The kernels contain constant right operands
	// (i + 1, b + 7, x - 100, …) and compare-fed branches, so immediate
	// folding and compare-and-branch fusion are actually exercised.
	const std::vector<Variant> variants = {
	    {"switch", true, true, true},  {"goto", false, false, false},   {"goto", true, true, true},
	    {"tailcall", true, true, true}, {"tailcall", false, false, false}, {"auto", true, true, true},
	    {"auto", true, false, true},   {"auto", false, true, false},
	};
	for (const auto& variant : variants) {
		DYNAMIC_SECTION(variant.name()) {
			auto alt = tbcEngine(variant.dispatch, variant.super, variant.imm, variant.coalesce);

			auto cArith = reference.registerFunction(tbcArith);
			auto aArith = alt.registerFunction(tbcArith);
			auto cFib = reference.registerFunction(tbcFib);
			auto aFib = alt.registerFunction(tbcFib);
			auto cSum = reference.registerFunction(tbcSumSquares);
			auto aSum = alt.registerFunction(tbcSumSquares);
			auto cBranch = reference.registerFunction(tbcBranchy);
			auto aBranch = alt.registerFunction(tbcBranchy);
			auto cCast = reference.registerFunction(tbcCasts);
			auto aCast = alt.registerFunction(tbcCasts);
			auto cMod = reference.registerFunction(tbcModBit);
			auto aMod = alt.registerFunction(tbcModBit);

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

#endif // ENABLE_TBC_BACKEND
