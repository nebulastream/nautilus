
#include "PathExplosionFunctions.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/Operations.hpp"
#include "nautilus/tracing/ScopedTraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/SSAVerifier.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include <catch2/catch_all.hpp>
#include <list>
#include <memory>

namespace nautilus::engine {

using TraceFn = std::unique_ptr<tracing::TraceModule> (*)(std::list<compiler::CompilableFunction>&,
                                                          const engine::Options&, common::Arena&);

namespace {

struct OpCounts {
	std::size_t returnOps = 0;
	std::size_t cmpOps = 0;
	std::size_t totalOps = 0;
	std::size_t totalBlocks = 0;
};

OpCounts countOps(const tracing::TraceModule& module) {
	OpCounts counts;
	for (const auto& fnName : module.getFunctionNames()) {
		const auto* trace = module.getFunction(fnName);
		if (trace == nullptr) {
			continue;
		}
		// getBlocks() is non-const for historical reasons; cast away to call it.
		auto& blocks = const_cast<tracing::ExecutionTrace*>(trace)->getBlocks();
		counts.totalBlocks += blocks.size();
		for (auto* block : blocks) {
			for (auto* op : block->operations) {
				++counts.totalOps;
				if (op->op == tracing::RETURN) {
					++counts.returnOps;
				} else if (op->op == tracing::CMP) {
					++counts.cmpOps;
				}
			}
		}
	}
	return counts;
}

template <typename Fn>
OpCounts traceWith(TraceFn traceFn, Fn func) {
	auto rootFunction = compiler::CompilableFunction("execute", func);
	std::list<compiler::CompilableFunction> functionsToTrace;
	functionsToTrace.push_back(rootFunction);
	common::Arena arena;
	auto module = traceFn(functionsToTrace, engine::Options(), arena);
	return countOps(*module);
}

/// Runs the full frontend pipeline (trace -> SSA -> IR) on @p func via @p traceFn
/// and asserts every phase completes without throwing and every SSA invariant
/// holds.  Returns the OpCounts from the raw trace for inspection.
template <typename Fn>
OpCounts traceAndLowerWith(TraceFn traceFn, Fn func) {
	auto rootFunction = compiler::CompilableFunction("execute", func);
	std::list<compiler::CompilableFunction> functionsToTrace;
	functionsToTrace.push_back(rootFunction);
	common::Arena arena;
	auto module = traceFn(functionsToTrace, engine::Options(), arena);
	auto counts = countOps(*module);

	auto sharedModule = std::shared_ptr<tracing::TraceModule>(std::move(module));
	auto ssaPhase = tracing::SSACreationPhase();
	auto afterSSA = ssaPhase.apply(sharedModule);
	REQUIRE(afterSSA != nullptr);
	for (const auto& fnName : afterSSA->getFunctionNames()) {
		auto result = tracing::VerifySSA(*afterSSA->getFunction(fnName));
		if (!result.valid) {
			for (const auto& error : result.errors) {
				FAIL(error);
			}
		}
	}

	auto irPhase = tracing::TraceToIRConversionPhase();
	auto ir = irPhase.apply(std::move(afterSSA));
	REQUIRE(ir != nullptr);

	return counts;
}

} // namespace

// Path-explosion fixtures: with the current ScopedTraceContext Snapshot scheme
// matching Lazy's, the *trace shape* is identical to Lazy.  The added value of
// the scoped tracer over Lazy at this point is the PathPredicateStore it
// maintains.  Once the per-function-sub-trace work lands (tracked in the
// PR description), these assertions will tighten to `scoped.returnOps <
// lazy.returnOps` on the post-call-branch and threeCallsNoBranch fixtures.
TEST_CASE("ScopedTraceContext: pathExplosion_baseline_oneCall trace matches Lazy", "[scoped-tracing][path-explosion]") {
	auto wrapper = details::createFunctionWrapper(pathExplosion_baseline_oneCall);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	REQUIRE(scoped.returnOps == lazy.returnOps);
	REQUIRE(scoped.returnOps == 3);
}

TEST_CASE("ScopedTraceContext: pathExplosion_baseline_threeCallsNoBranch trace matches Lazy",
          "[scoped-tracing][path-explosion]") {
	auto wrapper = details::createFunctionWrapper(pathExplosion_baseline_threeCallsNoBranch);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	REQUIRE(scoped.returnOps == lazy.returnOps);
	REQUIRE(scoped.cmpOps == lazy.cmpOps);
}

TEST_CASE("ScopedTraceContext: pathExplosion_postCallBranch_1 trace matches Lazy", "[scoped-tracing][path-explosion]") {
	auto wrapper = details::createFunctionWrapper(pathExplosion_postCallBranch_1);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	REQUIRE(scoped.returnOps == lazy.returnOps);
	REQUIRE(scoped.cmpOps == lazy.cmpOps);
}

TEST_CASE("ScopedTraceContext: pathExplosion_postCallBranch_3 trace matches Lazy", "[scoped-tracing][path-explosion]") {
	auto wrapper = details::createFunctionWrapper(pathExplosion_postCallBranch_3);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	REQUIRE(scoped.returnOps == lazy.returnOps);
	REQUIRE(scoped.totalOps == lazy.totalOps);
}

TEST_CASE("ScopedTraceContext: pathExplosion_independentIfs_4 stays linear", "[scoped-tracing][path-explosion]") {
	auto wrapper = details::createFunctionWrapper(pathExplosion_independentIfs_4);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	REQUIRE(scoped.returnOps == lazy.returnOps);
	REQUIRE(scoped.cmpOps == lazy.cmpOps);
}

TEST_CASE("ScopedTraceContext: pathExplosion_constraintBlind_dead completes and is no larger than Lazy",
          "[scoped-tracing][path-explosion]") {
	auto wrapper = details::createFunctionWrapper(pathExplosion_constraintBlind_dead);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	// scoped's safeCheckTag may pause one or more iterations in passive mode
	// when the predicate store and the per-iteration comparison cache combine
	// to enter the same-block fallback path; the resulting trace can be
	// strictly smaller than Lazy's but must always contain at least one
	// RETURN and never exceed Lazy.
	REQUIRE(scoped.returnOps >= 1);
	REQUIRE(scoped.returnOps <= lazy.returnOps);
}

TEST_CASE("ScopedTraceContext: full pipeline (trace -> SSA -> IR) is well-formed on every path-explosion fixture",
          "[scoped-tracing][well-formedness]") {
	// Smaller traces are only valuable if they remain well-formed.  Run each
	// path-explosion fixture end-to-end through SSACreationPhase and
	// TraceToIRConversionPhase, with SSA verification, to catch any silently
	// malformed output produced by the tag-stable Snapshot scheme.
	SECTION("baseline_oneCall") {
		traceAndLowerWith(&tracing::ScopedTraceContext::Trace,
		                  details::createFunctionWrapper(pathExplosion_baseline_oneCall));
	}
	SECTION("baseline_threeCallsNoBranch") {
		traceAndLowerWith(&tracing::ScopedTraceContext::Trace,
		                  details::createFunctionWrapper(pathExplosion_baseline_threeCallsNoBranch));
	}
	SECTION("independentIfs_4") {
		traceAndLowerWith(&tracing::ScopedTraceContext::Trace,
		                  details::createFunctionWrapper(pathExplosion_independentIfs_4));
	}
	SECTION("postCallBranch_1") {
		traceAndLowerWith(&tracing::ScopedTraceContext::Trace,
		                  details::createFunctionWrapper(pathExplosion_postCallBranch_1));
	}
	SECTION("postCallBranch_2") {
		traceAndLowerWith(&tracing::ScopedTraceContext::Trace,
		                  details::createFunctionWrapper(pathExplosion_postCallBranch_2));
	}
	SECTION("postCallBranch_3") {
		traceAndLowerWith(&tracing::ScopedTraceContext::Trace,
		                  details::createFunctionWrapper(pathExplosion_postCallBranch_3));
	}
	SECTION("constraintBlind_dead") {
		traceAndLowerWith(&tracing::ScopedTraceContext::Trace,
		                  details::createFunctionWrapper(pathExplosion_constraintBlind_dead));
	}
}

TEST_CASE("ScopedTraceContext: matches existing tracers on a function with no branches",
          "[scoped-tracing][correctness]") {
	auto noBranch = [](val<int32_t> x) -> val<int32_t> {
		return x + 1;
	};
	auto wrapper = details::createFunctionWrapper(noBranch);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	auto except = traceWith(&tracing::ExceptionBasedTraceContext::Trace, wrapper);
	REQUIRE(scoped.returnOps == lazy.returnOps);
	REQUIRE(scoped.returnOps == except.returnOps);
	REQUIRE(scoped.returnOps == 1);
}

} // namespace nautilus::engine
