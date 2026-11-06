#include "ExecutionTest.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include <catch2/catch_all.hpp>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

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

val<int32_t> statsAbs(val<int32_t> x) {
	val<int32_t> result = x;
	if (x < 0) {
		result = 0 - x;
	}
	return result;
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
	for (const auto* key : {"tracing.ms", "irGeneration.ms", "frontend.totalMs"}) {
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

TEST_CASE("CompilationStatistics: per-pass IR counts are opt-in via ir.passStatistics") {
	auto backend = getAnyBackend();
	if (backend.empty()) {
		SKIP("No compilation backend available");
	}

	// Timing keys are always recorded; the IR walks behind the entity counts
	// only run when asked for, so the default compile does not pay for them.
	{
		Options options;
		options.setOption("engine.backend", backend);
		NautilusEngine engine(options);
		auto fn = engine.registerFunction(statsAddOne);
		REQUIRE(fn(41) == 42);
		auto stats = fn.getStatistics();
		REQUIRE(stats != nullptr);
		REQUIRE(stats->contains("irPasses.totalMs"));
		REQUIRE_FALSE(stats->contains("irPasses.baseline.operations"));
		REQUIRE_FALSE(stats->contains("irPasses.exceptionRegionPreparation.operationsDelta"));
	}
	{
		Options options;
		options.setOption("engine.backend", backend);
		options.setOption("ir.passStatistics", true);
		NautilusEngine engine(options);
		auto fn = engine.registerFunction(statsAddOne);
		REQUIRE(fn(41) == 42);
		auto stats = fn.getStatistics();
		REQUIRE(stats != nullptr);
		REQUIRE(stats->contains("irPasses.baseline.operations"));
		REQUIRE(stats->contains("irPasses.exceptionRegionPreparation.operationsDelta"));
	}
}

#if defined(ENABLE_TRACING) && defined(ENABLE_BC_BACKEND)
namespace {

/// A lower-bound search: its loop is traced with two headers, one where `lo`
/// changes and one where `hi` changes, the first holding nothing but a jump
/// to the second.
val<int32_t> statsLowerBound(val<int32_t*> sorted, val<int32_t> length, val<int32_t> needle) {
	val<int32_t> lo = 0;
	val<int32_t> hi = length;
	while (lo < hi) {
		val<int32_t> mid = lo + (hi - lo) / 2;
		if (sorted[mid] < needle) {
			lo = mid + 1;
		} else {
			hi = mid;
		}
	}
	return lo;
}

/// Blocks and block arguments left once the IR passes are done, from the
/// baseline counts and every pass's delta.
std::pair<int64_t, int64_t> blocksAndArgumentsAfterPasses(const compiler::CompilationStatistics& stats) {
	int64_t blocks = std::get<int64_t>(*stats.find("irPasses.baseline.blocks"));
	int64_t arguments = std::get<int64_t>(*stats.find("irPasses.baseline.blockArguments"));
	for (const auto& [key, value] : stats) {
		if (key.ends_with(".blocksDelta")) {
			blocks -= std::get<int64_t>(value);
		} else if (key.ends_with(".blockArgumentsDelta")) {
			arguments -= std::get<int64_t>(value);
		}
	}
	return {blocks, arguments};
}

} // namespace

TEST_CASE("CompilationStatistics: a loop traced with two headers compiles to one") {
	std::vector<int32_t> sorted {1, 3, 5, 7, 9, 11};
	Options options;
	// bc runs the whole IR pipeline, so the counts below are the final shape.
	options.setOption("engine.backend", std::string("bc"));
	options.setOption("ir.passStatistics", true);
	NautilusEngine engine(options);
	auto fn = engine.registerFunction(statsLowerBound);
	REQUIRE(fn(sorted.data(), 6, 7) == 3);
	REQUIRE(fn(sorted.data(), 6, 12) == 6);
	auto stats = fn.getStatistics();
	REQUIRE(stats != nullptr);
	// Entry (three parameters), one loop header (`lo` and `hi`), the loop body,
	// the `lo` update and the exit. The tracer leaves a jump-only block where
	// `lo` changes in front of the header; threaded during conversion, it never
	// becomes a second header whose phi the body reads directly -- which could
	// then not be cleaned up, and LLVM does not if-convert a loop with two
	// headers.
	REQUIRE(blocksAndArgumentsAfterPasses(*stats) == std::pair<int64_t, int64_t> {5, 5});
}
#endif

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

TEST_CASE("CompilationStatistics: NAUTILUS_STATISTICS_FILE receives one JSON line per compilation") {
	auto backend = getAnyBackend();
	if (backend.empty()) {
		SKIP("No compilation backend available");
	}

	const auto path = std::filesystem::temp_directory_path() / "nautilus-statistics-test.jsonl";
	std::filesystem::remove(path);
	struct EnvironmentGuard {
		explicit EnvironmentGuard(const char* value) {
			setenv("NAUTILUS_STATISTICS_FILE", value, 1);
		}
		~EnvironmentGuard() {
			unsetenv("NAUTILUS_STATISTICS_FILE");
		}
	};

	Options options;
	options.setOption("engine.backend", backend);
	std::string reported;
	{
		const EnvironmentGuard guard(path.c_str());
		NautilusEngine engine(options);
		auto fn = engine.registerFunction(statsAddOne);
		REQUIRE(fn(41) == 42);
		reported = fn.getStatistics()->toJson();
	}

	std::ifstream in(path);
	std::string line;
	REQUIRE(std::getline(in, line));
	REQUIRE(line == reported);
	REQUIRE(line.find("\"compilation.totalMs\":") != std::string::npos);
	REQUIRE_FALSE(std::getline(in, line));
	std::filesystem::remove(path);
}

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

TEST_CASE("CompilationStatistics: tracing records its scopes, runs and trace size") {
	auto backend = getAnyBackend();
	if (backend.empty()) {
		SKIP("No compilation backend available");
	}

	Options options;
	options.setOption("engine.backend", backend);
	NautilusEngine engine(options);
	auto fn = engine.registerFunction(statsAbs);
	REQUIRE(fn(-3) == 3);

	auto stats = fn.getStatistics();
	REQUIRE(stats != nullptr);
	auto count = [&](const char* key) {
		INFO("missing key: " << key);
		const auto* value = stats->find(key);
		REQUIRE(value != nullptr);
		return std::get<int64_t>(*value);
	};
	CHECK(count("tracing.functions") == 1);
	CHECK(count("tracing.scopes") == 1);
	// One run per path through the branch.
	CHECK(count("tracing.iterations") == 2);
	CHECK(count("tracing.blocks") >= 3);
	CHECK(count("tracing.operations") > count("tracing.blocks"));
	// `result = x` and `result = 0 - x` record ASSIGNs.
	CHECK(count("tracing.copies") >= 2);
	// The second run replays the path to the branch before recording the other arm.
	CHECK(count("tracing.replayedOperations") > 0);
	CHECK(count("tracing.snapshots") >= count("tracing.operations") / 2);

	auto milliseconds = [&](const char* key) {
		INFO("missing key: " << key);
		const auto* value = stats->find(key);
		REQUIRE(value != nullptr);
		return std::get<double>(*value);
	};
	const auto firstRun = milliseconds("tracing.firstRun.ms");
	const auto rerun = milliseconds("tracing.rerun.ms");
	const auto finalize = milliseconds("tracing.finalize.ms");
	CHECK(firstRun > 0.0);
	CHECK(rerun > 0.0);
	CHECK(finalize >= 0.0);
	CHECK(firstRun + rerun + finalize <= milliseconds("tracing.ms"));
}

} // namespace nautilus::engine
