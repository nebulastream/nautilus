#include "common/ProfilingTestSupport.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/compiler/JitSymbolRegistry.hpp"
#include "nautilus/profiling/sampler.hpp"
#include "nautilus/region.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <string>

using namespace nautilus;

// What the sampler can see, per backend.
//
// Only the MLIR backend publishes to JitSymbolRegistry today, so only its
// frames carry nautilus names; see docs/profiling-backends.md for what the
// others would take. These tests exist to keep that document honest: they
// measure the behaviour it describes rather than restating it, and they are
// deliberately written so that *improving* a backend does not fail them --
// what is asserted is that nothing crashes, that MLIR resolves, and that a
// backend which publishes nothing produces no JIT-named frames.

namespace {

/// Two regions of deliberately unequal cost, with a mixing chain so the work
/// survives -O3 rather than being folded to a closed form.
val<int64_t> twoRegionKernel(val<int64_t> iterations) {
	val<int64_t> state = 1;
	region("hot", [&]() {
		val<int64_t> i = 0;
		for (; i < iterations; i = i + 1) {
			val<int64_t> value = state ^ i;
			value = value * 2654435761;
			value = value ^ (value >> 13);
			value = value + (value << 7);
			state = value ^ (value >> 17);
		}
	});
	region("cold", [&]() {
		val<int64_t> i = 0;
		for (; i < iterations / 64; i = i + 1) {
			state = state + i;
		}
	});
	return state;
}

struct Coverage {
	size_t registryRanges = 0;
	uint64_t samples = 0;
	uint64_t jitSamples = 0;
	std::string hottest;
	bool sampled = false;
};

/// Compiles and samples the kernel under @p backend.
Coverage measure(const std::string& backend, uint64_t iterations = 400'000) {
	compiler::JitSymbolRegistry::instance().clear();

	engine::Options options;
	options.setOption("engine.backend", backend);
	options.setOption("perf.sample", true);
	engine::NautilusEngine nautilusEngine(options);
	auto compiled = nautilusEngine.registerFunction(twoRegionKernel);
	compiled(100); // warm up: the first call is what compiles

	Coverage coverage;
	profiling::Sampler sampler {{"cycles", 20000, /*callchain=*/false, 256, 0}};
	if (!sampler.available()) {
		coverage.registryRanges = compiler::JitSymbolRegistry::instance().size();
		return coverage;
	}

	sampler.start();
	compiled(iterations);
	sampler.stopAll();

	coverage.sampled = true;
	coverage.registryRanges = compiler::JitSymbolRegistry::instance().size();
	coverage.samples = sampler.report().total();
	coverage.jitSamples = sampler.report().jitSamples();
	if (!sampler.report().sites().empty()) {
		coverage.hottest = sampler.report().sites().front().symbol;
	}
	return coverage;
}

} // namespace

TEST_CASE("the MLIR backend names functions and regions", "[profiling][backends]") {
	NAUTILUS_PROFILING_SKIP_IF_UNAVAILABLE();

	const auto coverage = measure("mlir");
	CHECK(coverage.registryRanges > 0);
	if (!coverage.sampled || coverage.samples == 0) {
		SKIP("no samples collected");
	}
	INFO("hottest: " << coverage.hottest);
	CHECK(coverage.jitSamples > 0);
	// Region-qualified, not just `execute`: this is the bar the other backends
	// are measured against in docs/profiling-backends.md.
	CHECK(coverage.hottest.find("::hot") != std::string::npos);
}

TEST_CASE("backends that publish nothing still profile safely", "[profiling][backends]") {
	NAUTILUS_PROFILING_SKIP_IF_UNAVAILABLE();

	// Sampling a backend the registry knows nothing about must degrade to
	// unresolved addresses (or to host frames, for an interpreter) rather than
	// misattributing them to whatever was compiled previously.
	for (const auto* backend : {"cpp", "asmjit", "bc"}) {
		const auto coverage = measure(backend, 200'000);
		INFO("backend: " << backend << ", hottest: " << coverage.hottest);
		CHECK(coverage.registryRanges == 0);
		CHECK(coverage.jitSamples == 0);
	}
}

TEST_CASE("an interpreted backend profiles its interpreter", "[profiling][backends]") {
	NAUTILUS_PROFILING_SKIP_IF_UNAVAILABLE();

	// Not a defect, and the reason docs/profiling-backends.md argues for saying
	// so in the report: the bytecode interpreter has no code corresponding to a
	// nautilus function, so the honest answer names the dispatch loop. A user
	// left to read it unaided concludes their pipeline is spending its time in
	// `regMov`.
	const auto coverage = measure("bc", 100'000);
	if (!coverage.sampled || coverage.samples == 0) {
		SKIP("no samples collected");
	}
	INFO("hottest: " << coverage.hottest);
	CHECK(coverage.jitSamples == 0);
}
