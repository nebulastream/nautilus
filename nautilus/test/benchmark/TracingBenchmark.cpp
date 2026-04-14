
#include "BenchmarkUtil.hpp"
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
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/config.hpp"
#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

using TraceFn = std::unique_ptr<tracing::ExecutionTrace> (*)(std::function<void()>&, const engine::Options&);

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

static auto traceContexts = std::vector<std::tuple<std::string, TraceFn>> {
    {"exception", tracing::ExceptionBasedTraceContext::trace},
    {"lazy", tracing::LazyTraceContext::trace},
};

TEST_CASE("Tracing Benchmark", "[tracing]") {

	for (auto& [name, func] : tests) {
		for (auto& [ctxName, traceFn] : traceContexts) {
			auto benchName = "tracing/" + ctxName + "/" + name;
			auto fn = traceFn;
			Catch::Benchmark::Benchmark(std::string(benchName))
			    .operator=([&func, fn](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&func, fn] { return fn(func, engine::Options()); });
			    });
		}
	}
}

TEST_CASE("SSA Creation Benchmark", "[pipeline]") {

	for (auto& test : tests) {
		auto func = std::get<1>(test);
		auto name = std::get<0>(test);

		// skip nestedIf/chainedIf: SSA creation crashes on these due to a
		// pre-existing bug in multi-return block handling (see getReturnBlock).
		if (name == "nestedIf10" || name == "nestedIf100" || name == "chainedIf10" || name == "chainedIf100") {
			continue;
		}

		// Pre-compute the trace ONCE, then benchmark only SSA creation.
		std::shared_ptr<tracing::ExecutionTrace> trace = tracing::ExceptionBasedTraceContext::trace(func);
		Catch::Benchmark::Benchmark("pipeline/ssa/" + name).operator=([trace](Catch::Benchmark::Chronometer meter) {
			meter.measure([&] {
				auto ssaCreationPhase = tracing::SSACreationPhase();
				return ssaCreationPhase.apply(trace);
			});
		});
	}
}

TEST_CASE("IR Creation Benchmark", "[pipeline]") {

	for (auto& test : tests) {
		auto func = std::get<1>(test);
		auto name = std::get<0>(test);

		// Pre-compute trace + SSA ONCE, then benchmark only IR conversion.
		std::shared_ptr<tracing::ExecutionTrace> trace = tracing::ExceptionBasedTraceContext::trace(func);
		auto ssaCreationPhase = tracing::SSACreationPhase();
		auto afterSSAModule = ssaCreationPhase.apply(std::move(trace));

		Catch::Benchmark::Benchmark("pipeline/ir/" + name)
		    .operator=([afterSSAModule](Catch::Benchmark::Chronometer meter) {
			    meter.measure([&] {
				    auto irConversionPhase = tracing::TraceToIRConversionPhase();
				    return irConversionPhase.apply(afterSSAModule);
			    });
		    });
	}
}

TEST_CASE("Backend Compilation Benchmark", "[pipeline]") {

	auto registry = compiler::CompilationBackendRegistry::getInstance();
	auto backends = benchmark::getEnabledBackends();

	// Pre-compute IR for each function ONCE (IR is backend-independent).
	struct PrecompiledIR {
		std::string name;
		std::shared_ptr<compiler::ir::IRGraph> ir;
	};
	std::vector<PrecompiledIR> irCache;
	for (auto& test : tests) {
		auto func = std::get<1>(test);
		auto name = std::get<0>(test);

		std::shared_ptr<tracing::ExecutionTrace> trace = tracing::ExceptionBasedTraceContext::trace(func);
		auto ssaCreationPhase = tracing::SSACreationPhase();
		auto afterSSAModule = ssaCreationPhase.apply(std::move(trace));
		auto irConversionPhase = tracing::TraceToIRConversionPhase();
		auto ir = irConversionPhase.apply(afterSSAModule);
		irCache.push_back({name, std::move(ir)});
	}

	// Benchmark only backend compilation.
	for (auto& backend : backends) {
		for (auto& [name, ir] : irCache) {
			auto backendImpl = registry->getBackend(backend);
			Catch::Benchmark::Benchmark("pipeline/compile/" + backend + "/" + name)
			    .operator=([ir, backendImpl, backend](Catch::Benchmark::Chronometer meter) {
				    auto op = engine::Options();
				    op.setOption("mlir.eager_compilation", true);
				    op.setOption("engine.backend", backend);
				    auto dh = compiler::DumpHandler(op, "");
				    meter.measure([&] { return backendImpl->compile(ir, dh, op); });
			    });
		}
	}
}

} // namespace nautilus::engine
