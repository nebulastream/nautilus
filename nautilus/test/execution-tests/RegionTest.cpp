#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/region.hpp"
#include "nautilus/static.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
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

// A live value escaping a region that also contains an internal branch: the
// value is constructed inside the region and is still alive at region end, so
// the region has to hand its liveness to the enclosing scope -- which must
// happen whether or not that engagement was the one that recorded the body,
// since the destruction happens either way.
//
// Only exercised under exceptionBasedTracing (see the dedicated test case at
// the bottom of this file); see docs/region.md for why lazyTracing does not
// support this yet.
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

	SECTION("region nested escape into enclosing region") {
		auto fn = engine.registerFunction(regionNestedEscapeToOuter);
		REQUIRE(fn() == 7);
	}

	SECTION("region nested escape out to function") {
		auto fn = engine.registerFunction(regionNestedEscapeToFunction);
		REQUIRE(fn() == 14);
	}

	SECTION("region triple nested escape") {
		auto fn = engine.registerFunction(regionTripleNestedEscape);
		REQUIRE(fn() == 7); // ((1) + 2) + 4
	}

	SECTION("region multiple escapes from one region") {
		auto fn = engine.registerFunction(regionMultipleEscapes);
		REQUIRE(fn() == 7); // 1 + 2 + 4
	}

	SECTION("region escape with multiple live copies") {
		auto fn = engine.registerFunction(regionEscapeWithMultipleCopies);
		REQUIRE(fn() == 15); // 5 * 3
	}

	SECTION("region sibling nested escapes") {
		auto fn = engine.registerFunction(regionSiblingNestedEscapes);
		REQUIRE(fn() == 6); // 1 + 5
	}

	SECTION("region nested escape re-entered in loop") {
		auto fn = engine.registerFunction(regionNestedEscapeInLoop);
		REQUIRE(fn(3) == 9);
		REQUIRE(fn(0) == 0);
	}

	SECTION("region nested escape under static unroll") {
		auto fn = engine.registerFunction(regionNestedEscapeStaticUnroll);
		REQUIRE(fn() == 6); // 1 + 2 + 3
	}

	SECTION("region branch after outer region") {
		auto fn = engine.registerFunction(regionBranchAfterOuterRegion);
		REQUIRE(fn(1) == 11);
		REQUIRE(fn(-1) == 21);
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
TEST_CASE("Region Compiler Test", "[region]") {
	nautilus::testing::forEachBackendWithTraceMode([](engine::NautilusEngine& engine) { runRegionTests(engine); });
}

// Escaping a *live* value out of a region that also contains internal control
// flow works under exceptionBasedTracing, which invokes the region body once
// per engagement. It is not yet supported under lazyTracing, whose local
// exploration re-invokes the body and resets the region's delta liveness
// between passes, discarding refs still alive in an enclosing C++ scope -- so
// this is pinned to the one tracer that supports it rather than run through
// forEachBackendWithTraceMode. See docs/region.md. Escaping by *assignment* to
// a captured val<T> (regionBranchWritesDifferentVars above) works under both.
TEST_CASE("Region Live Escape Across Branch (exception-based)", "[region]") {
	for (const auto& backend : nautilus::testing::availableBackends()) {
		DYNAMIC_SECTION(backend) {
			auto engine = nautilus::testing::makeEngine(backend, [](engine::Options& opts) {
				opts.setOption("engine.traceMode", std::string("exceptionBasedTracing"));
			});
			auto fn = engine.registerFunction(regionEscapeAcrossBranch);
			REQUIRE(fn(1) == 10);
			REQUIRE(fn(-1) == 20);

			auto afterInner = engine.registerFunction(regionBranchAfterInnerRegion);
			REQUIRE(afterInner(1) == 11);
			REQUIRE(afterInner(-1) == 21);

			auto loopAfterInner = engine.registerFunction(regionLoopAfterInnerRegion);
			REQUIRE(loopAfterInner(3) == 7);
			REQUIRE(loopAfterInner(0) == 1);

			auto betweenInner = engine.registerFunction(regionBranchBetweenInnerRegions);
			REQUIRE(betweenInner(1) == 111);
			REQUIRE(betweenInner(-1) == 121);

			auto beforeInner = engine.registerFunction(regionBranchBeforeInnerRegion);
			REQUIRE(beforeInner(1) == 11);
			REQUIRE(beforeInner(-1) == 21);

			auto afterBranch = engine.registerFunction(regionBranchNoInnerRegion);
			REQUIRE(afterBranch(1) == 11);
			REQUIRE(afterBranch(-1) == 21);
		}
	}
}

// The same shapes under lazyTracing, which cannot express them today. What is
// pinned here is not the limitation itself but the *diagnosis*: each one has to
// surface as a NautilusException naming region() and the trace mode, because
// the untrapped failures are a segfault in a much later IR pass (traced
// operations after internal control flow leave an empty, unterminated block)
// and a reference-count underflow (a val<T> left alive across an exploration
// pass). Both point nowhere near the region that caused them.
TEST_CASE("Region Unsupported Shapes Diagnose Cleanly (lazy)", "[region]") {
	for (const auto& backend : nautilus::testing::availableBackends()) {
		DYNAMIC_SECTION(backend) {
			auto engine = nautilus::testing::makeEngine(
			    backend, [](engine::Options& opts) { opts.setOption("engine.traceMode", std::string("lazyTracing")); });
			// The message is the contract here: whoever hits this needs to be
			// told which construct is unsupported and what to do instead, not
			// merely that something threw.
			auto diagnosisOf = [&](auto fnptr) {
				try {
					engine.registerFunction(fnptr);
				} catch (const std::exception& e) {
					return std::string(e.what());
				}
				return std::string("<no exception thrown>");
			};
			for (const auto& message :
			     {diagnosisOf(regionBranchNoInnerRegion), diagnosisOf(regionBranchAfterInnerRegion),
			      diagnosisOf(regionLoopAfterInnerRegion), diagnosisOf(regionBranchBetweenInnerRegions),
			      diagnosisOf(regionBranchBeforeInnerRegion), diagnosisOf(regionEscapeAcrossBranch)}) {
				INFO("diagnostic: " << message);
				REQUIRE(message.find("region()") != std::string::npos);
				REQUIRE(message.find("exceptionBasedTracing") != std::string::npos);
			}
		}
	}
}
#else
TEST_CASE("Region Compiler Test", "[region]") {
	SKIP("Region compilation requires the tracing/compiler pipeline (ENABLE_TRACING)");
}
#endif

} // namespace nautilus::engine
