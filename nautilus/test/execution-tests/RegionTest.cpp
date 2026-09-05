#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/function.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/region.hpp"
#include "nautilus/static.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <optional>
#include <source_location>
#include <string>
#include <vector>

#ifdef ENABLE_TRACING
// Only the white-box tests below reach past the engine API into the tracer and the IR
// pipeline, and they only exist when there is a tracer to drive.
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/ir/passes/BlockMergingPass.hpp"
#include "nautilus/compiler/ir/passes/EmptyBlockEliminationPass.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/compiler/ir/passes/IRStatistics.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include <list>
#include <memory>
#endif

namespace nautilus::engine {

val<int64_t> regionBasic() {
	val<int64_t> sum = 0;
	region([&]() { sum = sum + 42; });
	return sum;
}

/// Line of the region below; the unroll enters it once per iteration, so it is also the
/// line the IR's single table entry for that call site must name.
uint32_t staticUnrollRegionLine = 0;

val<int64_t> regionStaticUnroll() {
	val<int64_t> sum = 0;
	for (static_val<int32_t> j = 0; j < 3; j++) {
		staticUnrollRegionLine = __LINE__ + 1;
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

// A helper that wraps region() would otherwise describe its own body as the region's
// call site -- the same position for every caller. Building the attributes from the
// caller's location instead is what the RegionAttributes overload of region() is for.
template <typename F>
void namedRegionHelper(const char* name, F&& fn, std::source_location location = std::source_location::current()) {
	region(RegionAttributes {name, SourceLocation::from(location)}, std::forward<F>(fn));
}

/// Line of the region the helper below opens, captured next to the call site so the
/// expectation cannot drift when this file is edited.
uint32_t helperRegionLine = 0;

val<int64_t> regionThroughHelper() {
	val<int64_t> sum = 0;
	helperRegionLine = __LINE__ + 1;
	namedRegionHelper("helper", [&]() { sum = sum + 5; });
	return sum + 1;
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

	SECTION("region opened through a helper") {
		auto fn = engine.registerFunction(regionThroughHelper);
		REQUIRE(fn() == 6);
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

#ifdef ENABLE_TRACING

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

// --- Region attributes: what a region() call site says about itself. ---

namespace {

/// Lines of the region() call sites in regionAttributed below, captured next to each call
/// so the expectations cannot drift when this file is edited.
uint32_t outerRegionLine = 0;
uint32_t innerRegionLine = 0;
uint32_t unnamedRegionLine = 0;

val<int64_t> regionAttributed() {
	val<int64_t> sum = 0;
	outerRegionLine = __LINE__ + 1;
	region("accumulate", [&]() {
		innerRegionLine = __LINE__ + 1;
		region("inner", [&]() { sum = sum + 1; });
		sum = sum + 41;
	});
	unnamedRegionLine = __LINE__ + 1;
	region([&]() { sum = sum + 1; });
	return sum;
}

val<int64_t> regionedCalleeBody(val<int64_t> x) {
	val<int64_t> doubled = 0;
	region("callee", [&]() { doubled = x * 2; });
	return doubled;
}
static auto regionedCallee = NautilusFunction {"regionedCallee", regionedCalleeBody};

/// Two functions in one module, each with its own region: their table indexes both start
/// at zero, their ids must not.
val<int64_t> regionedCaller(val<int64_t> x) {
	val<int64_t> sum = 0;
	region("caller", [&]() { sum = regionedCallee(x) + 1; });
	return sum;
}

/// A branch inside a region: the arms are blocks of their own, and unlike the region's
/// entry and exit seams they survive the block-cleanup passes -- so they are what a
/// block-level region is for.
val<int64_t> regionedBranch(val<int64_t> x) {
	val<int64_t> sum = 0;
	region("branching", [&]() {
		if (x > 0) {
			sum = sum + 10;
		} else {
			sum = sum + 20;
		}
	});
	return sum + 1;
}

/// A value created inside a *named* region and still alive at its end: rejected, and the
/// diagnostic has to name the region the user wrote rather than leave them to find it.
uint32_t rejectedRegionLine = 0;

val<int64_t> regionNamedEscape() {
	val<int64_t> out = 0;
	std::vector<val<int64_t>> escaping;
	rejectedRegionLine = __LINE__ + 1;
	region("escaping", [&]() { escaping.push_back(val<int64_t>(7)); });
	out = escaping[0];
	return out;
}

/// Restores log::options::setLogSourceLocations on scope exit: it is process-wide state,
/// and every other test in this binary that dumps a trace or IR would inherit a leaked
/// value.
struct SourceLocationPrintingGuard {
	explicit SourceLocationPrintingGuard(bool enabled) : previous(log::options::getLogSourceLocations()) {
		log::options::setLogSourceLocations(enabled);
	}
	~SourceLocationPrintingGuard() {
		log::options::setLogSourceLocations(previous);
	}
	SourceLocationPrintingGuard(const SourceLocationPrintingGuard&) = delete;
	SourceLocationPrintingGuard& operator=(const SourceLocationPrintingGuard&) = delete;

	bool previous;
};

std::unique_ptr<tracing::TraceModule> traceWithLazyTracer(const std::function<void()>& func, common::Arena& arena) {
	std::list<compiler::CompilableFunction> functions {compiler::CompilableFunction("execute", func)};
	return tracing::LazyTraceContext::Trace(functions, engine::Options(), arena);
}

} // namespace

// The attributes are recorded against the blocks the region body was traced into, so a
// trace can be read back against the source it came from.
TEST_CASE("Region Attributes Are Recorded In The Trace", "[region]") {
	common::Arena arena;
	auto module = traceWithLazyTracer(details::createFunctionWrapper(regionAttributed), arena);
	auto* trace = module->getFunction("execute");
	REQUIRE(trace != nullptr);

	const auto& regions = trace->getRegions();
	INFO("trace:\n" << trace->toString());
	// Recorded in the order the tracer entered them: the outer region, the inner one
	// nested in its body, then the unnamed one after it.
	REQUIRE(regions.size() == 3);

	REQUIRE(regions[0].attributes.hasName());
	REQUIRE(std::string(regions[0].attributes.name) == "accumulate");
	REQUIRE(regions[0].attributes.location.line == outerRegionLine);
	REQUIRE(std::string(regions[0].attributes.location.file).find("RegionTest.cpp") != std::string::npos);
	REQUIRE(regions[0].attributes.location.column > 0);
	REQUIRE(std::string(regions[0].attributes.location.function).find("regionAttributed") != std::string::npos);

	REQUIRE(std::string(regions[1].attributes.name) == "inner");
	REQUIRE(regions[1].attributes.location.line == innerRegionLine);

	// An unnamed region still carries its position; only the name is absent.
	REQUIRE_FALSE(regions[2].attributes.hasName());
	REQUIRE(regions[2].attributes.name == nullptr);
	REQUIRE(regions[2].attributes.location.line == unnamedRegionLine);

	// A block names the region it belongs to: the entry block is the first block of the
	// body, while the exit block is where the *enclosing* scope resumes -- for the inner
	// region that is the outer region, for the outer ones the function body.
	for (uint32_t i = 0; i < regions.size(); i++) {
		REQUIRE(trace->getBlock(regions[i].entryBlock).regionIndex == i);
		REQUIRE(trace->getBlock(regions[i].exitBlock).regionIndex == regions[i].parent);
	}
	REQUIRE(regions[1].parent == 0);
	REQUIRE(trace->getBlock(0).regionIndex == tracing::NO_REGION);

	// And they are visible in the trace dump.
	const auto dump = trace->toString();
	REQUIRE(dump.find("region \"accumulate\" at ") != std::string::npos);
	REQUIRE(dump.find("region \"inner\" at ") != std::string::npos);
}

// What the attributes are for downstream: they survive the trace-to-IR conversion and the
// block-cleanup passes that collapse a region's entry and exit block, because they ride on
// the operations rather than on the blocks that bounded the body.
TEST_CASE("Region Attributes Survive Into The IR", "[region]") {
	auto ir = traceToCleanedIr(details::createFunctionWrapper(regionAttributed));
	REQUIRE(ir->getFunctionOperations().size() == 1);
	const auto* function = ir->getFunctionOperations().front();

	// One entry per region() call site: the outer region, the one nested in it, and the
	// unnamed one after it.
	const auto& specs = function->getRegionSpecs();
	INFO("ir:\n" << ir->toString());
	REQUIRE(specs.size() == 3);
	REQUIRE(std::string(specs[0].attributes.name) == "accumulate");
	REQUIRE(specs[0].parent == compiler::ir::NO_REGION);
	REQUIRE(specs[0].attributes.location.line == outerRegionLine);
	REQUIRE(std::string(specs[1].attributes.name) == "inner");
	// The nesting is recoverable from the table: the inner region names the outer one.
	REQUIRE(specs[1].parent == 0);
	REQUIRE(specs[1].attributes.location.line == innerRegionLine);
	REQUIRE_FALSE(specs[2].attributes.hasName());
	REQUIRE(specs[2].parent == compiler::ir::NO_REGION);

	// Every operation the body produced points back at its region, and operations traced
	// outside every region point at none.
	std::vector<size_t> operationsPerRegion(specs.size(), 0);
	size_t unattributed = 0;
	for (const auto* block : function->getBasicBlocks()) {
		for (const auto* operation : block->getOperations()) {
			const auto index = operation->getRegionIndex();
			if (index == compiler::ir::NO_REGION) {
				unattributed++;
				continue;
			}
			REQUIRE(function->findRegion(index) != nullptr);
			operationsPerRegion[index]++;
		}
	}
	for (size_t i = 0; i < specs.size(); i++) {
		INFO("region " << i << ": " << specs[i].attributes.toString());
		REQUIRE(operationsPerRegion[i] > 0);
	}
	// The function's own operations -- at least the return -- belong to no region.
	REQUIRE(unattributed > 0);

	// Every region carries an id of its own, and the ids are handed out by the module.
	REQUIRE(specs[0].id != specs[1].id);
	REQUIRE(specs[1].id != specs[2].id);
	REQUIRE(specs[0].id != specs[2].id);

	// This body is straight-line, so the cleanup passes merge all of it into one block.
	// That block holds code from three regions and from outside them, so it claims none
	// of them -- while each operation still names the region it came from exactly.
	REQUIRE(function->getBasicBlocks().size() == 1);
	REQUIRE(function->getEntryBlock()->getRegionIndex() == compiler::ir::NO_REGION);

	// The metadata is self-consistent after the pipeline, which the verifier checks.
	REQUIRE(compiler::ir::IRVerifier::verify(*ir).ok());

	// And the IR dump shows both attributes, with the nesting of the inner region.
	const auto dump = ir->toString();
	REQUIRE(dump.find("; region #") != std::string::npos);
	REQUIRE(dump.find("\"accumulate\" at ") != std::string::npos);
	REQUIRE(dump.find("\"inner\" at ") != std::string::npos);
	REQUIRE(dump.find("; nested in region #") != std::string::npos);
}

// A block that is wholly inside a region says so once, instead of every operation in it
// repeating the answer -- and a block outside every region still says nothing.
TEST_CASE("Region Blocks Name Their Region", "[region]") {
	auto ir = traceToCleanedIr(details::createFunctionWrapper(regionedBranch));
	const auto* function = ir->getFunctionOperations().front();
	INFO("ir:\n" << ir->toString());
	REQUIRE(function->getRegionSpecs().size() == 1);
	REQUIRE(compiler::ir::IRVerifier::verify(*ir).ok());

	// The two arms of the branch inside the region are blocks of their own; both belong
	// to the region, and every operation in them agrees with the block.
	size_t blocksInRegion = 0;
	size_t blocksOutside = 0;
	for (const auto* block : function->getBasicBlocks()) {
		if (block->getRegionIndex() == 0) {
			blocksInRegion++;
			for (const auto* operation : block->getOperations()) {
				// Operations a pass minted -- the branches these blocks now end in --
				// carry no region; the traced ones must agree with their block.
				if (operation->getRegionIndex() != compiler::ir::NO_REGION) {
					REQUIRE(function->isRegionNestedIn(operation->getRegionIndex(), block->getRegionIndex()));
				}
			}
		} else {
			REQUIRE(block->getRegionIndex() == compiler::ir::NO_REGION);
			blocksOutside++;
		}
	}
	REQUIRE(blocksInRegion >= 2);
	// The entry block, at least: the function's own code is outside the region.
	REQUIRE(blocksOutside >= 1);

	// A block that carries a region states it on its own line, so the operations under it
	// need not repeat it.
	const auto dump = ir->toString();
	REQUIRE(dump.find("): ; region #") != std::string::npos);
	REQUIRE(dump.find("\"branching\" at ") != std::string::npos);
}

// Region ids identify a region across the whole module: two functions each opening one
// region both index their own table at zero, and the ids are what tells them apart.
TEST_CASE("Region Ids Are Unique Across The Module", "[region]") {
	auto ir = traceToCleanedIr(details::createFunctionWrapper(regionedCaller));
	INFO("ir:\n" << ir->toString());
	REQUIRE(ir->getFunctionOperations().size() == 2);

	std::vector<uint32_t> ids;
	for (const auto* function : ir->getFunctionOperations()) {
		REQUIRE(function->getRegionSpecs().size() == 1);
		ids.push_back(function->getRegionSpecs()[0].id);
	}
	REQUIRE(ids.size() == 2);
	REQUIRE(ids[0] != ids[1]);
	REQUIRE(compiler::ir::IRVerifier::verify(*ir).ok());
}

// One entry per call site, not one per traced engagement: a region in a statically
// unrolled loop is entered once per iteration, and all of those are the same region().
TEST_CASE("Region IR Table Holds One Entry Per Call Site", "[region]") {
	auto ir = traceToCleanedIr(details::createFunctionWrapper(regionStaticUnroll));
	REQUIRE(ir->getFunctionOperations().size() == 1);
	const auto* function = ir->getFunctionOperations().front();
	INFO("ir:\n" << ir->toString());
	REQUIRE(function->getRegionSpecs().size() == 1);
	REQUIRE(function->getRegionSpecs()[0].attributes.location.line == staticUnrollRegionLine);
}

// Printing the source location is a choice, not a property of the metadata. A dump that
// has to be identical across machines and compilers -- a checked-in golden trace, say --
// turns it off, and the region is still named in every place it was named before.
TEST_CASE("Region Dumps Can Omit The Source Location", "[region]") {
	common::Arena arena;
	auto module = traceWithLazyTracer(details::createFunctionWrapper(regionAttributed), arena);
	auto* trace = module->getFunction("execute");
	REQUIRE(trace != nullptr);
	auto ir = traceToCleanedIr(details::createFunctionWrapper(regionAttributed));

	std::string traceWithLocations;
	std::string irWithLocations;
	{
		// On by default, so this is also what a user reading a dump sees.
		SourceLocationPrintingGuard guard(true);
		REQUIRE(log::options::getLogSourceLocations());
		traceWithLocations = trace->toString();
		irWithLocations = ir->toString();
	}
	INFO("trace with locations:\n" << traceWithLocations << "\nir with locations:\n" << irWithLocations);
	REQUIRE(traceWithLocations.find("RegionTest.cpp:" + std::to_string(outerRegionLine)) != std::string::npos);
	REQUIRE(irWithLocations.find("RegionTest.cpp:" + std::to_string(innerRegionLine)) != std::string::npos);

	std::string traceWithout;
	std::string irWithout;
	{
		SourceLocationPrintingGuard guard(false);
		traceWithout = trace->toString();
		irWithout = ir->toString();
	}
	INFO("trace without locations:\n" << traceWithout << "\nir without locations:\n" << irWithout);

	// Nothing of the call site's position survives -- no file, no line, no column.
	REQUIRE(traceWithout.find("RegionTest.cpp") == std::string::npos);
	REQUIRE(irWithout.find("RegionTest.cpp") == std::string::npos);
	REQUIRE(traceWithout.find(" at ") == std::string::npos);
	REQUIRE(irWithout.find(" at ") == std::string::npos);

	// What identifies a region does survive: its name everywhere, its id in the IR, and
	// the nesting of the inner region inside the outer one.
	REQUIRE(traceWithout.find("; region \"accumulate\"") != std::string::npos);
	REQUIRE(traceWithout.find("; region \"inner\"") != std::string::npos);
	REQUIRE(irWithout.find("; region #0 \"accumulate\"") != std::string::npos);
	REQUIRE(irWithout.find("; nested in region #0 \"accumulate\"") != std::string::npos);
	// An unnamed region has neither a name nor a location left, so it says so.
	REQUIRE(traceWithout.find("; region <unnamed>") != std::string::npos);

	// The flag is a printing choice: the attributes themselves are untouched by it.
	REQUIRE(std::string(trace->getRegions()[0].attributes.name) == "accumulate");
	REQUIRE(trace->getRegions()[0].attributes.location.line == outerRegionLine);

	// And the guard put the process-wide default back.
	REQUIRE(log::options::getLogSourceLocations());
}

// A helper that opens regions on its callers' behalf can hand region() the caller's
// position, so the recorded attributes point at the user's code and not at the helper.
TEST_CASE("Region Attributes Can Be Supplied Explicitly", "[region]") {
	common::Arena arena;
	auto module = traceWithLazyTracer(details::createFunctionWrapper(regionThroughHelper), arena);
	auto* trace = module->getFunction("execute");
	REQUIRE(trace != nullptr);

	const auto& regions = trace->getRegions();
	REQUIRE(regions.size() == 1);
	REQUIRE(std::string(regions[0].attributes.name) == "helper");
	REQUIRE(regions[0].attributes.location.line == helperRegionLine);
	REQUIRE(std::string(regions[0].attributes.location.function).find("regionThroughHelper") != std::string::npos);
}

// The point of carrying the attributes into the tracer: a rejected region body is
// reported against the call site the user wrote.
TEST_CASE("Region Diagnostics Name The Region", "[region]") {
	const auto backends = nautilus::testing::availableBackends();
	if (backends.empty()) {
		SKIP("no compilation backend available");
	}
	auto lazyEngine = nautilus::testing::makeEngine(backends.front(), [](engine::Options& opts) {
		opts.setOption("engine.traceMode", std::string("lazyTracing"));
	});

	std::string diagnosis = "<no exception thrown>";
	try {
		lazyEngine.registerFunction(regionNamedEscape);
	} catch (const std::exception& e) {
		diagnosis = e.what();
	}
	INFO(diagnosis);
	REQUIRE(diagnosis.find("region()") != std::string::npos);
	REQUIRE(diagnosis.find("\"escaping\"") != std::string::npos);
	REQUIRE(diagnosis.find("RegionTest.cpp:" + std::to_string(rejectedRegionLine)) != std::string::npos);
}

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
