
#include "ExecutionTest.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>

// Tests for the Post-Register-Allocation peephole pass on the AsmJit backend.
//
// The tests cover two things:
//   1. Integration probe: for a workload that provably contains the target
//      patterns, `fn.getStatistics()` exposes non-zero counters under the
//      `asmjit.peephole.*` namespace, proving the pass runs and writes into
//      the pipeline-wide CompilationStatistics.
//   2. Differential correctness: compiling the same function with the
//      peephole enabled vs. disabled must produce bit-identical return
//      values for every input we probe. This is the primary safety net
//      against future rule additions silently breaking correctness.

namespace nautilus::engine {

#ifdef ENABLE_ASMJIT_BACKEND
#ifdef ENABLE_TRACING

namespace {

// A loop-heavy function: SSA block-argument lowering emits one `mov rD, rS`
// per phi per iteration. Whether any post-RA self-moves remain depends on
// how aggressively asmjit's RA coalesces, so the differential tests below
// use this as a correctness probe rather than a stats probe.
val<int64_t> sumToN(val<int64_t> n) {
	val<int64_t> acc = (int64_t) 0;
	for (val<int64_t> i = (int64_t) 0; i < n; i = i + 1) {
		acc = acc + i;
	}
	return acc;
}

// Multi-phi loop with several loop-carried values. With more values
// competing for the same coalescing slots, a same-register self-move is
// substantially more likely to survive RA. Used as the integration probe
// that confirms Rule 1 actually fires through the Engine path.
val<int64_t> dotLikeKernel(val<int64_t> n) {
	val<int64_t> a = (int64_t) 0;
	val<int64_t> b = (int64_t) 1;
	val<int64_t> c = (int64_t) 2;
	val<int64_t> d = (int64_t) 3;
	for (val<int64_t> i = (int64_t) 0; i < n; i = i + 1) {
		val<int64_t> na = a + b;
		val<int64_t> nb = b + c;
		val<int64_t> nc = c + d;
		val<int64_t> nd = d + a;
		a = na;
		b = nb;
		c = nc;
		d = nd;
	}
	return a + b + c + d;
}

// A function returning the literal integer 0 provokes the zero-idiom
// pattern: `mov rax, 0; ret`. The `ret` in the forward-flag scan is
// classified as a flag clobberer (callee ABI), so the peephole fires.
val<int64_t> returnZero() {
	return (int64_t) 0;
}

// A branch-on-zero-const function exercises the upstream `cmp reg, 0` →
// `test reg, reg` rewrite in visitCompare. We cannot directly observe the
// encoding from test code, but we can assert correctness across all the
// edge cases that any compare-with-zero cares about.
val<int32_t> classifyVsZero(val<int32_t> x) {
	if (x == (int32_t) 0) {
		return (int32_t) 0;
	}
	if (x > (int32_t) 0) {
		return (int32_t) 1;
	}
	return (int32_t) -1;
}

// A ui32 argument goes through the `mov r32, r32` zero-extension idiom at
// binding time. If the peephole mistakenly deletes that self-move, this
// function will return garbage in the upper 32 bits of the result (and our
// differential harness catches it). This is the specific correctness case
// the conservative allow-list was designed for.
val<uint64_t> ui32Identity(val<uint32_t> x) {
	return static_cast<val<uint64_t>>(x);
}

engine::NautilusEngine makeAsmJitEngine(bool enablePeephole) {
	return nautilus::testing::makeEngine("asmjit", [enablePeephole](engine::Options& opts) {
		opts.setOption("asmjit.enablePostRAPeephole", enablePeephole);
		opts.setOption("engine.traceMode", "lazyTracing");
		// Force the legacy (non-tiered) path so stats land on the same
		// executable we query here. The default tiered pipeline starts
		// with the bytecode backend at tier-0 and promotes to the
		// default tier-1 backend asynchronously; by then the asmjit
		// peephole stats would not be on the currently-active executable.
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

TEST_CASE("PostRA peephole: pass publishes counters via CompilationStatistics") {
	// Compiling a workload that mixes (a) a multi-phi loop (Rule 1 candidate)
	// and (b) a function returning the literal zero (Rule 2 candidate) must
	// produce at least one rewrite total. This is the integration probe:
	// it proves the pass is added to the Compiler's pass list and is being
	// invoked after RA, and that it uses the pipeline-wide
	// CompilationStatistics sink the rest of the compiler reports into.
	//
	// The zero-idiom rewrite (`mov r, 0` → `xor r32, r32`) is x86-specific:
	// ARM64 has `wzr`/`xzr` so no such rewrite exists and the A64 peephole
	// does not publish `zeroIdiomsApplied`. The probe below is therefore
	// guarded to x86.
#if !defined(__aarch64__) && !defined(_M_ARM64)
	{
		auto engine = makeAsmJitEngine(true);
		auto zero = engine.registerFunction(returnZero);

		REQUIRE(zero() == 0);
		auto stats = zero.getStatistics();
		REQUIRE(stats != nullptr);
		// The returnZero function emits `mov rax, 0; ret`. The forward
		// flag scan classifies `ret` as a flag clobberer, so the
		// rewrite is safe and must fire exactly once.
		REQUIRE(getCounter(stats, "asmjit.peephole.zeroIdiomsApplied") >= 1);
	}
#endif

	{
		auto engine = makeAsmJitEngine(true);
		auto kernel = engine.registerFunction(dotLikeKernel);
		REQUIRE(kernel(10) >= 0);
		auto stats = kernel.getStatistics();
		REQUIRE(stats != nullptr);
		// Either rule must fire on this workload; assert the aggregated
		// total is positive to stay robust against asmjit RA tweaks.
		const int64_t total = getCounter(stats, "asmjit.peephole.selfMovesRemoved") +
		                      getCounter(stats, "asmjit.peephole.zeroIdiomsApplied");
		REQUIRE(total > 0);
	}
}

TEST_CASE("PostRA peephole: disabling pass suppresses counter emission") {
	auto engine = makeAsmJitEngine(false);
	auto fn = engine.registerFunction(returnZero);
	REQUIRE(fn() == 0);

	auto stats = fn.getStatistics();
	REQUIRE(stats != nullptr);
	// Keys must be absent when the pass is disabled — no one writes them.
	REQUIRE(stats->find("asmjit.peephole.selfMovesRemoved") == nullptr);
	REQUIRE(stats->find("asmjit.peephole.zeroIdiomsApplied") == nullptr);
}

TEST_CASE("PostRA peephole: differential correctness across inputs") {
	auto sumOn = makeAsmJitEngine(true).registerFunction(sumToN);
	auto sumOff = makeAsmJitEngine(false).registerFunction(sumToN);

	for (int64_t n : {0, 1, 2, 7, 13, 100, 10'000}) {
		INFO("sumToN differential n=" << n);
		REQUIRE(sumOn(n) == sumOff(n));
	}

	auto kernelOn = makeAsmJitEngine(true).registerFunction(dotLikeKernel);
	auto kernelOff = makeAsmJitEngine(false).registerFunction(dotLikeKernel);
	for (int64_t n : {0, 1, 7, 100, 1000}) {
		INFO("dotLikeKernel differential n=" << n);
		REQUIRE(kernelOn(n) == kernelOff(n));
	}
}

TEST_CASE("PostRA peephole: classifyVsZero differential correctness") {
	auto fnOn = makeAsmJitEngine(true).registerFunction(classifyVsZero);
	auto fnOff = makeAsmJitEngine(false).registerFunction(classifyVsZero);

	for (int32_t x : {-100, -1, 0, 1, 100, INT32_MIN, INT32_MAX}) {
		INFO("classifyVsZero differential x=" << x);
		REQUIRE(fnOn(x) == fnOff(x));
	}
}

TEST_CASE("PostRA peephole: ui32 argument zero-extend is preserved") {
	// Regression guard: `mov r32, r32` is load-bearing for ui32 argument
	// binding. The conservative allow-list excludes it from self-move
	// deletion. If the exclusion is ever removed, the upper 32 bits of
	// the result would carry garbage and this assertion would fail.
	auto fnOn = makeAsmJitEngine(true).registerFunction(ui32Identity);
	auto fnOff = makeAsmJitEngine(false).registerFunction(ui32Identity);

	for (uint32_t x : {0u, 1u, 0xDEADBEEFu, 0xFFFFFFFFu}) {
		INFO("ui32Identity x=" << x);
		REQUIRE(fnOn(x) == static_cast<uint64_t>(x));
		REQUIRE(fnOn(x) == fnOff(x));
	}
}

#endif // ENABLE_TRACING
#endif // ENABLE_ASMJIT_BACKEND

} // namespace nautilus::engine
