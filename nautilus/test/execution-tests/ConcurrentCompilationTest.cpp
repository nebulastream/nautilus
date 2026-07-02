#include "catch2/catch_test_macros.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include <atomic>
#include <catch2/catch_all.hpp>
#include <string>
#include <thread>
#include <vector>

namespace nautilus::engine {

// Kernels with varying signatures (and one with a traced loop) so each module
// registers several functions. The more a module traces, the wider the window
// in which concurrent compiles on a shared engine used to corrupt the single
// trace arena — so multi-function modules are exactly the stress we want.

val<int32_t> ccAddOne(val<int32_t> x) {
	return x + 1;
}

val<int64_t> ccSum(val<int64_t> a, val<int64_t> b) {
	return a + b;
}

val<int32_t> ccMultiply(val<int32_t> a, val<int32_t> b) {
	return a * b;
}

/// Traced loop: returns 1 + 2 + ... + n for n >= 1, and 0 for n <= 0.
val<int32_t> ccSumToN(val<int32_t> n) {
	val<int32_t> acc = 0;
	for (val<int32_t> i = 1; i <= n; i = i + 1) {
		acc = acc + i;
	}
	return acc;
}

namespace {

/// Build a multi-function module, compile it, and verify every compiled
/// function returns the expected result. Returns the number of mismatches
/// (0 == success).
int compileAndVerify(NautilusEngine& engine, int seed) {
	int errors = 0;
	auto module = engine.createModule();
	module.registerFunction("add_one", ccAddOne);
	module.registerFunction("sum", ccSum);
	module.registerFunction("multiply", ccMultiply);
	module.registerFunction("sum_to_n", ccSumToN);
	auto compiled = module.compile();

	auto addOne = compiled.getFunction<int32_t(int32_t)>("add_one");
	auto sum = compiled.getFunction<int64_t(int64_t, int64_t)>("sum");
	auto mul = compiled.getFunction<int32_t(int32_t, int32_t)>("multiply");
	auto sumToN = compiled.getFunction<int32_t(int32_t)>("sum_to_n");

	const int32_t a = seed % 100;
	const int32_t b = (seed * 3) % 100;
	const int32_t n = seed % 50;
	if (addOne(a) != a + 1) {
		++errors;
	}
	if (sum(a, b) != static_cast<int64_t>(a) + b) {
		++errors;
	}
	if (mul(a, b) != a * b) {
		++errors;
	}
	if (sumToN(n) != n * (n + 1) / 2) {
		++errors;
	}
	return errors;
}

/// Spawn several threads that each repeatedly compile-and-verify on the one
/// shared engine. Asserts every compilation produced correct results (and,
/// implicitly, that none of them crashed or corrupted shared engine state).
void runConcurrentCompilation(NautilusEngine& engine) {
	constexpr int NUM_THREADS = 8;
	constexpr int ITERATIONS = 25;

	std::atomic<int> errors {0};
	std::vector<std::thread> threads;
	threads.reserve(NUM_THREADS);

	for (int t = 0; t < NUM_THREADS; ++t) {
		threads.emplace_back([&engine, &errors, t]() {
			for (int i = 0; i < ITERATIONS; ++i) {
				errors.fetch_add(compileAndVerify(engine, t * ITERATIONS + i + 1), std::memory_order_relaxed);
			}
		});
	}

	for (auto& th : threads) {
		th.join();
	}

	REQUIRE(errors.load() == 0);
}

/// A backend suitable for thread-safe compiled execution, or empty if none.
std::string threadSafeBackend() {
#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)
	return "mlir";
#elif defined(ENABLE_TRACING) && defined(ENABLE_C_BACKEND)
	return "cpp";
#else
	return "";
#endif
}

/// A (tier0, tier1) backend pair for tiered compilation, or empty strings.
std::pair<std::string, std::string> tieredBackends() {
#if defined(ENABLE_TRACING) && defined(ENABLE_BC_BACKEND) && defined(ENABLE_MLIR_BACKEND)
	return {"bc", "mlir"};
#elif defined(ENABLE_TRACING) && defined(ENABLE_BC_BACKEND) && defined(ENABLE_C_BACKEND)
	return {"bc", "cpp"};
#else
	return {"", ""};
#endif
}

} // namespace

// One shared engine, many threads, each compiling many multi-function modules.
// Exercises the per-compile trace-arena pool on the explicit-backend
// (LegacyCompiler) path: before the fix, concurrent compiles raced on the
// engine's single trace arena and crashed.
TEST_CASE("Concurrent Compilation - Shared Engine, Explicit Backend") {
	const auto backend = threadSafeBackend();
	if (backend.empty()) {
		SKIP("No thread-safe compilation backend available");
	}

	Options options;
	options.setOption("engine.backend", backend);
	auto engine = NautilusEngine(options);

	runConcurrentCompilation(engine);
}

// Same workload on the DEFAULT tiered compiler (fast tier-0, tier-1 promoted in
// the background). Exercises the trace-arena pool plus the per-module promotion
// path under concurrency: every compile carries its own IR to its own
// promotion, so concurrent compiles must not race on compiler-level state.
// Destroying the engine joins any still-pending promotions.
TEST_CASE("Concurrent Compilation - Shared Engine, Default Tiered") {
	const auto [tier0, tier1] = tieredBackends();
	if (tier0.empty()) {
		SKIP("Need bc + a high-tier backend for tiered compilation");
	}

	// No explicit "engine.backend" → the engine selects the tiered compiler;
	// the tier backends are taken from these options.
	Options options;
	options.setOption("engine.tier0.backend", tier0);
	options.setOption("engine.tier1.backend", tier1);
	auto engine = NautilusEngine(options);

	runConcurrentCompilation(engine);
}

} // namespace nautilus::engine
