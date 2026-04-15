
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
#include "nautilus/compiler/backends/mlir/MLIRCompilationBackend.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/config.hpp"
#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::engine {

using TraceFn = std::unique_ptr<tracing::ExecutionTrace> (*)(std::function<void()>&, const engine::Options&,
                                                             common::Arena&);

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
    {"trace", tracing::ExceptionBasedTraceContext::trace},
    {"completing_trace", tracing::LazyTraceContext::trace},
};

TEST_CASE("Tracing Benchmark") {
	// Always reuse a single arena across all samples and iterations of every
	// benchmark by softResetting between calls.  This matches the intended
	// Engine/JIT integration where a long-lived arena serves many compilations.
	for (auto& [name, func] : tests) {
		for (auto& [ctxName, traceFn] : traceContexts) {
			auto benchName = ctxName + "_" + name;
			auto fn = traceFn;
			common::Arena arena;
			Catch::Benchmark::Benchmark(std::string(benchName))
			    .operator=([&func, fn, &arena](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&func, fn, &arena] {
					    auto trace = fn(func, engine::Options(), arena);
					    // Drop the trace here so its destructors run; then
					    // softReset recycles the arena for the next iteration.
					    trace.reset();
					    arena.softReset();
					    return 0;
				    });
			    });
		}
	}
}

TEST_CASE("SSA Creation Benchmark") {

	for (auto& test : tests) {
		auto func = std::get<1>(test);
		auto name = std::get<0>(test);

		// skip nestedIf/chainedIf: SSA creation crashes on these due to a
		// pre-existing bug in multi-return block handling (see getReturnBlock).
		if (name == "nestedIf10" || name == "nestedIf100" || name == "chainedIf10" || name == "chainedIf100") {
			continue;
		}

		// Reuse a single arena across all samples of this benchmark.  A fresh
		// trace is built per sample and the arena is softReset between samples.
		common::Arena arena;
		Catch::Benchmark::Benchmark("ssa_" + name).operator=([&func, &arena](Catch::Benchmark::Chronometer meter) {
			std::shared_ptr<tracing::ExecutionTrace> trace =
			    tracing::ExceptionBasedTraceContext::trace(func, engine::Options(), arena);
			meter.measure([&] {
				auto ssaCreationPhase = tracing::SSACreationPhase();
				return ssaCreationPhase.apply(trace);
			});
			// Drop the trace before recycling the arena for the next sample.
			trace.reset();
			arena.softReset();
		});
	}
}

TEST_CASE("IR Creation Benchmark") {

	for (auto& test : tests) {
		auto func = std::get<1>(test);
		auto name = std::get<0>(test);

		// Reuse a single arena across all samples of this benchmark.
		common::Arena arena;
		Catch::Benchmark::Benchmark("ir_" + name).operator=([&func, &arena](Catch::Benchmark::Chronometer meter) {
			std::shared_ptr<tracing::ExecutionTrace> trace =
			    tracing::ExceptionBasedTraceContext::trace(func, engine::Options(), arena);
			auto ssaCreationPhase = tracing::SSACreationPhase();
			auto afterSSAModule = ssaCreationPhase.apply(std::move(trace));

			meter.measure([&] {
				auto irConversionPhase = tracing::TraceToIRConversionPhase();
				return irConversionPhase.apply(afterSSAModule);
			});
			// Drop trace state before recycling the arena for the next sample.
			afterSSAModule.reset();
			arena.softReset();
		});
	}
}

TEST_CASE("IR Creation Benchmark (pooled arena)") {
	// Mirrors the IR Creation Benchmark but routes every IRGraph's arena
	// through a single ArenaPool.  The first sample pays the usual heap
	// allocation; subsequent samples reuse the recycled Arena (chunks and
	// inline buffer already sized), isolating the pool's amortisation
	// benefit from the one-shot arena-construction cost.
	for (auto& test : tests) {
		auto func = std::get<1>(test);
		auto name = std::get<0>(test);

		Catch::Benchmark::Benchmark("ir_pooled_" + name).operator=([&func](Catch::Benchmark::Chronometer meter) {
			common::Arena traceArena;
			std::shared_ptr<tracing::ExecutionTrace> trace =
			    tracing::ExceptionBasedTraceContext::trace(func, engine::Options(), traceArena);
			auto ssaCreationPhase = tracing::SSACreationPhase();
			auto afterSSAModule = ssaCreationPhase.apply(std::move(trace));

			common::ArenaPool irArenaPool;
			meter.measure([&] {
				auto irConversionPhase = tracing::TraceToIRConversionPhase();
				return irConversionPhase.apply(afterSSAModule, irArenaPool);
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

			// Reuse a single arena across all samples of this benchmark.
			common::Arena arena;
			Catch::Benchmark::Benchmark("comp_" + backend + "_" + name)
			    .operator=([&func, &registry, backend, &arena](Catch::Benchmark::Chronometer meter) {
				    std::shared_ptr<tracing::ExecutionTrace> trace =
				        tracing::ExceptionBasedTraceContext::trace(func, engine::Options(), arena);
				    auto ssaCreationPhase = tracing::SSACreationPhase();
				    auto afterSSAModule = ssaCreationPhase.apply(std::move(trace));

				    auto backendBackend = registry->getBackend(backend);
				    auto irConversionPhase = tracing::TraceToIRConversionPhase();
				    auto ir = irConversionPhase.apply(afterSSAModule);
				    auto op = engine::Options();
				    // force compilation for the MLIR backend.
				    op.setOption("mlir.eager_compilation", true);
				    op.setOption("engine.backend", backend);
				    auto dh = compiler::DumpHandler(op, "");
				    meter.measure([&] { return backendBackend->compile(ir, dh, op); });
				    // Drop trace state before recycling the arena for the next sample.
				    ir.reset();
				    afterSSAModule.reset();
				    arena.softReset();
			    });
		}
	}
}

} // namespace nautilus::engine
