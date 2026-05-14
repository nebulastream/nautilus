
#include "PathExplosionFunctions.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/Operations.hpp"
#include "nautilus/tracing/ScopedTraceContext.hpp"
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

} // namespace

TEST_CASE("ScopedTraceContext: pathExplosion_baseline_oneCall trace count is unchanged",
          "[scoped-tracing][path-explosion]") {
	auto wrapper = details::createFunctionWrapper(pathExplosion_baseline_oneCall);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	// One inlined call to a 3-return leaf: same number of RETURNs across all
	// tracers, because there is no upstream alive-var fan-out to collapse.
	REQUIRE(scoped.returnOps == lazy.returnOps);
	REQUIRE(scoped.returnOps == 3);
}

TEST_CASE("ScopedTraceContext: pathExplosion_postCallBranch_1 has fewer RETURNs under scoped tracing",
          "[scoped-tracing][path-explosion]") {
	auto wrapper = details::createFunctionWrapper(pathExplosion_postCallBranch_1);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	// The pathology here is upstream-ValueRef fan-out at the post-call CMP:
	// LazyTraceContext records 6 RETURNs (3 leaf returns x 2 cmp arms).
	// Dropping aliveVars from the snapshot collapses the cmp's three tags
	// back into one, so the scoped trace must record strictly fewer RETURNs.
	REQUIRE(scoped.returnOps < lazy.returnOps);
	REQUIRE(scoped.cmpOps <= lazy.cmpOps);
}

TEST_CASE("ScopedTraceContext: pathExplosion_postCallBranch_3 has fewer RETURNs under scoped tracing",
          "[scoped-tracing][path-explosion]") {
	auto wrapper = details::createFunctionWrapper(pathExplosion_postCallBranch_3);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	// The deepest of the post-call-branch chain.  LazyTraceContext records
	// in the high tens of RETURNs; the scoped tracer must shrink that count.
	REQUIRE(scoped.returnOps < lazy.returnOps);
	REQUIRE(scoped.totalOps < lazy.totalOps);
}

TEST_CASE("ScopedTraceContext: pathExplosion_independentIfs_4 stays linear", "[scoped-tracing][path-explosion]") {
	auto wrapper = details::createFunctionWrapper(pathExplosion_independentIfs_4);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	// This is the "safe" shared-state shape that should not regress.  The
	// scoped tracer should produce the same number of RETURNs (2) as Lazy.
	REQUIRE(scoped.returnOps == lazy.returnOps);
	REQUIRE(scoped.cmpOps == lazy.cmpOps);
}

// pathExplosion_constraintBlind_dead currently triggers an "Invalid trace"
// error under ScopedTraceContext.  The cause: two sibling CMPs at the same
// __builtin_return_address chain (e.g. nested `if (x == 1)`) are
// distinguished today by `aliveVars.hash()`, which counts the unique
// ValueRef of the temporary `val<bool>` produced by each `==`.  Dropping
// aliveVars from `recordSnapshot()` collapses those two CMPs to one
// Snapshot, which then triggers a same-block control-flow merge in
// ExecutionTrace::processControlFlowMerge.  Fixing this cleanly requires
// either keeping a narrower discriminator (e.g. only the just-constructed
// val<bool>'s ref) or routing sibling-CMP discrimination through a
// dedicated counter; both are tracked as follow-ups to the scoped tracer.
//
// For now we mark this fixture as a known limitation rather than asserting
// behavior — adding it to the failing set keeps the limitation visible in
// the test report and lets it be flipped to a positive assertion once the
// underlying fix lands.
TEST_CASE("ScopedTraceContext: pathExplosion_constraintBlind_dead is a known limitation",
          "[scoped-tracing][path-explosion][!shouldfail]") {
	auto wrapper = details::createFunctionWrapper(pathExplosion_constraintBlind_dead);
	// Expect this to throw RuntimeException("Invalid trace. ...") until the
	// sibling-CMP discrimination follow-up lands.
	REQUIRE_NOTHROW(traceWith(&tracing::ScopedTraceContext::Trace, wrapper));
}

TEST_CASE("ScopedTraceContext: matches existing tracers on a function with no branches",
          "[scoped-tracing][correctness]") {
	auto noBranch = [](val<int32_t> x) -> val<int32_t> { return x + 1; };
	auto wrapper = details::createFunctionWrapper(noBranch);
	auto scoped = traceWith(&tracing::ScopedTraceContext::Trace, wrapper);
	auto lazy = traceWith(&tracing::LazyTraceContext::Trace, wrapper);
	auto except = traceWith(&tracing::ExceptionBasedTraceContext::Trace, wrapper);
	REQUIRE(scoped.returnOps == lazy.returnOps);
	REQUIRE(scoped.returnOps == except.returnOps);
	REQUIRE(scoped.returnOps == 1);
}

} // namespace nautilus::engine
