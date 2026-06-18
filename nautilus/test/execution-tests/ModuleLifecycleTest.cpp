#include "nautilus/config.hpp"

#if defined(ENABLE_TRACING)

#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>

#if defined(ENABLE_MLIR_BACKEND)
#include "MLIRJitEventListenerTestFixture.hpp"
#endif

namespace nautilus::engine {

namespace {

val<int32_t> lifecycleAddOne(val<int32_t> x) {
	return x + 1;
}

val<int64_t> lifecycleSum(val<int64_t> a, val<int64_t> b) {
	return a + b;
}

} // namespace

// Reusing a single engine for many compiled modules must free each module's
// executable (and its JIT-owned code memory) independently when the module is
// dropped, while the engine and its shared infrastructure stay alive and
// usable.
TEST_CASE("Engine reused for many modules frees each module independently") {
	NautilusEngine engine;

	constexpr int iterations = 200;
	for (int i = 0; i < iterations; ++i) {
		auto module = engine.createModule();
		module.registerFunction("execute", lifecycleAddOne);
		auto compiled = module.compile();
		auto fn = compiled.getFunction<int32_t(int32_t)>("execute");
		REQUIRE(fn(i) == i + 1);
		// `compiled` (and its executable) is dropped at the end of this scope.
	}

	// The engine survived every module drop and is still usable.
	auto module = engine.createModule();
	module.registerFunction("execute", lifecycleSum);
	auto compiled = module.compile();
	auto execute = compiled.getFunction<int64_t(int64_t, int64_t)>("execute");
	REQUIRE(execute(20, 22) == 42);
}

// Per-module option overrides on the same engine compile correctly and do not
// leak across modules.
TEST_CASE("Per-module option overrides on a shared engine") {
	NautilusEngine engine;

	// Override via createModule(overrides).
	ModuleOptions overrides;
	overrides.setOption("mlir.optimizationLevel", 0);
	auto moduleA = engine.createModule(overrides);
	moduleA.registerFunction("execute", lifecycleAddOne);
	auto compiledA = moduleA.compile();
	REQUIRE(compiledA.getFunction<int32_t(int32_t)>("execute")(10) == 11);

	// Override via NautilusModule::setOption after creation.
	auto moduleB = engine.createModule();
	moduleB.setOption("mlir.optimizationLevel", 3);
	moduleB.registerFunction("execute", lifecycleSum);
	auto compiledB = moduleB.compile();
	REQUIRE(compiledB.getFunction<int64_t(int64_t, int64_t)>("execute")(1, 2) == 3);
}

#if defined(ENABLE_MLIR_BACKEND)

// With the MLIR backend, a JIT event listener lets us observe that every
// object loaded for a dropped module is also freed: the freed count converges
// to the loaded count once all modules have been destroyed, proving JIT memory
// is reclaimed per module while the engine lives on.
TEST_CASE("MLIR: dropped modules free their JIT objects on a shared engine") {
	nautilus::testing::CountingJITEventListenerHandle handle;

	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.addMLIRJitEventListener(handle.listener());

	NautilusEngine engine(options);

	constexpr int iterations = 50;
	for (int i = 0; i < iterations; ++i) {
		auto module = engine.createModule();
		module.registerFunction("execute", lifecycleAddOne);
		auto compiled = module.compile();
		REQUIRE(compiled.getFunction<int32_t(int32_t)>("execute")(i) == i + 1);
	}

	// Every loaded object belongs to a module that has now been dropped, so all
	// of them must have been freed.
	REQUIRE(handle.numObjectsLoaded() > 0);
	REQUIRE(handle.numObjectsFreed() == handle.numObjectsLoaded());

	// The engine is still usable after all modules were freed.
	auto module = engine.createModule();
	module.registerFunction("execute", lifecycleAddOne);
	auto compiled = module.compile();
	REQUIRE(compiled.getFunction<int32_t(int32_t)>("execute")(100) == 101);
}

#endif // ENABLE_MLIR_BACKEND

} // namespace nautilus::engine

#endif // ENABLE_TRACING
