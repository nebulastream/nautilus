
#include "ExecutionTest.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/select.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>

// Tests for the optional instruction-selection optimizations in the AsmJit
// lowering (asmjit.enableBranchFusion, ...). forEachBackend only exercises
// the default (all optimizations on), so this test pins two things:
//   1. Integration probe: for workloads that provably contain the target
//      patterns, `fn.getStatistics()` exposes non-zero counters under the
//      `asmjit.lowering.*` namespace, proving the optimization actually fires
//      through the Engine path.
//   2. Differential correctness: compiling the same function with each
//      optimization enabled vs. disabled must produce identical results for
//      every input we probe. This is the safety net against the optimized
//      lowering paths silently diverging from the reference paths.

namespace nautilus::engine {

#ifdef ENABLE_ASMJIT_BACKEND
#ifdef ENABLE_TRACING
#if !defined(__aarch64__) && !defined(_M_ARM64)

namespace {

// Loop whose header compare feeds only the loop branch — the canonical
// branch-fusion candidate (mirrors the fibonacci benchmark kernel).
val<int32_t> fibLike(val<int32_t> n) {
	val<int32_t> a = 0, b = 1, c = 0;
	for (val<int32_t> i = 2; i <= n; i = i + 1) {
		c = a + b;
		a = b;
		b = c;
	}
	return b;
}

// Signed and unsigned comparators across both if-arms; every comparator kind
// must pick the correctly negated jcc when fused.
val<int32_t> compareLadder(val<int32_t> x, val<uint32_t> u) {
	val<int32_t> r = 0;
	if (x < 10) {
		r = r + 1;
	}
	if (x <= -3) {
		r = r + 2;
	}
	if (x > 100) {
		r = r + 4;
	}
	if (x >= 0) {
		r = r + 8;
	}
	if (x == 42) {
		r = r + 16;
	}
	if (x != 7) {
		r = r + 32;
	}
	if (u < val<uint32_t>(5u)) {
		r = r + 64;
	}
	if (u >= val<uint32_t>(0x80000000u)) {
		r = r + 128;
	}
	return r;
}

// The compare's boolean is consumed by two separate branches, so fusion
// must NOT fire and the materialised boolean must still be produced.
val<int32_t> compareWithTwoUses(val<int32_t> x, val<int32_t> y) {
	val<bool> c = x < y;
	val<int32_t> r = 0;
	if (c) {
		r = 10;
	}
	if (c) {
		r = r + 1;
	}
	return r;
}

// Regression pin (differential fuzzer, see test/fuzz/README.md): a wrapped
// narrow unsigned subtraction feeding an unsigned compare — the compare must
// see the re-normalised (zero-extended) value even on the fused path.
val<uint32_t> u32WrapSubCompare(val<uint32_t> x, val<uint32_t> y) {
	val<uint32_t> wrapped = x - val<uint32_t>(4279714710u);
	val<uint32_t> result = 0u;
	if (wrapped <= y) {
		result = 1u;
	}
	return result;
}

// Regression pin (issue #321 shape): merge-block parameter reusing an SSA
// identifier that one arm re-defines. Guards the isFusibleCompare bail-out
// for identifiers that double as merge parameters.
val<uint64_t> zeroTripMergeAddConstant(val<uint64_t> c, val<uint64_t> p) {
	val<uint64_t> result = p;
	if (c != val<uint64_t>(static_cast<uint64_t>(0))) {
		val<uint64_t> acc = static_cast<uint64_t>(0);
		for (val<int32_t> i = 0; i < val<int32_t>(0); i = i + 1) {
			acc = acc + val<uint64_t>(static_cast<uint64_t>(1));
		}
		result = acc;
	}
	return result + val<uint64_t>(static_cast<uint64_t>(119));
}

// Float compares must stay on the unfused path (parity-flag handling); this
// pins that they still work while fusion handles the surrounding loop.
val<int32_t> floatThreshold(val<double> x) {
	val<int32_t> n = 0;
	for (val<int32_t> i = 0; i < 4; i = i + 1) {
		if (x > 1.5) {
			n = n + 1;
		}
		x = x + 1.0;
	}
	return n;
}

// Constant-heavy arithmetic mixing every foldable immediate site (add, sub,
// imul, shift, and/or/xor, compare) across signed and unsigned widths,
// including a constant outside the sign-extended imm32 range that must be
// rematerialised instead of folded.
val<int64_t> constArithMix(val<int64_t> x, val<uint32_t> u) {
	val<int64_t> a = x + 7;
	val<int64_t> b = a - 100;
	val<int64_t> c = b * 3;
	val<int64_t> d = c ^ 0x55;
	val<int64_t> e = d & 0x0F0F;
	val<int64_t> f = e | 0x11;
	val<int64_t> g = f << 3;
	val<int64_t> h = g >> 2;
	val<int64_t> big = x + 0x123456789LL; // does not fit imm32 -> rematerialised
	val<uint32_t> uw = u * 5u + 4279714710u;
	val<int64_t> r = h + big + static_cast<val<int64_t>>(uw);
	if (r > -1000000) {
		r = r + 1;
	}
	return r;
}

// A constant LEFT operand of a fused compare (the static-loop iteration
// shape): the fused branch lowering must rematerialise the deferred constant
// instead of reading it through the frame.
val<int32_t> constLeftCompare(val<int32_t> t) {
	val<int32_t> r = 0;
	if (val<int32_t>(6) > t) {
		r = r + 1;
	}
	if (val<int32_t>(-3) <= t) {
		r = r + 2;
	}
	return r;
}

// Regression (differential fuzzer, issue #327): a BAND of two constants
// folds to a constant in the IR pass, and the cast consuming it must be
// rewired to the replacement — a stale pointer to the dead BAND used to
// make the lowering throw. Pins the pass-level fix end-to-end through the
// backend's pointer-based constant recovery.
val<int32_t> foldedConstShiftCount(val<int32_t> x) {
	val<uint8_t> shift = val<uint8_t>((uint8_t) 202) & val<uint8_t>((uint8_t) 7);
	return x >> shift;
}

// Narrow-width wrap-around with constant operands: the folded immediate must
// produce the same canonically extended register pattern as the
// materialise-then-operate path.
val<int32_t> i8ConstWrap(val<int8_t> x) {
	val<int8_t> y = x + (int8_t) 100; // wraps for x > 27
	val<int8_t> z = y - (int8_t) 5;
	val<int32_t> r = 0;
	if (z < (int8_t) 0) {
		r = 1;
	}
	return r + static_cast<val<int32_t>>(z);
}

// Integer and float selects: the integer ones take the cmov path, the float
// one must stay on the branchy path.
val<int64_t> selectMix(val<int64_t> x, val<int64_t> y) {
	val<int64_t> lo = select(x < y, x, y);
	val<int64_t> hi = select(x < y, y, x);
	return hi - lo + select(x == y, val<int64_t>(100), val<int64_t>(0));
}

val<double> selectFloatKernel(val<bool> c, val<double> a, val<double> b) {
	return select(c, a, b);
}

engine::NautilusEngine makeAsmJitEngine(bool enableBranchFusion, bool enableConstFolding = true,
                                        bool enableSelectCmov = true) {
	return nautilus::testing::makeEngine(
	    "asmjit", [enableBranchFusion, enableConstFolding, enableSelectCmov](engine::Options& opts) {
		    opts.setOption("asmjit.enableBranchFusion", enableBranchFusion);
		    opts.setOption("asmjit.enableConstFolding", enableConstFolding);
		    opts.setOption("asmjit.enableSelectCmov", enableSelectCmov);
		    opts.setOption("engine.traceMode", "lazyTracing");
		    // Force the legacy (non-tiered) path so stats land on the same
		    // executable we query here (see PostRAPeepholeTest for details).
		    opts.setOption("engine.compilationStrategy", std::string("legacy"));
	    });
}

int64_t getCounter(const std::shared_ptr<const compiler::CompilationStatistics>& stats, const std::string& key) {
	REQUIRE(stats != nullptr);
	const auto* value = stats->find(key);
	if (value == nullptr) {
		return 0;
	}
	REQUIRE(std::holds_alternative<int64_t>(*value));
	return std::get<int64_t>(*value);
}

} // namespace

TEST_CASE("AsmJit branch fusion: publishes counters via CompilationStatistics") {
	auto engine = makeAsmJitEngine(true);
	auto fn = engine.registerFunction(fibLike);
	REQUIRE(fn(10) == 55);
	auto stats = fn.getStatistics();
	REQUIRE(stats != nullptr);
	// The loop-header compare feeds only the loop branch, so at least one
	// compare must have been fused.
	REQUIRE(getCounter(stats, "asmjit.lowering.fusedBranches") >= 1);
}

TEST_CASE("AsmJit branch fusion: disabling suppresses counter emission") {
	auto engine = makeAsmJitEngine(false);
	auto fn = engine.registerFunction(fibLike);
	REQUIRE(fn(10) == 55);
	auto stats = fn.getStatistics();
	REQUIRE(stats != nullptr);
	REQUIRE(stats->find("asmjit.lowering.fusedBranches") == nullptr);
}

TEST_CASE("AsmJit branch fusion: differential correctness across inputs") {
	auto fibOn = makeAsmJitEngine(true).registerFunction(fibLike);
	auto fibOff = makeAsmJitEngine(false).registerFunction(fibLike);
	for (int32_t n : {0, 1, 2, 3, 10, 30, 1000}) {
		INFO("fibLike differential n=" << n);
		REQUIRE(fibOn(n) == fibOff(n));
	}

	auto ladderOn = makeAsmJitEngine(true).registerFunction(compareLadder);
	auto ladderOff = makeAsmJitEngine(false).registerFunction(compareLadder);
	for (int32_t x : {-100, -3, -1, 0, 7, 10, 42, 100, 101, INT32_MIN, INT32_MAX}) {
		for (uint32_t u : {0u, 4u, 5u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu}) {
			INFO("compareLadder differential x=" << x << " u=" << u);
			REQUIRE(ladderOn(x, u) == ladderOff(x, u));
		}
	}

	auto twoUsesOn = makeAsmJitEngine(true).registerFunction(compareWithTwoUses);
	auto twoUsesOff = makeAsmJitEngine(false).registerFunction(compareWithTwoUses);
	for (int32_t x : {-5, 0, 3}) {
		for (int32_t y : {-5, 0, 3}) {
			INFO("compareWithTwoUses differential x=" << x << " y=" << y);
			REQUIRE(twoUsesOn(x, y) == twoUsesOff(x, y));
		}
	}
}

TEST_CASE("AsmJit branch fusion: pinned fuzzer regressions stay correct") {
	auto wrapOn = makeAsmJitEngine(true).registerFunction(u32WrapSubCompare);
	auto wrapOff = makeAsmJitEngine(false).registerFunction(u32WrapSubCompare);
	for (uint32_t x : {0u, 1u, 4279714709u, 4279714710u, 4279714711u, 0xFFFFFFFFu}) {
		for (uint32_t y : {0u, 15252586u, 0xFFFFFFFFu}) {
			INFO("u32WrapSubCompare differential x=" << x << " y=" << y);
			REQUIRE(wrapOn(x, y) == wrapOff(x, y));
		}
	}

	auto mergeOn = makeAsmJitEngine(true).registerFunction(zeroTripMergeAddConstant);
	auto mergeOff = makeAsmJitEngine(false).registerFunction(zeroTripMergeAddConstant);
	for (uint64_t c : {uint64_t(0), uint64_t(1), uint64_t(42)}) {
		for (uint64_t p : {uint64_t(0), uint64_t(7), uint64_t(1) << 60}) {
			INFO("zeroTripMergeAddConstant differential c=" << c << " p=" << p);
			REQUIRE(mergeOn(c, p) == mergeOff(c, p));
			REQUIRE(mergeOn(c, p) == (c != 0 ? uint64_t(119) : p + 119));
		}
	}
}

TEST_CASE("AsmJit branch fusion: float compares keep the unfused path") {
	auto fnOn = makeAsmJitEngine(true).registerFunction(floatThreshold);
	auto fnOff = makeAsmJitEngine(false).registerFunction(floatThreshold);
	for (double x : {-2.0, 0.0, 1.5, 1.6, 100.0}) {
		INFO("floatThreshold differential x=" << x);
		REQUIRE(fnOn(x) == fnOff(x));
	}
}

TEST_CASE("AsmJit const folding: publishes counters via CompilationStatistics") {
	auto engine = makeAsmJitEngine(true, true);
	auto fn = engine.registerFunction(constArithMix);
	fn(1, 1u);
	auto stats = fn.getStatistics();
	REQUIRE(stats != nullptr);
	REQUIRE(getCounter(stats, "asmjit.lowering.foldedImmediates") >= 5);

	auto off = makeAsmJitEngine(true, false);
	auto fnOff = off.registerFunction(constArithMix);
	fnOff(1, 1u);
	auto statsOff = fnOff.getStatistics();
	REQUIRE(statsOff != nullptr);
	REQUIRE(statsOff->find("asmjit.lowering.foldedImmediates") == nullptr);
}

TEST_CASE("AsmJit const folding: differential correctness across inputs") {
	auto mixOn = makeAsmJitEngine(true, true).registerFunction(constArithMix);
	auto mixOff = makeAsmJitEngine(true, false).registerFunction(constArithMix);
	for (int64_t x : {int64_t(-1000000), int64_t(-1), int64_t(0), int64_t(1), int64_t(12345), INT64_MAX / 2}) {
		for (uint32_t u : {0u, 1u, 858993459u, 0xFFFFFFFFu}) {
			INFO("constArithMix differential x=" << x << " u=" << u);
			REQUIRE(mixOn(x, u) == mixOff(x, u));
		}
	}

	auto wrapOn = makeAsmJitEngine(true, true).registerFunction(i8ConstWrap);
	auto wrapOff = makeAsmJitEngine(true, false).registerFunction(i8ConstWrap);
	for (int32_t x = -128; x <= 127; x++) {
		INFO("i8ConstWrap differential x=" << x);
		REQUIRE(wrapOn(static_cast<int8_t>(x)) == wrapOff(static_cast<int8_t>(x)));
	}

	auto leftOn = makeAsmJitEngine(true, true).registerFunction(constLeftCompare);
	auto leftOff = makeAsmJitEngine(true, false).registerFunction(constLeftCompare);
	for (int32_t t : {-100, -3, -2, 0, 5, 6, 7, 100}) {
		INFO("constLeftCompare differential t=" << t);
		REQUIRE(leftOn(t) == leftOff(t));
		REQUIRE(leftOn(t) == ((6 > t ? 1 : 0) + (-3 <= t ? 2 : 0)));
	}

	auto staleOn = makeAsmJitEngine(true, true).registerFunction(foldedConstShiftCount);
	auto staleOff = makeAsmJitEngine(true, false).registerFunction(foldedConstShiftCount);
	for (int32_t x : {-1024, -1, 0, 1, 12345, INT32_MAX}) {
		INFO("foldedConstShiftCount differential x=" << x);
		REQUIRE(staleOn(x) == staleOff(x));
		REQUIRE(staleOn(x) == (x >> 2));
	}

	// Both optimizations off vs both on -- the full-stack differential.
	auto allOn = makeAsmJitEngine(true, true).registerFunction(fibLike);
	auto allOff = makeAsmJitEngine(false, false).registerFunction(fibLike);
	for (int32_t n : {0, 1, 2, 10, 1000}) {
		INFO("fibLike all-opts differential n=" << n);
		REQUIRE(allOn(n) == allOff(n));
	}
}

TEST_CASE("AsmJit const folding: pinned fuzzer regressions stay correct") {
	auto wrapOn = makeAsmJitEngine(true, true).registerFunction(u32WrapSubCompare);
	auto wrapOff = makeAsmJitEngine(true, false).registerFunction(u32WrapSubCompare);
	for (uint32_t x : {0u, 1u, 4279714709u, 4279714710u, 4279714711u, 0xFFFFFFFFu}) {
		for (uint32_t y : {0u, 15252586u, 0xFFFFFFFFu}) {
			INFO("u32WrapSubCompare const-fold differential x=" << x << " y=" << y);
			REQUIRE(wrapOn(x, y) == wrapOff(x, y));
		}
	}

	auto mergeOn = makeAsmJitEngine(true, true).registerFunction(zeroTripMergeAddConstant);
	auto mergeOff = makeAsmJitEngine(true, false).registerFunction(zeroTripMergeAddConstant);
	for (uint64_t c : {uint64_t(0), uint64_t(1), uint64_t(42)}) {
		for (uint64_t p : {uint64_t(0), uint64_t(7), uint64_t(1) << 60}) {
			INFO("zeroTripMergeAddConstant const-fold differential c=" << c << " p=" << p);
			REQUIRE(mergeOn(c, p) == mergeOff(c, p));
			REQUIRE(mergeOn(c, p) == (c != 0 ? uint64_t(119) : p + 119));
		}
	}
}

TEST_CASE("AsmJit select cmov: differential correctness across inputs") {
	auto mixOn = makeAsmJitEngine(true, true, true).registerFunction(selectMix);
	auto mixOff = makeAsmJitEngine(true, true, false).registerFunction(selectMix);
	for (int64_t x : {int64_t(-100), int64_t(-1), int64_t(0), int64_t(1), int64_t(42), INT64_MAX / 2}) {
		for (int64_t y : {int64_t(-100), int64_t(0), int64_t(42), int64_t(7000)}) {
			INFO("selectMix differential x=" << x << " y=" << y);
			REQUIRE(mixOn(x, y) == mixOff(x, y));
			const int64_t expected = std::abs(x - y) + (x == y ? 100 : 0);
			REQUIRE(mixOn(x, y) == expected);
		}
	}

	auto fOn = makeAsmJitEngine(true, true, true).registerFunction(selectFloatKernel);
	auto fOff = makeAsmJitEngine(true, true, false).registerFunction(selectFloatKernel);
	for (bool c : {false, true}) {
		INFO("selectFloatKernel differential c=" << c);
		REQUIRE(fOn(c, 1.5, -2.5) == fOff(c, 1.5, -2.5));
		REQUIRE(fOn(c, 1.5, -2.5) == (c ? 1.5 : -2.5));
	}
}

#endif // !__aarch64__ && !_M_ARM64
#endif // ENABLE_TRACING
#endif // ENABLE_ASMJIT_BACKEND

} // namespace nautilus::engine
