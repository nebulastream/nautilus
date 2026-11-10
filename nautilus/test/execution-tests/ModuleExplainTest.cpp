#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/TieredCompiler.hpp"
#include "nautilus/config.hpp"
#include <algorithm>
#include <catch2/catch_all.hpp>
#include <string>

// `CompiledModule::explain()` (plan M4.9, 15 §4.3): the report of a module's tiers, promotion, functions and
// statistics, as text and as JSON, with the parts whose producers are not built yet marked as such.

#if defined(ENABLE_TRACING) && defined(ENABLE_TBC_BACKEND)

namespace nautilus::engine {

namespace {

val<int32_t> explainAddOne(val<int32_t> x) {
	return x + 1;
}

bool contains(const std::string& text, const std::string& part) {
	return text.find(part) != std::string::npos;
}

/// A structural check of one-line JSON: strings close, brackets pair up, nothing follows the top-level object.
bool wellFormedJson(const std::string& text) {
	std::string stack;
	bool inString = false;
	for (size_t i = 0; i < text.size(); ++i) {
		const char c = text[i];
		if (inString) {
			if (c == '\\') {
				++i;
			} else if (c == '"') {
				inString = false;
			}
			continue;
		}
		if (c == '"') {
			inString = true;
		} else if (c == '{' || c == '[') {
			stack += c;
		} else if (c == '}' || c == ']') {
			if (stack.empty() || stack.back() != (c == '}' ? '{' : '[')) {
				return false;
			}
			stack.pop_back();
			if (stack.empty() && i + 1 != text.size()) {
				return false;
			}
		}
	}
	return stack.empty() && !inString && !text.empty();
}

const ModuleExplanation::Function* findFunction(const ModuleExplanation& report, const std::string& name) {
	const auto it = std::find_if(report.functions.begin(), report.functions.end(),
	                             [&](const auto& function) { return function.name == name; });
	return it == report.functions.end() ? nullptr : &*it;
}

/// An engine with two `tbc` tiers and the policy `manual`, so a test holds the module on tier 0 until it promotes.
struct ManualTiers {
	ManualTiers() {
		Options options;
		options.setOption("engine.tiered.promotionThreads", 1);
		TieredCompilationConfig config;
		config.tier0.backend = "tbc";
		config.tier1.backend = "tbc";
		config.policy = PromotionPolicy::manual;
		auto jit = std::make_unique<compiler::TieredJITCompiler>(options, config, traceArenaPool, irArenaPool);
		engine = std::make_unique<NautilusEngine>(std::move(jit), options);
	}
	common::ArenaPool traceArenaPool;
	common::ArenaPool irArenaPool;
	std::unique_ptr<NautilusEngine> engine;
};

} // namespace

TEST_CASE("Explain - a single-tier module reports its tier, backend, functions and statistics", "[explain]") {
	auto engine = testing::makeEngine("tbc");
	auto module = engine.createModule("explain.single");
	module.registerFunction("add_one", explainAddOne);
	auto compiled = module.compile();

	const auto report = compiled.explain();
	CHECK(report.name == "explain.single");
	CHECK(report.stage == "compiled");
	CHECK(report.tier == "tier1");
	CHECK(report.backend == "tbc");
	CHECK(report.promotion == "none");
	REQUIRE(report.tiers.size() == 1);
	CHECK(report.tiers[0].tier == "tier1");
	CHECK(report.tiers[0].status == "active");
	REQUIRE(report.functionsAvailable);
	const auto* function = findFunction(report, "add_one");
	REQUIRE(function != nullptr);
	CHECK(function->exported);
	CHECK(function->backend == "tbc");
	CHECK(function->instructions > 0);
	CHECK(function->blocks > 0);
	REQUIRE(report.statistics != nullptr);
	CHECK(report.statistics->contains("compilation.totalMs"));

	// the parts without a producer yet are values, not omissions
	CHECK_FALSE(report.persistability.available);
	CHECK(report.persistability.status == "not available yet");
	CHECK(report.persistability.milestone == "M5.3");
	CHECK_FALSE(report.inlining.available);
	CHECK(report.inlining.milestone == "M3.8");

	const auto text = report.toString();
	CHECK(contains(text, "tier = tier1"));
	CHECK(contains(text, "function add_one:"));
	CHECK(contains(text, "persistability = not available yet (M5.3)"));
	CHECK(contains(text, "inlining = not available yet (M3.8)"));

	// the report is the same through the module's own statistics
	CHECK(report.statistics == compiled.getStatistics());
}

TEST_CASE("Explain - the JSON is one well-formed object with stable keys", "[explain]") {
	auto engine = testing::makeEngine("tbc");
	auto module = engine.createModule("explain.json \"quoted\"");
	module.registerFunction("add_one", explainAddOne);
	auto compiled = module.compile();

	const auto json = compiled.explain().toJson();
	CHECK(wellFormedJson(json));
	CHECK(json.find('\n') == std::string::npos);
	for (const auto* key : {"\"module\":", "\"tier\":\"tier1\"", "\"backend\":\"tbc\"", "\"promotion\":{\"status\":\"none\"",
	                        "\"tiers\":[", "\"functions\":[", "\"functionsAvailable\":true", "\"statistics\":{",
	                        "\"persistability\":{\"available\":false,\"status\":\"not available yet\",\"milestone\":\"M5.3\"}",
	                        "\"inlining\":{\"available\":false,\"status\":\"not available yet\",\"milestone\":\"M3.8\"}"}) {
		INFO(key);
		CHECK(contains(json, key));
	}
	// the module name is escaped
	CHECK(contains(json, "explain.json \\\"quoted\\\""));
}

TEST_CASE("Explain - a tiered module shows the promotion", "[explain]") {
	ManualTiers tiers;
	auto module = tiers.engine->createModule();
	module.registerFunction("add_one", explainAddOne);
	auto compiled = module.compile();

	const auto before = compiled.explain();
	CHECK(before.tier == "tier0");
	CHECK(before.promotion == "none");
	REQUIRE(before.tiers.size() == 2);
	CHECK(before.tiers[0].tier == "tier0");
	CHECK(before.tiers[0].backend == "tbc");
	CHECK(before.tiers[0].status == "active");
	CHECK(before.tiers[1].tier == "tier1");
	CHECK(before.tiers[1].status == "none");
	CHECK(before.name.empty());

	REQUIRE(compiled.promote());
	REQUIRE(compiled.waitForPromotion(std::chrono::seconds(60)));
	const auto after = compiled.explain();
	CHECK(after.tier == "tier1");
	CHECK(after.promotion == "done");
	CHECK(after.tiers[0].status == "replaced");
	CHECK(after.tiers[1].status == "active");
	CHECK(wellFormedJson(after.toJson()));
	CHECK(contains(after.toJson(), "\"promotion\":{\"status\":\"done\""));
}

TEST_CASE("Explain - an interpreted module reports no statistics", "[explain]") {
	auto engine = testing::makeEngine("interpreter");
	auto module = engine.createModule();
	module.registerFunction("add_one", explainAddOne);
	auto compiled = module.compile();

	const auto report = compiled.explain();
	CHECK(report.tier == "interpreted");
	CHECK(report.backend == "interpreter");
	CHECK(report.statistics == nullptr);
	const auto json = report.toJson();
	CHECK(wellFormedJson(json));
	CHECK(contains(json, "\"statistics\":null"));
	CHECK(contains(report.toString(), "statistics = none"));
}

} // namespace nautilus::engine

#endif
