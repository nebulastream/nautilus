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
#elif defined(ENABLE_TRACING) && defined(ENABLE_TBC_BACKEND)
	return "tbc";
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
	// An explicit backend forces single-tier compilation, so the stats on
	// the active executable carry both frontend and backend keys in the
	// same report.
	options.setOption("engine.backend", backend);

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

	// IR pass manager registers totals + at least the exception-region pass,
	// which every backend needs; the optimization passes depend on the
	// backend (see the policy test below).
	REQUIRE(stats->contains("irPasses.totalMs"));
	REQUIRE(stats->contains("irPasses.exceptionRegionPreparation.ms"));

	// Backend recorded its own total.
	REQUIRE(stats->contains("backend.totalMs"));
	REQUIRE(stats->contains("backend.name"));
	REQUIRE(std::get<std::string>(*stats->find("backend.name")) == backend);

	// The explicit backend runs as the single (tier-1) tier.
	REQUIRE(stats->contains("tier"));
	REQUIRE(std::get<std::string>(*stats->find("tier")) == "tier1");

	// End-to-end total covers everything.
	REQUIRE(stats->contains("compilation.totalMs"));
	REQUIRE(stats->contains("compilation.unitId"));
}

namespace {

/// Whether the module's statistics show the full IR optimization pipeline
/// ran: the pass manager records a timing per pass it ran, and constant
/// folding is only ever registered as part of the full pipeline.
[[maybe_unused]] bool ranIROptimizationPasses(const compiler::CompilationStatistics& stats) {
	return stats.contains("irPasses.ConstantFoldingAndCopyPropagation.ms");
}

[[maybe_unused]] bool ranBlockArgumentPruning(const compiler::CompilationStatistics& stats) {
	return stats.contains("irPasses.BlockArgumentPruning.ms");
}

} // namespace

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)
TEST_CASE("CompilationStatistics: a module compiled by mlir alone gets only block-argument pruning") {
	// LLVM's pipeline performs the other cleanups itself, so by default the
	// graph goes to the MLIR backend with only its block arguments pruned --
	// the exception-handling analyses still run.
	{
		Options options;
		options.setOption("engine.backend", std::string("mlir"));
		NautilusEngine engine(options);
		auto fn = engine.registerFunction(statsAddOne);
		REQUIRE(fn(41) == 42);
		auto stats = fn.getStatistics();
		REQUIRE(stats != nullptr);
		REQUIRE_FALSE(ranIROptimizationPasses(*stats));
		REQUIRE(ranBlockArgumentPruning(*stats));
		REQUIRE(stats->contains("irPasses.exceptionRegionPreparation.ms"));
	}
	// The option pins them on regardless of the backend.
	{
		Options options;
		options.setOption("engine.backend", std::string("mlir"));
		options.setOption("ir.runOptimizationPasses", true);
		NautilusEngine engine(options);
		auto fn = engine.registerFunction(statsAddOne);
		REQUIRE(fn(41) == 42);
		auto stats = fn.getStatistics();
		REQUIRE(stats != nullptr);
		REQUIRE(ranIROptimizationPasses(*stats));
	}
}
#endif

#if defined(ENABLE_TRACING) && defined(ENABLE_BC_BACKEND)
TEST_CASE("CompilationStatistics: a backend that executes the IR as it is gets the optimization passes") {
	{
		Options options;
		options.setOption("engine.backend", std::string("bc"));
		NautilusEngine engine(options);
		auto fn = engine.registerFunction(statsAddOne);
		REQUIRE(fn(41) == 42);
		auto stats = fn.getStatistics();
		REQUIRE(stats != nullptr);
		REQUIRE(ranIROptimizationPasses(*stats));
	}
	// ... unless the option turns them off.
	{
		Options options;
		options.setOption("engine.backend", std::string("bc"));
		options.setOption("ir.runOptimizationPasses", false);
		NautilusEngine engine(options);
		auto fn = engine.registerFunction(statsAddOne);
		REQUIRE(fn(41) == 42);
		auto stats = fn.getStatistics();
		REQUIRE(stats != nullptr);
		REQUIRE_FALSE(ranIROptimizationPasses(*stats));
		REQUIRE_FALSE(ranBlockArgumentPruning(*stats));
		REQUIRE(stats->contains("irPasses.exceptionRegionPreparation.ms"));
	}
}
#endif

#if defined(ENABLE_TRACING) && defined(ENABLE_BC_BACKEND) && defined(ENABLE_MLIR_BACKEND)
TEST_CASE("CompilationStatistics: a two-tier compile optimizes the graph its bc tier executes") {
	// The graph is traced once and shared by both tiers; tier 0 executes it as
	// it is, so it is optimized even though tier 1 is mlir.
	Options options;
	options.setOption("engine.tier0.backend", std::string("bc"));
	options.setOption("engine.tier1.backend", std::string("mlir"));
	options.setOption("engine.tiered.backgroundPromotion", true);
	NautilusEngine engine(options);
	auto fn = engine.registerFunction(statsAddOne);
	REQUIRE(fn(41) == 42);
	// The handle's statistics are the tier-0 executable's until promotion
	// swaps in tier 1; either way the frontend keys are the same report.
	auto stats = fn.getStatistics();
	REQUIRE(stats != nullptr);
	if (std::get<std::string>(*stats->find("tier")) == "tier0") {
		REQUIRE(ranIROptimizationPasses(*stats));
	}
}
#endif

TEST_CASE("CompilationStatistics: interpreted module has no stats") {
	Options options;
	options.setOption("engine.Compilation", false);
	NautilusEngine engine(options);
	auto fn = engine.registerFunction(statsAddOne);

	REQUIRE(fn(1) == 2);
	REQUIRE(fn.getStatistics() == nullptr);
}

// Backend-internal statistics keys (bc.*, tbc.*) are covered by
// backends/bc/CompilationStatisticsTest.cpp and
// backends/tbc/CompilationStatisticsTest.cpp instead of here.

} // namespace nautilus::engine
