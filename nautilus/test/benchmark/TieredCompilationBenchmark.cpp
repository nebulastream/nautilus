#include "nautilus/Engine.hpp"
#include "nautilus/compiler/TieredCompiler.hpp"
#include "nautilus/config.hpp"
#include <catch2/catch_all.hpp>

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
	// Measures compilation latency: tiered vs single-tier backends.

	using BenchFunc = std::pair<std::string, std::function<void(NautilusModule&)>>;
	std::vector<BenchFunc> testFuncs = {
	    {"addOne",
	     [](NautilusModule& m) {
		     m.registerFunction<val<int32_t>(val<int32_t>)>("f", benchAddOne);
	     }},
	    {"sumLoop",
	     [](NautilusModule& m) {
		     m.registerFunction<val<int32_t>(val<int32_t>)>("f", benchSumLoop);
	     }},
	};

	for (auto& [name, registerFn] : testFuncs) {
#if defined(ENABLE_BC_BACKEND) && defined(ENABLE_MLIR_BACKEND)
		Catch::Benchmark::Benchmark("tiered_compile_" + name)
		    .operator=([&registerFn](Catch::Benchmark::Chronometer meter) {
			    meter.measure([&registerFn] {
				    TieredCompilationConfig config;
				    config.tier0.backend = "bc";
				    config.tier1.backend = "mlir";
				    auto engine = NautilusEngine(std::make_unique<compiler::TieredJITCompiler>(Options(), config));
				    auto module = engine.createModule();
				    registerFn(module);
				    return module.compile();
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

	Catch::Benchmark::Benchmark("exec_interpreted_addOne").operator=([](Catch::Benchmark::Chronometer meter) {
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

TEST_CASE("Tiered End-to-End Benchmark") {
#if defined(ENABLE_BC_BACKEND) && defined(ENABLE_MLIR_BACKEND)
	Catch::Benchmark::Benchmark("e2e_tiered_bc_to_mlir").operator=([](Catch::Benchmark::Chronometer meter) {
		meter.measure([] {
			TieredCompilationConfig config;
			config.tier0.backend = "bc";
			config.tier1.backend = "mlir";
			auto tieredJit = std::make_unique<compiler::TieredJITCompiler>(Options(), config);
			auto* jit = tieredJit.get();
			auto engine = NautilusEngine(std::move(tieredJit));
			auto module = engine.createModule();
			module.registerFunction<val<int32_t>(val<int32_t>)>("f", benchAddOne);
			auto compiled = module.compile();
			auto fn = compiled.getFunction<int32_t(int32_t)>("f");
			auto result = fn(42);
			jit->waitForPendingPromotions();
			return result;
		});
	});

	Catch::Benchmark::Benchmark("e2e_single_mlir").operator=([](Catch::Benchmark::Chronometer meter) {
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
