// DemoConstantFolding.cpp — showcase the experimental constant-folding tracer.
//
// When Nautilus is built with -DENABLE_CONSTANT_TRACER=ON, operations on
// val<T> that have all-compile-time-known operands fold *while tracing*,
// so the generated IR never emits the corresponding ops. This demo walks
// through three ways that pays off:
//
//   1. Arithmetic chains: `(5 * 3 + 3 - 2) * (5 * 3 + 3 - 2) - 1` should
//      appear in the compiled IR as `return 255`, not as a graph of 5
//      arithmetic ops.
//
//   2. Loop unrolling: a for-loop whose trip count and body are all
//      Constant fully unrolls at trace time — the compiled IR is again a
//      single `return CONST(N)`, no loop at all.
//
//   3. Constant pointer / LUT lookup: `nautilus::assume_stable(ptr)`
//      lets the caller promise that a pointer's address is valid at
//      execution time *and* the pointed-to memory is stable across the
//      trace→execution boundary. When both the pointer and the index
//      are Constant, pointer arithmetic and LOAD both fold — a table
//      lookup collapses to a single CONST in the IR.
//
// Run with `--log-ir` to dump the generated IR and see the CONST returns
// for yourself. The fold count printed after each registration comes
// from `tracing::pe::getConstantTracerFoldsElided()`, which is reset at the
// start of each trace.
//
// When built *without* ENABLE_CONSTANT_TRACER, every demo still produces
// the correct answer — the fold paths simply no-op and the generated IR
// contains the full graph. The fold counter stays at 0.

#include <cstdint>
#include <cstring>
#include <iostream>
#include <nautilus/Engine.hpp>
#include <nautilus/tracing/TracingUtil.hpp>
#include <nautilus/val.hpp>
#include <string>

using namespace nautilus;

// -----------------------------------------------------------------------------
// Demo 1 — arithmetic chain: every op folds, IR is a single `return 255`.
// -----------------------------------------------------------------------------
val<int32_t> arithChain() {
	val<int32_t> a = 5;
	val<int32_t> b = 3;
	val<int32_t> c = a * b;     // 15  — fold
	val<int32_t> d = c + b - 2; // 16  — fold
	return d * d - 1;           // 255 — fold
}

// -----------------------------------------------------------------------------
// Demo 2 — loop unrolling: sum of i*i for i in [0,10).
//
// The induction variable starts Constant (i=0) and every update keeps it
// Constant (i+1 where i is Constant, 1 is literal). Every comparison
// `i < 10` folds to a Constant bool, so `traceBool` takes the live branch
// without emitting a CMP. The body `sum = sum + i*i` is all Constants so
// `sum` stays Constant too.
//
// `nautilus::unroll_scope(16)` is the per-loop opt-in: for the duration
// of the guard, the Stage 2 stratified-widening limit is raised to 16,
// so the first few back-edge revisits don't force an early materialize.
// Scope-wide (all program points evaluated during the guard's lifetime
// see the raised limit), but that's fine here — we want the one loop
// inside to unroll. The guard restores the previous limit on
// destruction, so other functions keep the default widening behavior.
// -----------------------------------------------------------------------------
val<int32_t> sumOfSquares() {
	val<int32_t> sum = 0;
	nautilus::unroll_scope unrollHint(16);
	for (val<int32_t> i = 0; i < 10; i = i + 1) {
		sum = sum + i * i;
	}
	return sum;
}

// -----------------------------------------------------------------------------
// Demo 3 — constant pointer / LUT lookup.
//
// kSquaresLut is a static array whose contents don't change between
// trace-time and execution — the classic "lookup table" pattern. Calling
// `nautilus::assume_stable(ptr)` flips a bit on the pointer val that
// authorizes the tracer to fold through it.
//
// With the bit set, `ptr + Constant_index` folds the pointer arithmetic
// *and* propagates the bit onto the result pointer; then `*result` folds
// the LOAD too. Net: the compiled IR for `lutLookupKnownIndex()` is a
// single `return 16` — no ADD, no LOAD.
//
// With a runtime index (`lutLookupDynamic(idx)`), the ADD has a non-
// Constant operand so the fold can't fire; the demo falls back to
// normal ADD + LOAD ops. `assume_stable` is a no-op in that case, so
// the two functions are identical modulo the index being known.
// -----------------------------------------------------------------------------
static const int64_t kSquaresLut[8] = {0, 1, 4, 9, 16, 25, 36, 49};

val<int64_t> lutLookupKnownIndex() {
	val<int64_t*> base = assume_stable(val<int64_t*>(const_cast<int64_t*>(kSquaresLut)));
	return *(base + 4); // folds to CONST 16
}

val<int64_t> lutLookupDynamic(val<int64_t> idx) {
	val<int64_t*> base = assume_stable(val<int64_t*>(const_cast<int64_t*>(kSquaresLut)));
	return *(base + idx); // ADD + LOAD emitted; runtime lookup
}

namespace {

// One-shot demo runner: register `fn`, print the fold count, call it.
template <typename Fn, typename... Args>
void runDemo(const char* label, const engine::NautilusEngine& engine, Fn fn, Args... args) {
	auto compiled = engine.registerFunction(fn);
	auto folds = tracing::pe::getConstantTracerFoldsElided();
	auto result = compiled(args...);
	std::cout << "  " << label << "\n";
	std::cout << "    folds elided at trace time : " << folds << "\n";
	std::cout << "    result                     : " << result << "\n\n";
}

} // namespace

int main(int argc, char* argv[]) {
	bool logIr = false;
	for (int i = 1; i < argc; ++i) {
		if (std::strcmp(argv[i], "--log-ir") == 0) {
			logIr = true;
		}
	}

	engine::Options options;
	options.setOption("engine.backend", "cpp");
	if (logIr) {
		// Dump the generated IR to stderr so the user can eyeball the
		// folded-away ops — a single `return CONST(...)` for the
		// fully-foldable demos.
		options.setOption("engine.logIr", true);
	}

	// The per-loop `nautilus::unroll_scope(16)` inside sumOfSquares()
	// raises the Stage 2 unroll limit for just that function — no
	// process-wide state to set here.
	engine::NautilusEngine engine(options);

	if (!tracing::pe::isConstantTracerEnabled()) {
		std::cout << "Note: NAUTILUS_CONSTANT_TRACER_ENABLED=0 — the tracer is\n"
		          << "  disabled at runtime, so no folds will fire. Every demo\n"
		          << "  still produces the correct answer; fold counts will be\n"
		          << "  zero and the IR will contain the full op graph.\n\n";
	}

	std::cout << "=== 1. Arithmetic chain ===\n";
	std::cout << "  (5 * 3 + 3 - 2)^2 - 1 — every op folds\n";
	runDemo("arithChain()", engine, arithChain);

	std::cout << "=== 2. Loop unrolling ===\n";
	std::cout << "  for i in [0,10): sum += i*i — trace fully unrolls\n";
	runDemo("sumOfSquares()", engine, sumOfSquares);

	std::cout << "=== 3. Constant pointer (assume_stable) ===\n";
	std::cout << "  kSquaresLut[4] — `assume_stable` + known index: ADD + LOAD fold\n";
	runDemo("lutLookupKnownIndex()", engine, lutLookupKnownIndex);

	std::cout << "  kSquaresLut[idx] — runtime index: ADD + LOAD remain in the trace\n";
	runDemo("lutLookupDynamic(5)", engine, lutLookupDynamic, int64_t {5});

	return 0;
}
