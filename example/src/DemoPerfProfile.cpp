// A traced kernel shaped for profiling: run it under `perf` and read where its time went,
// broken down by nautilus::region() rather than by one opaque `execute` symbol.
//
// See docs/profiling.md for the full workflow; the short version is
//
//     tools/nautilus-perf.sh record -- ./demo_perf_profile
//     tools/nautilus-perf.sh report
//     tools/nautilus-perf.sh flamegraph perf.jit.data flame.svg
//     tools/nautilus-perf.sh annotate-ir
//
// The last one prints this kernel's whole Nautilus-IR dump with a per-line percentage
// gutter, which is where a region's cost becomes readable per IR operation rather than
// per symbol. It reads the `nautilus_debug_<pid>_<n>.ir` file a perf-enabled compile
// leaves in the process's *working directory* -- so run all of this from one directory
// and do not clean it between steps.
//
// Two engine options below are what make that work at all, and both are set in main():
// `perf` turns on the MLIR backend's jitdump writer, and `engine.backend=mlir` pins the
// backend, because the default tiered compiler starts on an interpreting tier-0 backend
// that emits no jitdump.
//
// The kernel is deliberately *unbalanced*: the leaf regions below do very different amounts
// of work per element, so the profile has an obviously right answer to check the tooling
// against -- `mix` should hold most of the samples, `scan` should be a sliver. A kernel
// whose regions all cost the same tells you nothing about whether attribution works.

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <nautilus/Engine.hpp>
#include <nautilus/nautilus_function.hpp>
#include <nautilus/region.hpp>
#include <nautilus/val_ptr.hpp>
#include <vector>

using namespace nautilus;
using namespace nautilus::engine;

/// The `merge` region's callee, on the host side rather than traced.
///
/// `noinline` keeps it a real call in the generated code instead of being folded into its
/// caller, which is the point of having it here: it gives the profile a genuine host-side
/// frame sitting *under* a JIT frame, which is the case that exercises unwinding out of JIT
/// code -- what the jitdump's JIT_CODE_UNWINDING_INFO record and the `frame-pointer=all`
/// attribute `perf` adds exist for.
///
/// It is also deliberately more than one or two instructions long. A leaf small enough that
/// most samples land on its `ret` reads as a child of `main` rather than of the region that
/// called it: by the `ret` the epilogue has already popped `%rbp`, so a frame-pointer walk
/// starts one frame too high. That is a property of `--call-graph fp` on tiny leaves, not of
/// the jitdump -- but it makes for a confusing first profile, so this callee avoids it.
__attribute__((noinline)) int64_t mergeAccumulators(int64_t left, int64_t right) noexcept {
	auto merged = static_cast<uint64_t>(left) ^ (static_cast<uint64_t>(right) + 0x9e3779b97f4a7c15ULL);
	merged ^= merged >> 33;
	merged *= 0xff51afd7ed558ccdULL;
	merged ^= merged >> 29;
	merged *= 0xc4ceb9fe1a85ec53ULL;
	return static_cast<int64_t>(merged ^ (merged >> 32));
}

/// The `mix` region's per-element step, traced as its own Nautilus function.
///
/// Registered separately, so it lowers to its own func.func -- and at -O3 the MLIR inliner
/// pulls it back into the loop that calls it. That inlined body is attributed to the region
/// it was inlined *into* (`...::mix`), not named as a frame of its own; docs/region.md
/// explains why the jitdump has to work that way.
val<int64_t> mixStepBody(val<int64_t> state, val<int64_t> value) {
	val<int64_t> x = state ^ (value * 2654435761LL);
	x = x + (x << 13);
	x = x ^ (x >> 7);
	x = x + (x << 3);
	return x ^ (x >> 17);
}

NautilusFunction mixStep {"mixStep", mixStepBody};

/// A small query-shaped pipeline, in nested regions:
///
///     query                    the whole pipeline
///       scan                   one cheap pass: load + predicate, count the matches
///       aggregate              the expensive half
///         mix                  per-element hashing -- where nearly all the time goes
///         merge                a folding pass that calls out to the host
///
/// Three levels deep (`query` > `aggregate` > `mix`) on purpose: a flame graph built from
/// this has to stack all three, which is the interesting case. `rounds` scales only the
/// trip counts, never the traced IR shape, so a smoke run and a real sampling run compile
/// to exactly the same code.
val<int64_t> queryKernel(val<int64_t*> data, val<int32_t> len, val<int32_t> rounds, val<int64_t> threshold) {
	val<int64_t> matches = 0;
	val<int64_t> state = 0x9e3779b9LL;

	region("query", [&]() {
		region("scan", [&]() {
			for (val<int32_t> r = 0; r < rounds; r = r + 1) {
				for (val<int32_t> i = 0; i < len; i = i + 1) {
					// `data[i]` is a reference-like proxy, not a value: load it into a
					// val<int64_t> of its own before using it. Passing the proxy straight
					// into a traced call hands the call the pointer instead of the loaded
					// element, which fails MLIR verification rather than miscompiling.
					val<int64_t> value = data[i];
					if (value > threshold) {
						matches = matches + 1;
					}
				}
			}
		});

		region("aggregate", [&]() {
			region("mix", [&]() {
				for (val<int32_t> r = 0; r < rounds; r = r + 1) {
					for (val<int32_t> i = 0; i < len; i = i + 1) {
						val<int64_t> value = data[i];
						state = mixStep(state, value);
					}
				}
			});

			region("merge", [&]() {
				// Every eighth element only, so this region and its host callee stay a
				// readable slice of the profile rather than crowding out `mix`.
				for (val<int32_t> r = 0; r < rounds; r = r + 1) {
					for (val<int32_t> i = 0; i < len; i = i + 8) {
						val<int64_t> value = data[i];
						state = invoke(mergeAccumulators, state, value);
					}
				}
			});
		});
	});

	return state + matches;
}

int main(int argc, char* argv[]) {
	// argv[1]: rounds, argv[2]: elements per round. The defaults run for a couple of
	// seconds, which `perf record`'s default sampling rate turns into a few thousand
	// samples -- enough for the per-region shares to be stable. Pass `1 64` for a smoke
	// run that only checks the thing compiles and prints a result.
	const int32_t rounds = argc > 1 ? std::atoi(argv[1]) : 1'000'000;
	const int32_t len = argc > 2 ? std::atoi(argv[2]) : 256;

	Options options;
	// Pin the MLIR backend. The default is the tiered compiler, which starts every
	// function on an interpreting tier-0 backend (no jitdump at all) and promotes to MLIR
	// in the background -- so a profile would be split across two artifacts at two
	// addresses, only one of which perf can even name.
	options.setOption("engine.backend", std::string("mlir"));
	// Emit perf jitdump records for every JIT-linked object. This keeps optimization at
	// the level you asked for (-O3 by default), so what gets profiled is the code you
	// would actually ship; `debug` is the option that trades that away for stepping
	// fidelity, and setting both clamps this one to -O0.
	options.setOption("perf", true);

	NautilusEngine engine(options);
	auto function = engine.registerFunction(queryKernel);

	std::vector<int64_t> data(static_cast<size_t>(len));
	for (int32_t i = 0; i < len; ++i) {
		// Alternating signs so the `scan` predicate is genuinely data-dependent and the
		// branch does not fold away or become perfectly predicted.
		data[static_cast<size_t>(i)] = (i % 3 == 0) ? -(i + 1) : (i + 1);
	}

	const int64_t result = function(data.data(), len, rounds, 0);

	std::cout << "queryKernel(rounds=" << rounds << ", len=" << len << ") = " << result << "\n\n"
	          << "Profile it with:\n"
	          << "  tools/nautilus-perf.sh record -- ./demo_perf_profile " << rounds << " " << len << "\n"
	          << "  tools/nautilus-perf.sh report\n"
	          << "  tools/nautilus-perf.sh flamegraph perf.jit.data flame.svg\n"
	          << "  tools/nautilus-perf.sh annotate-ir\n\n"
	          << "Expect symbols named execute::query::scan, execute::query::aggregate::mix and\n"
	          << "execute::query::aggregate::merge, with mix holding most of the samples, and\n"
	          << "annotate-ir attributing those samples to individual IR operations.\n"
	          << "See docs/profiling.md.\n";
	return 0;
}
