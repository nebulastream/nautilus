// title: Opt-in passes: LICM, CSE & strength reduction
// description: Enable the three opt-in passes in the Passes menu, recompile, and watch their tabs appear with diffs.
#include <nautilus/Engine.hpp>
#include <nautilus/val.hpp>

using namespace nautilus;

// This kernel is bait for the three opt-in optimization passes. Open the
// "Passes" menu, enable LICM, local CSE, and strength reduction, then
// recompile and compare the new pass tabs:
//
//   * `a * b + 42` never changes inside the loop
//     → loop-invariant code motion hoists it in front of the loop.
//   * `x * x + x` for the same x appears twice in one block
//     → local CSE computes it once and reuses the result.
//   * `i * 8` — multiplication by a power of two
//     → strength reduction rewrites it as `i << 3`.
void playground_register(engine::NautilusModule& m) {
	m.registerFunction<val<int64_t>(val<int64_t>, val<int64_t>, val<int64_t>)>(
	    "optBait", [](val<int64_t> n, val<int64_t> a, val<int64_t> b) {
		    val<int64_t> acc = 0;
		    for (val<int64_t> i = 0; i < n; i++) {
			    // Loop-invariant: neither a nor b changes in the loop.
			    val<int64_t> invariant = a * b + 42;

			    // Common subexpression, twice in the same block.
			    val<int64_t> first = i * i + i;
			    val<int64_t> second = i * i + i;

			    // Strength-reducible: * 8 becomes << 3.
			    val<int64_t> scaled = i * 8;

			    acc += invariant + first + second + scaled;
		    }
		    return acc;
	    });
}
