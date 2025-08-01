
#include "TracingUtil.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/compiler/backends/mlir/MLIRCompilationBackend.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/config.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

val<int8_t> addExpression(val<int8_t> x) {
	val<int8_t> y = (int8_t) 2;
	return y + x;
}

val<int32_t> fib(val<int32_t> n) {
	val<int32_t> a = 0, b = 1, c;
	for (val<int> i = 2; i <= n; i = i + 1) {
		c = a + b;
		a = b;
		b = c;
	}
	return b;
}
val<int32_t> sum(val<int32_t*> array, val<int32_t> length) {
	val<int32_t> sum = val<int32_t>(0);
	for (val<int32_t> i = 0; i < length; i = i + 1) {
		val<int32_t> value = array[i];
		sum = sum + value;
	}
	return sum;
}

void runAddBenchmark(Catch::Benchmark::Chronometer& meter, Options& options) {
	auto engine = engine::NautilusEngine(options);
	auto func = engine.registerFunction(addExpression);
	meter.measure([&] { return func(42); });
}

void runFibBenchmark(Catch::Benchmark::Chronometer& meter, Options& options) {
	auto engine = engine::NautilusEngine(options);
	auto func = engine.registerFunction(fib);
	meter.measure([&] { return func(15000); });
}

void runArraySumBenchmark(Catch::Benchmark::Chronometer& meter, Options& options) {
	auto engine = engine::NautilusEngine(options);
	auto func = engine.registerFunction(sum);
	auto size = 1024 * 1024 * 10;
	auto buffer = (int32_t*) malloc(size);
	meter.measure([&] { return func(buffer, size / sizeof(int32_t)); });
}

static auto benchmarks =
    std::vector<std::tuple<std::string, std::function<void(Catch::Benchmark::Chronometer& meter, Options& options)>>> {
        {"add", runAddBenchmark},
        {"fibonacci", runFibBenchmark},
        {"sum", runArraySumBenchmark},
    };

TEST_CASE("Execution Benchmark") {

	std::vector<std::string> backends = {};
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	backends.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	backends.emplace_back("asmjit");
#endif
	for (auto& backend : backends) {
		for (auto& test : benchmarks) {
			auto func = std::get<1>(test);
			auto name = std::get<0>(test);

			Catch::Benchmark::Benchmark("exec_" + backend + "_" + name)
			    .operator=([&func, backend](Catch::Benchmark::Chronometer meter) {
				    auto op = engine::Options();
				    // force compilation for the MLIR backend.
				    op.setOption("mlir.eager_compilation", true);
				    op.setOption("engine.backend", backend);
				    func(meter, op);
			    });
		}
	}
}

} // namespace nautilus::engine
