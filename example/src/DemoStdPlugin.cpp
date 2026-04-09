// DemoStdPlugin.cpp — showcase the `nautilus-std` plugin.
//
// The std plugin exposes C++ standard-library types and functions as
// traceable nautilus primitives. Each header wraps a different part of
// libstdc++/libc++:
//
//   <nautilus/std/vector.h>    val<std::vector<T>>  (operator[], size(), ...)
//   <nautilus/std/cmath.h>     nautilus::sqrt, nautilus::pow, ...
//   <nautilus/std/string.h>    val<std::string>
//   <nautilus/std/bit.h>       popcount, clz, ctz
//   <nautilus/std/cstring.h>   memcpy, memset
//
// This demo re-implements the conditionalSum problem from DemoJit.cpp using
// `val<std::vector<int32_t>>` — the same logical aggregation, but with a
// container-based API instead of raw pointers. It then adds an `rmsError`
// function that combines vector iteration with `nautilus::sqrt` from the
// cmath wrapper.
//
// NOTE ON THE COMPILER: main() does not override any engine options, so the
// engine runs with the default tiered JIT compiler. See
// nautilus/src/nautilus/compiler/Engine.cpp — when
// `engine.compilationStrategy` is unset it defaults to "tiered" and
// constructs a TieredJITCompiler. That means this demo is exercising the
// production-recommended compilation pipeline.

#include <cstdint>
#include <iostream>
#include <nautilus/Engine.hpp>
#include <nautilus/std/cmath.h>
#include <nautilus/std/vector.h>
#include <nautilus/val.hpp>
#include <vector>

using namespace nautilus;

// conditionalSum over std::vector inputs. A mask entry of 0 means "skip",
// anything non-zero means "include". The caller owns the vectors, so each
// wrapper is released at the end to prevent the val<std::vector<...>>
// destructor from deleting the external storage.
val<int32_t> conditionalSumVec(val<std::vector<int32_t>*> values_ptr, val<std::vector<int32_t>*> mask_ptr) {
	val<std::vector<int32_t>> values(values_ptr);
	val<std::vector<int32_t>> mask(mask_ptr);

	val<int32_t> sum = 0;
	for (val<size_t> i = 0; i < values.size(); i++) {
		if (mask[i] != 0) {
			sum += values[i];
		}
	}

	// Release ownership so ~val<std::vector<...>> does not free the caller's data.
	values.release();
	mask.release();
	return sum;
}

// Root-mean-square error between two equal-length vectors of doubles.
// Exercises both `val<std::vector<double>>` iteration and `nautilus::sqrt`
// from the std cmath wrapper.
val<double> rmsError(val<std::vector<double>*> a_ptr, val<std::vector<double>*> b_ptr) {
	val<std::vector<double>> a(a_ptr);
	val<std::vector<double>> b(b_ptr);

	val<double> acc = 0.0;
	val<size_t> n = a.size();
	for (val<size_t> i = 0; i < n; i++) {
		val<double> diff = a[i] - b[i];
		acc += diff * diff;
	}
	val<double> mean = acc / n;
	val<double> result = nautilus::sqrt(mean);

	a.release();
	b.release();
	return result;
}

int main(int, char*[]) {
	// Default options → tiered JIT compiler (see file header).
	engine::Options options;
	auto engine = engine::NautilusEngine(options);
	std::cout << "Using default engine configuration (tiered JIT compiler).\n";
	std::cout << "Active backend: " << engine.getNameOfBackend() << "\n\n";

	// --- conditionalSum over std::vector ---
	auto sumFn = engine.registerFunction(conditionalSumVec);
	std::vector<int32_t> values = {1, 2, 3, 4};
	std::vector<int32_t> mask = {1, 1, 0, 1}; // include 1, 2, 4 → 7
	auto sumResult = sumFn(&values, &mask);
	std::cout << "conditionalSumVec([1,2,3,4], [1,1,0,1]) = " << sumResult << "  (expected 7)\n";

	// --- rmsError with cmath::sqrt ---
	auto rmsFn = engine.registerFunction(rmsError);
	std::vector<double> predicted = {1.0, 2.0, 3.0, 4.0};
	std::vector<double> actual = {1.5, 1.5, 3.5, 4.5};
	// diffs: -0.5, +0.5, -0.5, -0.5 → squared: 0.25 x 4 → mean 0.25 → sqrt 0.5
	auto rmsResult = rmsFn(&predicted, &actual);
	std::cout << "rmsError(...)                            = " << rmsResult << "  (expected 0.5)\n";

	return 0;
}
