#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/region.hpp>

/// Fixtures for the region golden traces (test/data/region-tests).
///
/// Every region() call site records the position it was written at (docs/region.md), so
/// the line numbers below are part of the checked-in dumps. Editing this file shifts them:
/// delete the affected files under test/data/region-tests and re-run the tracing tests
/// twice -- the first run rewrites them, the second passes.
///
/// The golden comparison normalises the source path to its file name and drops the column,
/// because a compiler chooses the column itself: for one and the same call, GCC reports the
/// callee's closing position and Clang the start of the expression. See
/// testing::normalizeSourceLocations.
namespace nautilus::engine {

/// The simplest region there is: a name, a location, and one traced operation inside.
val<int64_t> regionNamed(val<int64_t> x) {
	val<int64_t> sum = 0;
	region("accumulate", [&]() { sum = x + 42; });
	return sum;
}

/// The same, unnamed: the region still carries where it was written.
val<int64_t> regionUnnamed(val<int64_t> x) {
	val<int64_t> sum = 0;
	region([&]() { sum = x + 42; });
	return sum;
}

/// A region inside a region: the inner one's operations name it, and it names the outer.
val<int64_t> regionNested(val<int64_t> x) {
	val<int64_t> sum = 0;
	region("outer", [&]() {
		region("inner", [&]() { sum = x + 1; });
		sum = sum + 2;
	});
	return sum;
}

/// A branch inside a region: its arms are blocks that outlive the region's own seams, so
/// they are where a block-level region shows up in the IR dumps.
val<int64_t> regionBranch(val<int64_t> x) {
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

} // namespace nautilus::engine
