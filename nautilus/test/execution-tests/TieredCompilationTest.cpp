#include "catch2/catch_test_macros.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/TieredCompilation.hpp"
#include "nautilus/config.hpp"
#include <catch2/catch_all.hpp>
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
#if defined(ENABLE_TRACING) && defined(ENABLE_BC_BACKEND) && defined(ENABLE_MLIR_BACKEND)
	return {"bc", "mlir"};
#elif defined(ENABLE_TRACING) && defined(ENABLE_BC_BACKEND) && defined(ENABLE_C_BACKEND)
	return {"bc", "cpp"};
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

	auto engine = NautilusEngine(std::make_unique<compiler::TieredJITCompiler>(Options(), config));
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
}

TEST_CASE("Tiered Compilation - Background Promotion Completes") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier0Backend.empty()) {
		SKIP("Need at least two compilation backends for tiered compilation");
	}

	TieredCompilationConfig config;
	config.tier0.backend = tier0Backend;
	config.tier1.backend = tier1Backend;

	auto engine = NautilusEngine(std::make_unique<compiler::TieredJITCompiler>(Options(), config));
	auto module = engine.createModule();
	module.registerFunction("add_one", tieredAddOne);

	auto compiled = module.compile();
	auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");

	// Works immediately via tier 0
	REQUIRE(fn(5) == 6);

	// Wait for tier 1 to complete
	auto* tieredExe =
	    dynamic_cast<compiler::TieredExecutable*>(const_cast<compiler::Executable*>(compiled.getExecutable()));
	REQUIRE(tieredExe != nullptr);
	tieredExe->waitForPromotion();

	REQUIRE(tieredExe->isPromoted());

	// Still correct after promotion
	REQUIRE(fn(5) == 6);
	REQUIRE(fn(0) == 1);
	REQUIRE(fn(-100) == -99);
}

TEST_CASE("Tiered Compilation - Results Correct After Promotion") {
	auto [tier0Backend, tier1Backend] = getTieredBackends();
	if (tier0Backend.empty()) {
		SKIP("Need at least two compilation backends for tiered compilation");
	}

	TieredCompilationConfig config;
	config.tier0.backend = tier0Backend;
	config.tier1.backend = tier1Backend;

	auto engine = NautilusEngine(std::make_unique<compiler::TieredJITCompiler>(Options(), config));
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
	auto* tieredExe =
	    dynamic_cast<compiler::TieredExecutable*>(const_cast<compiler::Executable*>(compiled.getExecutable()));
	REQUIRE(tieredExe != nullptr);
	tieredExe->waitForPromotion();

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
#if defined(ENABLE_BC_BACKEND) && defined(ENABLE_MLIR_BACKEND)
	combos.emplace_back("bc", "mlir");
#endif
#if defined(ENABLE_BC_BACKEND) && defined(ENABLE_C_BACKEND)
	combos.emplace_back("bc", "cpp");
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

			auto engine = NautilusEngine(std::make_unique<compiler::TieredJITCompiler>(Options(), config));
			auto module = engine.createModule();
			module.registerFunction("add_one", tieredAddOne);

			auto compiled = module.compile();
			auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");

			REQUIRE(fn(42) == 43);
			REQUIRE(fn(-1) == 0);

			auto* tieredExe =
			    dynamic_cast<compiler::TieredExecutable*>(const_cast<compiler::Executable*>(compiled.getExecutable()));
			REQUIRE(tieredExe != nullptr);
			tieredExe->waitForPromotion();
			REQUIRE(tieredExe->isPromoted());

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

	// The executable should be a TieredExecutable
	auto* tieredExe =
	    dynamic_cast<compiler::TieredExecutable*>(const_cast<compiler::Executable*>(compiled.getExecutable()));
	REQUIRE(tieredExe != nullptr);
	tieredExe->waitForPromotion();
	REQUIRE(tieredExe->isPromoted());

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

	auto engine = NautilusEngine(std::make_unique<compiler::TieredJITCompiler>(Options(), config));
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
	auto* tieredExe =
	    dynamic_cast<compiler::TieredExecutable*>(const_cast<compiler::Executable*>(compiled.getExecutable()));
	REQUIRE(tieredExe != nullptr);
	tieredExe->waitForPromotion();
	REQUIRE(tieredExe->isPromoted());

	// All functions work post-promotion
	REQUIRE(addFn(99) == 100);
	REQUIRE(sumFn(100, 200) == 300);
	REQUIRE(mulFn(7, 8) == 56);
}

TEST_CASE("Tiered Compilation - Standard JITCompiler Still Works") {
	// Verify the original JITCompiler path is unchanged
	std::vector<std::string> backends;
#ifdef ENABLE_TRACING
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif
#endif

	for (auto& backend : backends) {
		DYNAMIC_SECTION("standard " + backend) {
			Options options;
			options.setOption("engine.backend", backend);
			auto engine = NautilusEngine(options);

			auto module = engine.createModule();
			module.registerFunction("add_one", tieredAddOne);
			auto compiled = module.compile();
			auto fn = compiled.getFunction<int32_t(int32_t)>("add_one");

			REQUIRE(fn(5) == 6);
			REQUIRE(fn(0) == 1);
		}
	}
}

} // namespace nautilus::engine
