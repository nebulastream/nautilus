#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>

// BCLoweringProvider::process(BasicBlockInvocation) sequences the register moves
// for a block invocation (e.g. a loop back-edge) as a parallel copy instead of
// unconditionally staging every argument through a temp: a plain chain (a=b; b=c)
// costs one MOV per argument, but a genuine permutation cycle among the loop-carried
// registers (e.g. a literal swap a,b = b,a) needs a temp to break the cycle. These
// kernels are built specifically to force 2- and 3-element cycles, plus a cycle mixed
// with an independent chained value, and check the bc backend's result against the
// non-compiling "interpreter" execution mode (an independent implementation that
// never goes through BCLoweringProvider).
#ifdef ENABLE_BC_BACKEND

namespace nautilus::engine {

namespace {

val<int64_t> swap2(val<int64_t> a0, val<int64_t> b0, val<int64_t> n) {
	val<int64_t> a = a0;
	val<int64_t> b = b0;
	for (val<int64_t> i = 0; i < n; i = i + 1) {
		val<int64_t> tmp = a;
		a = b;
		b = tmp;
	}
	return a * 1000003 + b;
}

val<int64_t> rotate3(val<int64_t> a0, val<int64_t> b0, val<int64_t> c0, val<int64_t> n) {
	val<int64_t> a = a0;
	val<int64_t> b = b0;
	val<int64_t> c = c0;
	for (val<int64_t> i = 0; i < n; i = i + 1) {
		val<int64_t> t = c;
		c = b;
		b = a;
		a = t;
	}
	return a * 1000003 * 1000003 + b * 1000003 + c;
}

// A 3-cycle among a/b/c plus an independent value d that chains off a each
// iteration, so the parallel copy must resolve the cycle without disturbing d's read.
val<int64_t> mixedCycleAndChain(val<int64_t> a0, val<int64_t> b0, val<int64_t> c0, val<int64_t> d0, val<int64_t> n) {
	val<int64_t> a = a0;
	val<int64_t> b = b0;
	val<int64_t> c = c0;
	val<int64_t> d = d0;
	for (val<int64_t> i = 0; i < n; i = i + 1) {
		val<int64_t> na = b;
		val<int64_t> nb = c;
		val<int64_t> nc = a;
		val<int64_t> nd = d + a;
		a = na;
		b = nb;
		c = nc;
		d = nd;
	}
	return a + b * 100 + c * 10000 + d * 1000000;
}

} // namespace

TEST_CASE("BC register coalescing handles loop-carried permutation cycles") {
	engine::Options bcOptions;
	bcOptions.setOption("engine.backend", std::string("bc"));
	// Coalescing is opt-in (off by default); enable it explicitly so this
	// test actually exercises emitParallelCopy instead of the default
	// always-stage-through-a-temp path.
	bcOptions.setOption("bc.registerCoalescing", true);
	engine::NautilusEngine bc(bcOptions);

	engine::Options refOptions;
	refOptions.setOption("engine.Compilation", false);
	engine::NautilusEngine reference(refOptions);

	auto bcSwap = bc.registerFunction(swap2);
	auto refSwap = reference.registerFunction(swap2);
	auto bcRotate = bc.registerFunction(rotate3);
	auto refRotate = reference.registerFunction(rotate3);
	auto bcMixed = bc.registerFunction(mixedCycleAndChain);
	auto refMixed = reference.registerFunction(mixedCycleAndChain);

	for (int64_t n = 0; n <= 20; n++) {
		REQUIRE(bcSwap(3, 7, n) == refSwap(3, 7, n));
		REQUIRE(bcRotate(2, 5, 11, n) == refRotate(2, 5, 11, n));
		REQUIRE(bcMixed(1, 2, 3, 4, n) == refMixed(1, 2, 3, 4, n));
	}
}

} // namespace nautilus::engine

#endif // ENABLE_BC_BACKEND
