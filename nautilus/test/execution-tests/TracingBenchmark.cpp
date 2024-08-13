
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

std::uint64_t Fibonacci(std::uint64_t number) {
	return number < 2 ? 1 : Fibonacci(number - 1) + Fibonacci(number - 2);
}

val<int8_t> addFunction(val<int8_t> x) {
	val<int8_t> y = (int8_t) 2;
	return y + x;
}

TEST_CASE("Tracing Benchmark") {
	BENCHMARK("Fibonacci 35") {
		auto func = details::createFunctionWrapper(addFunction);
		return tracing::TraceContext::trace(func);
	};
}

} // namespace nautilus::engine
