#include "catch2/catch_test_macros.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/TieredCompiler.hpp"
#include "nautilus/config.hpp"
#include <catch2/catch_all.hpp>
#include <cstdlib>
#include <thread>
#include <vector>

namespace nautilus::engine {

val<int32_t> tieredAddOne(val<int32_t> x) {
	return x + 1;
}

val<int64_t> tieredSum(val<int64_t> a, val<int64_t> b) {
	return a + b;
}

val<int32_t> tieredMultiply(val<int32_t> a, val<int32_t> b) {
	return a * b;
}

/// Returns a pair of backend names suitable for tiered compilation (tier0, tier1).
/// Returns empty strings if not enough backends available.
std::pair<std::string, std::string> getTieredBackends() {
#if defined(ENABLE_TRACING) && defined(ENABLE_TBC_BACKEND) && defined(ENABLE_MLIR_BACKEND)
	return {"tbc", "mlir"};
#elif defined(ENABLE_TRACING) && defined(ENABLE_TBC_BACKEND) && defined(ENABLE_C_BACKEND)
	return {"tbc", "cpp"};
#else
	return {"", ""};
#endif
}

TEST_CASE("Tiered Compilation - Tier 0 Executes Correctly") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier0Backend.empty()) {
		SKIP("Need at least two compilation backends for tiered compilation");
	}

	TieredCompilationConfig config;
	config.tier0.backend = tier0Backend;
	config.tier1.backend = tier1Backend;

	common::ArenaPool traceArenaPool;
	common::ArenaPool irArenaPool;
	auto tieredJit = std::make_unique<compiler::TieredJITCompiler>(Options(), config, traceArenaPool, irArenaPool);
	auto* jit = tieredJit.get();
	auto engine = NautilusEngine(std::move(tieredJit));
	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);
	module.registerFunction("sum", tieredSum);
	module.registerFunction("multiply", tieredMultiply);

	auto compiled = module.compile();

	auto addOneFn = compiled.getFunction<int32_t(int32_t)>("add_one");
	auto sumFn = compiled.getFunction<int64_t(int64_t, int64_t)>("sum");
	auto mulFn = compiled.getFunction<int32_t(int32_t, int32_t)>("multiply");

	// Should produce correct results immediately (tier 0)
	REQUIRE(addOneFn(5) == 6);
	REQUIRE(addOneFn(0) == 1);
	REQUIRE(addOneFn(-1) == 0);
	REQUIRE(sumFn(3, 4) == 7);
	REQUIRE(sumFn(0, 0) == 0);
	REQUIRE(sumFn(-5, 10) == 5);
	REQUIRE(mulFn(3, 4) == 12);
	REQUIRE(mulFn(0, 100) == 0);

	jit->waitForPendingPromotions();
}

TEST_CASE("Tiered Compilation - Background Promotion Completes") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier0Backend.empty()) {
		SKIP("Need at least two compilation backends for tiered compilation");
	}

	TieredCompilationConfig config;
	config.tier0.backend = tier0Backend;
	config.tier1.backend = tier1Backend;

	common::ArenaPool traceArenaPool;
	common::ArenaPool irArenaPool;
	auto tieredJit = std::make_unique<compiler::TieredJITCompiler>(Options(), config, traceArenaPool, irArenaPool);
	auto* jit = tieredJit.get();
	auto engine = NautilusEngine(std::move(tieredJit));
	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);

	auto compiled = module.compile();
	auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");

	// Works immediately via tier 0
	REQUIRE(fn(5) == 6);

	// Wait for tier 1 to complete
	jit->waitForPendingPromotions();
	REQUIRE(jit->allPromotionsComplete());

	// Still correct after promotion
	REQUIRE(fn(5) == 6);
	REQUIRE(fn(0) == 1);
	REQUIRE(fn(-100) == -99);
}

TEST_CASE("Tiered Compilation - Finished Promotion Threads Are Reaped") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier0Backend.empty()) {
		SKIP("Need at least two compilation backends for tiered compilation");
	}

	TieredCompilationConfig config;
	config.tier0.backend = tier0Backend;
	config.tier1.backend = tier1Backend;

	common::ArenaPool traceArenaPool;
	common::ArenaPool irArenaPool;
	auto tieredJit = std::make_unique<compiler::TieredJITCompiler>(Options(), config, traceArenaPool, irArenaPool);
	auto* jit = tieredJit.get();
	auto engine = NautilusEngine(std::move(tieredJit));

	// Short-lived modules, each promoted before the next is compiled. Without
	// reaping, the compiler would hold one finished thread (and its stack) per
	// module until it is destroyed.
	constexpr int modules = 32;
	for (int i = 0; i < modules; i++) {
		auto module = engine.createModule();
		module.registerFunction("add_one", tieredAddOne);
		auto compiled = module.compile();
		REQUIRE(compiled.getFunction<int32_t(int32_t)>("add_one")(i) == i + 1);
		while (!jit->allPromotionsComplete()) {
			std::this_thread::yield();
		}
	}
	REQUIRE(jit->retainedPromotionThreads() <= 1);

	jit->waitForPendingPromotions();
	REQUIRE(jit->retainedPromotionThreads() == 0);
}

TEST_CASE("Tiered Compilation - Results Correct After Promotion") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier0Backend.empty()) {
		SKIP("Need at least two compilation backends for tiered compilation");
	}

	TieredCompilationConfig config;
	config.tier0.backend = tier0Backend;
	config.tier1.backend = tier1Backend;

	common::ArenaPool traceArenaPool;
	common::ArenaPool irArenaPool;
	auto tieredJit = std::make_unique<compiler::TieredJITCompiler>(Options(), config, traceArenaPool, irArenaPool);
	auto* jit = tieredJit.get();
	auto engine = NautilusEngine(std::move(tieredJit));
	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);
	module.registerFunction("sum", tieredSum);

	auto compiled = module.compile();
	auto addOneFn = compiled.getFunction<int32_t(int32_t)>("add_one");
	auto sumFn = compiled.getFunction<int64_t(int64_t, int64_t)>("sum");

	// Call before promotion
	REQUIRE(addOneFn(10) == 11);
	REQUIRE(sumFn(3, 4) == 7);

	// Wait for promotion
	jit->waitForPendingPromotions();

	// Call after promotion — results must still be correct
	REQUIRE(addOneFn(5) == 6);
	REQUIRE(addOneFn(0) == 1);
	REQUIRE(addOneFn(-100) == -99);
	REQUIRE(sumFn(10, 20) == 30);
	REQUIRE(sumFn(-5, 5) == 0);
}

TEST_CASE("Tiered Compilation - Custom Backend Per Tier") {
	std::vector<std::pair<std::string, std::string>> combos;

#if defined(ENABLE_TRACING)
#if defined(ENABLE_TBC_BACKEND) && defined(ENABLE_MLIR_BACKEND)
	combos.emplace_back("tbc", "mlir");
#endif
#if defined(ENABLE_TBC_BACKEND) && defined(ENABLE_C_BACKEND)
	combos.emplace_back("tbc", "cpp");
#endif
#endif

	if (combos.empty()) {
		SKIP("Need at least two compilation backends");
	}

	for (auto& [t0, t1] : combos) {
		DYNAMIC_SECTION("tier0=" + t0 + " tier1=" + t1) {
			TieredCompilationConfig config;
			config.tier0.backend = t0;
			config.tier1.backend = t1;

			common::ArenaPool traceArenaPool;
			common::ArenaPool irArenaPool;
			auto tieredJit =
			    std::make_unique<compiler::TieredJITCompiler>(Options(), config, traceArenaPool, irArenaPool);
			auto* jit = tieredJit.get();
			auto engine = NautilusEngine(std::move(tieredJit));
			auto module = engine.createModule();
			module.registerFunction("add_one", tieredAddOne);

			auto compiled = module.compile();
			auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");

			REQUIRE(fn(42) == 43);
			REQUIRE(fn(-1) == 0);

			jit->waitForPendingPromotions();
			REQUIRE(jit->allPromotionsComplete());

			REQUIRE(fn(42) == 43);
			REQUIRE(fn(-1) == 0);
		}
	}
}

TEST_CASE("Tiered Compilation - Options Based Configuration") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier0Backend.empty()) {
		SKIP("Need at least two compilation backends for tiered compilation");
	}

	Options options;
	options.setOption("engine.tier0.backend", tier0Backend);
	options.setOption("engine.tier1.backend", tier1Backend);

	// NautilusEngine auto-selects TieredJITCompiler when tier options are set
	auto engine = NautilusEngine(options);
	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);

	auto compiled = module.compile();
	auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");

	REQUIRE(fn(100) == 101);

	// The engine manages promotion — just verify results are correct.
	// Promotion will complete when the engine (and its JIT compiler) is destroyed.
	// For this test, we verify immediate execution works.
	REQUIRE(fn(100) == 101);
}

TEST_CASE("Tiered Compilation - Multiple Functions In Module") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier0Backend.empty()) {
		SKIP("Need at least two compilation backends for tiered compilation");
	}

	TieredCompilationConfig config;
	config.tier0.backend = tier0Backend;
	config.tier1.backend = tier1Backend;

	common::ArenaPool traceArenaPool;
	common::ArenaPool irArenaPool;
	auto tieredJit = std::make_unique<compiler::TieredJITCompiler>(Options(), config, traceArenaPool, irArenaPool);
	auto* jit = tieredJit.get();
	auto engine = NautilusEngine(std::move(tieredJit));
	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);
	module.registerFunction("sum", tieredSum);
	module.registerFunction("multiply", tieredMultiply);

	auto compiled = module.compile();
	auto addFn = compiled.getFunction<int32_t(int32_t)>("add_one");
	auto sumFn = compiled.getFunction<int64_t(int64_t, int64_t)>("sum");
	auto mulFn = compiled.getFunction<int32_t(int32_t, int32_t)>("multiply");

	// All functions work pre-promotion
	REQUIRE(addFn(99) == 100);
	REQUIRE(sumFn(100, 200) == 300);
	REQUIRE(mulFn(7, 8) == 56);

	// Wait for promotion
	jit->waitForPendingPromotions();
	REQUIRE(jit->allPromotionsComplete());

	// All functions work post-promotion
	REQUIRE(addFn(99) == 100);
	REQUIRE(sumFn(100, 200) == 300);
	REQUIRE(mulFn(7, 8) == 56);
}

TEST_CASE("Tiered Compilation - Single Tier Mode") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier0Backend.empty()) {
		SKIP("Need at least two compilation backends for tiered compilation");
	}

	// Single-tier mode: the compiler should compile directly with the
	// high-performance tier-1 backend and perform no background promotion.
	TieredCompilationConfig config;
	config.tier0.backend = tier0Backend;
	config.tier1.backend = tier1Backend;
	config.backgroundPromotion = false;

	common::ArenaPool traceArenaPool;
	common::ArenaPool irArenaPool;
	auto tieredJit = std::make_unique<compiler::TieredJITCompiler>(Options(), config, traceArenaPool, irArenaPool);
	auto* jit = tieredJit.get();
	auto engine = NautilusEngine(std::move(tieredJit));
	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);
	module.registerFunction("sum", tieredSum);

	auto compiled = module.compile();
	auto addOneFn = compiled.getFunction<int32_t(int32_t)>("add_one");
	auto sumFn = compiled.getFunction<int64_t(int64_t, int64_t)>("sum");

	REQUIRE(addOneFn(5) == 6);
	REQUIRE(sumFn(3, 4) == 7);

	// No background promotion was started, so it is immediately complete and
	// waiting is a no-op.
	REQUIRE(jit->allPromotionsComplete());
	jit->waitForPendingPromotions();
	REQUIRE(jit->allPromotionsComplete());

	// Results remain correct.
	REQUIRE(addOneFn(0) == 1);
	REQUIRE(sumFn(-5, 10) == 5);
}

TEST_CASE("Tiered Compilation - Single Tier Mode Via Option") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier0Backend.empty()) {
		SKIP("Need at least two compilation backends for tiered compilation");
	}

	// Configure single-tier mode purely through engine options.
	Options options;
	options.setOption("engine.tier0.backend", tier0Backend);
	options.setOption("engine.tier1.backend", tier1Backend);
	options.setOption("engine.tiered.backgroundPromotion", false);

	auto engine = NautilusEngine(options);
	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);

	auto compiled = module.compile();
	auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");

	REQUIRE(fn(100) == 101);
	REQUIRE(fn(-1) == 0);
}

TEST_CASE("Tiered Compilation - Interpreter Tier 0") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier1Backend.empty()) {
		SKIP("Need a tier-1 compilation backend");
	}

	// Interpreter tier-0: the module executes uncompiled (direct invocation
	// of the original callables) until the background tier-1 promotion swaps
	// in its executable.
	TieredCompilationConfig config;
	config.tier0.backend = INTERPRETER_BACKEND;
	config.tier1.backend = tier1Backend;

	common::ArenaPool traceArenaPool;
	common::ArenaPool irArenaPool;
	auto tieredJit = std::make_unique<compiler::TieredJITCompiler>(Options(), config, traceArenaPool, irArenaPool);
	auto* jit = tieredJit.get();
	auto engine = NautilusEngine(std::move(tieredJit));
	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);
	module.registerFunction("sum", tieredSum);

	auto compiled = module.compile();
	auto addOneFn = compiled.getFunction<int32_t(int32_t)>("add_one");
	auto sumFn = compiled.getFunction<int64_t(int64_t, int64_t)>("sum");

	// Correct results immediately, served by the interpreted path.
	REQUIRE(addOneFn(5) == 6);
	REQUIRE(sumFn(3, 4) == 7);

	// Wait for the tier-1 promotion to swap in the compiled executable.
	jit->waitForPendingPromotions();
	REQUIRE(jit->allPromotionsComplete());

	// Still correct after promotion, and the executable's stats carry the
	// tier-1 report.
	REQUIRE(addOneFn(0) == 1);
	REQUIRE(sumFn(-5, 10) == 5);
	auto stats = compiled.getStatistics();
	REQUIRE(stats != nullptr);
	REQUIRE(std::get<std::string>(*stats->find("tier")) == "tier1");
	REQUIRE(std::get<std::string>(*stats->find("backend.name")) == tier1Backend);
}

TEST_CASE("Tiered Compilation - Interpreter Tier 0 Via Option") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier1Backend.empty()) {
		SKIP("Need a tier-1 compilation backend");
	}

	Options options;
	options.setOption("engine.tier0.backend", std::string(INTERPRETER_BACKEND));
	options.setOption("engine.tier1.backend", tier1Backend);

	auto engine = NautilusEngine(options);
	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);

	auto compiled = module.compile();
	auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");

	// Served interpreted until promotion completes; correct either way.
	REQUIRE(fn(100) == 101);
	REQUIRE(fn(-1) == 0);
}

TEST_CASE("Tiered Compilation - Default Tier 0 Selection Order") {
	// With no backend or tier options set, tier 0 is selected by
	// availability: asmjit, then tbc, then the interpreter.
	auto engine = NautilusEngine(Options());
	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);

	auto compiled = module.compile();
	auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");
	REQUIRE(fn(41) == 42);

	auto stats = compiled.getStatistics();
#if defined(ENABLE_TRACING) && defined(ENABLE_ASMJIT_BACKEND)
	REQUIRE(stats != nullptr);
	if (std::get<std::string>(*stats->find("tier")) == "tier0") {
		REQUIRE(std::get<std::string>(*stats->find("backend.name")) == "asmjit");
	}
#elif defined(ENABLE_TRACING) && defined(ENABLE_TBC_BACKEND)
	REQUIRE(stats != nullptr);
	if (std::get<std::string>(*stats->find("tier")) == "tier0") {
		REQUIRE(std::get<std::string>(*stats->find("backend.name")) == "tbc");
	}
#else
	// Neither asmjit nor tbc: the module runs interpreted (no stats) until
	// the background promotion completes.
	(void) stats;
#endif
}

TEST_CASE("Tiered Compilation - Explicit Backend Runs Single-Tier") {
	// `engine.backend` pins a single backend: the tiered compiler runs in
	// single-tier mode with tier1 = that backend, compiles synchronously and
	// never promotes.
	std::vector<std::string> backends;
#ifdef ENABLE_TRACING
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_TBC_BACKEND
	backends.emplace_back("tbc");
#endif
#endif

	for (auto& backend : backends) {
		DYNAMIC_SECTION("explicit " + backend) {
			Options options;
			options.setOption("engine.backend", backend);
			auto engine = NautilusEngine(options);

			// Single-tier mode reports the bare backend name.
			REQUIRE(engine.getNameOfBackend() == backend);

			auto module = engine.createModule();
			module.registerFunction("add_one", tieredAddOne);
			auto compiled = module.compile();
			auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");

			// Correct results immediately: the compile is synchronous and the
			// executable was produced by the pinned backend.
			REQUIRE(fn(5) == 6);
			REQUIRE(fn(0) == 1);

			// The pinned backend ran as the single (tier-1) tier.
			auto stats = compiled.getStatistics();
			REQUIRE(stats != nullptr);
			REQUIRE(stats->contains("tier"));
			REQUIRE(std::get<std::string>(*stats->find("tier")) == "tier1");
			REQUIRE(std::get<std::string>(*stats->find("backend.name")) == backend);
		}
	}
}

namespace {
/// Sets NAUTILUS_OPTIONS for the scope of one test.
struct NautilusOptionsGuard {
	explicit NautilusOptionsGuard(const char* value) {
		setenv("NAUTILUS_OPTIONS", value, 1);
	}
	~NautilusOptionsGuard() {
		unsetenv("NAUTILUS_OPTIONS");
	}
};
} // namespace

TEST_CASE("Tiered Compilation - NAUTILUS_OPTIONS Layers Typed Options Over The Engine's") {
	const NautilusOptionsGuard guard("dump.all=true;ir.maxPipelineIterations=7;custom.ratio=0.5;custom.name=x;;");
	Options options;
	options.setOption("dump.all", false);
	options.setOption("custom.kept", 3);
	auto engine = NautilusEngine(options);
	const auto module = engine.createModule();
	const auto& applied = module.getOptions();
	REQUIRE(applied.getOptionOrDefault("dump.all", false) == true);
	REQUIRE(applied.getOptionOrDefault("ir.maxPipelineIterations", 0) == 7);
	REQUIRE(applied.getOptionOrDefault("custom.ratio", 0.0) == 0.5);
	REQUIRE(applied.getOptionOrDefault<std::string>("custom.name", "") == "x");
	REQUIRE(applied.getOptionOrDefault("custom.kept", 0) == 3);
}

TEST_CASE("Tiered Compilation - NAUTILUS_OPTIONS Rejects An Entry Without A Value") {
	const NautilusOptionsGuard guard("engine.backend");
	REQUIRE_THROWS(NautilusEngine(Options()));
}

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND) && defined(ENABLE_ASMJIT_BACKEND)
TEST_CASE("Tiered Compilation - NAUTILUS_OPTIONS Replaces The Selected Backend") {
	// The environment wins over the application's own choice, so a workload can
	// be moved to another backend without touching its code.
	const NautilusOptionsGuard guard("engine.backend=asmjit");
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	auto engine = NautilusEngine(options);
	REQUIRE(engine.getNameOfBackend() == "asmjit");

	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);
	auto compiled = module.compile();
	REQUIRE(compiled.getFunction<int32_t(int32_t)>("add_one")(5) == 6);
	REQUIRE(std::get<std::string>(*compiled.getStatistics()->find("backend.name")) == "asmjit");
}
#endif

} // namespace nautilus::engine
