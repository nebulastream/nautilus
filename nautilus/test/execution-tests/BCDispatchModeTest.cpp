#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_std.hpp"
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

// Plain struct with no user-declared constructor: val<AllocaLocals> default
// construction does not zero its alloca'd backing memory (val_std.hpp only invokes
// a constructor for non-trivially-default-constructible types), so its initial
// contents are exactly whatever the interpreter handed back for that alloca slot.
// Combined with calling the same compiled function repeatedly below, this is the
// right shape to catch a regression where pooled alloca buffers (bc.regfileReuse)
// leak a previous invocation's bytes into the next instead of being re-zeroed.
struct AllocaLocals {
	int32_t a;
	int32_t b;
};

val<int32_t> bcAllocaReuse(val<int32_t> x) {
	val<AllocaLocals> value;
	if (x > 0) {
		value.set(&AllocaLocals::a, x);
		value.set(&AllocaLocals::b, x * 2);
	}
	// When x <= 0 neither field is written, so a correct implementation must
	// observe zero-initialized memory here, not leftover values from a prior call.
	return value.get(&AllocaLocals::a) + value.get(&AllocaLocals::b);
}

engine::NautilusEngine bcEngine(const std::string& dispatch, bool reuseRegisterFile = false,
                                bool superinstructions = false, bool immediates = false) {
	engine::Options options;
	options.setOption("engine.backend", std::string("bc"));
	options.setOption("bc.dispatch", dispatch);
	options.setOption("bc.regfileReuse", reuseRegisterFile);
	options.setOption("bc.superinstructions", superinstructions);
	options.setOption("bc.immediates", immediates);
	return engine::NautilusEngine(options);
}

struct Variant {
	std::string dispatch;
	bool reuse;
	bool super;
	bool imm;
	std::string name() const {
		return dispatch + (reuse ? "_regfileReuse" : "") + (super ? "_superinstructions" : "") +
		       (imm ? "_immediates" : "");
	}
};

} // namespace

TEST_CASE("BC dispatch modes produce identical results") {
	auto call = bcEngine("call");
	// Every (dispatch, regfileReuse, superinstructions, immediates) combination must
	// match the plain "call" reference. regfileReuse recycles the per-invocation
	// register file (guards against leaked state); superinstructions fuses
	// compare+branch and immediates folds constant operands on the threaded path
	// (both guarded against the reference). The kernels below contain constant
	// operands (i+1, b+7, x-100, …) so immediate folding is actually exercised.
	const std::vector<Variant> variants = {
	    {"call", true, false, false},     {"switch", false, false, false},   {"switch", true, false, false},
	    {"switch", false, true, false},   {"threaded", false, false, false}, {"threaded", true, false, false},
	    {"threaded", false, true, false}, {"threaded", true, true, false},   {"threaded", false, false, true},
	    {"threaded", false, true, true},  {"threaded", true, true, true}};
	for (const auto& variant : variants) {
		DYNAMIC_SECTION(variant.name()) {
			auto alt = bcEngine(variant.dispatch, variant.reuse, variant.super, variant.imm);

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
			auto cAllocaReuse = call.registerFunction(bcAllocaReuse);
			auto aAllocaReuse = alt.registerFunction(bcAllocaReuse);

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

			// Call the same compiled function repeatedly, alternating between an input
			// that writes the alloca-backed struct's fields and one that doesn't. A
			// regfileReuse=true variant that pooled the alloca buffer without re-zeroing
			// it would leak the prior call's fields into this call's "untouched" reads.
			for (int32_t i = 0; i < 4; i++) {
				REQUIRE(aAllocaReuse(50 + i) == cAllocaReuse(50 + i));
				REQUIRE(aAllocaReuse(-1) == cAllocaReuse(-1));
			}
		}
	}
}

TEST_CASE("BC backend with no bc.* options set matches the explicit call/false/false/false reference") {
	// Pins BCInterpreterBackend's shipped defaults (threaded/true/true/true) against
	// the same reference used above, without constructing the engine through
	// bcEngine() — i.e. leaving every "bc.*" option unset, exactly as a production
	// caller that never heard of these options would.
	engine::Options defaultOptions;
	defaultOptions.setOption("engine.backend", std::string("bc"));
	engine::NautilusEngine withDefaults(defaultOptions);

	auto call = bcEngine("call");

	auto cArith = call.registerFunction(bcArith);
	auto dArith = withDefaults.registerFunction(bcArith);
	auto cFib = call.registerFunction(bcFib);
	auto dFib = withDefaults.registerFunction(bcFib);
	auto cBranch = call.registerFunction(bcBranchy);
	auto dBranch = withDefaults.registerFunction(bcBranchy);
	auto cAllocaReuse = call.registerFunction(bcAllocaReuse);
	auto dAllocaReuse = withDefaults.registerFunction(bcAllocaReuse);

	for (int64_t a = -20; a <= 20; a += 3) {
		REQUIRE(dFib(a) == cFib(a));
		REQUIRE(dBranch(a) == cBranch(a));
		for (int64_t b = -20; b <= 20; b += 5) {
			REQUIRE(dArith(a, b) == cArith(a, b));
		}
	}
	for (int32_t i = 0; i < 4; i++) {
		REQUIRE(dAllocaReuse(50 + i) == cAllocaReuse(50 + i));
		REQUIRE(dAllocaReuse(-1) == cAllocaReuse(-1));
	}
}

} // namespace nautilus::engine

#endif // ENABLE_BC_BACKEND
