#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/ir/passes/BlockMergingPass.hpp"
#include "nautilus/compiler/ir/passes/EmptyBlockEliminationPass.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/compiler/ir/passes/IRStatistics.hpp"
#include "nautilus/function.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/region.hpp"
#include "nautilus/static.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace nautilus::engine {

val<int64_t> regionBasic() {
	val<int64_t> sum = 0;
	region([&]() { sum = sum + 42; });
	return sum;
}

val<int64_t> regionStaticUnroll() {
	val<int64_t> sum = 0;
	for (static_val<int32_t> j = 0; j < 3; j++) {
		region([&]() { sum = sum + val<int32_t>(j); });
	}
	return sum;
}

val<int64_t> regionReplayClosedBody(val<int64_t> n) {
	val<int64_t> sum = 0;
	val<int64_t> i = 0;
	while (i < n) {
		region([&]() { sum = sum + 1; });
		i = i + 1;
	}
	return sum;
}

val<int64_t> regionEscapedValue() {
	val<int64_t> out = 0;
	region([&]() {
		val<int64_t> t = 7;
		out = t;
	});
	return out + 1;
}

val<int64_t> regionInternalBranch(val<int64_t> x) {
	val<int64_t> sum = 0;
	region([&]() {
		if (x > 0) {
			sum = sum + 10;
		} else {
			sum = sum - 1;
		}
	});
	return sum;
}

// Three sequential independent branches in one region: exercises multiple
// local exploration passes (a fresh SymbolicExecutionContext per region --
// see docs/region.md) resolving several internal CMPs within a single
// region invocation, and the lazily-created exit block converging all eight
// combinations' tails.
val<int64_t> regionChainedIf(val<int64_t> a, val<int64_t> b, val<int64_t> c) {
	val<int64_t> sum = 0;
	region([&]() {
		if (a > 0) {
			sum = sum + 1;
		} else {
			sum = sum + 100;
		}
		if (b > 0) {
			sum = sum + 2;
		} else {
			sum = sum + 200;
		}
		if (c > 0) {
			sum = sum + 4;
		} else {
			sum = sum + 400;
		}
	});
	return sum;
}

// Each branch escapes a write to a different captured variable: exercises
// that local re-invocation of the region body doesn't corrupt a captured
// val<T>'s trace ref (val<T>::state is const), so each variable ends up with
// the correctly-merged value regardless of which local pass most recently
// wrote to it.
val<int64_t> regionBranchWritesDifferentVars(val<int64_t> x) {
	val<int64_t> a = 0;
	val<int64_t> b = 0;
	region([&]() {
		if (x > 0) {
			a = 10;
		} else {
			b = 20;
		}
	});
	return a + b;
}

// A native loop inside a region: the loop's back-edge is a CMP whose tag,
// on revisit, is detected via ExecutionTrace::checkTag within a single local
// pass (the same mechanism that already handles top-level loops) rather than
// through SymbolicExecutionContext's cross-pass exploration.
val<int64_t> regionInternalLoop(val<int64_t> n) {
	val<int64_t> sum = 0;
	region([&]() {
		val<int64_t> i = 0;
		while (i < n) {
			sum = sum + 1;
			i = i + 1;
		}
	});
	return sum;
}

val<int64_t> regionNested() {
	val<int64_t> sum = 0;
	region([&]() {
		region([&]() { sum = sum + 1; });
		sum = sum + 2;
	});
	return sum;
}

// A value constructed inside the *inner* region but owned by a container in
// the *outer* region's scope: it is still alive when the inner region ends, so
// it escapes into the enclosing region rather than all the way out to the
// function. Its liveness bookkeeping has to follow it there, because the
// freeValRef for its eventual destruction (below, while the outer region is
// still the active scope) is routed to the outer region too.
val<int64_t> regionNestedEscapeToOuter() {
	val<int64_t> sum = 0;
	region([&]() {
		std::vector<val<int64_t>> v;
		region([&]() { v.push_back(val<int64_t>(7)); });
		sum = sum + v[0];
	}); // v dies here, inside the outer region
	return sum;
}

// The same escape, but the container outlives *both* regions, so the value is
// still alive at each region end in turn -- the case that used to hand the same
// ref to the enclosing function once per region boundary crossed.
val<int64_t> regionNestedEscapeToFunction() {
	val<int64_t> sum = 0;
	std::vector<val<int64_t>> v;
	region([&]() {
		region([&]() { v.push_back(val<int64_t>(7)); });
		sum = sum + v[0];
	});
	return sum + v[0];
}

// Three nesting levels, each handing a freshly created value one level out:
// the innermost escapes into the middle region, the middle's into the outer,
// the outer's out to the function. Each transfer targets a different scope, so
// a rule that always credited the function (or always the immediate parent)
// gets a different level wrong.
val<int64_t> regionTripleNestedEscape() {
	std::vector<val<int64_t>> outer;
	region([&]() {
		std::vector<val<int64_t>> mid;
		region([&]() {
			std::vector<val<int64_t>> inner;
			region([&]() { inner.push_back(val<int64_t>(1)); });
			mid.push_back(inner[0] + 2);
		});
		outer.push_back(mid[0] + 4);
	});
	return outer[0];
}

// Several distinct refs alive at once at the inner region's end, so the
// transfer walks more than one entry.
val<int64_t> regionMultipleEscapes() {
	val<int64_t> sum = 0;
	region([&]() {
		std::vector<val<int64_t>> v;
		region([&]() {
			v.push_back(val<int64_t>(1));
			v.push_back(val<int64_t>(2));
			v.push_back(val<int64_t>(4));
		});
		sum = sum + v[0] + v[1] + v[2];
	});
	return sum;
}

// One escaping value held by several live copies: the transfer has to move the
// whole reference count, not just mark the ref alive once, or the enclosing
// scope's count goes negative on the second destruction.
val<int64_t> regionEscapeWithMultipleCopies() {
	val<int64_t> sum = 0;
	region([&]() {
		std::vector<val<int64_t>> v;
		region([&]() {
			val<int64_t> t = 5;
			v.push_back(t);
			v.push_back(t);
			v.push_back(t);
		});
		sum = sum + v[0] + v[1] + v[2];
	});
	return sum;
}

// A live value built in ONE branch arm and carried out of the region. Each arm
// constructs a different value with no merge point between them, so which one
// escapes depends on which arm ran -- the region equivalent of returning a
// value from the lambda. lazyTracing's per-pass escape-set check rejects it;
// exceptionBasedTracing traces one arm per engagement and accepts it. Escaping
// by assignment to a captured val<T> (regionBranchWritesDifferentVars above)
// merges across branches and works under both.
val<int64_t> regionEscapeAcrossBranch(val<int64_t> x) {
	std::optional<val<int64_t>> slot;
	region([&]() {
		if (x > 0) {
			slot.emplace(val<int64_t>(10));
		} else {
			slot.emplace(val<int64_t>(20));
		}
	});
	return *slot;
}

// A live value escaping a region that also contains an internal branch, built
// unconditionally so both arms agree on what escapes. The region body is
// re-invoked once per exploration pass, so the escaping value is rebuilt each
// pass while the previous pass's copy is still held by the enclosing scope --
// the region's liveness bookkeeping has to carry both across the pass boundary.
val<int64_t> regionLiveEscapeWithInternalBranch(val<int64_t> x) {
	std::vector<val<int64_t>> v;
	val<int64_t> sum = 0;
	region([&]() {
		v.push_back(val<int64_t>(7));
		if (x > 0) {
			sum = sum + 1;
		} else {
			sum = sum + 2;
		}
	});
	return sum + v[0];
}

// Two sibling regions nested in the same outer region: the first escapes a
// value, the second closes cleanly and takes the memoization path. The second
// must not inherit or re-transfer what the first left behind.
val<int64_t> regionSiblingNestedEscapes() {
	val<int64_t> sum = 0;
	region([&]() {
		std::optional<val<int64_t>> slot;
		region([&]() { slot.emplace(val<int64_t>(5)); });
		region([&]() { sum = sum + 1; });
		sum = sum + *slot;
	});
	return sum;
}

// The whole nest re-entered under a dynamic loop, so the same call sites are
// engaged repeatedly and any liveness left over from one engagement shows up
// in the next one's parent-state hash.
val<int64_t> regionNestedEscapeInLoop(val<int64_t> n) {
	val<int64_t> sum = 0;
	val<int64_t> i = 0;
	while (i < n) {
		region([&]() {
			std::optional<val<int64_t>> slot;
			region([&]() { slot.emplace(val<int64_t>(3)); });
			sum = sum + *slot;
		});
		i = i + 1;
	}
	return sum;
}

// Same nest re-entered under static unrolling, so each engagement sees a
// different enclosing state hash P -- the key regions are memoized under.
val<int64_t> regionNestedEscapeStaticUnroll() {
	val<int64_t> sum = 0;
	for (static_val<int32_t> j = 0; j < 3; j++) {
		region([&]() {
			std::optional<val<int64_t>> slot;
			region([&]() { slot.emplace(val<int64_t>(j) + 1); });
			sum = sum + *slot;
		});
	}
	return sum;
}

// Control flow in the *outer* region, positioned after the inner region has
// already closed: the inner region's exit wiring (or its memoized
// continuation) has to leave the cursor somewhere the following branch can
// legally extend from, and the branch's own exploration must not re-enter or
// disturb the finished inner region.
val<int64_t> regionBranchAfterInnerRegion(val<int64_t> x) {
	val<int64_t> sum = 0;
	region([&]() {
		region([&]() { sum = sum + 1; });
		if (x > 0) {
			sum = sum + 10;
		} else {
			sum = sum + 20;
		}
	});
	return sum;
}

// Same, with a native loop after the inner region instead of a branch.
val<int64_t> regionLoopAfterInnerRegion(val<int64_t> n) {
	val<int64_t> sum = 0;
	region([&]() {
		region([&]() { sum = sum + 1; });
		val<int64_t> i = 0;
		while (i < n) {
			sum = sum + 2;
			i = i + 1;
		}
	});
	return sum;
}

// Inner region, then a branch, then a *second* inner region inside one arm:
// the second region is entered under a parent state that only one arm reaches,
// so its memo key must not collide with the first region's.
val<int64_t> regionBranchBetweenInnerRegions(val<int64_t> x) {
	val<int64_t> sum = 0;
	region([&]() {
		region([&]() { sum = sum + 1; });
		if (x > 0) {
			region([&]() { sum = sum + 10; });
		} else {
			sum = sum + 20;
		}
		region([&]() { sum = sum + 100; });
	});
	return sum;
}

// Control flow following the *outer* region, at function level.
val<int64_t> regionBranchAfterOuterRegion(val<int64_t> x) {
	val<int64_t> sum = 0;
	region([&]() { region([&]() { sum = sum + 1; }); });
	if (x > 0) {
		sum = sum + 10;
	} else {
		sum = sum + 20;
	}
	return sum;
}

// Branch before the inner region, with the inner region trailing it.
val<int64_t> regionBranchBeforeInnerRegion(val<int64_t> x) {
	val<int64_t> sum = 0;
	region([&]() {
		if (x > 0) {
			sum = sum + 10;
		} else {
			sum = sum + 20;
		}
		region([&]() { sum = sum + 1; });
	});
	return sum;
}

// The minimal shape: a branch inside a region followed by one more traced
// operation. No nesting, no escaping value -- just work after the branch.
val<int64_t> regionBranchNoInnerRegion(val<int64_t> x) {
	val<int64_t> sum = 0;
	region([&]() {
		if (x > 0) {
			sum = sum + 10;
		} else {
			sum = sum + 20;
		}
		sum = sum + 1;
	});
	return sum;
}

// Calls inside a region. These are traced at region level like every other
// operation: gating them on the enclosing function's pause/replay state instead,
// while still tagging them against the region, meant a call on a path the region
// re-explored recorded a duplicate tag and aborted the whole function's trace
// ("Invalid trace. This is maybe caused by a constant loop").
int64_t regionRuntimeAdd(int64_t a, int64_t b) {
	return a + b;
}

val<int64_t> regionCallOnly(val<int64_t> x) {
	val<int64_t> sum = 0;
	nautilus::region([&]() { sum = invoke<>(regionRuntimeAdd, x, val<int64_t>(5)); });
	return sum;
}

// The call sits before the branch, so it is re-executed on every local
// exploration pass -- the shape that used to abort tracing.
val<int64_t> regionCallThenBranch(val<int64_t> x) {
	val<int64_t> sum = 0;
	nautilus::region([&]() {
		sum = invoke<>(regionRuntimeAdd, x, val<int64_t>(5));
		if (x > 0) {
			sum = sum + 100;
		} else {
			sum = sum + 200;
		}
	});
	return sum;
}

// A different call per arm: each pass runs its own call site.
val<int64_t> regionCallInsideBranchArm(val<int64_t> x) {
	val<int64_t> sum = 0;
	nautilus::region([&]() {
		if (x > 0) {
			sum = invoke<>(regionRuntimeAdd, x, val<int64_t>(5));
		} else {
			sum = invoke<>(regionRuntimeAdd, x, val<int64_t>(50));
		}
	});
	return sum;
}

val<int64_t> regionNautilusCalleeBody(val<int64_t> x) {
	return x * 2;
}
static auto regionNautilusCallee = NautilusFunction {"regionNautilusCallee", regionNautilusCalleeBody};

// A nautilus::function call before a branch in the same region: the callee is
// queued and traced as its own function, the region records only the CALL.
val<int64_t> regionNautilusCallThenBranch(val<int64_t> x) {
	val<int64_t> sum = 0;
	nautilus::region([&]() {
		sum = regionNautilusCallee(x);
		if (x > 0) {
			sum = sum + 100;
		} else {
			sum = sum + 200;
		}
	});
	return sum;
}

val<int64_t> regionEmptyAndUnnamed() {
	val<int64_t> sum = 0;
	region("empty", [&]() {});
	region([&]() {});
	return sum + 1;
}

val<int64_t> regionOuterDynamicInnerStatic(val<int64_t> n) {
	val<int64_t> sum = 0;
	val<int64_t> i = 0;
	while (i < n) {
		for (static_val<int32_t> j = 0; j < 3; j++) {
			region([&]() { sum = sum + val<int64_t>(j); });
		}
		i = i + 1;
	}
	return sum;
}

void runRegionTests(engine::NautilusEngine& engine) {
	SECTION("region basic") {
		auto fn = engine.registerFunction(regionBasic);
		REQUIRE(fn() == 42);
	}

	SECTION("region preserves static unrolling") {
		auto fn = engine.registerFunction(regionStaticUnroll);
		REQUIRE(fn() == 3); // 0+1+2; folding to 0+0+0 would give 0
	}

	SECTION("region replay skips closed body") {
		auto fn = engine.registerFunction(regionReplayClosedBody);
		REQUIRE(fn(4) == 4);
		REQUIRE(fn(0) == 0);
	}

	SECTION("region escaped value lifecycle") {
		auto fn = engine.registerFunction(regionEscapedValue);
		REQUIRE(fn() == 8);
	}

	SECTION("region internal branch") {
		auto fn = engine.registerFunction(regionInternalBranch);
		REQUIRE(fn(1) == 10);
		REQUIRE(fn(-1) == -1);
	}

	SECTION("region chained internal branches") {
		auto fn = engine.registerFunction(regionChainedIf);
		REQUIRE(fn(1, 1, 1) == 7);      // 1+2+4
		REQUIRE(fn(-1, -1, -1) == 700); // 100+200+400
		REQUIRE(fn(1, -1, 1) == 205);   // 1+200+4
		REQUIRE(fn(-1, 1, -1) == 502);  // 100+2+400
		REQUIRE(fn(1, 1, -1) == 403);   // 1+2+400
	}

	SECTION("region branches write different captured vars") {
		auto fn = engine.registerFunction(regionBranchWritesDifferentVars);
		REQUIRE(fn(1) == 10);
		REQUIRE(fn(-1) == 20);
	}

	SECTION("region internal loop") {
		auto fn = engine.registerFunction(regionInternalLoop);
		REQUIRE(fn(5) == 5);
		REQUIRE(fn(0) == 0);
	}

	SECTION("region nesting") {
		auto fn = engine.registerFunction(regionNested);
		REQUIRE(fn() == 3);
	}

	SECTION("region branch after outer region") {
		auto fn = engine.registerFunction(regionBranchAfterOuterRegion);
		REQUIRE(fn(1) == 11);
		REQUIRE(fn(-1) == 21);
	}

	SECTION("region branch after inner region") {
		auto fn = engine.registerFunction(regionBranchAfterInnerRegion);
		REQUIRE(fn(1) == 11);
		REQUIRE(fn(-1) == 21);
	}

	SECTION("region loop after inner region") {
		auto fn = engine.registerFunction(regionLoopAfterInnerRegion);
		REQUIRE(fn(3) == 7); // 1 + 3*2
		REQUIRE(fn(0) == 1);
	}

	SECTION("region branch between inner regions") {
		auto fn = engine.registerFunction(regionBranchBetweenInnerRegions);
		REQUIRE(fn(1) == 111);  // 1 + 10 + 100
		REQUIRE(fn(-1) == 121); // 1 + 20 + 100
	}

	SECTION("region branch before inner region") {
		auto fn = engine.registerFunction(regionBranchBeforeInnerRegion);
		REQUIRE(fn(1) == 11);
		REQUIRE(fn(-1) == 21);
	}

	SECTION("region operation after internal branch") {
		auto fn = engine.registerFunction(regionBranchNoInnerRegion);
		REQUIRE(fn(1) == 11);
		REQUIRE(fn(-1) == 21);
	}

	SECTION("region runtime call alone") {
		auto fn = engine.registerFunction(regionCallOnly);
		REQUIRE(fn(1) == 6);
	}

	SECTION("region runtime call before internal branch") {
		auto fn = engine.registerFunction(regionCallThenBranch);
		REQUIRE(fn(1) == 106);  // (1+5) + 100
		REQUIRE(fn(-1) == 204); // (-1+5) + 200
	}

	SECTION("region runtime call inside branch arm") {
		auto fn = engine.registerFunction(regionCallInsideBranchArm);
		REQUIRE(fn(1) == 6);
		REQUIRE(fn(-1) == 49);
	}

	SECTION("region nautilus function call before internal branch") {
		auto fn = engine.registerFunction(regionNautilusCallThenBranch);
		REQUIRE(fn(1) == 102);  // 1*2 + 100
		REQUIRE(fn(-1) == 198); // -1*2 + 200
	}

	SECTION("region empty and unnamed") {
		auto fn = engine.registerFunction(regionEmptyAndUnnamed);
		REQUIRE(fn() == 1);
	}

	SECTION("region outer dynamic inner static") {
		auto fn = engine.registerFunction(regionOuterDynamicInnerStatic);
		REQUIRE(fn(2) == 6); // 2 * (0+1+2); region re-entered under 3 parent-hash values
		REQUIRE(fn(0) == 0);
	}
}

// --- White-box tests: these pin the mechanism itself, not just the results. ---
// Everything above compiles and runs a regioned function and checks its value, which
// stays green even if region() degrades to tracing the body inline. The two tests below
// fail in that case: the first because the enclosing function would need more than one
// pass, the second because the trace would not be equivalent.

namespace {

// Counted at *trace* time: the compiled function never runs this C++ code.
int64_t functionPasses = 0;
int64_t regionBodyPasses = 0;

val<int64_t> countedRegioned(val<int64_t> a, val<int64_t> b, val<int64_t> c) {
	functionPasses++;
	val<int64_t> sum = 0;
	region([&]() {
		regionBodyPasses++;
		if (a > 0) {
			sum = sum + 1;
		} else {
			sum = sum + 100;
		}
	});
	region([&]() {
		regionBodyPasses++;
		if (b > 0) {
			sum = sum + 2;
		} else {
			sum = sum + 200;
		}
	});
	region([&]() {
		regionBodyPasses++;
		if (c > 0) {
			sum = sum + 4;
		} else {
			sum = sum + 400;
		}
	});
	return sum;
}

val<int64_t> countedUnregioned(val<int64_t> a, val<int64_t> b, val<int64_t> c) {
	functionPasses++;
	val<int64_t> sum = 0;
	if (a > 0) {
		sum = sum + 1;
	} else {
		sum = sum + 100;
	}
	if (b > 0) {
		sum = sum + 2;
	} else {
		sum = sum + 200;
	}
	if (c > 0) {
		sum = sum + 4;
	} else {
		sum = sum + 400;
	}
	return sum;
}

/// Traces @p func with the lazy tracer and returns its IR after the two block-level
/// cleanup passes the default pipeline runs.
std::shared_ptr<compiler::ir::IRGraph> traceToCleanedIr(const std::function<void()>& func) {
	auto rootFunction = compiler::CompilableFunction("execute", func);
	std::list<compiler::CompilableFunction> functionsToTrace;
	functionsToTrace.push_back(rootFunction);
	common::Arena arena;
	auto traceModule = tracing::LazyTraceContext::Trace(functionsToTrace, engine::Options(), arena);
	auto ssa = tracing::SSACreationPhase().apply(std::shared_ptr<tracing::TraceModule>(std::move(traceModule)));
	auto ir = tracing::TraceToIRConversionPhase().apply(std::move(ssa));
	engine::Options passOpts;
	compiler::ir::IRPassManager passManager(passOpts);
	// The seams a region introduces -- its entry and exit block -- are exactly what
	// these two passes exist to collapse, so equivalence is asserted after them.
	passManager.addPass(std::make_unique<compiler::ir::EmptyBlockEliminationPass>());
	passManager.addPass(std::make_unique<compiler::ir::BlockMergingPass>());
	passManager.run(*ir);
	return ir;
}

} // namespace

// The reason regions exist: a branch inside a region is resolved by re-running only that
// region's body, so the enclosing function does not have to be re-run once per branch.
TEST_CASE("Region Bounds Branch Exploration To The Region", "[region]") {
	common::Arena arena;

	functionPasses = 0;
	regionBodyPasses = 0;
	{
		auto wrapper = details::createFunctionWrapper(countedRegioned);
		std::list<compiler::CompilableFunction> functions {compiler::CompilableFunction("execute", wrapper)};
		tracing::LazyTraceContext::Trace(functions, engine::Options(), arena);
	}
	const auto regionedFunctionPasses = functionPasses;
	const auto totalRegionBodyPasses = regionBodyPasses;

	functionPasses = 0;
	{
		auto wrapper = details::createFunctionWrapper(countedUnregioned);
		std::list<compiler::CompilableFunction> functions {compiler::CompilableFunction("execute", wrapper)};
		common::Arena unregionedArena;
		tracing::LazyTraceContext::Trace(functions, engine::Options(), unregionedArena);
	}
	const auto unregionedFunctionPasses = functionPasses;

	INFO("regioned function passes: " << regionedFunctionPasses << ", region body passes: " << totalRegionBodyPasses
	                                  << ", unregioned function passes: " << unregionedFunctionPasses);

	// Every branch is inside a region, so the enclosing function has nothing left to
	// explore and is traced in a single pass.
	REQUIRE(regionedFunctionPasses == 1);
	// Without regions the same three branches force the whole function to be re-run.
	REQUIRE(unregionedFunctionPasses > regionedFunctionPasses);
	// Each region costs a small, bounded number of local passes that does not compound
	// with the size of the enclosing function: three regions, one branch each.
	REQUIRE(totalRegionBodyPasses <= 9);
}

// A regioned function must not cost anything in the generated code: same control-flow
// graph, and no more operations than the unregioned equivalent. The entry and exit block
// a region adds are single-predecessor seams, which is exactly what the two block-cleanup
// passes in traceToCleanedIr collapse -- so they leave no trace in the CFG.
//
// The two are not textually identical, and asserting that they were would be asserting
// something false. Exploring each region independently means the code after a region's
// internal branch is recorded once, in the merge block, whereas the unregioned tracer
// re-records it on each incoming path: for the three ifs below the unregioned form
// materializes five `0` constants and threads one of them through a block argument,
// against three constants and no such argument for the regioned form. The regioned IR is
// the smaller of the two, which is why this asserts <= rather than ==.
TEST_CASE("Region IR Matches Unregioned IR", "[region]") {
	auto regioned = traceToCleanedIr(details::createFunctionWrapper(countedRegioned));
	auto unregioned = traceToCleanedIr(details::createFunctionWrapper(countedUnregioned));
	auto regionedStats = compiler::ir::computeStatistics(*regioned);
	auto unregionedStats = compiler::ir::computeStatistics(*unregioned);
	INFO("regioned:\n" << regioned->toString() << "\nunregioned:\n" << unregioned->toString());
	REQUIRE(regionedStats.numFunctions == unregionedStats.numFunctions);
	REQUIRE(regionedStats.numBlocks == unregionedStats.numBlocks);
	REQUIRE(regionedStats.numOperations <= unregionedStats.numOperations);
}

#ifdef ENABLE_TRACING
TEST_CASE("Region Compiler Test", "[region]") {
	nautilus::testing::forEachBackendWithTraceMode([](engine::NautilusEngine& engine) { runRegionTests(engine); });
}

// Every way a value created inside a region body can outlive it. lazyTracing rejects all
// of them; see LazyTraceContext::traceScopeExit for why it cannot do anything else. What
// is pinned here is that each is *diagnosed* -- an exception naming region(), not a crash
// and not a silently wrong trace -- and that exceptionBasedTracing, which inlines region
// bodies into the enclosing function, still traces every one of them correctly.
//
// The supported way to carry a value out of a region is to assign to a val<T> declared
// outside it; regionEscapedValue and regionBranchWritesDifferentVars in the suite above
// cover that and work under both tracers.
namespace {

/// Registers @p fn on a fresh lazyTracing engine and requires it to be rejected.
/// Note the real backend: the "interpreter" engine sets engine.Compilation = false and so
/// never traces at all, which would make every one of these checks vacuously pass.
template <typename F>
void requireRejectedByLazyTracing(const std::string& backend, const char* name, F fn) {
	auto lazyEngine = nautilus::testing::makeEngine(
	    backend, [](engine::Options& opts) { opts.setOption("engine.traceMode", std::string("lazyTracing")); });
	std::string diagnosis = "<no exception thrown>";
	try {
		lazyEngine.registerFunction(fn);
	} catch (const std::exception& e) {
		diagnosis = e.what();
	}
	INFO(name << ": " << diagnosis);
	REQUIRE(diagnosis.find("region()") != std::string::npos);
	REQUIRE(diagnosis.find("exceptionBasedTracing") != std::string::npos);
}

} // namespace

TEST_CASE("Region Rejects Values Outliving The Body", "[region]") {
	// Both halves are decided while tracing, before code generation, so one backend is
	// enough here; the suite above covers these paths across every backend.
	const auto backends = nautilus::testing::availableBackends();
	if (backends.empty()) {
		SKIP("no compilation backend available");
	}
	const auto& backend = backends.front();
	auto ebEngine = nautilus::testing::makeEngine(backend, [](engine::Options& opts) {
		opts.setOption("engine.traceMode", std::string("exceptionBasedTracing"));
	});

	SECTION("exceptionBasedTracing traces them") {
		REQUIRE(ebEngine.registerFunction(regionNestedEscapeToOuter)() == 7);
		REQUIRE(ebEngine.registerFunction(regionNestedEscapeToFunction)() == 14);
		REQUIRE(ebEngine.registerFunction(regionTripleNestedEscape)() == 7);        // ((1) + 2) + 4
		REQUIRE(ebEngine.registerFunction(regionMultipleEscapes)() == 7);           // 1 + 2 + 4
		REQUIRE(ebEngine.registerFunction(regionEscapeWithMultipleCopies)() == 15); // 5 * 3
		REQUIRE(ebEngine.registerFunction(regionSiblingNestedEscapes)() == 6);      // 1 + 5
		REQUIRE(ebEngine.registerFunction(regionNestedEscapeStaticUnroll)() == 6);  // 1 + 2 + 3

		auto inLoop = ebEngine.registerFunction(regionNestedEscapeInLoop);
		REQUIRE(inLoop(3) == 9);
		REQUIRE(inLoop(0) == 0);

		auto liveEscape = ebEngine.registerFunction(regionLiveEscapeWithInternalBranch);
		REQUIRE(liveEscape(1) == 8);  // 7 + 1
		REQUIRE(liveEscape(-1) == 9); // 7 + 2

		auto acrossBranch = ebEngine.registerFunction(regionEscapeAcrossBranch);
		REQUIRE(acrossBranch(1) == 10);
		REQUIRE(acrossBranch(-1) == 20);
	}

	SECTION("lazyTracing rejects them") {
		requireRejectedByLazyTracing(backend, "regionNestedEscapeToOuter", regionNestedEscapeToOuter);
		requireRejectedByLazyTracing(backend, "regionNestedEscapeToFunction", regionNestedEscapeToFunction);
		requireRejectedByLazyTracing(backend, "regionTripleNestedEscape", regionTripleNestedEscape);
		requireRejectedByLazyTracing(backend, "regionMultipleEscapes", regionMultipleEscapes);
		requireRejectedByLazyTracing(backend, "regionEscapeWithMultipleCopies", regionEscapeWithMultipleCopies);
		requireRejectedByLazyTracing(backend, "regionSiblingNestedEscapes", regionSiblingNestedEscapes);
		requireRejectedByLazyTracing(backend, "regionNestedEscapeInLoop", regionNestedEscapeInLoop);
		requireRejectedByLazyTracing(backend, "regionNestedEscapeStaticUnroll", regionNestedEscapeStaticUnroll);
		requireRejectedByLazyTracing(backend, "regionLiveEscapeWithInternalBranch", regionLiveEscapeWithInternalBranch);
		requireRejectedByLazyTracing(backend, "regionEscapeAcrossBranch", regionEscapeAcrossBranch);
	}
}

#else
TEST_CASE("Region Compiler Test", "[region]") {
	SKIP("Region compilation requires the tracing/compiler pipeline (ENABLE_TRACING)");
}
#endif

} // namespace nautilus::engine
