
#include "BoolOperations.hpp"
#include "CastFunctions.hpp"
#include "ControlFlowFunctions.hpp"
#include "EnumFunction.hpp"
#include "ExpressionFunctions.hpp"
#include "LoopFunctions.hpp"
#include "NestedIfBenchmarks.hpp"
#include "PathExplosionFunctions.hpp"
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
#include "nautilus/tracing/ScopedTraceContext.hpp"
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
    {"scoped_trace", tracing::ScopedTraceContext::trace},
};

// Path-explosion fixtures benchmarked separately: these are the shapes where
// ScopedTraceContext's predicate-store + constant-folding pruning is expected
// to outperform Lazy / Exception (see the PR description tables).
//
// Benchmark names MUST stay short.  Catch2 wraps long benchmark names onto a
// continuation line which breaks github-action-benchmark's regex parser
// ("No benchmark found for bench suite" failure).  The longest combined name
// (`completing_trace_<key>`) must fit on one row, so each key here is <=11
// chars.  Cross-reference: `pe3` => baseline_threeCallsNoBranch, etc.
static auto pathExplosionTests = std::vector<std::tuple<std::string, std::function<void()>>> {
    {"peOne", details::createFunctionWrapper(pathExplosion_baseline_oneCall)},
    {"pe3Calls", details::createFunctionWrapper(pathExplosion_baseline_threeCallsNoBranch)},
    {"peIndIfs", details::createFunctionWrapper(pathExplosion_independentIfs_4)},
    {"pePost1", details::createFunctionWrapper(pathExplosion_postCallBranch_1)},
    {"pePost3", details::createFunctionWrapper(pathExplosion_postCallBranch_3)},
    {"peBlind", details::createFunctionWrapper(pathExplosion_constraintBlind_dead)},
};

TEST_CASE("Tracing Benchmark") {
	// Route every sample's arena through a single ArenaPool so chunk memory
	// is recycled across samples (and across the many iterations within each
	// sample).  This matches the intended Engine/JIT integration where a
	// long-lived ArenaPool serves many compilations.
	for (auto& [name, func] : tests) {
		for (auto& [ctxName, traceFn] : traceContexts) {
			auto benchName = ctxName + "_" + name;
			auto fn = traceFn;
			common::ArenaPool pool;
			Catch::Benchmark::Benchmark(std::string(benchName))
			    .operator=([&func, fn, &pool](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&func, fn, &pool] {
					    auto arena = pool.acquire();
					    auto trace = fn(func, engine::Options(), *arena);
					    // Drop the trace first; then the arena handle goes
					    // out of scope and is recycled into the pool.
					    trace.reset();
					    return 0;
				    });
			    });
		}
	}
}

TEST_CASE("Path Explosion Tracing Benchmark") {
	// Runs every path-explosion fixture against all three tracers so the
	// scoped tracer's predicate-store + constant-folding wins (documented in
	// PR #288: 27 -> 9 on threeCallsNoBranch, 54 -> 12 on postCallBranch_3,
	// 4 -> 2 on constraintBlind_dead) show up as concrete wall-time numbers.
	for (auto& [name, func] : pathExplosionTests) {
		for (auto& [ctxName, traceFn] : traceContexts) {
			// Short combined name (`scoped_trace_pe_3calls`) so the line stays
			// on one row in Catch2's output — long names wrap and break the
			// github-action-benchmark regex.
			auto benchName = ctxName + "_" + name;
			auto fn = traceFn;
			common::ArenaPool pool;
			Catch::Benchmark::Benchmark(std::string(benchName))
			    .operator=([&func, fn, &pool](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&func, fn, &pool] {
					    auto arena = pool.acquire();
					    auto trace = fn(func, engine::Options(), *arena);
					    trace.reset();
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

		// Route the per-sample trace arena through a single ArenaPool so
		// chunk memory is recycled across samples.
		common::ArenaPool pool;
		Catch::Benchmark::Benchmark("ssa_" + name).operator=([&func, &pool](Catch::Benchmark::Chronometer meter) {
			auto arena = pool.acquire();
			std::shared_ptr<tracing::ExecutionTrace> trace =
			    tracing::ExceptionBasedTraceContext::trace(func, engine::Options(), *arena);
			meter.measure([&] {
				auto ssaCreationPhase = tracing::SSACreationPhase();
				return ssaCreationPhase.apply(trace);
			});
			// Drop the trace before the arena handle is recycled into the
			// pool at the end of this sample.
			trace.reset();
		});
	}
}

TEST_CASE("IR Creation Benchmark") {
	// Route both the trace arena and every IRGraph's arena through a single
	// ArenaPool.  The first sample pays the usual heap allocation; subsequent
	// samples (and iterations within a sample) recycle Arenas whose chunks
	// and inline buffers are already sized for the workload, matching the
	// amortisation pattern the engine relies on in production.
	for (auto& test : tests) {
		auto func = std::get<1>(test);
		auto name = std::get<0>(test);

		common::ArenaPool pool;
		Catch::Benchmark::Benchmark("ir_" + name).operator=([&func, &pool](Catch::Benchmark::Chronometer meter) {
			auto traceArena = pool.acquire();
			std::shared_ptr<tracing::ExecutionTrace> trace =
			    tracing::ExceptionBasedTraceContext::trace(func, engine::Options(), *traceArena);
			auto ssaCreationPhase = tracing::SSACreationPhase();
			auto afterSSAModule = ssaCreationPhase.apply(std::move(trace));

			meter.measure([&] {
				auto irConversionPhase = tracing::TraceToIRConversionPhase();
				return irConversionPhase.apply(afterSSAModule, pool);
			});
			// Drop trace state before the trace-arena handle is recycled
			// into the pool at the end of this sample.
			afterSSAModule.reset();
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

			// Route both the trace arena and every IRGraph's arena through a
			// single ArenaPool so chunk memory is recycled across samples.
			common::ArenaPool pool;
			Catch::Benchmark::Benchmark("comp_" + backend + "_" + name)
			    .operator=([&func, &registry, backend, &pool](Catch::Benchmark::Chronometer meter) {
				    auto traceArena = pool.acquire();
				    std::shared_ptr<tracing::ExecutionTrace> trace =
				        tracing::ExceptionBasedTraceContext::trace(func, engine::Options(), *traceArena);
				    auto ssaCreationPhase = tracing::SSACreationPhase();
				    auto afterSSAModule = ssaCreationPhase.apply(std::move(trace));

				    auto backendBackend = registry->getBackend(backend);
				    auto irConversionPhase = tracing::TraceToIRConversionPhase();
				    auto ir = irConversionPhase.apply(afterSSAModule, pool);
				    auto op = engine::Options();
				    // force compilation for the MLIR backend.
				    op.setOption("mlir.eager_compilation", true);
				    op.setOption("engine.backend", backend);
				    auto dh = compiler::DumpHandler(op, "");
				    meter.measure([&] { return backendBackend->compile(ir, dh, op); });
				    // Drop trace/IR state before the trace-arena handle is
				    // recycled into the pool at the end of this sample.
				    ir.reset();
				    afterSSAModule.reset();
			    });
		}
	}
}

} // namespace nautilus::engine
