#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/function.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <stdexcept>

// The tbc backend's copy-and-patch JIT (tbc.mode=jit) must produce results
// identical to the interpreter on the same bytecode — the strongest oracle
// available, since both modes execute the exact same TBCProgram. This test
// pins the JIT against a de-optimized switch-dispatch interpreter reference
// across every lowering-option combination (so superinstruction and
// immediate-folded opcodes hit their stencils), and covers the paths the
// generic suite exercises less directly: internal Nautilus-to-Nautilus
// calls, external calls, and C++ exceptions traversing stitched frames.
//
// The whole execution suite additionally runs on stitched code via the
// "tbc-jit" pseudo-backend in testing::availableBackends().
#ifdef ENABLE_TBC_JIT

namespace nautilus::engine {

namespace {

val<int64_t> jitArith(val<int64_t> a, val<int64_t> b) {
	return (a + b) * (a - b) + (a * b) - (b + 7);
}

val<int64_t> jitFib(val<int64_t> n) {
	val<int64_t> a = 0;
	val<int64_t> b = 1;
	for (val<int64_t> i = 0; i < n; i = i + 1) {
		val<int64_t> t = a + b;
		a = b;
		b = t;
	}
	return a;
}

val<int64_t> jitSumSquares(val<int32_t> n) {
	val<int64_t> sum = 0;
	for (val<int32_t> i = 0; i < n; i++) {
		val<int64_t> v = i;
		sum += v * v;
	}
	return sum;
}

val<int64_t> jitBranchy(val<int64_t> x) {
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

val<int32_t> jitCasts(val<int64_t> x) {
	val<int32_t> a = static_cast<val<int32_t>>(x);
	val<float> f = static_cast<val<float>>(a);
	val<double> d = static_cast<val<double>>(f);
	val<int64_t> back = static_cast<val<int64_t>>(d);
	return static_cast<val<int32_t>>(back + a);
}

val<int64_t> jitModBit(val<int64_t> a, val<int64_t> b) {
	val<int64_t> m = a % (b + 1);
	return m + (a & b) + (a | b) + (a ^ b);
}

val<double> jitFloats(val<double> x, val<float> y) {
	val<double> acc = 0.0;
	for (val<int32_t> i = 0; i < 8; i++) {
		acc = acc * 1.5 + x - val<double>(y);
	}
	return -acc;
}

val<int64_t> jitSelect(val<int64_t> a, val<int64_t> b) {
	val<int64_t> lo = a < b ? a : b;
	val<int64_t> hi = a < b ? b : a;
	return hi * 1000 + lo;
}

// Internal Nautilus-to-Nautilus calls (CALL opcode + stitched entries).
val<int64_t> jitCalleeHelper(val<int64_t> x, val<int64_t> y) {
	return x * 2 + y;
}
static auto jitCallee = NautilusFunction {"jitCallee", jitCalleeHelper};

val<int64_t> jitCaller(val<int64_t> a, val<int64_t> b) {
	val<int64_t> first = jitCallee(a, b);
	val<int64_t> second = jitCallee(first, a);
	return first + second;
}

void jitVoidHelperBody(val<int64_t*> p) {
	*p = *p + val<int64_t>(41);
}
static auto jitVoidCallee = NautilusFunction {"jitVoidCallee", jitVoidHelperBody};

val<int64_t> jitVoidCaller(val<int64_t*> p) {
	jitVoidCallee(p);
	return *p;
}

// External calls (CALL_EXT opcode via the dyncall helper).
int64_t nativeMix(int32_t a, int64_t b, double c) {
	return a + b + static_cast<int64_t>(c * 10);
}

val<int64_t> jitExternal(val<int32_t> a, val<int64_t> b, val<double> c) {
	return invoke(nativeMix, a, b, c) + 1;
}

// Note on exceptions: a C++ exception thrown by an invoke() target dies in
// dyncall's assembly (no unwind info) on EVERY tbc mode — interpreter
// included — so that is not part of the engine's contract. Exceptions that
// arise in the JIT's own helpers (e.g. VM stack overflow during an internal
// CALL) are bridged through VMContext::pendingException and rethrown by the
// entry shim; TBCJitWhiteBoxTest.cpp exercises that path.

NautilusEngine jitEngine(const std::string& mode, bool superinstructions, bool immediates, bool coalescing,
                         const std::string& dispatch = "auto") {
	engine::Options options;
	options.setOption("engine.backend", std::string("tbc"));
	options.setOption("tbc.mode", mode);
	options.setOption("tbc.dispatch", dispatch);
	options.setOption("tbc.superinstructions", superinstructions);
	options.setOption("tbc.immediates", immediates);
	options.setOption("tbc.coalescing", coalescing);
	return NautilusEngine(options);
}

bool jitAvailable() {
	return compiler::tbc::jit::jitRuntimeAvailable();
}

} // namespace

TEST_CASE("TBC copy-and-patch JIT matches the interpreter") {
	if (!jitAvailable()) {
		SKIP("tbc-jit runtime unavailable on this build");
	}
	// Reference: portable switch skin with every lowering optimization off.
	auto reference = jitEngine("interp", false, false, false, "switch");

	struct Variant {
		bool super;
		bool imm;
		bool coalesce;
		std::string name() const {
			return std::string("jit") + (super ? "_superinstructions" : "") + (imm ? "_immediates" : "") +
			       (coalesce ? "_coalescing" : "");
		}
	};
	// The lowering options change which opcodes are emitted (fused
	// compare-and-branch, immediate-folded arithmetic, LOAD_off/STORE_off),
	// so each combination exercises a different stencil population.
	const std::vector<Variant> variants = {
	    {false, false, false},
	    {true, false, false},
	    {false, true, false},
	    {true, true, true},
	};
	for (const auto& variant : variants) {
		DYNAMIC_SECTION(variant.name()) {
			auto jit = jitEngine("jit", variant.super, variant.imm, variant.coalesce);

			auto cArith = reference.registerFunction(jitArith);
			auto jArith = jit.registerFunction(jitArith);
			auto cFib = reference.registerFunction(jitFib);
			auto jFib = jit.registerFunction(jitFib);
			auto cSum = reference.registerFunction(jitSumSquares);
			auto jSum = jit.registerFunction(jitSumSquares);
			auto cBranch = reference.registerFunction(jitBranchy);
			auto jBranch = jit.registerFunction(jitBranchy);
			auto cCast = reference.registerFunction(jitCasts);
			auto jCast = jit.registerFunction(jitCasts);
			auto cMod = reference.registerFunction(jitModBit);
			auto jMod = jit.registerFunction(jitModBit);
			auto cFloat = reference.registerFunction(jitFloats);
			auto jFloat = jit.registerFunction(jitFloats);
			auto cSel = reference.registerFunction(jitSelect);
			auto jSel = jit.registerFunction(jitSelect);

			for (int64_t a = -20; a <= 20; a += 3) {
				REQUIRE(jFib(a) == cFib(a));
				REQUIRE(jBranch(a) == cBranch(a));
				REQUIRE(jCast(a * 1000003) == cCast(a * 1000003));
				for (int64_t b = -20; b <= 20; b += 5) {
					REQUIRE(jArith(a, b) == cArith(a, b));
					REQUIRE(jMod(a, b) == cMod(a, b));
					REQUIRE(jSel(a, b) == cSel(a, b));
					REQUIRE(jFloat(static_cast<double>(a), static_cast<float>(b)) ==
					        cFloat(static_cast<double>(a), static_cast<float>(b)));
				}
			}
			for (int32_t n = 0; n <= 5000; n += 777) {
				REQUIRE(jSum(n) == cSum(n));
			}
		}
	}
}

TEST_CASE("TBC JIT internal and external calls") {
	if (!jitAvailable()) {
		SKIP("tbc-jit runtime unavailable on this build");
	}
	auto reference = jitEngine("interp", false, false, false, "switch");
	auto jit = jitEngine("jit", true, true, true);

	SECTION("internal calls run through stitched entries") {
		auto cCaller = reference.registerFunction(jitCaller);
		auto jCaller = jit.registerFunction(jitCaller);
		for (int64_t a = -5; a <= 5; ++a) {
			REQUIRE(jCaller(a, a + 3) == cCaller(a, a + 3));
		}
	}

	SECTION("internal void calls") {
		auto cVoid = reference.registerFunction(jitVoidCaller);
		auto jVoid = jit.registerFunction(jitVoidCaller);
		int64_t cell = 1;
		REQUIRE(jVoid(&cell) == 42);
		cell = 1;
		REQUIRE(cVoid(&cell) == 42);
	}

	SECTION("external calls marshal through dyncall") {
		auto cExt = reference.registerFunction(jitExternal);
		auto jExt = jit.registerFunction(jitExternal);
		REQUIRE(jExt(3, 100, 2.5) == cExt(3, 100, 2.5));
		REQUIRE(jExt(-7, 0, -0.5) == cExt(-7, 0, -0.5));
	}
}

TEST_CASE("TBC tbc.mode=auto degrades, tbc.mode=jit is strict") {
	// auto must work on every build: jit when available, interpreter
	// otherwise.
	auto autoEngine = jitEngine("auto", true, true, true);
	auto fn = autoEngine.registerFunction(jitFib);
	REQUIRE(fn(30) == 832040);

	if (!jitAvailable()) {
		// Strict mode must fail loudly instead of silently testing the
		// interpreter. Depending on the engine's compilation settings the
		// throw can surface at registration or at first call, so the whole
		// sequence goes inside the assertion.
		REQUIRE_THROWS([&] {
			auto strict = jitEngine("jit", true, true, true);
			auto strictFn = strict.registerFunction(jitFib);
			return strictFn(1);
		}());
	}
}

} // namespace nautilus::engine

#endif // ENABLE_TBC_JIT
