// Plugin pass hooks (docs/plans/nautilus-0.4/03-conversion-and-passes.md §5, plan M3.9). The test plugin registers its
// passes from a static initializer, before any engine exists (the registry freezes with the first engine, M1.7).
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/TieredCompiler.hpp"
#include "nautilus/config.hpp"
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include <atomic>
#include <catch2/catch_all.hpp>
#include <mutex>

using namespace nautilus;
using namespace nautilus::engine;

namespace {

struct Counters {
	std::atomic<int> afterRequired {0}, afterOptional {0}, beforeRequired {0}, beforeOptional {0};
	std::mutex mutex;
	std::vector<std::string> backends; // what BeforeLowering passes were told
	void reset() {
		afterRequired = afterOptional = beforeRequired = beforeOptional = 0;
		const std::lock_guard<std::mutex> lock(mutex);
		backends.clear();
	}
};
Counters counters;

/// A copy of the module in which every constant `from` of type i64 is `to`: a pass that changes what the code computes,
/// so that a test sees whether the backend got the module the pass returned.
std::optional<nir::Module> rewriteConstant(const nir::Module& in, uint64_t from, uint64_t to) {
	auto records = std::vector<nir::ConstRecord>(in.consts.records().begin(), in.consts.records().end());
	bool changed = false;
	for (auto& r : records) {
		if (static_cast<nir::TypeId>(r.type) == nir::TypeId::I64 && r.bits == from) {
			r.bits = to;
			changed = true;
		}
	}
	if (!changed) {
		return std::nullopt;
	}
	nir::Module out = in;
	out.consts.reset(std::move(records), std::vector<std::byte>(in.consts.bytes().begin(), in.consts.bytes().end()));
	return out;
}

// the test plugin
const bool registered = [] {
	using namespace nautilus::ext;
	auto& registry = ExtensionRegistry::instance();
	registry.registerPass(PassInfo {"testpass.afterRequired", PassPoint::AfterConversion, PassRequirement::Required,
	                                [](const nir::Module& m, const PassEnvironment&) {
		                                counters.afterRequired++;
		                                return rewriteConstant(m, 100, 101);
	                                }});
	registry.registerPass(PassInfo {"testpass.afterOptional", PassPoint::AfterConversion, PassRequirement::Optional,
	                                [](const nir::Module& m, const PassEnvironment&) {
		                                counters.afterOptional++;
		                                return rewriteConstant(m, 200, 202);
	                                }});
	registry.registerPass(PassInfo {"testpass.beforeRequired", PassPoint::BeforeLowering, PassRequirement::Required,
	                                [](const nir::Module& m, const PassEnvironment& env) {
		                                counters.beforeRequired++;
		                                const std::lock_guard<std::mutex> lock(counters.mutex);
		                                counters.backends.emplace_back(env.backend);
		                                return rewriteConstant(m, 300, 303);
	                                }});
	registry.registerPass(PassInfo {"testpass.beforeOptional", PassPoint::BeforeLowering, PassRequirement::Optional,
	                                [](const nir::Module& m, const PassEnvironment&) {
		                                counters.beforeOptional++;
		                                return rewriteConstant(m, 400, 404);
	                                }});
	return true;
}();

val<int64_t> plus100(val<int64_t> x) {
	return x + 100;
}
val<int64_t> plus200(val<int64_t> x) {
	return x + 200;
}
val<int64_t> plus300(val<int64_t> x) {
	return x + 300;
}
val<int64_t> plus400(val<int64_t> x) {
	return x + 400;
}

/// The result of the four functions, each `x + c`, at 0: what the passes did to the constants.
struct Results {
	int64_t a, b, c, d;
};

Results compileAndRun(NautilusEngine& engine, std::optional<CompiledModule>* keep = nullptr) {
	auto module = engine.createModule();
	module.registerFunction("p100", plus100);
	module.registerFunction("p200", plus200);
	module.registerFunction("p300", plus300);
	module.registerFunction("p400", plus400);
	auto compiled = module.compile();
	Results r {compiled.getFunction<int64_t(int64_t)>("p100")(0), compiled.getFunction<int64_t(int64_t)>("p200")(0),
	           compiled.getFunction<int64_t(int64_t)>("p300")(0), compiled.getFunction<int64_t(int64_t)>("p400")(0)};
	if (keep != nullptr) {
		keep->emplace(std::move(compiled));
	}
	return r;
}

std::string firstBackend() {
#if defined(ENABLE_TBC_BACKEND)
	return "tbc";
#elif defined(ENABLE_ASMJIT_BACKEND)
	return "asmjit";
#else
	return "";
#endif
}

[[maybe_unused]] std::pair<std::string, std::string> tiers() {
#if defined(ENABLE_TRACING) && defined(ENABLE_TBC_BACKEND) && defined(ENABLE_C_BACKEND)
	return {"tbc", "cpp"};
#elif defined(ENABLE_TRACING) && defined(ENABLE_TBC_BACKEND) && defined(ENABLE_MLIR_BACKEND)
	return {"tbc", "mlir"};
#else
	return {"", ""};
#endif
}

} // namespace

TEST_CASE("the test plugin registered its passes before the first engine", "[passes][plugin]") {
	REQUIRE(registered);
	auto& registry = ext::ExtensionRegistry::instance();
	REQUIRE(registry.passesAt(ext::PassPoint::AfterConversion).size() == 2);
	REQUIRE(registry.passesAt(ext::PassPoint::BeforeLowering).size() == 2);
	REQUIRE(registry.hasPassesAt(ext::PassPoint::AfterConversion));
	REQUIRE_FALSE(registry.frozen());
	// a late registration is rejected once the registry is frozen
	engine::Options options;
	if (!firstBackend().empty()) {
		options.setOption("engine.backend", firstBackend());
		NautilusEngine engine(options);
		REQUIRE(registry.frozen());
		REQUIRE_THROWS_AS(registry.registerPass("testpass.late"), ext::RegistrationError);
	}
}

TEST_CASE("a compile with one backend runs the required and the optional passes of both points", "[passes][plugin]") {
	if (firstBackend().empty()) {
		SKIP("no backend");
	}
	counters.reset();
	Options options;
	options.setOption("engine.backend", firstBackend());
	NautilusEngine engine(options);
	const auto r = compileAndRun(engine);
	REQUIRE(counters.afterRequired == 1);
	REQUIRE(counters.afterOptional == 1);
	REQUIRE(counters.beforeRequired == 1);
	REQUIRE(counters.beforeOptional == 1);
	// each pass changed what the code computes, and the backend ran the module the last pass returned
	REQUIRE(r.a == 101);
	REQUIRE(r.b == 202);
	REQUIRE(r.c == 303);
	REQUIRE(r.d == 404);
	REQUIRE(counters.backends == std::vector<std::string> {firstBackend()});
}

TEST_CASE("only the required passes run when the optional ones are switched off", "[passes][plugin]") {
	if (firstBackend().empty()) {
		SKIP("no backend");
	}
	for (const char* option : {"ir.runOptimizationPasses", "ir.runPasses"}) {
		counters.reset();
		Options options;
		options.setOption("engine.backend", firstBackend());
		options.setOption(option, false);
		NautilusEngine engine(options);
		const auto r = compileAndRun(engine);
		INFO(option);
		REQUIRE(counters.afterRequired == 1);
		REQUIRE(counters.beforeRequired == 1);
		REQUIRE(counters.afterOptional == 0);
		REQUIRE(counters.beforeOptional == 0);
		REQUIRE(r.a == 101);
		REQUIRE(r.b == 200);
		REQUIRE(r.c == 303);
		REQUIRE(r.d == 400);
	}
}

TEST_CASE("a required pass runs on the latency-critical compile of a promoted module and an optional one does not",
          "[passes][plugin][tiered]") {
	auto [tier0, tier1] = tiers();
	if (tier0.empty()) {
		SKIP("two backends are needed for two tiers");
	}
	counters.reset();
	Options options;
	TieredCompilationConfig config;
	config.tier0.backend = tier0;
	config.tier1.backend = tier1;
	config.policy = PromotionPolicy::manual;
	common::ArenaPool traceArenaPool, irArenaPool;
	auto jit = std::make_unique<compiler::TieredJITCompiler>(options, config, traceArenaPool, irArenaPool);
	NautilusEngine engine(std::move(jit), options);

	std::optional<CompiledModule> kept;
	const auto tier0Results = compileAndRun(engine, &kept);
	auto& compiled = *kept;
	// tier 0: the required passes of both points ran once, the optional ones not at all
	REQUIRE(counters.afterRequired == 1);
	REQUIRE(counters.beforeRequired == 1);
	REQUIRE(counters.afterOptional == 0);
	REQUIRE(counters.beforeOptional == 0);
	REQUIRE(tier0Results.a == 101);
	REQUIRE(tier0Results.b == 200);
	REQUIRE(tier0Results.c == 303);
	REQUIRE(tier0Results.d == 400);

	// tier 1 runs everything again that belongs to a compile of its own: the module's required pass of
	// `afterConversion` is part of the shared conversion and ran once
	REQUIRE(compiled.promote());
	REQUIRE(compiled.waitForPromotion(std::chrono::seconds(60)));
	engine.waitForPromotions();
	REQUIRE(compiled.tier() == Tier::tier1);
	REQUIRE(counters.afterRequired == 1);
	REQUIRE(counters.beforeRequired == 2);
	REQUIRE(counters.afterOptional == 1);
	REQUIRE(counters.beforeOptional == 1);
	{
		const std::lock_guard<std::mutex> lock(counters.mutex);
		REQUIRE(counters.backends == std::vector<std::string> {tier0, tier1});
	}
	REQUIRE(compiled.getFunction<int64_t(int64_t)>("p400")(0) == 404);
	REQUIRE(compiled.getFunction<int64_t(int64_t)>("p200")(0) == 202);
	REQUIRE(compiled.getFunction<int64_t(int64_t)>("p100")(0) == 101);
}

TEST_CASE("ir.runOptimizationPasses=true runs the optional passes in the tier-0 compile too",
          "[passes][plugin][tiered]") {
	auto [tier0, tier1] = tiers();
	if (tier0.empty()) {
		SKIP("two backends are needed for two tiers");
	}
	counters.reset();
	Options options;
	options.setOption("ir.runOptimizationPasses", true);
	TieredCompilationConfig config;
	config.tier0.backend = tier0;
	config.tier1.backend = tier1;
	config.policy = PromotionPolicy::manual;
	common::ArenaPool traceArenaPool, irArenaPool;
	auto jit = std::make_unique<compiler::TieredJITCompiler>(options, config, traceArenaPool, irArenaPool);
	NautilusEngine engine(std::move(jit), options);
	const auto r = compileAndRun(engine);
	REQUIRE(counters.afterOptional == 1);
	REQUIRE(counters.beforeOptional == 1);
	REQUIRE(r.b == 202);
	REQUIRE(r.d == 404);
}

TEST_CASE("the statistics count the passes that ran", "[passes][plugin]") {
	if (firstBackend().empty()) {
		SKIP("no backend");
	}
	counters.reset();
	Options options;
	options.setOption("engine.backend", firstBackend());
	NautilusEngine engine(options);
	std::optional<CompiledModule> compiled;
	compileAndRun(engine, &compiled);
	auto stats = compiled->getStatistics();
	REQUIRE(stats != nullptr);
	for (const char* pass :
	     {"testpass.afterRequired", "testpass.afterOptional", "testpass.beforeRequired", "testpass.beforeOptional"}) {
		INFO(pass);
		const auto* runs = stats->find(std::string("irPasses.") + pass + ".runs");
		REQUIRE(runs != nullptr);
		REQUIRE(std::get<int64_t>(*runs) == 1);
		const auto* changes = stats->find(std::string("irPasses.") + pass + ".changes");
		REQUIRE(changes != nullptr);
		REQUIRE(std::get<int64_t>(*changes) == 1);
		REQUIRE(stats->contains(std::string("irPasses.") + pass + ".ms"));
	}
	// afterRequired (conversion), afterOptional, the optional core group (empty: no round) and beforeLowering
	INFO(stats->toString());
	REQUIRE(std::get<int64_t>(*stats->find("irPasses.pipelineIterations")) >= 3);
}
