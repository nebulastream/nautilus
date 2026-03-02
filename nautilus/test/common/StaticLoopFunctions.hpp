#pragma once

#include <nautilus/Engine.hpp>

namespace nautilus::engine {

val<int32_t> staticLoop() {
	val<int32_t> agg = val<int32_t>(1);
	for (static_val<int> start = 0; start < 10; start = start + 1) {
		agg = agg + 10;
	}
	return agg;
}

val<int32_t> staticLoopWithIf(val<int32_t> agg) {
	for (static_val<int> start = 0; start < 10; start = start + 1) {
		if (agg > 5) {
			agg = agg + 1;
		}
		agg = agg + 10;
	}
	return agg;
}

val<int32_t> staticLoopWithDynamicLoop(val<int32_t> counter) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < counter; i = i + 1) {
		for (static_val<int> start = 0; start < 3; start = start + 1) {
			if (i > 5) {
				sum = sum + 1;
			}
			sum = sum + 10;
		}
	}
	return sum;
}

val<int32_t> staticLoopWithDynamicLoopPostIncrement(val<int32_t> counter) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < counter; i++) {
		for (static_val<int> start = 0; start < 3; start++) {
			if (i > 5) {
				sum = sum + 1;
			}
			sum = sum + 10;
		}
	}
	return sum;
}

val<int32_t> staticLoopWithDynamicLoopPreIncrement(val<int32_t> counter) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < counter; ++i) {
		for (static_val<int> start = 0; start < 3; ++start) {
			if (i > 5) {
				sum = sum + 1;
			}
			sum = sum + 10;
		}
	}
	return sum;
}

val<int32_t> staticLoopWithDynamicLoopNotEqual(val<int32_t> counter) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i != counter; i++) {
		for (static_val<int> start = 0; start != 3; start++) {
			if (i > 5) {
				sum = sum + 1;
			}
			sum = sum + 10;
		}
	}
	return sum;
}

std::vector<int> vec = {1, 2, 3, 4, 5};

val<int32_t> staticIterator(val<int32_t> ref) {
	val<int32_t> sum = 0;
	for (auto x : static_iterable(vec)) {
		if (ref > x) {
			sum = sum + 1;
		}
	}
	return sum;
}

const std::vector<int> constVec = {1, 2, 3, 4, 5};

val<int32_t> staticConstIterator(val<int32_t> ref) {
	val<int32_t> sum = 0;
	for (auto x : static_iterable(constVec)) {
		if (ref > x) {
			sum = sum + 1;
		}
	}
	return sum;
}

val<int32_t> staticLoopIncrement() {
	val<int32_t> agg = val<int32_t>(1);
	for (static_val<int> start = 0; start < 10; start++) {
		agg = agg + 10;
	}
	return agg;
}

val<int32_t> staticWhileLoopDecrement() {
	val<int32_t> agg = val<int32_t>(1);
	static_val<int32_t> upperLimit = 10;
	while (upperLimit >= 0) {
		agg = agg + upperLimit;
		upperLimit--;
	}

	return agg;
}

// --- New static_val tracing test variations ---

// Pre-increment variant of static for loop
val<int32_t> staticPreIncrement() {
	val<int32_t> agg = val<int32_t>(1);
	for (static_val<int> start = 0; start < 10; ++start) {
		agg = agg + 10;
	}
	return agg;
}

// Pre-decrement while loop
val<int32_t> staticPreDecrement() {
	val<int32_t> agg = val<int32_t>(1);
	static_val<int32_t> limit = 10;
	while (limit > 0) {
		agg = agg + limit;
		--limit;
	}
	return agg;
}

// Two nested static loops, both fully unrolled
val<int32_t> staticNestedLoop() {
	val<int32_t> agg = val<int32_t>(0);
	for (static_val<int> i = 0; i < 3; ++i) {
		for (static_val<int> j = 0; j < 4; ++j) {
			agg = agg + 1;
		}
	}
	return agg;
}

// Multiple val accumulators modified inside a single static loop
val<int32_t> staticLoopMultipleAccumulators() {
	val<int32_t> sum = val<int32_t>(0);
	val<int32_t> product = val<int32_t>(1);
	for (static_val<int> i = 0; i < 5; ++i) {
		sum = sum + 10;
		product = product + product;
	}
	return sum + product;
}

// Boundary: static loop that runs exactly once
val<int32_t> staticLoopSingleIteration() {
	val<int32_t> agg = val<int32_t>(0);
	for (static_val<int> i = 0; i < 1; ++i) {
		agg = agg + 42;
	}
	return agg;
}

// Static countdown: for loop counting down
val<int32_t> staticCountdown() {
	val<int32_t> agg = val<int32_t>(0);
	for (static_val<int> i = 5; i > 0; --i) {
		agg = agg + i;
	}
	return agg;
}

// Static loop interacting with a dynamic argument
val<int32_t> staticLoopWithArg(val<int32_t> base) {
	val<int32_t> agg = base;
	for (static_val<int> i = 0; i < 5; ++i) {
		agg = agg + 1;
	}
	return agg;
}

// Sequential static loops (two independent loops in sequence)
val<int32_t> staticSequentialLoops() {
	val<int32_t> agg = val<int32_t>(0);
	for (static_val<int> i = 0; i < 3; ++i) {
		agg = agg + 10;
	}
	for (static_val<int> j = 0; j < 2; ++j) {
		agg = agg + 100;
	}
	return agg;
}

// Static loop accumulating the static counter value into a dynamic val
val<int32_t> staticLoopAccumulateCounter() {
	val<int32_t> agg = val<int32_t>(0);
	for (static_val<int32_t> i = 1; i <= 5; ++i) {
		agg = agg + i;
	}
	return agg;
}

// Larger array for iteration test
std::vector<int> largeVec = {10, 20, 30, 40, 50, 60, 70, 80};

val<int32_t> staticIteratorSum(val<int32_t> ref) {
	val<int32_t> sum = 0;
	for (auto x : static_iterable(largeVec)) {
		sum = sum + x;
	}
	return sum + ref;
}

} // namespace nautilus::engine
