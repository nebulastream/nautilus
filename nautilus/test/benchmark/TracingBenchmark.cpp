
#include "BoolOperations.hpp"
#include "CastFunctions.hpp"
#include "ControlFlowFunctions.hpp"
#include "EnumFunction.hpp"
#include "ExpressionFunctions.hpp"
#include "LoopFunctions.hpp"
#include "NestedIfBenchmarks.hpp"
#include "PointerFunctions.hpp"
#include "RunctimeCallFunctions.hpp"
#include "StaticLoopFunctions.hpp"
#include "TracingUtil.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/compiler/CompilableFunction.hpp"
#include "nautilus/compiler/backends/mlir/MLIRCompilationBackend.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/config.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

static auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
    {"add", details::createFunctionWrapper(int8AddExpression)},
    {"ifThenElse", details::createFunctionWrapper(ifThenCondition)},
    {"deeplyNestedIfElse", details::createFunctionWrapper(deeplyNestedIfElseIfCondition)},
    {"loop", details::createFunctionWrapper(sumLoop)},
    {"ifInsideLoop", details::createFunctionWrapper(ifInsideLoop)},
    {"loopDirectCall", details::createFunctionWrapper(loopDirectCall)},
    {"pointerLoop", details::createFunctionWrapper(sumArray)},
    {"staticLoop", details::createFunctionWrapper(staticLoop)},
    {"fibonacci", details::createFunctionWrapper(fibonacci)},
    {"gcd", details::createFunctionWrapper(gcd)},
    {"nestedIf10", details::createFunctionWrapper(nestedIf10)},
    {"nestedIf100", details::createFunctionWrapper(nestedIf100)},
    {"chainedIf10", details::createFunctionWrapper(chainedIf10)},
    {"chainedIf100", details::createFunctionWrapper(chainedIf100)},
};

TEST_CASE("Tracing Benchmark") {

	for (auto& test : tests) {
		auto func = std::get<1>(test);
		auto name = std::get<0>(test);
		Catch::Benchmark::Benchmark("trace_" + name).operator=([&func](Catch::Benchmark::Chronometer meter) {
			meter.measure([&func] {
				// Create a CompilableFunction for the root "execute" function
				auto rootFunction = compiler::CompilableFunction("execute", func);
				std::list<compiler::CompilableFunction> functionsToTrace;
				functionsToTrace.push_back(rootFunction);

				// Trace all functions (initially just "execute", but may include nested functions)
				auto options = engine::Options();
				auto traceModule = tracing::TraceContext::get()->startTrace(functionsToTrace, options);

				return traceModule;
			});
		});
	}
}

TEST_CASE("SSA Creation Benchmark") {

	for (auto& test : tests) {
		auto func = std::get<1>(test);
		auto name = std::get<0>(test);

		Catch::Benchmark::Benchmark("ssa_" + name).operator=([&func](Catch::Benchmark::Chronometer meter) {
			std::vector<std::shared_ptr<tracing::TraceModule>> traces;
			traces.reserve(static_cast<size_t>(meter.runs()));
			for (int i = 0; i < meter.runs(); ++i) {
				auto rootFunction = compiler::CompilableFunction("execute", func);
				std::list<compiler::CompilableFunction> functionsToTrace;
				functionsToTrace.push_back(rootFunction);

				auto options = engine::Options();
				traces.emplace_back(tracing::TraceContext::get()->startTrace(functionsToTrace, options));
			}

			auto ssaCreationPhase = tracing::SSACreationPhase();
			meter.measure([&](int i) { return ssaCreationPhase.apply(traces[static_cast<size_t>(i)]); });
		});
	}
}

TEST_CASE("IR Creation Benchmark") {

	for (auto& test : tests) {
		auto func = std::get<1>(test);
		auto name = std::get<0>(test);

		Catch::Benchmark::Benchmark("ir_" + name).operator=([&func](Catch::Benchmark::Chronometer meter) {
			// Create a CompilableFunction for the root "execute" function
			auto rootFunction = compiler::CompilableFunction("execute", func);
			std::list<compiler::CompilableFunction> functionsToTrace;
			functionsToTrace.push_back(rootFunction);

			// Trace all functions (initially just "execute", but may include nested functions)
			auto options = engine::Options();
			auto traceModule = tracing::TraceContext::get()->startTrace(functionsToTrace, options);

			auto ssaCreationPhase = tracing::SSACreationPhase();
			auto afterSSAModule = ssaCreationPhase.apply(std::move(traceModule));

			meter.measure([&] {
				auto irConversionPhase = tracing::TraceToIRConversionPhase();
				return irConversionPhase.apply(afterSSAModule);
			});
		});
	}
}

TEST_CASE("Backend Compilation Benchmark") {

	auto registry = compiler::CompilationBackendRegistry::getInstance();

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
		for (auto& test : tests) {
			auto func = std::get<1>(test);
			auto name = std::get<0>(test);

			Catch::Benchmark::Benchmark("comp_" + backend + "_" + name)
			    .operator=([&func, &registry, backend](Catch::Benchmark::Chronometer meter) {
				    // Create a CompilableFunction for the root "execute" function
				    auto rootFunction = compiler::CompilableFunction("execute", func);
				    std::list<compiler::CompilableFunction> functionsToTrace;
				    functionsToTrace.push_back(rootFunction);

				    // Trace all functions (initially just "execute", but may include nested functions)
				    auto options = engine::Options();
				    auto traceModule = tracing::TraceContext::get()->startTrace(functionsToTrace, options);

				    auto ssaCreationPhase = tracing::SSACreationPhase();
				    auto afterSSAModule = ssaCreationPhase.apply(std::move(traceModule));

				    auto backendBackend = registry->getBackend(backend);
				    auto irConversionPhase = tracing::TraceToIRConversionPhase();
				    auto ir = irConversionPhase.apply(afterSSAModule);
				    auto op = engine::Options();
				    // force compilation for the MLIR backend.
				    op.setOption("mlir.eager_compilation", true);
				    op.setOption("engine.backend", backend);
				    auto dh = compiler::DumpHandler(op, "");
				    meter.measure([&] { return backendBackend->compile(ir, dh, op); });
			    });
		}
	}
}

} // namespace nautilus::engine
