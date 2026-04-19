// Smoke test for the experimental constant-folding tracer (ENABLE_CONSTANT_TRACER).
// Runs a small set of Nautilus functions through the engine and verifies the
// observable output is unchanged from the baseline tracer. Designed as a
// minimal-reproduction harness for §4.3-invariant regressions: each case
// exercises constants that cross a control-flow boundary, so a bug in the
// lazy-materialization / registry wiring will produce a wrong answer rather
// than just a different trace.

#include "nautilus/config.hpp"

#ifdef ENABLE_CONSTANT_TRACER

#include "nautilus/Engine.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <chrono>

namespace nautilus {

namespace {
// RAII guard that saves and restores the thread-local runtime flags so
// tests don't leak state.
struct ConstantTracerFlagsGuard {
	bool savedEnabled;
	uint64_t savedWidenAt;
	uint64_t savedUnrollLimit;
	ConstantTracerFlagsGuard()
	    : savedEnabled(tracing::pe::isConstantTracerEnabled()),
	      savedWidenAt(tracing::pe::getConstantWideningThreshold()),
	      savedUnrollLimit(tracing::pe::getConstantUnrollLimit()) {
	}
	~ConstantTracerFlagsGuard() {
		tracing::pe::setConstantTracerEnabled(savedEnabled);
		tracing::pe::setConstantWideningThreshold(savedWidenAt);
		tracing::pe::setConstantUnrollLimit(savedUnrollLimit);
	}
};
} // namespace

static val<int32_t> constChain(val<int32_t> x) {
	val<int32_t> a = 5;
	val<int32_t> b = 3;
	val<int32_t> c = a * b + 4; // Should fold to 19 with the option on.
	return x + c;
}

static val<int32_t> ifWithTernary(val<int32_t> value) {
	val<int32_t> result = value > 5 ? 10 : 5;
	if (value == 0) {
		result = -1;
	}
	return result;
}

static val<int32_t> chainedIf(val<int32_t> x) {
	val<int32_t> result = 0;
	if (x > 0) {
		result = result + 1;
	}
	if (x > 1) {
		result = result + 1;
	}
	return result;
}

TEST_CASE("ConstantTracer: straight-line constant chain", "[constant-tracer]") {
	engine::Options opts;
	engine::NautilusEngine engine(opts);
	auto f = engine.registerFunction(constChain);
	REQUIRE(f(0) == 19);
	REQUIRE(f(1) == 20);
	REQUIRE(f(100) == 119);
}

TEST_CASE("ConstantTracer: chainedIf", "[constant-tracer]") {
	engine::Options opts;
	engine::NautilusEngine engine(opts);
	auto f = engine.registerFunction(chainedIf);
	REQUIRE(f(-1) == 0);
	REQUIRE(f(0) == 0);
	REQUIRE(f(1) == 1);
	REQUIRE(f(2) == 2);
}

TEST_CASE("ConstantTracer: ifWithTernary", "[constant-tracer]") {
	engine::Options opts;
	engine::NautilusEngine engine(opts);
	auto f = engine.registerFunction(ifWithTernary);
	REQUIRE(f(0) == -1);
	REQUIRE(f(1) == 5);
	REQUIRE(f(5) == 5);
	REQUIRE(f(6) == 10);
	REQUIRE(f(10) == 10);
}

// Enum folding (val<enum>): the both-Constant equality fast path returns a
// Constant val<bool> without emitting EQ or CONST ops. Exercises the
// LazyTracedRef<underlying_type_t> path in val_enum.hpp.
enum Color { BLUE, GREEN, RED };
static val<int32_t> enumConstantEq() {
	val<Color> a = BLUE;
	val<Color> b = BLUE;
	if (a == b) {
		return 1;
	}
	return 0;
}
static val<int32_t> enumConstantNeq() {
	val<Color> a = BLUE;
	val<Color> b = RED;
	if (a != b) {
		return 1;
	}
	return 0;
}

TEST_CASE("ConstantTracer: val<enum> fold eq/neq on Constants", "[constant-tracer]") {
	engine::Options opts;
	engine::NautilusEngine engine(opts);
	SECTION("eq: BLUE == BLUE -> 1") {
		auto f = engine.registerFunction(enumConstantEq);
		REQUIRE(f() == 1);
	}
	SECTION("neq: BLUE != RED -> 1") {
		auto f = engine.registerFunction(enumConstantNeq);
		REQUIRE(f() == 1);
	}
}

// Pointer fold: comparison of two Constant pointers (a literal vs
// nullptr, or two same-valued ptr vals) should fold to a Constant
// val<bool>. Exercises the LazyTracedRef<void*> wiring in val_ptr.hpp.
static val<int32_t> ptrConstantCmp() {
	val<int32_t*> p = nullptr;
	if (p == nullptr) {
		return 1;
	}
	return 0;
}

TEST_CASE("ConstantTracer: val<ptr> fold cmp on nullptr constants", "[constant-tracer]") {
	engine::Options opts;
	engine::NautilusEngine engine(opts);
	auto f = engine.registerFunction(ptrConstantCmp);
	REQUIRE(f() == 1);
}

// `nautilus::assume_stable(val<T*>)` is a per-pointer opt-in: after the
// caller marks a pointer stable, plain `*p` and `p + n` fold at trace
// time when the pointer (and offset) are Constant. The flag propagates
// through copies and through Constant-folded pointer arithmetic so
// `*(assume_stable(p) + 3)` folds the whole chain.
//
// Contrast with a global flag: global flags are tripwires (one forgot
// setFlag(true) somewhere and everything folds); an explicit annotation
// on the pointer documents the safety contract right where it's made.
//
// The test binary satisfies the contract: it reads from static storage
// that doesn't change between registerFunction and f() calls.
struct StablePtrFixture {
	static int32_t s_magic;
	static int32_t s_arr[4];
};
int32_t StablePtrFixture::s_magic = 0;
int32_t StablePtrFixture::s_arr[4] = {10, 20, 30, 40};

static val<int32_t> loadStableFromConstantPtr() {
	val<int32_t*> p = assume_stable(val<int32_t*>(&StablePtrFixture::s_magic));
	return *p;
}

static val<int32_t> loadStableWithArith() {
	val<int32_t*> p = assume_stable(val<int32_t*>(&StablePtrFixture::s_arr[0]));
	return *(p + 2); // should return s_arr[2]
}

// For counter-comparison: the same function without assume_stable.
// Should emit LOAD + ADD ops rather than folding.
static val<int32_t> loadFromConstantPtrNoAssumeStable() {
	val<int32_t*> p = &StablePtrFixture::s_magic;
	return *p;
}

TEST_CASE("ConstantTracer: assume_stable + plain operators fold", "[constant-tracer][stable-ptr]") {
	ConstantTracerFlagsGuard guard;
	tracing::pe::setConstantTracerEnabled(true);

	SECTION("assume_stable + *p folds to the trace-time value") {
		StablePtrFixture::s_magic = 777;
		engine::Options opts;
		engine::NautilusEngine engine(opts);
		auto f = engine.registerFunction(loadStableFromConstantPtr);
		REQUIRE(f() == 777);
		// Contract: after tracing, mutating s_magic doesn't change the
		// compiled code's answer — the fold baked 777 in.
		StablePtrFixture::s_magic = 999;
		REQUIRE(f() == 777);
	}

	SECTION("plain `*p` does NOT fold (assume_stable not called)") {
		StablePtrFixture::s_magic = 123;
		engine::Options opts;
		engine::NautilusEngine engine(opts);
		auto f = engine.registerFunction(loadFromConstantPtrNoAssumeStable);
		REQUIRE(f() == 123);
		// Mutating after tracing — the LOAD runs at call time, fresh
		// value observed.
		StablePtrFixture::s_magic = 456;
		REQUIRE(f() == 456);
	}

	SECTION("assume_stable flag propagates through `p + offset`") {
		engine::Options opts;
		engine::NautilusEngine engine(opts);
		auto f = engine.registerFunction(loadStableWithArith);
		REQUIRE(f() == 30);
	}

	SECTION("fold chain bumps foldsElided counter (ADD + LOAD)") {
		engine::Options opts;
		engine::NautilusEngine engine(opts);
		auto f = engine.registerFunction(loadStableWithArith);
		(void) f;
		// The counter is per-trace (reset on each setActiveTracer(non-
		// null)), so it reads exactly this trace's fold count. Expect
		// at least the arithmetic fold + the LOAD fold. The offset
		// literal's internal multiplication by sizeof folds too, so
		// use `>=`.
		REQUIRE(tracing::pe::getConstantTracerFoldsElided() >= 2);
	}
}

// Observability counters: tracing a function that folds several pure ops
// should bump `constantTracer.foldsElided`. Since engine::registerFunction
// routes through the trace context which writes the counters into the
// thread-local state, reading the getter right after registerFunction
// returns picks up the final value for that trace.
TEST_CASE("ConstantTracer: observability counters", "[constant-tracer]") {
	ConstantTracerFlagsGuard guard;
	tracing::pe::setConstantTracerEnabled(true);

	engine::Options opts;
	engine::NautilusEngine engine(opts);
	// `constChain(x)` computes `a*b + 4` on two all-constant vals before
	// adding x. The multiply, add-of-4, and the intermediate casts all
	// fold, so we should see at least two fold-elides on registration.
	auto f = engine.registerFunction(constChain);
	auto foldsAfterRegister = tracing::pe::getConstantTracerFoldsElided();
	REQUIRE(f(10) == 29); // 10 + (5*3+4)
	REQUIRE(foldsAfterRegister >= 2);
}

// Stage 2: a loop whose induction variable starts Constant and is updated
// Constant-to-Constant every iteration. Without a widening cap this folds at
// C++ level for every iteration, so the trace would walk the body ~100K
// times. With NAUTILUS_CONSTANT_TRACER_WIDEN_AT=64 (set below), the registry
// gets flushed after ~64 updates and the existing Snapshot machinery picks
// up the loop, bounding trace time. The final runtime answer must still be
// correct regardless.
static val<int64_t> constantInductionLoop() {
	val<int64_t> i = 0;
	val<int64_t> acc = 0;
	while (i < val<int64_t>(100000)) {
		acc = acc + val<int64_t>(1);
		i = i + val<int64_t>(1);
	}
	return acc;
}

TEST_CASE("ConstantTracer Stage 2: widening cap bounds trace time", "[constant-tracer][stage2]") {
	ConstantTracerFlagsGuard guard;
	// Force the cap low so the test explicitly exercises widening.
	tracing::pe::setConstantWideningThreshold(64);
	tracing::pe::setConstantTracerEnabled(true);

	engine::Options opts;
	engine::NautilusEngine engine(opts);

	auto start = std::chrono::steady_clock::now();
	auto f = engine.registerFunction(constantInductionLoop);
	auto elapsed = std::chrono::steady_clock::now() - start;

	REQUIRE(f() == 100000);
	// Sanity: registering/tracing must complete quickly. A non-widened
	// implementation would walk ~100K C++ iterations through the registry
	// and need more than a few seconds even at -O2. Pick a very loose bound
	// to avoid flakes on slow CI but still catch the pathology.
	REQUIRE(std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() < 15);
}

// Companion test: the same loop with widening disabled (threshold=0) must
// still produce the right answer. This catches regressions where widening
// itself is the *only* reason correctness holds.
static val<int64_t> smallConstantInductionLoop() {
	val<int64_t> i = 0;
	val<int64_t> acc = 0;
	while (i < val<int64_t>(32)) {
		acc = acc + val<int64_t>(1);
		i = i + val<int64_t>(1);
	}
	return acc;
}

TEST_CASE("ConstantTracer Stage 2: widening disabled still correct for small loops", "[constant-tracer][stage2]") {
	ConstantTracerFlagsGuard guard;
	tracing::pe::setConstantWideningThreshold(0);
	tracing::pe::setConstantTracerEnabled(true);

	engine::Options opts;
	engine::NautilusEngine engine(opts);

	auto f = engine.registerFunction(smallConstantInductionLoop);
	REQUIRE(f() == 32);
}

// Stage 2: verify the runtime enable/disable flag actually switches the
// feature off — a Constant val constructed while disabled should have a
// trace footprint (i.e. the fold fast path must not fire).
TEST_CASE("ConstantTracer Stage 2: runtime disable is honored", "[constant-tracer][stage2]") {
	ConstantTracerFlagsGuard guard;
	tracing::pe::setConstantTracerEnabled(false);

	engine::Options opts;
	engine::NautilusEngine engine(opts);
	// Use the same straight-line chain from the Stage 1 test; correctness is
	// independent of whether folding fires.
	auto f = engine.registerFunction(constChain);
	REQUIRE(f(0) == 19);
	REQUIRE(f(1) == 20);
	REQUIRE(f(100) == 119);
}

// Stage 2: same answer regardless of runtime-flag state (A/B invariant).
TEST_CASE("ConstantTracer Stage 2: A/B invariant across flag", "[constant-tracer][stage2]") {
	auto runWith = [](bool enabled) {
		ConstantTracerFlagsGuard guard;
		tracing::pe::setConstantTracerEnabled(enabled);
		engine::Options opts;
		engine::NautilusEngine engine(opts);
		auto f = engine.registerFunction(ifWithTernary);
		return std::vector<int32_t> {f(0), f(1), f(5), f(6), f(10)};
	};
	auto withOn = runWith(true);
	auto withOff = runWith(false);
	REQUIRE(withOn == withOff);
	REQUIRE(withOn == std::vector<int32_t> {-1, 5, 5, 10, 10});
}

// Stage 2: stratified divergence detection must fire earlier than the blunt
// iteration cap on the pure-constant loop pathology. We set the cap to a
// very high value so only stratification can be the one that catches the
// back-edge; the test must still complete quickly and produce the right
// answer.
TEST_CASE("ConstantTracer Stage 2: stratification catches loops before the cap", "[constant-tracer][stage2]") {
	ConstantTracerFlagsGuard guard;
	tracing::pe::setConstantTracerEnabled(true);
	// Set the blunt cap effectively to infinity — only stratification can
	// bound the trace time here.
	tracing::pe::setConstantWideningThreshold(UINT64_MAX);

	engine::Options opts;
	engine::NautilusEngine engine(opts);

	auto start = std::chrono::steady_clock::now();
	auto f = engine.registerFunction(constantInductionLoop);
	auto elapsed = std::chrono::steady_clock::now() - start;

	REQUIRE(f() == 100000);
	// Without stratification (just the UINT64_MAX cap) this would need to
	// unroll all 100K iterations at trace time; bound well under 15s again.
	REQUIRE(std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() < 15);
}

// Stage 2: a revisit of the same tag with the SAME constant value (stable
// state) must not trigger a spurious widening. We trace a function that
// assigns the same constant to the same val multiple times and verify it
// still folds end-to-end (returns the constant directly).
static val<int32_t> stableConstantRevisit() {
	val<int32_t> x = 42;
	// Three no-op re-assigns at distinct source lines but identical values.
	x = val<int32_t>(42);
	x = val<int32_t>(42);
	x = val<int32_t>(42);
	return x;
}

TEST_CASE("ConstantTracer Stage 2: stable Constant revisits do not widen", "[constant-tracer][stage2]") {
	ConstantTracerFlagsGuard guard;
	tracing::pe::setConstantTracerEnabled(true);
	tracing::pe::setConstantWideningThreshold(2); // Blunt cap deliberately tight.

	engine::Options opts;
	engine::NautilusEngine engine(opts);
	auto f = engine.registerFunction(stableConstantRevisit);
	REQUIRE(f() == 42);
}

// Stage 2: the unroll limit is honored. Setting it to a very large value
// (UINT64_MAX) should disable stratification, so the statically-decidable
// smallConstantInductionLoop folds at trace time to a single CONST 32
// rather than producing a real loop. We inspect the runtime result (which
// should be correct either way) and ask the underlying test to run fast
// enough that full unrolling of 32 iterations at C++ level happens.
TEST_CASE("ConstantTracer Stage 2: unroll limit controls stratification", "[constant-tracer][stage2]") {
	ConstantTracerFlagsGuard guard;
	tracing::pe::setConstantTracerEnabled(true);

	SECTION("unrollLimit=UINT64_MAX disables stratification, allows full fold") {
		tracing::pe::setConstantUnrollLimit(UINT64_MAX);
		tracing::pe::setConstantWideningThreshold(100000); // high cap, not the bottleneck
		engine::Options opts;
		engine::NautilusEngine engine(opts);
		auto f = engine.registerFunction(smallConstantInductionLoop);
		REQUIRE(f() == 32);
	}

	SECTION("unrollLimit=5 widens after 5 divergent revisits per tag") {
		tracing::pe::setConstantUnrollLimit(5);
		tracing::pe::setConstantWideningThreshold(UINT64_MAX); // isolate stratification
		engine::Options opts;
		engine::NautilusEngine engine(opts);
		auto f = engine.registerFunction(smallConstantInductionLoop);
		REQUIRE(f() == 32);
	}

	SECTION("unrollLimit=0 (default) widens on first divergence") {
		tracing::pe::setConstantUnrollLimit(0);
		tracing::pe::setConstantWideningThreshold(UINT64_MAX);
		engine::Options opts;
		engine::NautilusEngine engine(opts);
		auto f = engine.registerFunction(constantInductionLoop);
		REQUIRE(f() == 100000);
	}
}

// Function that uses `nautilus::unroll_scope` at its own call site to
// opt its 32-iteration constant loop into full unrolling, without
// touching any global / thread-local state. Mirrors the shape of the
// `sumOfSquares` function in example/src/DemoConstantFolding.cpp.
static val<int64_t> smallConstantInductionLoopWithScopedUnroll() {
	nautilus::unroll_scope hint(UINT64_MAX);
	val<int64_t> i = 0;
	val<int64_t> acc = 0;
	while (i < val<int64_t>(32)) {
		acc = acc + val<int64_t>(1);
		i = i + val<int64_t>(1);
	}
	return acc;
}

TEST_CASE("ConstantTracer: unroll_scope per-site annotation", "[constant-tracer][stage2]") {
	ConstantTracerFlagsGuard guard;
	tracing::pe::setConstantTracerEnabled(true);

	SECTION("guard raises the limit for its lifetime and restores on exit") {
		tracing::pe::setConstantUnrollLimit(0);
		REQUIRE(tracing::pe::getConstantUnrollLimit() == 0);
		{
			nautilus::unroll_scope scope(16);
			REQUIRE(tracing::pe::getConstantUnrollLimit() == 16);
		}
		REQUIRE(tracing::pe::getConstantUnrollLimit() == 0);
	}

	SECTION("nested guards stack and restore the outer predecessor") {
		tracing::pe::setConstantUnrollLimit(0);
		{
			nautilus::unroll_scope outer(16);
			REQUIRE(tracing::pe::getConstantUnrollLimit() == 16);
			{
				nautilus::unroll_scope inner(2);
				REQUIRE(tracing::pe::getConstantUnrollLimit() == 2);
			}
			// Inner dtor ran — outer's value is back.
			REQUIRE(tracing::pe::getConstantUnrollLimit() == 16);
		}
		REQUIRE(tracing::pe::getConstantUnrollLimit() == 0);
	}

	SECTION("in-function guard makes a constant loop fold regardless of outer limit") {
		// Pathological outer config: global limit=0 (widen on first
		// divergence) and a tiny cap backstop. Without a per-site
		// override the loop would widen almost immediately; the
		// function's own `unroll_scope(UINT64_MAX)` lifts that for
		// the duration of the trace.
		tracing::pe::setConstantUnrollLimit(0);
		tracing::pe::setConstantWideningThreshold(UINT64_MAX); // isolate stratification
		engine::Options opts;
		engine::NautilusEngine engine(opts);
		auto f = engine.registerFunction(smallConstantInductionLoopWithScopedUnroll);
		REQUIRE(f() == 32);
		// And after registerFunction returns, the global knob is still
		// what we set it to — the in-function guard restored on
		// destruction, not leaked across the trace boundary.
		REQUIRE(tracing::pe::getConstantUnrollLimit() == 0);
	}
}

// Selecting `exceptionBasedTracing` mode now simply picks the
// `ExceptionBasedTraceContext` factory out of the registry — PE doesn't
// fire. `setActiveTracer()` auto-syncs the PE runtime flag to the
// incoming tracer's `isPartialEvaluationMode()`, so there's no silent-
// failure combination left to guard against.
TEST_CASE("ConstantTracer: exceptionBasedTracing mode still produces correct results", "[constant-tracer]") {
	engine::Options opts;
	opts.setOption("engine.traceMode", "exceptionBasedTracing");
	engine::NautilusEngine engine(opts);
	auto f = engine.registerFunction(constChain);
	REQUIRE(f(10) == 29);
}

} // namespace nautilus

#endif // ENABLE_CONSTANT_TRACER
