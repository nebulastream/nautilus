
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

val<int8_t> int8AddExpression(val<int8_t> x);

void runAddBenchmark(Catch::Benchmark::Chronometer& meter, Options& options) {
	auto engine = engine::NautilusEngine(options);
	auto func = engine.registerFunction(int8AddExpression);
	meter.measure([&] { return func(42); });
}

static auto benchmarks = std::vector<std::tuple<std::string, std::function<void(Catch::Benchmark::Chronometer& meter, Options& options)>>> {
    {"add", runAddBenchmark},
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
