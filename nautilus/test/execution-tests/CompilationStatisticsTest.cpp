#include "ExecutionTest.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
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

	// The IR pass manager always records its total: the terminal passes run
	// for every backend. The optimization group -- and with it the
	// empty-block pass -- runs only for a backend that benefits from it.
	REQUIRE(stats->contains("irPasses.totalMs"));
	REQUIRE(stats->contains("irPasses.optimizationGroup"));
	const bool optimizes =
	    compiler::CompilationBackendRegistry::getInstance()->getBackend(backend)->benefitsFromIROptimizationPasses();
	REQUIRE(std::get<int64_t>(*stats->find("irPasses.optimizationGroup")) == (optimizes ? 1 : 0));
	REQUIRE(stats->contains("irPasses.EmptyBlockElimination.ms") == optimizes);

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

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)
TEST_CASE("CompilationStatistics: mlir skips the IR optimization group by default") {
	// Single-tier mlir hands the IR straight to LLVM, whose -O3 pipeline
	// subsumes the Nautilus-IR optimization group, so the pipeline skips it.
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	NautilusEngine engine(options);
	auto fn = engine.registerFunction(statsAddOne);
	REQUIRE(fn(41) == 42);
	auto stats = fn.getStatistics();
	REQUIRE(stats != nullptr);
	REQUIRE(std::get<int64_t>(*stats->find("irPasses.optimizationGroup")) == 0);
	REQUIRE_FALSE(stats->contains("irPasses.EmptyBlockElimination.ms"));
	REQUIRE_FALSE(stats->contains("irPasses.ConstantFoldingAndCopyPropagation.ms"));
	// The terminal passes are not optimizations and still run.
	REQUIRE(stats->contains("irPasses.exceptionRegionPreparation.ms"));
}

TEST_CASE("CompilationStatistics: ir.forceOptimizationPasses re-enables the group on mlir") {
	// The override brings the group back for A/B runs and for tooling that
	// wants the per-pass dumps.
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("ir.forceOptimizationPasses", true);
	NautilusEngine engine(options);
	auto fn = engine.registerFunction(statsAddOne);
	REQUIRE(fn(41) == 42);
	auto stats = fn.getStatistics();
	REQUIRE(stats != nullptr);
	REQUIRE(std::get<int64_t>(*stats->find("irPasses.optimizationGroup")) == 1);
	REQUIRE(stats->contains("irPasses.EmptyBlockElimination.ms"));
}
#endif

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND) &&                                                         \
    (defined(ENABLE_BC_BACKEND) || defined(ENABLE_ASMJIT_BACKEND))
TEST_CASE("CompilationStatistics: two-tier compile keeps the IR optimization group for tier 0") {
	// The tier-0 backend executes the IR as it is, so the graph both tiers
	// share is optimized even though tier 1 (mlir) would not need it.
	Options options;
#ifdef ENABLE_ASMJIT_BACKEND
	options.setOption("engine.tier0.backend", std::string("asmjit"));
#else
	options.setOption("engine.tier0.backend", std::string("bc"));
#endif
	options.setOption("engine.tier1.backend", std::string("mlir"));
	options.setOption("engine.tiered.backgroundPromotion", true);
	NautilusEngine engine(options);
	auto fn = engine.registerFunction(statsAddOne);
	REQUIRE(fn(41) == 42);
	auto stats = fn.getStatistics();
	REQUIRE(stats != nullptr);
	REQUIRE(std::get<int64_t>(*stats->find("irPasses.optimizationGroup")) == 1);
	REQUIRE(stats->contains("irPasses.EmptyBlockElimination.ms"));
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
