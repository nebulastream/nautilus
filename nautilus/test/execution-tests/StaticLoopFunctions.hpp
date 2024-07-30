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

} // namespace nautilus::engine
