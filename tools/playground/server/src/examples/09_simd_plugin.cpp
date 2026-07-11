// title: SIMD plugin: vectorized aggregation
// description: val<vec<T>> processes Lanes() elements per step and lowers to real vector instructions in MLIR/LLVM.
#include <nautilus/Engine.hpp>
#include <nautilus/val.hpp>
#include <nautilus/vector.hpp> // nautilus-simd plugin

using namespace nautilus;

// The SIMD-vectorized variant of the classic conditional-sum kernel (see
// "Pointers & memory"). The mask holds 0 (skip) or 1 (include) per element;
// multiplying value and mask vectors zeroes out the masked lanes.
//
// vec<int32_t>::Lanes() probes the CPU *at tracing time* (SSE/NEON = 4,
// AVX2 = 8, AVX-512 = 16 lanes) and is captured as a compile-time constant,
// so the generated code is specialized for the machine tracing it. With the
// MLIR backend, Load/Mul/ReduceAdd lower to MLIR's vector dialect — look
// for `vector<Nxi32>` types in the MLIR tab and `<N x i32>` in the LLVM IR.
void playground_register(engine::NautilusModule& m) {
	m.registerFunction<val<int32_t>(val<int32_t>, val<const int32_t*>, val<const int32_t*>)>(
	    "conditionalSumSimd", [](val<int32_t> size, val<const int32_t*> values, val<const int32_t*> mask) {
		    const size_t lanes = vec<int32_t>::Lanes();
		    const val<int32_t> stride = static_cast<int32_t>(lanes);

		    val<int32_t> sum = 0;
		    val<int32_t> i = 0;

		    // Main vector loop: Lanes() elements per iteration.
		    while (i + stride <= size) {
			    auto v = val<vec<int32_t>>::Load(values + i);
			    auto msk = val<vec<int32_t>>::Load(mask + i);
			    sum += (v * msk).ReduceAdd();
			    i = i + stride;
		    }

		    // Scalar tail for the remaining size % Lanes() elements.
		    while (i < size) {
			    if (mask[i] != 0) {
				    sum += values[i];
			    }
			    i = i + 1;
		    }
		    return sum;
	    });
}
