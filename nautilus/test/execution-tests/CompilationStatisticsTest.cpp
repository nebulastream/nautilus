#include "ExecutionTest.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

namespace {

/// Returns the first compilation backend actually linked into this build.
/// Matches ModuleTest.cpp's fallback ordering so the integration test runs
/// wherever any backend is available.
std::string getAnyBackend() {
#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)
	return "mlir";
#elif defined(ENABLE_TRACING) && defined(ENABLE_C_BACKEND)
	return "cpp";
#elif defined(ENABLE_TRACING) && defined(ENABLE_BC_BACKEND)
	return "bc";
#else
	return "";
#endif
}

val<int32_t> statsAddOne(val<int32_t> x) {
	return x + 1;
}

} // namespace

TEST_CASE("CompilationStatistics: compiled module exposes pipeline stats") {
	auto backend = getAnyBackend();
	if (backend.empty()) {
		SKIP("No compilation backend available");
	}

	Options options;
	options.setOption("engine.backend", backend);
	// Force the non-tiered path so the tier-0 stats carry both frontend
	// and backend keys in the same report.
	options.setOption("engine.compilationStrategy", std::string("legacy"));

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(statsAddOne);

	// Sanity-check: compiled function still works.
	REQUIRE(fn(41) == 42);

	auto stats = fn.getStatistics();
	REQUIRE(stats != nullptr);

	// Frontend stage timings are always recorded.
	for (const auto* key : {"tracing.ms", "ssaCreation.ms", "irGeneration.ms", "frontend.totalMs"}) {
		INFO("missing key: " << key);
		const auto* value = stats->find(key);
		REQUIRE(value != nullptr);
		REQUIRE(std::holds_alternative<double>(*value));
		REQUIRE(std::get<double>(*value) >= 0.0);
	}

	// IR entity counts appear after IR generation.
	const auto* operations = stats->find("ir.operations");
	REQUIRE(operations != nullptr);
	REQUIRE(std::get<int64_t>(*operations) > 0);

	// IR pass manager registers totals + at least the empty-block pass.
	REQUIRE(stats->contains("irPasses.totalMs"));
	REQUIRE(stats->contains("irPasses.EmptyBlockElimination.ms"));

	// Backend recorded its own total.
	REQUIRE(stats->contains("backend.totalMs"));
	REQUIRE(stats->contains("backend.name"));
	REQUIRE(std::get<std::string>(*stats->find("backend.name")) == backend);

	// End-to-end total covers everything.
	REQUIRE(stats->contains("compilation.totalMs"));
	REQUIRE(stats->contains("compilation.unitId"));
}

TEST_CASE("CompilationStatistics: interpreted module has no stats") {
	Options options;
	options.setOption("engine.Compilation", false);
	NautilusEngine engine(options);
	auto fn = engine.registerFunction(statsAddOne);

	REQUIRE(fn(1) == 2);
	REQUIRE(fn.getStatistics() == nullptr);
}

#if defined(ENABLE_TRACING) && defined(ENABLE_BC_BACKEND)

namespace {

// A larger workload than statsAddOne so that register reuse has
// something visible to do; without enough live ranges the allocator's
// effect on the register count would be invisible (a single arithmetic
// op already maps 1:1 to its register).
val<int32_t> statsLoopSum(val<int32_t> n) {
	val<int32_t> sum = val<int32_t>(0);
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		sum = sum + i;
	}
	return sum;
}

} // namespace

TEST_CASE("CompilationStatistics: bytecode backend reports code size") {
	Options options;
	options.setOption("engine.backend", std::string {"bc"});
	options.setOption("engine.compilationStrategy", std::string("legacy"));

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(statsAddOne);
	REQUIRE(fn(5) == 6);

	auto stats = fn.getStatistics();
	REQUIRE(stats != nullptr);
	REQUIRE(stats->contains("bc.instructions"));
	REQUIRE(stats->contains("bc.codeSize.bytes"));
	REQUIRE(std::get<int64_t>(*stats->find("bc.instructions")) > 0);
	REQUIRE(std::get<int64_t>(*stats->find("bc.codeSize.bytes")) > 0);
	// Register-allocator stats are always emitted by the BC backend,
	// regardless of whether the allocator itself is enabled.
	REQUIRE(stats->contains("bc.registers.total"));
	REQUIRE(stats->contains("bc.registers.max"));
	REQUIRE(stats->contains("bc.registerAllocator.enabled"));
	REQUIRE(std::get<int64_t>(*stats->find("bc.registers.total")) > 0);
	REQUIRE(std::get<int64_t>(*stats->find("bc.registers.max")) > 0);
}

TEST_CASE("CompilationStatistics: bc.registerAllocator option shrinks the register file") {
	auto compileWithAllocator = [](bool enabled) {
		Options options;
		options.setOption("engine.backend", std::string {"bc"});
		options.setOption("engine.compilationStrategy", std::string("legacy"));
		options.setOption("bc.registerAllocator", enabled);
		auto engine = std::make_unique<NautilusEngine>(options);
		auto fn = engine->registerFunction(statsLoopSum);
		// Run the function so we know the produced bytecode is correct
		// even with the allocator enabled.
		REQUIRE(fn(10) == 45);
		auto stats = fn.getStatistics();
		REQUIRE(stats != nullptr);
		REQUIRE(std::get<std::string>(*stats->find("bc.registerAllocator.enabled")) ==
		        std::string(enabled ? "true" : "false"));
		auto maxRegs = std::get<int64_t>(*stats->find("bc.registers.max"));
		// Hold the engine alive until after the stats are read.
		(void) engine;
		return maxRegs;
	};

	auto withAlloc = compileWithAllocator(true);
	auto withoutAlloc = compileWithAllocator(false);

	INFO("registers with allocator=" << withAlloc << " without=" << withoutAlloc);
	REQUIRE(withAlloc <= withoutAlloc);
	REQUIRE(withAlloc < withoutAlloc); // for this workload reuse is always possible
}
#endif

} // namespace nautilus::engine
