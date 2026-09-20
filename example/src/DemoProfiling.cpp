// A query-shaped kernel, profiled from inside the process with the nautilus-profiling
// plugin.
//
//     ./demo_profiling
//
// It prints counter totals for exactly one call, a per-region breakdown of where the cycles
// went, the Nautilus-IR dump annotated with those samples, and a flame graph -- with no
// `perf record`, no `perf inject --jit`, no `-k mono`, and nothing that has to still exist
// when the numbers are read. docs/profiling.md covers the out-of-process path, which needs
// none of this demo's code: it is an engine option and a `perf` recipe around your own
// application.
//
// What the in-process path adds over the jitdump one is threefold: absolute counter totals
// rather than percentages, a result the program can read (and a test can assert on), and
// attribution with no external tooling in the loop. What it does not change is the code
// being measured: counters are programmed before the window and read after it, so what runs
// in between is exactly what would have run unmeasured.
//
// Two engine options make the sampling half work, and both are set in main(): `perf.sample`
// publishes each compiled code range under its region()-qualified name, and
// `engine.backend=mlir` pins the backend, because the default tiered compiler starts on an
// interpreting tier that publishes nothing.

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <nautilus/Engine.hpp>
#include <nautilus/nautilus_function.hpp>
#include <nautilus/profiling/counters.hpp>
#include <nautilus/profiling/sampler.hpp>
#include <nautilus/region.hpp>
#include <nautilus/val_ptr.hpp>
#include <vector>

using namespace nautilus;
using namespace nautilus::engine;

/// The `merge` region's callee, on the host side rather than traced -- reached
/// through invoke(), so the generated code makes a real call out of JIT memory into the
/// host binary.
///
/// It is here to put a genuine *host* frame in the profile, under a JIT one. That is what
/// exercises the sampler's second resolution path: a JIT address is named from nautilus's
/// own symbol registry, and an address like this one has to fall through to ELF symbols
/// instead. Both show up in the report, told apart by `SymbolOrigin`, and the flame graph
/// colours them differently.
///
/// It is deliberately *not* in the anonymous namespace below, and the demo links with
/// `-rdynamic`. Nautilus names an invoke()d callee with `dladdr`, which reads only the
/// dynamic symbol table: an internal-linkage function is `LOCAL` and never reaches it, and
/// even an external one is absent from an executable's `.dynsym` unless the link exports
/// it. Without both, this call shows up in the IR dump -- and so in the annotation printed
/// below -- as a bare `call 0x55f3...` instead of a name.
///
/// `noinline` keeps it a real call rather than something the optimizer folds into its
/// caller. It is also deliberately more than a couple of instructions long: a leaf small
/// enough that most samples land on its `ret` reads as a child of `main` rather than of the
/// region that called it, because by the `ret` the epilogue has already popped `%rbp` and a
/// frame-pointer walk starts one frame too high. That is a property of frame-pointer
/// unwinding on tiny leaves, not of this plugin -- but it makes for a confusing first
/// profile, so this callee avoids it.
__attribute__((noinline)) int64_t mergeAccumulators(int64_t left, int64_t right) noexcept {
	auto merged = static_cast<uint64_t>(left) ^ (static_cast<uint64_t>(right) + 0x9e3779b97f4a7c15ULL);
	merged ^= merged >> 33;
	merged *= 0xff51afd7ed558ccdULL;
	merged ^= merged >> 29;
	merged *= 0xc4ceb9fe1a85ec53ULL;
	return static_cast<int64_t>(merged ^ (merged >> 32));
}

namespace {

/// The `mix` region's per-element step, traced as its own Nautilus function.
///
/// Registered separately below, so it lowers to its own `func.func` -- and at -O3 the MLIR
/// inliner pulls it back into the loop that calls it. That inlined body is attributed to the
/// region it was inlined *into* (`...::mix`) rather than named as a frame of its own, while
/// its operations keep their own lines in the IR dump; docs/region.md explains why.
///
/// The body is a mixing chain with no closed form, which matters more than it looks: a loop
/// that merely accumulates `i` is summable, and LLVM's scalar evolution replaces the whole
/// thing with a single multiply -- leaving a kernel that finishes before one sample fires
/// and a profile with nothing in it.
val<int64_t> mixStepBody(val<int64_t> state, val<int64_t> value) {
	val<int64_t> mixed = state ^ (value * 2654435761LL);
	mixed = mixed + (mixed << 13);
	mixed = mixed ^ (mixed >> 7);
	mixed = mixed + (mixed << 3);
	return mixed ^ (mixed >> 17);
}

NautilusFunction mixStep {"mixStep", mixStepBody};

/// query > { scan, aggregate > { mix, merge } } -- three levels deep, with deliberately
/// unequal per-element cost so the profile has an obviously right answer: `mix` should hold
/// most of the samples and `scan` should be a sliver.
val<int64_t> queryKernel(val<int64_t*> data, val<int32_t> len, val<int32_t> rounds, val<int64_t> threshold) {
	val<int64_t> matches = 0;
	val<int64_t> state = 0x9e3779b9LL;

	region("query", [&]() {
		region("scan", [&]() {
			for (val<int32_t> r = 0; r < rounds; r = r + 1) {
				for (val<int32_t> i = 0; i < len; i = i + 1) {
					// `data[i]` is a reference-like proxy, not a value: load it into a
					// val<int64_t> of its own before using it.
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
				// Every fourth element only, so the host callee is a readable slice of the
				// profile rather than crowding out `mix`.
				for (val<int32_t> r = 0; r < rounds; r = r + 1) {
					for (val<int32_t> i = 0; i < len; i = i + 4) {
						val<int64_t> value = data[i];
						// invoke() is the call out to the host. Unlike mixStep above --
						// traced, and inlined back into its region at -O3 -- this stays a
						// real call, and the profile names it from the host binary's ELF
						// symbols rather than from the JIT symbol registry.
						state = invoke(mergeAccumulators, state, value);
					}
				}
			});
		});
	});

	return state + matches;
}

} // namespace

int main(int argc, char* argv[]) {
	// argv[1]: rounds, argv[2]: elements per round. Pass `1000 64` for a smoke run.
	const int32_t rounds = argc > 1 ? std::atoi(argv[1]) : 200'000;
	const int32_t len = argc > 2 ? std::atoi(argv[2]) : 256;

	if (!profiling::available()) {
		// Counters are privileged: containers, a hardened perf_event_paranoid and VMs
		// without a virtualized PMU all deny them. The plugin never throws for that, so
		// this demo reports and carries on rather than failing.
		std::cout << "hardware counters unavailable: " << profiling::unavailableReason() << "\n"
		          << "The kernel below still runs; only the measurements are missing.\n\n";
	}

	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	// Publishes each compiled code range under its region()-qualified name, which is what
	// the sampler resolves against.
	options.setOption("perf.sample", true);

	NautilusEngine engine(options);
	auto function = engine.registerFunction(queryKernel);

	std::vector<int64_t> data(static_cast<size_t>(len));
	for (int32_t i = 0; i < len; ++i) {
		// Alternating signs so the `scan` predicate is genuinely data-dependent.
		data[static_cast<size_t>(i)] = (i % 3 == 0) ? -(i + 1) : (i + 1);
	}

	// Warm up outside every measurement window. LLJIT materializes lazily, so the first
	// call is what triggers LLVM codegen -- and that compile runs in this process, so
	// measuring across it measures the compiler.
	function(data.data(), len, 1, 0);

	// ---- Counting: exact totals for the whole window -------------------------------
	profiling::CounterSet counters;
	counters.start();
	const int64_t result = function(data.data(), len, rounds, 0);
	counters.stop();

	std::cout << "queryKernel(rounds=" << rounds << ", len=" << len << ") = " << result << "\n\n";
	if (counters.available()) {
		std::cout << "Hardware counters for exactly that call:\n" << counters.result().table() << "\n";
	}

	// ---- Sampling: the breakdown within a window of the same shape ------------------
	// A shorter period than the default: one kernel invocation is a small window, and the
	// default would collect too few samples for the per-region shares to be stable.
	// callchain=true so the stacks carry host frames beneath the JIT ones, which is
	// what makes the flame graph below worth looking at rather than one row of leaves.
	profiling::Sampler sampler {
	    {"cycles", /*period=*/50'000, /*callchain=*/true, /*bufferPages=*/256, /*maxThreads=*/0}};
	sampler.start();
	function(data.data(), len, rounds, 0);
	// stopAll() rather than stop(): stop() ends this thread's window, and
	// draining the ring buffers into the merged report happens once, here.
	sampler.stopAll();

	if (sampler.available() && !sampler.report().empty()) {
		const auto& report = sampler.report();
		std::cout << "Where those cycles went, by region:\n" << report.table(10) << "\n";

		// A self-contained flame graph: no flamegraph.pl, no FlameGraph clone,
		// nothing to install. Frames are coloured by where the code came from,
		// so JIT-compiled regions stand out from host frames at a glance.
		const std::string flamePath = "nautilus-flame.svg";
		if (report.writeFlameGraph(flamePath, "demo_profiling -- queryKernel")) {
			std::cout << "Flame graph written to " << flamePath << " (open it in a browser).\n";
		} else {
			std::cout << "Could not write " << flamePath << ".\n";
		}

		// One level below the symbol table: which IR operation inside the hot
		// region is the hot one. The dump is the `nautilus_debug_<pid>_<n>.ir`
		// file a perf-enabled compile leaves in the working directory, which is
		// what the samples' DWARF points at -- so this needs no argument and no
		// `perf annotate`.
		if (!report.sourceLines().empty()) {
			std::cout << "The hottest IR operations, with three lines of context:\n\n"
			          << report.annotateSource(/*path=*/ {}, /*context=*/3) << "\n";
		} else {
			std::cout << "No IR line table published; annotation needs `perf.sample` with the MLIR backend.\n\n";
		}

		std::cout << "The same stacks in folded form, for flamegraph.pl or your own renderer:\n";
		const auto folded = report.foldedStacks();
		size_t printed = 0;
		for (size_t start = 0; start < folded.size() && printed < 3;) {
			const size_t end = folded.find('\n', start);
			std::cout << "  " << folded.substr(start, end - start) << "\n";
			start = end == std::string::npos ? folded.size() : end + 1;
			++printed;
		}
		std::cout << "\n";
	} else if (sampler.available()) {
		std::cout << "No samples collected; try a larger `rounds`.\n\n";
	} else {
		std::cout << "Sampling unavailable: " << sampler.unavailableReason() << "\n\n";
	}

	std::cout << "See docs/profiling.md, which also covers profiling nautilus from outside the\n"
	          << "process with `perf record` -- an engine option and a recipe, needing none of\n"
	          << "this demo's code.\n";
	return 0;
}
