
#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/ReplayTraceContext.hpp"
#include "nautilus/tracing/exceptions/TraceDivergenceException.hpp"
#include "nautilus/tracing/exceptions/TraceIncompleteException.hpp"
#include "nautilus/tracing/exceptions/TraceLimitException.hpp"
#include <catch2/catch_all.hpp>
#include <string>

#ifdef ENABLE_TRACING

namespace nautilus::engine {

namespace {

size_t countOccurrences(const std::string& haystack, const std::string& needle) {
	size_t count = 0;
	for (size_t pos = haystack.find(needle); pos != std::string::npos; pos = haystack.find(needle, pos + 1)) {
		count++;
	}
	return count;
}

std::string traceWithReplayContext(std::function<void()> wrapper, const engine::Options& options = {}) {
	common::Arena arena;
	auto trace = tracing::ReplayTraceContext::trace(wrapper, options, arena);
	return trace->toString();
}

std::string traceWithLazyContext(std::function<void()> wrapper, const engine::Options& options = {}) {
	common::Arena arena;
	auto trace = tracing::LazyTraceContext::trace(wrapper, options, arena);
	return trace->toString();
}

// --- traced functions under test ---

val<int32_t> prunedTrueBranch(val<int32_t> x) {
	val<int32_t> c = 42;
	if (c == 42) {
		return x + 1;
	}
	return x + 2;
}

val<int32_t> prunedFalseBranch(val<int32_t> x) {
	val<int32_t> c = 42;
	if (c == 43) {
		return x + 1;
	}
	return x + 2;
}

val<int32_t> runtimeBranch(val<int32_t> x) {
	val<int32_t> r = 1;
	if (x == 42) {
		r = r + 1;
	}
	return r + x;
}

val<int32_t> sumLoop(val<int32_t> n) {
	val<int32_t> agg = 1;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		agg = agg + 10;
	}
	return agg;
}

val<int32_t> manyBranches(val<int32_t> x) {
	val<int32_t> r = 0;
	if (x == 1) {
		r = r + 1;
	}
	if (x == 2) {
		r = r + 2;
	}
	if (x == 3) {
		r = r + 3;
	}
	if (x == 4) {
		r = r + 4;
	}
	return r;
}

val<int32_t> breakOnlyLoop(val<int32_t> x) {
	val<int32_t> i = 0;
	for (;;) {
		if (i >= x) {
			break;
		}
		i = i + 1;
	}
	return i;
}

int nonDeterminismCounter = 0;
val<int32_t> nonDeterministicFunction(val<int32_t> x) {
	nonDeterminismCounter++;
	if (nonDeterminismCounter > 1) {
		// Different operation sequence on re-execution.
		auto y = x * x;
		if (y == 4) {
			return y;
		}
		return x;
	}
	auto y = x + x;
	if (y == 4) {
		return y;
	}
	return x;
}

} // namespace

TEST_CASE("Replay tracing prunes constant-condition branches") {
	SECTION("constant true condition") {
		auto trace = traceWithReplayContext(details::createFunctionWrapper(&prunedTrueBranch));
		// The branch is pruned: no CMP is recorded and the trace is a single
		// straight-line block.
		REQUIRE(countOccurrences(trace, "CMP") == 0);
		REQUIRE(countOccurrences(trace, "B1(") == 0);
		REQUIRE(countOccurrences(trace, "RETURN") == 1);
	}
	SECTION("constant false condition") {
		auto trace = traceWithReplayContext(details::createFunctionWrapper(&prunedFalseBranch));
		REQUIRE(countOccurrences(trace, "CMP") == 0);
		REQUIRE(countOccurrences(trace, "B1(") == 0);
		REQUIRE(countOccurrences(trace, "RETURN") == 1);
	}
	SECTION("runtime condition is not pruned") {
		auto trace = traceWithReplayContext(details::createFunctionWrapper(&runtimeBranch));
		REQUIRE(countOccurrences(trace, "CMP") == 1);
	}
}

TEST_CASE("Replay tracing produces the same trace as lazy tracing for single-branch functions") {
	SECTION("if without else") {
		auto replayTrace = traceWithReplayContext(details::createFunctionWrapper(&runtimeBranch));
		auto lazyTrace = traceWithLazyContext(details::createFunctionWrapper(&runtimeBranch));
		REQUIRE(replayTrace == lazyTrace);
	}
	SECTION("counted loop") {
		auto replayTrace = traceWithReplayContext(details::createFunctionWrapper(&sumLoop));
		auto lazyTrace = traceWithLazyContext(details::createFunctionWrapper(&sumLoop));
		REQUIRE(replayTrace == lazyTrace);
	}
}

TEST_CASE("Replay tracing detects non-deterministic traced functions") {
	nonDeterminismCounter = 0;
	auto wrapper = details::createFunctionWrapper(&nonDeterministicFunction);
	common::Arena arena;
	REQUIRE_THROWS_AS(tracing::ReplayTraceContext::trace(wrapper, engine::Options(), arena),
	                  TraceDivergenceException);
}

TEST_CASE("Replay tracing enforces the exploration iteration limit") {
	engine::Options options;
	options.setOption("engine.traceMaxIterations", 2);
	auto wrapper = details::createFunctionWrapper(&manyBranches);
	common::Arena arena;
	REQUIRE_THROWS_AS(tracing::ReplayTraceContext::trace(wrapper, options, arena), TraceLimitException);
}

TEST_CASE("Replay tracing reports loops that passive mode cannot exit") {
	engine::Options options;
	options.setOption("engine.tracePassiveStepLimit", 1000);
	auto wrapper = details::createFunctionWrapper(&breakOnlyLoop);
	common::Arena arena;
	REQUIRE_THROWS_AS(tracing::ReplayTraceContext::trace(wrapper, options, arena), TraceIncompleteException);
}

TEST_CASE("Replay tracing executes correctly across backends") {
	for (const auto& backend : nautilus::testing::availableBackends()) {
		DYNAMIC_SECTION(backend) {
			auto engine = nautilus::testing::makeEngine(
			    backend, [&](engine::Options& options) { options.setOption("engine.traceMode", "replayTracing"); });
			SECTION("pruned branch") {
				auto f = engine.registerFunction(prunedTrueBranch);
				REQUIRE(f(int32_t(5)) == 6);
				auto g = engine.registerFunction(prunedFalseBranch);
				REQUIRE(g(int32_t(5)) == 7);
			}
			SECTION("runtime branch") {
				auto f = engine.registerFunction(runtimeBranch);
				REQUIRE(f(int32_t(42)) == 44);
				REQUIRE(f(int32_t(1)) == 2);
			}
			SECTION("loop") {
				auto f = engine.registerFunction(sumLoop);
				REQUIRE(f(int32_t(3)) == 31);
				REQUIRE(f(int32_t(0)) == 1);
			}
			SECTION("sequential branches") {
				auto f = engine.registerFunction(manyBranches);
				REQUIRE(f(int32_t(3)) == 3);
				REQUIRE(f(int32_t(7)) == 0);
			}
		}
	}
}


} // namespace nautilus::engine

#endif // ENABLE_TRACING

