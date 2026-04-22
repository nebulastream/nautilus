// Standalone demo: exercises nested Nautilus functions, a native runtime
// invoke(), per-module Perfetto tracks, and the SIGPROF sampler.
//
// Produces a Chrome Trace JSON file at /tmp/nautilus-nested-profile.json
// that can be dragged into https://ui.perfetto.dev.
#include "nautilus/Engine.hpp"
#include "nautilus/function.hpp"
#include "nautilus/profile/Instrument.hpp"
#include "nautilus/profile/JitSymbols.hpp"
#include "nautilus/profile/MLIR.hpp"
#include "nautilus/profile/Profiler.hpp"
#include "nautilus/profile/Recorder.hpp"
#include "nautilus/profile/Sampler.hpp"
#include "nautilus/profile/TraceWriter.hpp"
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <iostream>

using namespace nautilus;

// --- Native runtime functions reached via invoke() ----------------------------

// Native "expensive" function reached via invoke(). Dominates the per-call
// cost so the sampler will actually observe it on the stack.
// extern "C" + visibility("default") + -rdynamic give it a dynamic-symtab
// entry that dladdr can name from a signal handler.
extern "C" __attribute__((noinline, visibility("default"))) int64_t nativeMix(int64_t x, int64_t seed) noexcept {
	uint64_t v = static_cast<uint64_t>(x) ^ static_cast<uint64_t>(seed);
	// Splittable-random-style mix, iterated heavily so the sampler can
	// catch us in the middle even with per-event clock_gettime overhead.
	for (int i = 0; i < 4096; ++i) {
		v ^= v >> 30;
		v *= 0xbf58476d1ce4e5b9ULL;
		v ^= v >> 27;
		v *= 0x94d049bb133111ebULL;
		v ^= v >> 31;
		v += static_cast<uint64_t>(seed) + static_cast<uint64_t>(i);
	}
	return static_cast<int64_t>(v) & 0x7fffffffffffffffLL;
}

// --- Inner Nautilus functions ------------------------------------------------
//
// These are ordinary val-typed functions. When the outer pipeline traces, a
// call to one of them is inlined into the trace, so its region events
// appear as nested spans under the caller in the final trace.

static val<int64_t> hashStep(val<int64_t> x) {
	profile::ScopedRegion region("hash_step");
	val<int64_t> acc = x;
	for (val<int64_t> i = 0; i < val<int64_t>(8); i = i + val<int64_t>(1)) {
		acc = acc * val<int64_t>(31) + i;
	}
	return acc;
}

static val<int64_t> crunchStep(val<int64_t> x) {
	profile::ScopedRegion region("crunch_step");
	val<int64_t> acc = x;
	for (val<int64_t> i = 0; i < val<int64_t>(4); i = i + val<int64_t>(1)) {
		acc = acc + invoke(&nativeMix, acc, i);
	}
	return acc;
}

// --- Outer pipeline ----------------------------------------------------------

static val<int64_t> pipeline(val<int64_t> n) {
	profile::ScopedRegion total("pipeline.total");
	val<int64_t> sum = 0;
	for (val<int64_t> i = 0; i < n; i = i + val<int64_t>(1)) {
		profile::ScopedRegion iter("pipeline.iter");
		profile::traceCounter("pipeline.i", i);
		auto h = hashStep(i);
		auto c = crunchStep(h);
		sum = sum + c;
	}
	return sum;
}

// --- Driver ------------------------------------------------------------------

int main(int /*argc*/, char* /*argv*/[]) {
	engine::Options options;
	options.setOption("engine.backend", std::string {"mlir"});
	// Authoritative (name, addr, size) for every JIT'd function comes from
	// this listener — no more /proc/self/maps heuristic, no static
	// initializer ordering tricks.
	options.addMLIRJitEventListener(profile::mlirJitEventListener());
	engine::NautilusEngine engine(options);

	auto f = engine.registerFunction(pipeline);

	// Warm call (traces the function, runs code once).
	auto warm = f(5);
	std::cout << "warm result: " << warm << "\n";

	// Start the in-process SIGPROF sampler and group the measured run under a
	// Perfetto track called "nested_demo".
	profile::SamplerConfig cfg;
	cfg.interval = std::chrono::microseconds(500); // 2 kHz
	profile::startSampler(cfg);
	profile::registerThisThread();
	profile::openModule("nested_demo");

	// Do enough work for the sampler to have many opportunities to fire.
	int64_t total = 0;
	for (int i = 0; i < 50; ++i) {
		total += f(2000);
	}
	std::cout << "measured total: " << total << "\n";

	profile::closeModule("nested_demo");
	profile::unregisterThisThread();
	profile::stopSampler();

	const std::string trace_path = "/tmp/nautilus-nested-profile.json";
	if (!profile::flushTrace(trace_path)) {
		std::cerr << "flushTrace failed\n";
		return 1;
	}
	const std::string perf_map = profile::writePerfMap();
	if (!perf_map.empty()) {
		std::cerr << "wrote " << perf_map << "\n";
	}

	// Emit a small stderr summary so the operator sees what just happened
	// without having to grep the trace file by hand.
	auto events = profile::takeRecordedEvents();
	size_t beg = 0, end = 0, ctr = 0, samp = 0;
	for (const auto& e : events) {
		switch (e.kind) {
		case profile::Event::Kind::Begin:
			++beg;
			break;
		case profile::Event::Kind::End:
			++end;
			break;
		case profile::Event::Kind::CounterI64:
			++ctr;
			break;
		case profile::Event::Kind::Sample:
			++samp;
			break;
		}
	}
	std::cerr << "wrote " << trace_path << "\n";
	std::cerr << "post-flush leftover: begin=" << beg << " end=" << end << " counter=" << ctr << " sample=" << samp
	          << "\n";
	return 0;
}
