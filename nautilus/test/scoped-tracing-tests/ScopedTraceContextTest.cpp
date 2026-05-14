
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

// Path-explosion fixtures: ScopedTraceContext combines the PathPredicateStore
// (dominator-based pruning) with a per-iteration constant tracker that
// statically folds CMPs whose operands both reduce to known integer literals.
// Together these prune dead arms in cases where Lazy traces every alternative
// once.  Per-function sub-traces (the remaining lever for the *baseline*
// threeCallsNoBranch case, where the leaf's body is still inlined) are still
// a follow-up.
TEST_CASE("ScopedTraceContext: pathExplosion_baseline_oneCall trace matches Lazy", "[scoped-tracing][path-explosion]") {
	auto wrapper = details::createFunctionWrapper(pathExplosion_baseline_oneCall);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	// Single inlined leaf call -> 3 RETURNs (one per leaf branch); the
	// constant tracker has nothing downstream to prune.
	REQUIRE(scoped.returnOps == lazy.returnOps);
	REQUIRE(scoped.returnOps == 3);
}

TEST_CASE("ScopedTraceContext: pathExplosion_baseline_threeCallsNoBranch shrinks 27 -> 9",
          "[scoped-tracing][path-explosion]") {
	auto wrapper = details::createFunctionWrapper(pathExplosion_baseline_threeCallsNoBranch);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	// Lazy inlines three times -> 3^3 = 27 RETURN paths.  ScopedTraceContext
	// folds the CONST returns from the first two leaf calls into the second /
	// third calls' internal CMPs, so the third call's `v == 1` and `v < 10`
	// become static for two of the upstream paths.  Result: 9 RETURNs vs 27.
	REQUIRE(lazy.returnOps == 27);
	REQUIRE(scoped.returnOps == 9);
}

TEST_CASE("ScopedTraceContext: pathExplosion_postCallBranch_1 shrinks 6 -> 4", "[scoped-tracing][path-explosion]") {
	auto wrapper = details::createFunctionWrapper(pathExplosion_postCallBranch_1);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	// Lazy: 3 leaf returns x 2 post-call arms = 6 RETURNs.  Scoped folds
	// `1 == 42` and `42 == 42` statically, so only the symbolic
	// `v+v+1 == 42` path explores both arms.  Result: 4 RETURNs.
	REQUIRE(lazy.returnOps == 6);
	REQUIRE(scoped.returnOps == 4);
}

TEST_CASE("ScopedTraceContext: pathExplosion_postCallBranch_3 shrinks 54 -> 12", "[scoped-tracing][path-explosion]") {
	auto wrapper = details::createFunctionWrapper(pathExplosion_postCallBranch_3);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	// Lazy: ~54 RETURNs from 3^3 x 2 arms.  Scoped: the constant-folding
	// cascade through three chained leaf calls collapses most of the dead
	// arms.  Final RETURN count: 12 (4.5x reduction).
	REQUIRE(lazy.returnOps == 54);
	REQUIRE(scoped.returnOps == 12);
}

TEST_CASE("ScopedTraceContext: pathExplosion_independentIfs_4 stays linear", "[scoped-tracing][path-explosion]") {
	auto wrapper = details::createFunctionWrapper(pathExplosion_independentIfs_4);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	REQUIRE(scoped.returnOps == lazy.returnOps);
	REQUIRE(scoped.cmpOps == lazy.cmpOps);
}

TEST_CASE("ScopedTraceContext: pathExplosion_constraintBlind_dead prunes dead arms via PathPredicateStore",
          "[scoped-tracing][path-explosion]") {
	auto wrapper = details::createFunctionWrapper(pathExplosion_constraintBlind_dead);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	// Lazy records 4 RETURNs because it explores both arms of every CMP,
	// including the inner `if (x == 1)` arms that the outer `if (x == 1)`
	// already constrains.  ScopedTraceContext consults PathPredicateStore at
	// each CMP and asks SymbolicExecutionContext to skip the dead arm via
	// recordPrunedNoThrow, so only the two logically-reachable paths
	// (outer-true + inner-true-by-implication, outer-false + inner-false-by-
	// implication) reach a RETURN.
	REQUIRE(lazy.returnOps == 4);
	REQUIRE(scoped.returnOps == 2);
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

TEST_CASE("ScopedTraceContext: redundant dominator CMP is pruned via PathPredicateStore",
          "[scoped-tracing][predicate-pruning]") {
	// Minimal custom fixture: `if (x == 1) { if (x == 1) return 1; } return 0`
	// has only two reachable arms but Lazy explores three RETURN sites because
	// the inner `x == 1` is constraint-blind to the outer.  ScopedTraceContext
	// should prune the inner false arm.
	auto redundant = [](val<int32_t> x) -> val<int32_t> {
		if (x == 1) {
			if (x == 1) {
				return val<int32_t>(1);
			}
		}
		return val<int32_t>(0);
	};
	auto wrapper = details::createFunctionWrapper(redundant);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	REQUIRE(scoped.returnOps < lazy.returnOps);
}

TEST_CASE("ScopedTraceContext: contradicted dominator CMP is pruned via PathPredicateStore",
          "[scoped-tracing][predicate-pruning]") {
	// `if (x == 5) { if (x != 5) return -1; } return 0` — the inner CMP is
	// contradicted by the outer.  Scoped should never reach the inner true
	// arm.
	auto contradicted = [](val<int32_t> x) -> val<int32_t> {
		if (x == 5) {
			if (x != 5) {
				return val<int32_t>(-1);
			}
		}
		return val<int32_t>(0);
	};
	auto wrapper = details::createFunctionWrapper(contradicted);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	REQUIRE(scoped.returnOps < lazy.returnOps);
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
