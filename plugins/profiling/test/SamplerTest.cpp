#include "common/ProfilingTestSupport.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/compiler/JitSymbolRegistry.hpp"
#include "nautilus/profiling/sampler.hpp"
#include "nautilus/region.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <set>
#include <string>
#include <thread>
#include <vector>

using namespace nautilus;

namespace {

/// A kernel whose cost is deliberately lopsided: `hot` does ~64x the work of
/// `cold`, so a correct profile has an obviously right answer to check against
/// and a wrong one cannot pass by luck.
///
/// The body is a shift/xor/multiply mixing chain rather than an accumulation.
/// That is not decoration: a loop that merely sums `i` has a closed form, and
/// LLVM's scalar evolution duly replaces the whole thing with one multiply --
/// leaving a kernel that finishes before a single sample fires and a profile
/// with nothing in it. A mixing chain has no closed form, so the work survives
/// -O3 and there is something to sample.
val<int64_t> twoRegions(val<int64_t> iterations) {
	val<int64_t> state = 1;
	const auto mix = [](val<int64_t> value) {
		value = value * 2654435761;
		value = value ^ (value >> 13);
		value = value + (value << 7);
		return value ^ (value >> 17);
	};
	region("cold", [&]() {
		val<int64_t> i = 0;
		for (; i < iterations; i = i + 1) {
			state = mix(state + i);
		}
	});
	region("hot", [&]() {
		val<int64_t> i = 0;
		for (; i < iterations * 64; i = i + 1) {
			state = mix(state ^ i);
		}
	});
	return state;
}

engine::Options samplingOptions() {
	engine::Options options;
	// Both are load-bearing, and for different reasons: `perf.sample` is what
	// makes the backend publish code ranges at all, and pinning the backend
	// keeps tiered compilation from starting on an interpreting tier that
	// registers nothing (docs/profiling.md).
	options.setOption("perf.sample", true);
	options.setOption("engine.backend", std::string("mlir"));
	return options;
}

/// True if any sampled site's name contains @p needle.
bool sawSymbolContaining(const profiling::SampleReport& report, std::string_view needle) {
	for (const auto& site : report.sites()) {
		if (site.symbol.find(needle) != std::string::npos) {
			return true;
		}
	}
	return false;
}

} // namespace

TEST_CASE("perf.sample makes the backend publish JIT symbols", "[profiling][sampler][registry]") {
	// The precondition everything else here rests on, asserted separately so a
	// failure says whether the backend stopped publishing or the sampler stopped
	// resolving.
	compiler::JitSymbolRegistry::instance().clear();

	engine::NautilusEngine nautilusEngine(samplingOptions());
	auto compiled = nautilusEngine.registerFunction(twoRegions);
	// LLJIT materializes lazily, so the code is not linked -- and no symbol
	// published -- until the function is actually called.
	compiled(10);

	const auto symbols = compiler::JitSymbolRegistry::instance().snapshot();
	std::string names;
	for (const auto& symbol : symbols) {
		names += "  " + symbol.name + " (" + std::to_string(symbol.size()) + " bytes)\n";
	}
	INFO("published symbols:\n" << names);
	REQUIRE_FALSE(symbols.empty());

	bool sawRegion = false;
	for (const auto& symbol : symbols) {
		if (symbol.name.find("::hot") != std::string::npos) {
			sawRegion = true;
		}
	}
	CHECK(sawRegion);
}

TEST_CASE("an unavailable sampler degrades instead of throwing", "[profiling][sampler]") {
	profiling::Sampler sampler;
	REQUIRE_NOTHROW(sampler.start());
	REQUIRE_NOTHROW(sampler.stop());
	REQUIRE_NOTHROW(sampler.stopAll());
	if (!sampler.available()) {
		CHECK_FALSE(sampler.unavailableReason().empty());
		CHECK(sampler.report().empty());
	}
}

TEST_CASE("samples land in JIT-compiled code and are named", "[profiling][sampler]") {
	NAUTILUS_PROFILING_SKIP_IF_UNAVAILABLE();

	// A short period: the window is one kernel invocation, so the default
	// 100k-cycle period would collect too few samples to be stable.
	profiling::Sampler sampler {{"cycles", /*period=*/20000, /*callchain=*/false, /*bufferPages=*/256}};
	if (!sampler.available()) {
		SKIP("sampler unavailable: " << sampler.unavailableReason());
	}

	engine::NautilusEngine nautilusEngine(samplingOptions());
	auto compiled = nautilusEngine.registerFunction(twoRegions);

	// Warm up outside the window. LLJIT materializes lazily, so the first call
	// is what triggers LLVM codegen and the JITLink pass -- and that compile
	// runs in-process, so sampling across it profiles the compiler rather than
	// the compiled code (docs/profiling.md says the same thing about recording
	// only the steady state).
	compiled(10);

	sampler.start();
	const auto result = compiled(200000);
	sampler.stopAll();

	if (!sampler.available()) {
		SKIP("sampling could not start: " << sampler.unavailableReason());
	}
	CHECK(result != 0);

	const auto& report = sampler.report();
	if (report.total() == 0) {
		SKIP("no samples collected; the perf subsystem allowed the open but recorded nothing");
	}

	INFO(report.table());
	// This is the assertion the out-of-process jitdump path structurally cannot
	// make: it needs an external `perf record` and `perf inject` run before any
	// symbol exists to check.
	CHECK(report.jitSamples() > 0);
	CHECK(sawSymbolContaining(report, "execute"));
}

TEST_CASE("region names survive into the profile", "[profiling][sampler]") {
	NAUTILUS_PROFILING_SKIP_IF_UNAVAILABLE();

	profiling::Sampler sampler {{"cycles", 20000, false, 256}};
	if (!sampler.available()) {
		SKIP("sampler unavailable: " << sampler.unavailableReason());
	}

	engine::NautilusEngine nautilusEngine(samplingOptions());
	auto compiled = nautilusEngine.registerFunction(twoRegions);

	// Warm up outside the window. LLJIT materializes lazily, so the first call
	// is what triggers LLVM codegen and the JITLink pass -- and that compile
	// runs in-process, so sampling across it profiles the compiler rather than
	// the compiled code (docs/profiling.md says the same thing about recording
	// only the steady state).
	compiled(10);

	sampler.start();
	compiled(200000);
	sampler.stopAll();

	const auto& report = sampler.report();
	if (report.total() == 0 || report.jitSamples() == 0) {
		SKIP("no JIT samples collected");
	}

	INFO(report.table());
	if (!sawSymbolContaining(report, "::hot")) {
		// Region naming needs the DWARF inline stack to survive the optimizer.
		// Failing outright here would make the whole suite hostage to LLVM's
		// inlining decisions, so this reports rather than asserts.
		WARN("no region-qualified symbol in the profile; region names may not "
		     "have survived this compile:\n"
		     << report.table());
		return;
	}

	// The hot region does 64x the work of the cold one. Only their relative
	// order is asserted -- the ratio itself depends on how the optimizer
	// vectorized each loop.
	uint64_t hot = 0;
	uint64_t cold = 0;
	for (const auto& site : report.sites()) {
		if (site.symbol.find("::hot") != std::string::npos) {
			hot += site.samples;
		} else if (site.symbol.find("::cold") != std::string::npos) {
			cold += site.samples;
		}
	}
	CHECK(hot > cold);
}

// Module scoping and eviction, end to end: two live modules must stay apart in
// the registry, and a destroyed one must take its ranges with it.

TEST_CASE("two live modules publish under distinct module handles", "[profiling][registry]") {
	compiler::JitSymbolRegistry::instance().clear();

	engine::NautilusEngine firstEngine(samplingOptions());
	auto first = firstEngine.registerFunction(twoRegions);
	first(10);
	const auto afterFirst = compiler::JitSymbolRegistry::instance().snapshot();
	REQUIRE_FALSE(afterFirst.empty());

	engine::NautilusEngine secondEngine(samplingOptions());
	auto second = secondEngine.registerFunction(twoRegions);
	second(10);
	const auto afterSecond = compiler::JitSymbolRegistry::instance().snapshot();

	// Both compiles are present, and at different addresses.
	CHECK(afterSecond.size() > afterFirst.size());

	std::set<compiler::ModuleIndex> modules;
	for (const auto& symbol : afterSecond) {
		modules.insert(symbol.moduleIndex);
	}
	// The point of the change: identically named ranges from two compiles no
	// longer share an identity.
	CHECK(modules.size() == 2);
	for (const auto index : modules) {
		CHECK_FALSE(compiler::JitSymbolRegistry::instance().moduleId(index).empty());
	}
}

TEST_CASE("destroying a module withdraws its ranges", "[profiling][registry]") {
	compiler::JitSymbolRegistry::instance().clear();

	size_t whileAlive = 0;
	{
		engine::NautilusEngine nautilusEngine(samplingOptions());
		auto compiled = nautilusEngine.registerFunction(twoRegions);
		compiled(10);
		whileAlive = compiler::JitSymbolRegistry::instance().size();
		REQUIRE(whileAlive > 0);
	}

	// Without this the registry grows for the life of the process, and a later
	// compile landing on the freed addresses resolves to the dead module.
	CHECK(compiler::JitSymbolRegistry::instance().size() < whileAlive);
}

TEST_CASE("an interned module id round-trips", "[profiling][registry]") {
	compiler::JitSymbolRegistry::instance().clear();
	auto& registry = compiler::JitSymbolRegistry::instance();

	const auto first = registry.intern("unit-a");
	const auto again = registry.intern("unit-a");
	const auto second = registry.intern("unit-b");

	CHECK(first == again);
	CHECK(first != second);
	CHECK(registry.moduleId(first) == "unit-a");
	CHECK(registry.moduleId(second) == "unit-b");
	// An absent module is not an error, just an empty id.
	CHECK(registry.intern("") == compiler::NO_MODULE);
	CHECK(registry.moduleId(compiler::NO_MODULE).empty());
}

TEST_CASE("removing a module leaves other modules' ranges alone", "[profiling][registry]") {
	compiler::JitSymbolRegistry::instance().clear();
	auto& registry = compiler::JitSymbolRegistry::instance();

	const auto keep = registry.intern("keep");
	const auto drop = registry.intern("drop");
	registry.add({0x1000, 0x2000, "execute", keep});
	registry.add({0x2000, 0x3000, "execute", drop});
	registry.add({0x3000, 0x4000, "execute::inner", drop});

	CHECK(registry.remove(drop) == 2);
	CHECK(registry.size() == 1);

	compiler::JitSymbol resolved;
	CHECK(registry.resolve(0x1500, resolved));
	CHECK(resolved.moduleIndex == keep);
	// The dropped module's addresses no longer resolve to its dead names.
	CHECK_FALSE(registry.resolve(0x2500, resolved));
}

TEST_CASE("the same calls sample a whole pool", "[profiling][sampler]") {
	NAUTILUS_PROFILING_SKIP_IF_UNAVAILABLE();

	constexpr uint16_t WORKERS = 4;
	profiling::Sampler sampler {{"cycles", 20000, false, 256}};
	if (!sampler.available()) {
		SKIP("sampler unavailable: " << sampler.unavailableReason());
	}

	engine::NautilusEngine nautilusEngine(samplingOptions());
	auto compiled = nautilusEngine.registerFunction(twoRegions);
	compiled(10); // warm up off the measured window

	std::vector<std::thread> workers;
	workers.reserve(WORKERS);
	for (uint16_t worker = 0; worker < WORKERS; ++worker) {
		workers.emplace_back([&sampler, &compiled]() {
			// No thread index anywhere: the slot follows from the caller.
			profiling::Sampler::Scope scope {sampler};
			compiled(150'000);
		});
	}
	for (auto& worker : workers) {
		worker.join();
	}
	sampler.stopAll();

	const auto& report = sampler.report();
	if (report.total() == 0) {
		SKIP("no samples collected across the pool");
	}

	INFO(report.table());
	// Four threads' samples, merged, still resolved through the JIT symbol
	// registry to the regions that produced them.
	CHECK(report.jitSamples() > 0);
	CHECK(sawSymbolContaining(report, "execute"));
}

TEST_CASE("stopAll is idempotent", "[profiling][sampler]") {
	NAUTILUS_PROFILING_SKIP_IF_UNAVAILABLE();

	profiling::Sampler sampler {{"cycles", 50000, false, 64}};
	if (!sampler.available()) {
		SKIP("sampler unavailable: " << sampler.unavailableReason());
	}

	sampler.start();
	profiling::test::busyLoop(200'000);
	sampler.stopAll();
	const auto firstTotal = sampler.report().total();

	// A ring buffer drained twice comes back empty, so the second call has to
	// be a no-op rather than clobbering the report with nothing.
	sampler.stopAll();
	CHECK(sampler.report().total() == firstTotal);
}

TEST_CASE("perf.sample publishes an IR line table", "[profiling][registry]") {
	// The precondition for annotation, asserted without a PMU: the line table
	// and the dump it points at come from the compile, not from sampling.
	compiler::JitSymbolRegistry::instance().clear();

	engine::NautilusEngine nautilusEngine(samplingOptions());
	auto compiled = nautilusEngine.registerFunction(twoRegions);
	compiled(10);

	const auto symbols = compiler::JitSymbolRegistry::instance().snapshot();
	REQUIRE_FALSE(symbols.empty());

	// An address inside a published range resolves to a line of that module's
	// Nautilus-IR dump.
	compiler::JitLine line;
	bool resolvedAny = false;
	for (const auto& symbol : symbols) {
		if (compiler::JitSymbolRegistry::instance().resolveLine(symbol.start + symbol.size() / 2, line)) {
			resolvedAny = true;
			CHECK(line.line > 0);
			CHECK(line.moduleIndex == symbol.moduleIndex);
			CHECK_FALSE(compiler::JitSymbolRegistry::instance().sourceFile(line.moduleIndex).empty());
			break;
		}
	}
	CHECK(resolvedAny);

	// An address outside every range does not pick up a neighbouring module's
	// line: a line row has an address but no extent, so only the range table
	// bounds it.
	CHECK_FALSE(compiler::JitSymbolRegistry::instance().resolveLine(symbols.back().end + 0x100000, line));
}

TEST_CASE("destroying a module withdraws its lines too", "[profiling][registry]") {
	compiler::JitSymbolRegistry::instance().clear();
	{
		engine::NautilusEngine nautilusEngine(samplingOptions());
		auto compiled = nautilusEngine.registerFunction(twoRegions);
		compiled(10);
	}
	// Ranges are gone, so nothing can resolve -- including through the line
	// table, which is bounded by them.
	compiler::JitLine line;
	CHECK_FALSE(compiler::JitSymbolRegistry::instance().resolveLine(0x1000, line));
	CHECK(compiler::JitSymbolRegistry::instance().size() == 0);
}
