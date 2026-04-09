// DemoSimdPlugin.cpp — showcase the `nautilus-simd` plugin.
//
// The simd plugin adds a `val<vec<T>>` specialization that lowers to real
// SIMD instructions in the compiled code (via MLIR's vector dialect) or
// falls back to a lane-count scalar loop when the backend cannot emit
// vector ops.
//
// ## How lane count maps to hardware
//
// `vec<T>::Lanes()` calls `RuntimeSimdWidth()` which probes the CPU at
// runtime (cpuid on x86, feature registers on ARM) and returns the SIMD
// register width **in bytes**:
//
//   16 bytes  →  SSE2     (x86)  or  NEON    (ARM)
//   32 bytes  →  AVX/AVX2 (x86)
//   64 bytes  →  AVX-512  (x86)
//
// The lane count for a given element type is then:
//
//   Lanes = RegisterWidthInBytes / sizeof(T)
//
// Examples with a 64-byte (AVX-512) register:
//   vec<int32_t>::Lanes()  = 64 / 4  = 16  (sixteen 32-bit ints per register)
//   vec<float>::Lanes()    = 64 / 4  = 16
//   vec<double>::Lanes()   = 64 / 8  =  8  (eight 64-bit doubles)
//   vec<int16_t>::Lanes()  = 64 / 2  = 32
//
// With a 32-byte (AVX2) register the same types give 8, 8, 4, 16 lanes.
//
// This value is queried once, at tracing time.  From the tracer's point of
// view it is a plain `size_t` constant — the JIT-compiled code is therefore
// specialised for the width of the machine it runs on.  A single
// `val<vec<int32_t>>::Load(ptr)` becomes, for example, one `vmovdqu`
// (AVX-512) or one `vld1q` (NEON) in the generated assembly.
//
// The MLIR backend lowers `val<vec<T>>` operations to MLIR's vector dialect
// which maps directly to LLVM vector types.  LLVM's backend then selects
// the right hardware instruction during code generation.
//
// ## This demo
//
// Re-implements the conditionalSum problem from DemoJit.cpp, but processes
// `Lanes()` elements at a time using `val<vec<int32_t>>`.  A scalar tail
// loop handles the final `size % Lanes()` elements.

#include <cstdint>
#include <iostream>
#include <nautilus/Engine.hpp>
#include <nautilus/val.hpp>
#include <nautilus/vector.hpp> // simd plugin
#include <vector>

using namespace nautilus;

// SIMD-vectorised conditionalSum.
//
// `mask` is expected to contain 0 (skip) or 1 (include) in each lane — the
// kernel multiplies the value vector by the mask vector so masked-out lanes
// contribute zero to the running accumulator.
val<int32_t> conditionalSumSimd(val<int32_t> size, val<const int32_t*> values, val<const int32_t*> mask) {
	// Lanes() is a C++ runtime query — during tracing it returns an ordinary
	// size_t which the trace captures as a compile-time constant step size.
	const size_t L = vec<int32_t>::Lanes();
	const val<int32_t> stride = static_cast<int32_t>(L);

	val<int32_t> sum = 0;
	val<int32_t> i = 0;

	// Main vector loop: process L elements per iteration.
	while (i + stride <= size) {
		auto v = val<vec<int32_t>>::Load(values + i);
		auto m = val<vec<int32_t>>::Load(mask + i);
		sum += (v * m).ReduceAdd();
		i = i + stride;
	}

	// Scalar tail for the remaining size % L elements.
	while (i < size) {
		if (mask[i] != 0) {
			sum += values[i];
		}
		i = i + 1;
	}
	return sum;
}

int main(int, char*[]) {
	engine::Options options;
	// Use the MLIR backend directly so the vector intrinsics lower to real
	// SIMD instructions (e.g. vmovdqu / vpmulld on AVX-512) instead of
	// falling back to scalar loops through the bytecode interpreter.
	// Use the MLIR backend directly (not tiered) so the vector intrinsics
	// lower to real SIMD instructions in a single synchronous compilation
	// pass, rather than first running through the bytecode interpreter.
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.compilationStrategy", std::string("legacy"));
	auto engine = engine::NautilusEngine(options);

	const size_t widthBytes = RuntimeSimdWidth();
	std::cout << "Backend:              " << engine.getNameOfBackend() << "\n";
	std::cout << "SIMD register width:  " << widthBytes << " bytes";
	if (widthBytes == 16) {
		std::cout << "  (SSE2 / NEON)";
	} else if (widthBytes == 32) {
		std::cout << "  (AVX / AVX2)";
	} else if (widthBytes == 64) {
		std::cout << "  (AVX-512)";
	}
	std::cout << "\n";
	std::cout << "vec<int32_t>::Lanes() = " << widthBytes << " / sizeof(int32_t) = " << vec<int32_t>::Lanes() << "\n";
	std::cout << "vec<float>::Lanes()   = " << widthBytes << " / sizeof(float)   = " << vec<float>::Lanes() << "\n";
	std::cout << "vec<double>::Lanes()  = " << widthBytes << " / sizeof(double)  = " << vec<double>::Lanes() << "\n\n";

	auto fn = engine.registerFunction(conditionalSumSimd);

	// Same payload as DemoJit.cpp: sum of {1, 2, 4} → 7.
	std::vector<int32_t> values = {1, 2, 3, 4};
	std::vector<int32_t> mask = {1, 1, 0, 1};
	auto resultSmall = fn(static_cast<int32_t>(values.size()), values.data(), mask.data());
	std::cout << "conditionalSumSimd([1,2,3,4], mask=[1,1,0,1]) = " << resultSmall << "  (expected 7)\n";

	// Larger payload where the main vector loop actually engages: sum of all
	// even values in [0, 32).
	constexpr int32_t N = 32;
	std::vector<int32_t> bigValues(N);
	std::vector<int32_t> bigMask(N);
	int32_t expected = 0;
	for (int32_t k = 0; k < N; k++) {
		bigValues[k] = k;
		bigMask[k] = (k % 2 == 0) ? 1 : 0;
		if (bigMask[k]) {
			expected += bigValues[k];
		}
	}
	auto resultBig = fn(N, bigValues.data(), bigMask.data());
	std::cout << "conditionalSumSimd(sum of evens in [0,32)) = " << resultBig << "  (expected " << expected << ")\n";

	return 0;
}
