#include "nautilus/Engine.hpp"
#include "nautilus/TieredCompilation.hpp"
#include "nautilus/config.hpp"
#include <catch2/catch_all.hpp>
#include <chrono>

namespace nautilus::engine {

// Local benchmark functions (avoid ODR issues with shared test headers)
static val<int32_t> benchAddOne(val<int32_t> x) {
	return x + 1;
}

static val<int32_t> benchSumLoop(val<int32_t> upperLimit) {
	val<int32_t> sum = val<int32_t>(0);
	for (val<int32_t> i = val<int32_t>(0); i < upperLimit; i = i + val<int32_t>(1)) {
		sum = sum + i;
	}
	return sum;
}

TEST_CASE("Tiered Compilation Latency Benchmark") {
	// Measures compilation latency: tiered (tier 0 only) vs single-tier backends.

	using BenchFunc = std::pair<std::string, std::function<void(NautilusModule&)>>;
	std::vector<BenchFunc> testFuncs = {
	    {"addOne",
	     [](NautilusModule& m) {
		     m.registerFunction<val<int32_t>(val<int32_t>)>("f", benchAddOne);
	     }},
	    {"sumLoop",
	     [](NautilusModule& m) { m.registerFunction<val<int32_t>(val<int32_t>)>("f", benchSumLoop); }},
	};

	for (auto& [name, registerFn] : testFuncs) {
#ifdef ENABLE_BC_BACKEND
		Catch::Benchmark::Benchmark("tiered_compile_" + name)
		    .operator=([&registerFn](Catch::Benchmark::Chronometer meter) {
			    meter.measure([&registerFn] {
				    auto engine = NautilusEngine();
				    auto module = engine.createModule();
				    registerFn(module);
				    TieredCompilationConfig config;
				    config.tier0.backend = "bc";
				    config.tier1.backend = "bc";
				    config.tier1.threshold = 0; // disabled
				    return module.compileTiered(config);
			    });
		    });
#endif

		std::vector<std::string> backends;
#ifdef ENABLE_MLIR_BACKEND
		backends.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
		backends.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
		backends.emplace_back("bc");
#endif

		for (auto& backend : backends) {
			Catch::Benchmark::Benchmark("single_compile_" + backend + "_" + name)
			    .operator=([&registerFn, &backend](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&registerFn, &backend] {
					    Options opts;
					    opts.setOption("engine.backend", backend);
					    auto engine = NautilusEngine(opts);
					    auto module = engine.createModule();
					    registerFn(module);
					    return module.compile();
				    });
			    });
		}
	}
}

TEST_CASE("Tiered Execution Throughput Benchmark") {
	// Measures execution speed under different backends.

	std::vector<std::string> backends;
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	backends.emplace_back("cpp");
#endif

	for (auto& backend : backends) {
		Catch::Benchmark::Benchmark("exec_" + backend + "_addOne")
		    .operator=([&backend](Catch::Benchmark::Chronometer meter) {
			    Options opts;
			    opts.setOption("engine.backend", backend);
			    auto engine = NautilusEngine(opts);
			    auto module = engine.createModule();
			    module.registerFunction<val<int32_t>(val<int32_t>)>("f", benchAddOne);
			    auto compiled = module.compile();
			    auto fn = compiled.getFunction<int32_t(int32_t)>("f");
			    meter.measure([&fn] { return fn(42); });
		    });
	}

	// Interpreted baseline
	Catch::Benchmark::Benchmark("exec_interpreted_addOne")
	    .operator=([](Catch::Benchmark::Chronometer meter) {
		    Options opts;
		    opts.setOption("engine.Compilation", false);
		    auto engine = NautilusEngine(opts);
		    auto module = engine.createModule();
		    module.registerFunction<val<int32_t>(val<int32_t>)>("f", benchAddOne);
		    auto compiled = module.compile();
		    auto fn = compiled.getFunction<int32_t(int32_t)>("f");
		    meter.measure([&fn] { return fn(42); });
	    });
}

TEST_CASE("Tiered Promotion Overhead Benchmark") {
	// Measures wall-clock time from threshold hit to tier 1 active.

#if defined(ENABLE_BC_BACKEND) && defined(ENABLE_MLIR_BACKEND)
	auto thresholds = std::vector<int> {10, 100, 1000};

	for (auto threshold : thresholds) {
		Catch::Benchmark::Benchmark("promotion_overhead_threshold_" + std::to_string(threshold))
		    .operator=([threshold](Catch::Benchmark::Chronometer meter) {
			    meter.measure([threshold] {
				    auto engine = NautilusEngine();
				    auto module = engine.createModule();
				    module.registerFunction<val<int32_t>(val<int32_t>)>("f", benchAddOne);

				    TieredCompilationConfig config;
				    config.tier0.backend = "bc";
				    config.tier1.backend = "mlir";
				    config.tier1.threshold = threshold;

				    auto compiled = module.compileTiered(config);
				    auto fn = compiled.getFunction<int32_t(int32_t)>("f");

				    for (int i = 0; i <= threshold; ++i) {
					    fn(i);
				    }

				    compiled.waitForPromotion();
			    });
		    });
	}
#else
	SKIP("Need bc and mlir backends for promotion benchmark");
#endif
}

TEST_CASE("Tiered End-to-End Benchmark") {
	// Measures total time from cold start to peak performance.

#if defined(ENABLE_BC_BACKEND) && defined(ENABLE_MLIR_BACKEND)
	constexpr int THRESHOLD = 100;

	Catch::Benchmark::Benchmark("e2e_tiered_bc_to_mlir")
	    .operator=([](Catch::Benchmark::Chronometer meter) {
		    meter.measure([] {
			    auto engine = NautilusEngine();
			    auto module = engine.createModule();
			    module.registerFunction<val<int32_t>(val<int32_t>)>("f", benchAddOne);

			    TieredCompilationConfig config;
			    config.tier0.backend = "bc";
			    config.tier1.backend = "mlir";
			    config.tier1.threshold = THRESHOLD;

			    auto compiled = module.compileTiered(config);
			    auto fn = compiled.getFunction<int32_t(int32_t)>("f");

			    for (int i = 0; i <= THRESHOLD; ++i) {
				    fn(i);
			    }

			    compiled.waitForPromotion();
			    return fn(42);
		    });
	    });

	Catch::Benchmark::Benchmark("e2e_single_mlir")
	    .operator=([](Catch::Benchmark::Chronometer meter) {
		    meter.measure([] {
			    Options opts;
			    opts.setOption("engine.backend", "mlir");
			    auto engine = NautilusEngine(opts);
			    auto module = engine.createModule();
			    module.registerFunction<val<int32_t>(val<int32_t>)>("f", benchAddOne);
			    auto compiled = module.compile();
			    auto fn = compiled.getFunction<int32_t(int32_t)>("f");
			    return fn(42);
		    });
	    });
#else
	SKIP("Need bc and mlir backends for end-to-end benchmark");
#endif
}

} // namespace nautilus::engine
