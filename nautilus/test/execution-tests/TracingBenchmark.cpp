
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

val<int8_t> addFunction(val<int8_t> x) {
	val<int8_t> y = (int8_t) 2;
	return y + x;
}

val<int8_t> ifThenElse(val<int8_t> x) {
	if (x > 42) {
		x = x + 2;
	} else {
		x = x + 1;
	}
	return x * 2;
}

val<int8_t> loop(val<int8_t> x) {
	for (val<int8_t> i = 0; i < 1000; i++) {
		x = x + 1;
	}
	return x;
}

static auto tests = std::vector<std::tuple<std::string, std::function<void()>>> {
    {"add", details::createFunctionWrapper(addFunction)},
    {"ifThenElse", details::createFunctionWrapper(ifThenElse)},
    {"loop", details::createFunctionWrapper(loop)},
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
