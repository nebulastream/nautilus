
#include "BoolOperations.hpp"
#include "CastFunctions.hpp"
#include "ControlFlowFunctions.hpp"
#include "EnumFunction.hpp"
#include "ExpressionFunctions.hpp"
#include "LoopFunctions.hpp"
#include "PointerFunctions.hpp"
#include "RunctimeCallFunctions.hpp"
#include "StaticLoopFunctions.hpp"
#include "TracingUtil.hpp"
#include "nautilus/Engine.hpp"
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
};

TEST_CASE("Tracing Benchmark") {

	for (auto& test : tests) {
		auto func = std::get<1>(test);
		auto name = std::get<0>(test);
		Catch::Benchmark::Benchmark("trace_" + name).operator=([&func](Catch::Benchmark::Chronometer meter) { meter.measure([&func] { return tracing::TraceContext::trace(func); }); });
	}
}

TEST_CASE("SSA Creation Benchmark") {

	for (auto& test : tests) {
		auto func = std::get<1>(test);
		auto name = std::get<0>(test);

		Catch::Benchmark::Benchmark("ssa_" + name).operator=([&func](Catch::Benchmark::Chronometer meter) {
			std::shared_ptr<tracing::ExecutionTrace> trace = tracing::TraceContext::trace(func);
			meter.measure([&] {
				auto ssaCreationPhase = tracing::SSACreationPhase();
				return ssaCreationPhase.apply(trace);
			});
		});
	}
}

TEST_CASE("IR Creation Benchmark") {

	for (auto& test : tests) {
		auto func = std::get<1>(test);
		auto name = std::get<0>(test);

		Catch::Benchmark::Benchmark("ir_" + name).operator=([&func](Catch::Benchmark::Chronometer meter) {
			std::shared_ptr<tracing::ExecutionTrace> trace = tracing::TraceContext::trace(func);
			auto ssaCreationPhase = tracing::SSACreationPhase();
			trace = ssaCreationPhase.apply(trace);

			meter.measure([&] {
				auto irConversionPhase = tracing::TraceToIRConversionPhase();
				return irConversionPhase.apply(trace);
			});
		});
	}
}

} // namespace nautilus::engine
