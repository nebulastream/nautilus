// A CPU-heavy Nautilus function set up for `perf` profiling via the MLIR backend's perf
// jitdump support (docs/profiling.md, tools/nautilus-perf.sh).
//
// Structure mirrors PerfFixtureFunctions.hpp's perfCompositeKernel: nested region()s so a
// flame graph can show where time actually goes (outer -> hot / mix), a Nautilus-to-Nautilus
// call inside the hot region, and a native invoke() call inside the mix region -- both are
// data-dependent so nothing folds away at -O3.

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

// The "mix" region's external callee: kept out-of-line/noinline so it lowers to a real
// CALL_EXT rather than being inlined away, giving the profile a genuine host-side frame.
__attribute__((noinline)) int64_t nativeMix(int64_t acc, int64_t v) noexcept {
	return (acc ^ (v * 2654435761ULL)) + 0x9e3779b97f4a7c15ULL;
}

// The "hot" region's per-element step, traced as its own Nautilus function so it gets its
// own func.func / JIT_CODE_LOAD record and shows up as its own row in the profile.
val<int64_t> hotStepBody(val<int64_t> acc, val<int64_t> v) {
	return acc + v * 3 - (v >> 1);
}

NautilusFunction hotStep {"hotStep", hotStepBody};

// rounds controls the hot loop's trip count -- and so the run's wall-clock time -- without
// changing the traced IR shape, so the same compiled module works for a quick smoke test
// (small rounds) and a real sampling run (large rounds).
val<int64_t> perfDemoKernel(val<int64_t*> data, val<int32_t> len, val<int32_t> rounds) {
	val<int64_t> acc = 0;
	region("outer", [&]() {
		region("hot", [&]() {
			for (val<int32_t> r = 0; r < rounds; r = r + 1) {
				for (val<int32_t> i = 0; i < len; i = i + 1) {
					val<int64_t> v = data[i];
					if (v > 0) {
						acc = hotStep(acc, v);
					} else {
						acc = acc - v;
					}
				}
			}
		});
		region("mix", [&]() {
			for (val<int32_t> i = 0; i < len; i = i + 1) {
				acc = invoke(nativeMix, acc, data[i]);
			}
		});
	});
	return acc;
}

int main(int argc, char* argv[]) {
	// argv[1]: rounds (trip count of the hot loop), argv[2]: input length.
	// Defaults are picked so a default run takes roughly a second -- long enough for `perf
	// record`'s default sampling rate to collect a useful profile.
	const int32_t rounds = argc > 1 ? std::atoi(argv[1]) : 20'000'000;
	const int32_t len = argc > 2 ? std::atoi(argv[2]) : 64;

	Options options;
	// Pin the MLIR backend: the default is tiered compilation, which starts on an
	// interpreting tier-0 backend that emits no jitdump at all (docs/profiling.md).
	options.setOption("engine.backend", std::string("mlir"));
	// The whole point: attribution down to individual Nautilus IR operations and region()
	// bodies, rather than to the post-inline MLIR module.
	options.setOption("perf", true);

	NautilusEngine engine(options);
	auto function = engine.registerFunction(perfDemoKernel);

	std::vector<int64_t> data(static_cast<size_t>(len));
	for (int32_t i = 0; i < len; ++i) {
		// Alternate sign so both the "hot" (v > 0) and "acc - v" branches execute.
		data[static_cast<size_t>(i)] = (i % 3 == 0) ? -(i + 1) : (i + 1);
	}

	auto result = function(data.data(), len, rounds);
	std::cout << "perfDemoKernel(rounds=" << rounds << ", len=" << len << ") = " << result << std::endl;
	return 0;
}
