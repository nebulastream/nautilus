// Explicit control flow on region scopes (docs/plans/nautilus-0.4/01-tracing-frontend.md §5, M2.5): the constructs of
// nautilus/control_flow.hpp, alone and mixed with native control flow, against a C++ oracle on every backend, and the
// single-pass guarantees through the tracing statistics.

#include "ExecutionTest.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/control_flow.hpp"
#include "nautilus/region.hpp"
#include "nautilus/static.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_std.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace nautilus::engine {

namespace {

// ---- functions with an oracle ---------------------------------------------------------------------------------------

val<int64_t> cfIfElse(val<int64_t> x) {
	val<int64_t> r = 0;
	If(x > 0, [&] { r = 1; }, [&] { r = 2; });
	return r * 10 + x;
}
int64_t cfIfElseRef(int64_t x) {
	return (x > 0 ? 1 : 2) * 10 + x;
}

val<int64_t> cfValueIf(val<int64_t> x) {
	val<int64_t> a = x * 3;
	val<int64_t> b = x + 100;
	val<int64_t> m = If(a > b, [&] { return a; }, [&] { return b; });
	return m;
}
int64_t cfValueIfRef(int64_t x) {
	return std::max(x * 3, x + 100);
}

val<int64_t> cfSumFor(val<int64_t> n) {
	val<int64_t> sum = 0;
	For(val<int64_t>(0), n, [&](val<int64_t> i) { sum = sum + i; });
	return sum;
}
int64_t cfSumForRef(int64_t n) {
	int64_t sum = 0;
	for (int64_t i = 0; i < n; ++i) {
		sum += i;
	}
	return sum;
}

val<int64_t> cfWhileBreak(val<int64_t> n) {
	val<int64_t> i = 0;
	val<int64_t> sum = 0;
	While([&] { return i < 100; },
	      [&] {
		      If(i == n, [&] { Break(); });
		      sum = sum + i;
		      i = i + 1;
	      });
	return sum;
}
int64_t cfWhileBreakRef(int64_t n) {
	int64_t sum = 0;
	for (int64_t i = 0; i < 100; ++i) {
		if (i == n) {
			break;
		}
		sum += i;
	}
	return sum;
}

val<int64_t> cfForContinue(val<int64_t> n) {
	val<int64_t> sum = 0;
	For(val<int64_t>(0), n, [&](val<int64_t> i) {
		If(i % 2 == 0, [&] { Continue(); });
		sum = sum + i;
	});
	return sum;
}
int64_t cfForContinueRef(int64_t n) {
	int64_t sum = 0;
	for (int64_t i = 0; i < n; ++i) {
		if (i % 2 == 0) {
			continue;
		}
		sum += i;
	}
	return sum;
}

// ops after a Break in the same lambda run in C++ but are not recorded
val<int64_t> cfOpsAfterBreak(val<int64_t> n) {
	val<int64_t> i = 0;
	val<int64_t> sum = 0;
	While([&] { return i < n; },
	      [&] {
		      If(i == 3, [&] {
			      Break();
			      sum = sum + 1000; // not recorded
		      });
		      sum = sum + i;
		      i = i + 1;
	      });
	return sum;
}
int64_t cfOpsAfterBreakRef(int64_t n) {
	int64_t sum = 0;
	for (int64_t i = 0; i < n; ++i) {
		if (i == 3) {
			break;
		}
		sum += i;
	}
	return sum;
}

val<int64_t> cfReturnInArm(val<int64_t> x) {
	If(x < 0, [&] { Return(val<int64_t>(-1)); });
	return x * 2;
}
int64_t cfReturnInArmRef(int64_t x) {
	return x < 0 ? -1 : x * 2;
}

val<int64_t> cfReturnNested(val<int64_t> n) {
	val<int64_t> sum = 0;
	For(val<int64_t>(0), n, [&](val<int64_t> i) {
		If(i == 3, [&] { If(n > 4, [&] { Return(sum * 100); }); });
		sum = sum + i;
	});
	return sum + 7;
}
int64_t cfReturnNestedRef(int64_t n) {
	int64_t sum = 0;
	for (int64_t i = 0; i < n; ++i) {
		if (i == 3 && n > 4) {
			return sum * 100;
		}
		sum += i;
	}
	return sum + 7;
}

// an arm in which every path leaves: the code after the construct is unreachable
val<int64_t> cfAllArmsLeave(val<int64_t> n) {
	val<int64_t> sum = 0;
	val<int64_t> i = 0;
	While([&] { return i < n; },
	      [&] {
		      If(i > 2, [&] { Break(); }, [&] { Continue(); });
		      sum = sum + 1000; // unreachable
	      });
	i = i + 1;
	return sum + i;
}
int64_t cfAllArmsLeaveRef(int64_t n) {
	// the loop never advances i inside the body: Continue re-checks i < n forever for n > 0, so the oracle runs only n
	// <= 0
	(void) n;
	return 1;
}

val<int64_t> cfNested(val<int64_t> n) {
	val<int64_t> total = 0;
	For(val<int64_t>(0), n, [&](val<int64_t> i) {
		For(val<int64_t>(0), i,
		    [&](val<int64_t> j) { If((i + j) % 3 == 0, [&] { total = total + 1; }, [&] { total = total + 2; }); });
	});
	return total;
}
int64_t cfNestedRef(int64_t n) {
	int64_t total = 0;
	for (int64_t i = 0; i < n; ++i) {
		for (int64_t j = 0; j < i; ++j) {
			total += (i + j) % 3 == 0 ? 1 : 2;
		}
	}
	return total;
}

// ---- mixed with native control flow --------------------------------------------------------------------------------

val<int64_t> cfNativeIfInArm(val<int64_t> x) {
	val<int64_t> r = 0;
	If(
	    x > 0,
	    [&] {
		    if (x > 10) {
			    r = 1;
		    } else {
			    r = 2;
		    }
	    },
	    [&] { r = 3; });
	if (x == 5) {
		r = r + 100;
	}
	return r;
}
int64_t cfNativeIfInArmRef(int64_t x) {
	int64_t r = x > 0 ? (x > 10 ? 1 : 2) : 3;
	return x == 5 ? r + 100 : r;
}

val<int64_t> cfNativeBeforeAndAfter(val<int64_t> x) {
	val<int64_t> r = 0;
	if (x > 3) {
		r = r + 1;
	}
	For(val<int64_t>(0), val<int64_t>(4), [&](val<int64_t> i) { r = r + i; });
	if (x < 0) {
		r = r + 50;
	}
	If(x == 7, [&] { r = r * 2; });
	if (x > 100) {
		r = r + 1000;
	}
	return r;
}
int64_t cfNativeBeforeAndAfterRef(int64_t x) {
	int64_t r = 0;
	if (x > 3) {
		r += 1;
	}
	r += 6;
	if (x < 0) {
		r += 50;
	}
	if (x == 7) {
		r *= 2;
	}
	if (x > 100) {
		r += 1000;
	}
	return r;
}

val<int64_t> cfNativeLoopInBody(val<int64_t> n) {
	val<int64_t> i = 0;
	val<int64_t> s = 0;
	While([&] { return i < n; },
	      [&] {
		      val<int64_t> j = 0;
		      while (j < i) {
			      s = s + 1;
			      j = j + 1;
		      }
		      i = i + 1;
	      });
	return s;
}
int64_t cfNativeLoopInBodyRef(int64_t n) {
	int64_t s = 0;
	for (int64_t i = 0; i < n; ++i) {
		s += i;
	}
	return s;
}

// Break inside an implicit if, and If(c, Break) followed by recorded operations in the same body
val<int64_t> cfBreakInNativeIf(val<int64_t> n) {
	val<int64_t> i = 0;
	val<int64_t> sum = 0;
	While([&] { return i < 1000; },
	      [&] {
		      if (i >= n) {
			      Break();
		      }
		      sum = sum + i;
		      i = i + 1;
	      });
	return sum;
}
int64_t cfBreakInNativeIfRef(int64_t n) {
	int64_t sum = 0;
	for (int64_t i = 0; i < 1000 && i < n; ++i) {
		sum += i;
	}
	return sum;
}

val<int64_t> cfRegionInArm(val<int64_t> x) {
	val<int64_t> r = 0;
	If(x > 0, [&] {
		region([&] {
			if (x > 5) {
				r = 1;
			} else {
				r = 2;
			}
		});
		r = r + 10;
	});
	return r;
}
int64_t cfRegionInArmRef(int64_t x) {
	return x > 0 ? (x > 5 ? 1 : 2) + 10 : 0;
}

// the explicit twin of the corpus' chainedIf
int chainLength = 30;
[[maybe_unused]] val<int64_t> cfChainedIf(val<int64_t> x) {
	val<int64_t> r = 0;
	for (static_val<int> i = 0; i < chainLength; i++) {
		If(x > i, [&] { r = r + 1; });
	}
	return r;
}
[[maybe_unused]] int64_t cfChainedIfRef(int64_t x) {
	int64_t r = 0;
	for (int i = 0; i < chainLength; ++i) {
		r += x > i ? 1 : 0;
	}
	return r;
}

// ---- owned values
// -----------------------------------------------------------------------------------------------------

struct Tracked {
	static inline int constructed = 0;
	static inline int destroyed = 0;
	Tracked() {
		++constructed;
	}
	Tracked(const Tracked&) {
		++constructed;
	}
	~Tracked() {
		++destroyed;
	}
};

val<int64_t> cfOwnedBreak(val<int64_t> n) {
	val<Tracked> outer;
	val<int64_t> i = 0;
	val<int64_t> sum = 0;
	While([&] { return i < n; },
	      [&] {
		      val<Tracked> inner;
		      If(i == 3, [&] { Break(); });
		      sum = sum + i;
		      i = i + 1;
	      });
	return sum;
}
int64_t cfOwnedBreakRef(int64_t n) {
	int64_t sum = 0;
	for (int64_t i = 0; i < n && i != 3; ++i) {
		sum += i;
	}
	return sum;
}

val<int64_t> cfOwnedReturn(val<int64_t> n) {
	val<Tracked> outer;
	For(val<int64_t>(0), n, [&](val<int64_t> i) {
		val<Tracked> inner;
		If(i == 2, [&] { Return(i); });
	});
	return val<int64_t>(-1);
}
int64_t cfOwnedReturnRef(int64_t n) {
	return n > 2 ? 2 : -1;
}

// ---- errors
// -------------------------------------------------------------------------------------------------------------

[[maybe_unused]] val<int64_t> cfBreakThroughRegion(val<int64_t> n) {
	val<int64_t> i = 0;
	While([&] { return i < n; },
	      [&] {
		      region([&] { Break(); });
		      i = i + 1;
	      });
	return i;
}

[[maybe_unused]] val<int64_t> cfBreakOutsideLoop(val<int64_t> n) {
	If(n > 0, [&] { Break(); });
	return n;
}

// ---- the harness
// ---------------------------------------------------------------------------------------------------------

/// The engines that trace with the NIR tracer, where every construct is recorded as blocks, and the interpreter.
std::vector<std::string> nirEngines() {
	std::vector<std::string> names {"interpreter"};
#ifdef ENABLE_TRACING
	for (const auto& backend : testing::availableBackends()) {
		if (backend.rfind("legacy-", 0) != 0 && backend.rfind("nir-", 0) != 0) {
			names.push_back(backend);
		}
	}
#endif
	return names;
}

/// The engines that compile, with the NIR tracer.
std::vector<std::string> tracedEngines() {
	auto names = nirEngines();
	names.erase(names.begin());
	return names;
}

/// The engines that trace with the 0.3 tracer, where the constructs trace as the native control flow they stand for.
std::vector<std::string> legacyEngines() {
	std::vector<std::string> names;
#ifdef ENABLE_TRACING
	for (const auto& backend : testing::availableBackends()) {
		if (backend.rfind("legacy-", 0) == 0) {
			names.push_back(backend);
		}
	}
#endif
	return names;
}

void checkAgainstOracle(const std::vector<std::string>& engines, val<int64_t> (*function)(val<int64_t>),
                        int64_t (*oracle)(int64_t), const std::vector<int64_t>& inputs,
                        const testing::OptionsTweak& tweak = {}) {
	for (const auto& name : engines) {
		DYNAMIC_SECTION(name) {
			auto engine = testing::makeEngine(name, tweak);
			auto compiled = engine.registerFunction(function);
			for (const auto x : inputs) {
				INFO("input " << x);
				CHECK(compiled(x) == oracle(x));
			}
		}
	}
}

[[maybe_unused]] int64_t counter(const compiler::CompilationStatistics& stats, const char* key) {
	const auto* value = stats.find(key);
	REQUIRE(value != nullptr);
	return std::get<int64_t>(*value);
}

} // namespace

TEST_CASE("Explicit control flow: If, value If, For, While with Break and Continue", "[explicit-control-flow]") {
	const std::vector<int64_t> small {-3, 0, 1, 2, 5, 9, 13};
	SECTION("If") {
		checkAgainstOracle(nirEngines(), cfIfElse, cfIfElseRef, small);
	}
	SECTION("value If") {
		checkAgainstOracle(nirEngines(), cfValueIf, cfValueIfRef, {-5, 0, 40, 50, 51, 200});
	}
	SECTION("For") {
		checkAgainstOracle(nirEngines(), cfSumFor, cfSumForRef, small);
	}
	SECTION("While with Break in an If") {
		checkAgainstOracle(nirEngines(), cfWhileBreak, cfWhileBreakRef, {0, 1, 7, 99, 150});
	}
	SECTION("For with Continue") {
		checkAgainstOracle(nirEngines(), cfForContinue, cfForContinueRef, {0, 1, 2, 10});
	}
	SECTION("operations after Break in the same lambda") {
		checkAgainstOracle(nirEngines(), cfOpsAfterBreak, cfOpsAfterBreakRef, {0, 2, 3, 4, 10});
	}
	SECTION("nested loops and arms") {
		checkAgainstOracle(nirEngines(), cfNested, cfNestedRef, {0, 1, 2, 5, 8});
	}
}

TEST_CASE("Explicit control flow: Return from nested arms", "[explicit-control-flow]") {
	SECTION("Return in an arm") {
		checkAgainstOracle(tracedEngines(), cfReturnInArm, cfReturnInArmRef, {-4, 0, 4});
	}
	SECTION("Return in a nested arm of a loop") {
		checkAgainstOracle(tracedEngines(), cfReturnNested, cfReturnNestedRef, {0, 3, 4, 5, 9});
	}
}

TEST_CASE("Explicit control flow: an arm whose paths all leave makes the rest unreachable", "[explicit-control-flow]") {
	for (const auto& name : tracedEngines()) {
		DYNAMIC_SECTION(name) {
			auto engine = testing::makeEngine(name);
			auto compiled = engine.registerFunction(cfAllArmsLeave);
			// n <= 0 skips the loop; with n > 0 the first iterations Continue (i stays 0 > 2 is false) forever, so only
			// the loop-free input is run
			CHECK(compiled(0) == cfAllArmsLeaveRef(0));
			CHECK(compiled(-1) == cfAllArmsLeaveRef(-1));
		}
	}
}

TEST_CASE("Explicit control flow mixed with native control flow", "[explicit-control-flow]") {
	SECTION("a native if inside an arm") {
		checkAgainstOracle(nirEngines(), cfNativeIfInArm, cfNativeIfInArmRef, {-1, 0, 3, 5, 10, 11, 40});
	}
	SECTION("native ifs before and after a construct") {
		checkAgainstOracle(nirEngines(), cfNativeBeforeAndAfter, cfNativeBeforeAndAfterRef,
		                   {-10, 0, 4, 7, 50, 101, 1000});
	}
	SECTION("a native loop in a loop body") {
		checkAgainstOracle(nirEngines(), cfNativeLoopInBody, cfNativeLoopInBodyRef, {0, 1, 4, 9});
	}
	SECTION("Break inside a native if of a While body") {
		checkAgainstOracle(nirEngines(), cfBreakInNativeIf, cfBreakInNativeIfRef, {0, 1, 5, 999, 2000});
	}
	SECTION("a region() inside an arm") {
		checkAgainstOracle(nirEngines(), cfRegionInArm, cfRegionInArmRef, {-2, 1, 5, 6, 30});
	}
	SECTION("the 0.3 tracer traces the constructs as native control flow") {
		checkAgainstOracle(legacyEngines(), cfNativeIfInArm, cfNativeIfInArmRef, {-1, 3, 5, 11});
		checkAgainstOracle(legacyEngines(), cfWhileBreak, cfWhileBreakRef, {0, 7, 150});
		checkAgainstOracle(legacyEngines(), cfForContinue, cfForContinueRef, {0, 1, 10});
		checkAgainstOracle(legacyEngines(), cfValueIf, cfValueIfRef, {-5, 40, 200});
	}
}

TEST_CASE("Explicit control flow: owned values die on the exit path", "[explicit-control-flow]") {
	for (const auto& name : nirEngines()) {
		DYNAMIC_SECTION(name) {
			auto engine = testing::makeEngine(name);
			{
				auto compiled = engine.registerFunction(cfOwnedBreak);
				Tracked::constructed = Tracked::destroyed = 0;
				CHECK(compiled(10) == cfOwnedBreakRef(10));
				CHECK(Tracked::constructed == Tracked::destroyed);
				CHECK(Tracked::constructed ==
				      1 + 4); // the outer value and one inner per iteration, the last one broken
				Tracked::constructed = Tracked::destroyed = 0;
				CHECK(compiled(2) == cfOwnedBreakRef(2));
				CHECK(Tracked::constructed == Tracked::destroyed);
				CHECK(Tracked::constructed == 1 + 2);
			}
			if (name != "interpreter") {
				auto compiled = engine.registerFunction(cfOwnedReturn);
				Tracked::constructed = Tracked::destroyed = 0;
				CHECK(compiled(10) == cfOwnedReturnRef(10));
				CHECK(Tracked::constructed == Tracked::destroyed);
				CHECK(Tracked::constructed == 1 + 3);
				Tracked::constructed = Tracked::destroyed = 0;
				CHECK(compiled(1) == cfOwnedReturnRef(1));
				CHECK(Tracked::constructed == Tracked::destroyed);
			}
		}
	}
}

#ifdef ENABLE_TRACING
TEST_CASE("Explicit control flow: exits do not cross a region, and need a loop", "[explicit-control-flow]") {
	for (const auto& name : nirEngines()) {
		if (name == "interpreter") {
			continue;
		}
		DYNAMIC_SECTION(name) {
			auto engine = testing::makeEngine(name);
			CHECK_THROWS(engine.registerFunction(cfBreakThroughRegion));
			CHECK_THROWS(engine.registerFunction(cfBreakOutsideLoop));
		}
	}
}

TEST_CASE("Explicit control flow traces in one pass", "[explicit-control-flow][statistics]") {
	for (const auto& name : tracedEngines()) {
		DYNAMIC_SECTION(name) {
			for (const int length : {1, 10, 30}) {
				chainLength = length;
				DYNAMIC_SECTION("chainedIf " << length) {
					auto engine = testing::makeEngine(name);
					auto compiled = engine.registerFunction(cfChainedIf);
					for (const int64_t x : {-1, 0, 3, 100}) {
						CHECK(compiled(x) == cfChainedIfRef(x));
					}
					auto stats = compiled.getStatistics();
					REQUIRE(stats != nullptr);
					CHECK(counter(*stats, "tracing.iterations") == 1 + 2 * length); // the function, and two arms per if
					CHECK(counter(*stats, "tracing.replayedOperations") == 0);
					CHECK(counter(*stats, "tracing.pausedOperations") == 0);
					CHECK(counter(*stats, "tracing.constructs") == length);
					CHECK(counter(*stats, "tracing.constructRestarts") == 0);
				}
			}
			chainLength = 30;
		}
	}
}

TEST_CASE("Explicit control flow: snapshots are one per construct and per operation outside constructs",
          "[explicit-control-flow][statistics]") {
	auto snapshotsOf = [&](const std::string& name, auto function) {
		auto engine = testing::makeEngine(name);
		auto compiled = engine.registerFunction(function);
		compiled(1);
		auto stats = compiled.getStatistics();
		REQUIRE(stats != nullptr);
		return counter(*stats, "tracing.snapshots");
	};
	for (const auto& name : tracedEngines()) {
		DYNAMIC_SECTION(name) {
			// the same function with bigger arms and a bigger loop body: the operations inside constructs take none
			const auto small = snapshotsOf(
			    name, +[](val<int64_t> x) -> val<int64_t> {
				    val<int64_t> r = 0;
				    If(x > 0, [&] { r = r + 1; });
				    return r;
			    });
			const auto large = snapshotsOf(
			    name, +[](val<int64_t> x) -> val<int64_t> {
				    val<int64_t> r = 0;
				    If(x > 0, [&] {
					    r = r + 1;
					    r = r * 3;
					    r = r - x;
					    r = r + x * 7;
					    r = r ^ 5;
				    });
				    return r;
			    });
			CHECK(small == large);
			// per construct: one snapshot; outside: one per recorded operation (the constant, the comparison, the
			// constant of the comparison's literal, the return)
			const auto none = snapshotsOf(
			    name, +[](val<int64_t> x) -> val<int64_t> {
				    val<int64_t> r = 0;
				    if (x > 0) {
					    r = r + 1;
				    }
				    return r;
			    });
			CHECK(small < none);
		}
	}
}

TEST_CASE("Explicit control flow: restarts roll back what an arm recorded", "[explicit-control-flow][statistics]") {
	// an implicit branch inside an arm restarts the arm with snapshots
	for (const auto& name : tracedEngines()) {
		DYNAMIC_SECTION(name) {
			auto engine = testing::makeEngine(name);
			auto compiled = engine.registerFunction(cfNativeIfInArm);
			CHECK(compiled(20) == cfNativeIfInArmRef(20));
			CHECK(compiled(5) == cfNativeIfInArmRef(5));
			CHECK(compiled(-1) == cfNativeIfInArmRef(-1));
			auto stats = compiled.getStatistics();
			REQUIRE(stats != nullptr);
			CHECK(counter(*stats, "tracing.constructRestarts") == 1);
		}
	}
}

TEST_CASE("Explicit control flow: every arm restarted once gives the same results", "[explicit-control-flow]") {
	const auto force = [](Options& options) {
		options.setOption("tracing.forceConstructRestarts", true);
	};
	const auto engines = tracedEngines();
	SECTION("If and loops") {
		checkAgainstOracle(engines, cfNested, cfNestedRef, {0, 1, 5}, force);
		checkAgainstOracle(engines, cfWhileBreak, cfWhileBreakRef, {0, 7, 150}, force);
		checkAgainstOracle(engines, cfForContinue, cfForContinueRef, {0, 1, 10}, force);
		checkAgainstOracle(engines, cfOpsAfterBreak, cfOpsAfterBreakRef, {0, 3, 10}, force);
	}
	SECTION("Return and mixed") {
		checkAgainstOracle(engines, cfReturnNested, cfReturnNestedRef, {0, 3, 9}, force);
		checkAgainstOracle(engines, cfNativeIfInArm, cfNativeIfInArmRef, {-1, 5, 11}, force);
		checkAgainstOracle(engines, cfRegionInArm, cfRegionInArmRef, {-2, 5, 6}, force);
		checkAgainstOracle(engines, cfBreakInNativeIf, cfBreakInNativeIfRef, {0, 5, 2000}, force);
	}
	SECTION("a restart is counted") {
		auto engine = testing::makeEngine(engines.front(), force);
		auto compiled = engine.registerFunction(cfNested);
		CHECK(compiled(4) == cfNestedRef(4));
		auto stats = compiled.getStatistics();
		REQUIRE(stats != nullptr);
		CHECK(counter(*stats, "tracing.constructRestarts") > 0);
	}
}

TEST_CASE("Explicit control flow: the explicit twin is traced in fewer passes than the implicit chain",
          "[explicit-control-flow][statistics]") {
	const auto engines = tracedEngines();
	if (engines.empty()) {
		SKIP("No compilation backend available");
	}
	chainLength = 20;
	auto implicitChain = +[](val<int64_t> x) -> val<int64_t> {
		val<int64_t> r = 0;
		for (static_val<int> i = 0; i < chainLength; i++) {
			if (x > i) {
				r = r + 1;
			}
		}
		return r;
	};
	auto engine = testing::makeEngine(engines.front());
	auto implicit = engine.registerFunction(implicitChain);
	auto explicitChain = engine.registerFunction(cfChainedIf);
	implicit(5);
	explicitChain(5);
	auto implicitStats = implicit.getStatistics();
	auto explicitStats = explicitChain.getStatistics();
	REQUIRE(implicitStats != nullptr);
	REQUIRE(explicitStats != nullptr);
	CHECK(counter(*implicitStats, "tracing.replayedOperations") > 0);
	CHECK(counter(*explicitStats, "tracing.replayedOperations") == 0);
	chainLength = 30;
}
#endif

} // namespace nautilus::engine
