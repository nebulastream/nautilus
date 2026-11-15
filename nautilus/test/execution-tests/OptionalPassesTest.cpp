// The optional passes in the engine (plan M3.8, M3.10): the statistics of a default compile and of one with the passes
// on.
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include <catch2/catch_all.hpp>
#include <nautilus/nautilus_function.hpp>

using namespace nautilus;
using namespace nautilus::engine;

namespace {

val<int64_t> optionalInvariantLoop(val<int64_t> n, val<int64_t> a, val<int64_t> b) {
	val<int64_t> sum = 0;
	for (val<int64_t> i = 0; i < n; i = i + 1) {
		sum = sum + a * b + i;
	}
	return sum;
}

val<int64_t> optionalTwice(val<int64_t> x) {
	return x + x;
}
static auto optionalTwiceFunction = NautilusFunction {"optionalTwice", optionalTwice};

val<int64_t> optionalCaller(val<int64_t> x) {
	return optionalTwiceFunction(optionalTwiceFunction(x)) + 1;
}

std::vector<std::string> backends() {
	std::vector<std::string> result;
#ifdef ENABLE_TBC_BACKEND
	result.push_back("tbc");
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	result.push_back("asmjit");
#endif
#ifdef ENABLE_C_BACKEND
	result.push_back("cpp");
#endif
	return result;
}

int64_t statistic(const compiler::CompilationStatistics& stats, const std::string& key) {
	const auto* value = stats.find(key);
	REQUIRE(value != nullptr);
	return std::get<int64_t>(*value);
}

} // namespace

TEST_CASE("no pass runs on a default compile: irPasses.pipelineIterations is 0", "[passes][statistics]") {
	for (const auto& backend : backends()) {
		INFO(backend);
		Options options;
		options.setOption("engine.backend", backend);
		NautilusEngine engine(options);
		auto fn = engine.registerFunction(optionalInvariantLoop);
		REQUIRE(fn(3, 3, 4) == 39);
		auto stats = fn.getStatistics();
		REQUIRE(stats != nullptr);
		REQUIRE(statistic(*stats, "irPasses.pipelineIterations") == 0);
		REQUIRE_FALSE(stats->contains("irPasses.licm.runs"));
		REQUIRE_FALSE(stats->contains("irPasses.inline.runs"));
	}
}

TEST_CASE("the optional passes report runs, changes and time", "[passes][statistics]") {
	for (const auto& backend : backends()) {
		INFO(backend);
		Options options;
		options.setOption("engine.backend", backend);
		options.setOption("ir.enableLICM", true);
		options.setOption("ir.enableStrengthReduction", true);
		options.setOption("ir.inline", true);
		options.setOption("ir.verifyAfterEachPass", true);
		NautilusEngine engine(options);
		auto loop = engine.registerFunction(optionalInvariantLoop);
		REQUIRE(loop(0, 1, 1) == 0);
		REQUIRE(loop(3, 3, 4) == 39);
		REQUIRE(loop(10, 2, 5) == 145);
		auto stats = loop.getStatistics();
		REQUIRE(statistic(*stats, "irPasses.pipelineIterations") >= 1);
		REQUIRE(statistic(*stats, "irPasses.licm.runs") >= 1);
		REQUIRE(statistic(*stats, "irPasses.licm.changes") >= 1);
		REQUIRE(stats->contains("irPasses.licm.ms"));
		REQUIRE(stats->contains("irPasses.totalMs"));

		auto caller = engine.registerFunction(optionalCaller);
		REQUIRE(caller(5) == 21);
		auto callerStats = caller.getStatistics();
		REQUIRE(statistic(*callerStats, "irPasses.inline.changes") >= 1);
	}
}

TEST_CASE("the optional passes do not run on the tier-0 compile unless asked to", "[passes][statistics]") {
	if (backends().empty()) {
		SKIP("no compiler in this build: the engine interprets and has no statistics");
	}
	Options options;
	options.setOption("ir.enableLICM", true);
	options.setOption("ir.runOptimizationPasses", false);
	NautilusEngine engine(options);
	auto fn = engine.registerFunction(optionalInvariantLoop);
	REQUIRE(fn(3, 3, 4) == 39);
	REQUIRE(statistic(*fn.getStatistics(), "irPasses.pipelineIterations") == 0);
}

TEST_CASE("explain() reports what the inlining pass decided", "[passes][explain]") {
	if (backends().empty()) {
		SKIP("no compiler in this build: nothing is inlined");
	}
	Options options;
	options.setOption("engine.backend", backends().front());
	options.setOption("ir.inline", true);
	NautilusEngine engine(options);
	auto module = engine.createModule();
	module.registerFunction("caller", optionalCaller);
	auto compiled = module.compile();
	REQUIRE(compiled.getFunction<int64_t(int64_t)>("caller")(5) == 21);
	const auto report = compiled.explain();
	REQUIRE(report.inlining.status == "ran");
	bool inlined = false;
	for (const auto& line : report.inlining.details) {
		inlined = inlined || line.find("inlined: optionalTwice") != std::string::npos;
	}
	REQUIRE(inlined);
	REQUIRE(report.toJson().find("\"inlining\":{\"available\":true,\"status\":\"ran\"") != std::string::npos);
}
