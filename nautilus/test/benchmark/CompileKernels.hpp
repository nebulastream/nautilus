#pragma once

// The kernels of the compile-time harness (CompileTimeBench.cpp) and of the tracing profiling driver
// (TracingCorpus.cpp). `kernelSize` is read at trace time, so one function covers every size.

#include "nautilus/function.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/region.hpp"
#include "nautilus/static.hpp"
#include <cstdint>

namespace compile_kernels {
using namespace nautilus;

// N is read at trace time, so one function covers every size.
inline int kernelSize = 0;

inline val<int8_t> addExpression(val<int8_t> x) {
	val<int8_t> y = (int8_t) 2;
	return y + x;
}

inline val<int32_t> fib(val<int32_t> n) {
	val<int32_t> a = 0, b = 1, c;
	for (val<int> i = 2; i <= n; i = i + 1) {
		c = a + b;
		a = b;
		b = c;
	}
	return b;
}

inline val<int64_t> sumArr(val<int32_t*> array, val<int32_t> length) {
	val<int32_t> s = val<int32_t>(0);
	for (val<int32_t> i = 0; i < length; i = i + 1) {
		val<int32_t> v = array[i];
		s = s + v;
	}
	return val<int64_t>(s);
}

inline val<int64_t> callCalleeBody(val<int64_t> x, val<int64_t> y) {
	return x * 2 + y;
}
inline auto callBenchCallee = NautilusFunction {"callBenchCallee", callCalleeBody};

inline val<int64_t> internalCallLoop(val<int32_t> n) {
	val<int64_t> acc = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		acc = callBenchCallee(acc, val<int64_t>(1));
	}
	return acc;
}

// Straight-line code: four operations per iteration of a static_val loop, no branches.
inline val<int64_t> straight(val<int64_t> x) {
	val<int64_t> acc = x;
	for (static_val<int32_t> i = 0; i < kernelSize; i++) {
		acc = (acc * 3 + i) ^ (acc >> 1);
	}
	return acc;
}

// N calls of a Nautilus function, unrolled by a static_val loop.
inline val<int64_t> calls(val<int64_t> x) {
	val<int64_t> acc = x;
	for (static_val<int32_t> i = 0; i < kernelSize; i++) {
		acc = callBenchCallee(acc, val<int64_t>(1));
	}
	return acc;
}

// N independent ifs in a row. A plain `for (int i...)` loop around an if is not a shape that traces (the loop
// counter is not part of the tag), so the iterations are unrolled by a static_val: every if adds a traced path.
inline val<int64_t> chainedIf(val<int64_t> x) {
	val<int64_t> r = 0;
	for (static_val<int32_t> i = 0; i < kernelSize; i++) {
		if (x > i) {
			r = r + 1;
		}
	}
	return r;
}

// The same with an else arm.
inline val<int64_t> chainedIfElse(val<int64_t> x) {
	val<int64_t> r = 0;
	for (static_val<int32_t> i = 0; i < kernelSize; i++) {
		if (x > i) {
			r = r + 1;
		} else {
			r = r - 2;
		}
	}
	return r;
}

// Each if inside its own region(): replay is bounded to the region.
inline val<int64_t> chainedIfRegion(val<int64_t> x) {
	val<int64_t> r = 0;
	for (static_val<int32_t> i = 0; i < kernelSize; i++) {
		region([&]() {
			if (x > i) {
				r = r + 1;
			}
		});
	}
	return r;
}

// A val loop around N unrolled operations.
inline val<int64_t> loopBody(val<int64_t> n) {
	val<int64_t> r = 1;
	for (val<int64_t> j = 0; j < n; j = j + 1) {
		for (static_val<int32_t> i = 0; i < kernelSize; i++) {
			r = r + j * i;
		}
	}
	return r;
}

} // namespace compile_kernels
