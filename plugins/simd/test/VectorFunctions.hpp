#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/vector.hpp>

namespace nautilus::engine {

// ============================================================================
// Float arithmetic — val<vec<float>> with operators
// ============================================================================

void vectorAddFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = val<vec<float>>::Load(a);
	auto vb = val<vec<float>>::Load(b);
	(va + vb).Store(c);
}

void vectorSubFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	(val<vec<float>>::Load(a) - val<vec<float>>::Load(b)).Store(c);
}

void vectorMulFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	(val<vec<float>>::Load(a) * val<vec<float>>::Load(b)).Store(c);
}

void vectorDivFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	(val<vec<float>>::Load(a) / val<vec<float>>::Load(b)).Store(c);
}

void vectorNegFloat(val<const float*> a, val<float*> c) {
	(-val<vec<float>>::Load(a)).Store(c);
}

void vectorAbsFloat(val<const float*> a, val<float*> c) {
	val<vec<float>>::Load(a).Abs().Store(c);
}

// ============================================================================
// Float min/max/fma — using top-level convenience functions
// ============================================================================

void vectorMinFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	Min(val<vec<float>>::Load(a), val<vec<float>>::Load(b)).Store(c);
}

void vectorMaxFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	Max(val<vec<float>>::Load(a), val<vec<float>>::Load(b)).Store(c);
}

void vectorFmaFloat(val<const float*> a, val<const float*> b, val<const float*> c, val<float*> d) {
	Fma(val<vec<float>>::Load(a), val<vec<float>>::Load(b), val<vec<float>>::Load(c)).Store(d);
}

// ============================================================================
// Float reductions — member methods
// ============================================================================

val<float> vectorReduceAddFloat(val<const float*> a) {
	return val<vec<float>>::Load(a).ReduceAdd();
}

val<float> vectorReduceMinFloat(val<const float*> a) {
	return val<vec<float>>::Load(a).ReduceMin();
}

val<float> vectorReduceMaxFloat(val<const float*> a) {
	return val<vec<float>>::Load(a).ReduceMax();
}

// ============================================================================
// Chained operations
// ============================================================================

void vectorMulSubFloat(val<const float*> a, val<const float*> b, val<const float*> c, val<float*> d) {
	auto va = val<vec<float>>::Load(a);
	auto vb = val<vec<float>>::Load(b);
	auto vc = val<vec<float>>::Load(c);
	(va * vb - vc).Store(d);
}

// ============================================================================
// Compound assignment
// ============================================================================

void vectorCompoundAddFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = val<vec<float>>::Load(a);
	va += val<vec<float>>::Load(b);
	va.Store(c);
}

void vectorCompoundMulFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = val<vec<float>>::Load(a);
	va *= val<vec<float>>::Load(b);
	va.Store(c);
}

// ============================================================================
// Comparison operators
// ============================================================================

void vectorLtFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	(val<vec<float>>::Load(a) < val<vec<float>>::Load(b)).Store(c);
}

void vectorGeFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	(val<vec<float>>::Load(a) >= val<vec<float>>::Load(b)).Store(c);
}

void vectorEqFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	(val<vec<float>>::Load(a) == val<vec<float>>::Load(b)).Store(c);
}

void vectorNeFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	(val<vec<float>>::Load(a) != val<vec<float>>::Load(b)).Store(c);
}

void vectorLeFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	(val<vec<float>>::Load(a) <= val<vec<float>>::Load(b)).Store(c);
}

void vectorGtFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	(val<vec<float>>::Load(a) > val<vec<float>>::Load(b)).Store(c);
}

void vectorEqInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(val<vec<int32_t>>::Load(a) == val<vec<int32_t>>::Load(b)).Store(c);
}

void vectorNeInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(val<vec<int32_t>>::Load(a) != val<vec<int32_t>>::Load(b)).Store(c);
}

void vectorLtInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(val<vec<int32_t>>::Load(a) < val<vec<int32_t>>::Load(b)).Store(c);
}

void vectorGtInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(val<vec<int32_t>>::Load(a) > val<vec<int32_t>>::Load(b)).Store(c);
}

// ============================================================================
// Blend — float and int
// ============================================================================

void vectorBlendFloat(val<const float*> a, val<const float*> b, val<const float*> mask_arr, val<float*> c) {
	auto va = val<vec<float>>::Load(a);
	auto vb = val<vec<float>>::Load(b);
	auto vmask = val<vec<float>>::Load(mask_arr);
	Blend(vmask, va, vb).Store(c);
}

void vectorBlendInt(val<const int32_t*> a, val<const int32_t*> b, val<const int32_t*> mask_arr, val<int32_t*> c) {
	auto va = val<vec<int32_t>>::Load(a);
	auto vb = val<vec<int32_t>>::Load(b);
	auto vmask = val<vec<int32_t>>::Load(mask_arr);
	Blend(vmask, va, vb).Store(c);
}

// ============================================================================
// Bitwise operators — int and float
// ============================================================================

void vectorAndInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(val<vec<int32_t>>::Load(a) & val<vec<int32_t>>::Load(b)).Store(c);
}

void vectorOrInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(val<vec<int32_t>>::Load(a) | val<vec<int32_t>>::Load(b)).Store(c);
}

void vectorXorInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(val<vec<int32_t>>::Load(a) ^ val<vec<int32_t>>::Load(b)).Store(c);
}

void vectorAndFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	(val<vec<float>>::Load(a) & val<vec<float>>::Load(b)).Store(c);
}

void vectorOrFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	(val<vec<float>>::Load(a) | val<vec<float>>::Load(b)).Store(c);
}

// ============================================================================
// Compare → Blend chain (realistic pattern)
// ============================================================================

void vectorCmpBlendFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = val<vec<float>>::Load(a);
	auto vb = val<vec<float>>::Load(b);
	auto mask = va > vb;          // where a > b
	Blend(mask, va, vb).Store(c); // pick a where a > b, else b (i.e. max)
}

// ============================================================================
// Integer arithmetic + reductions
// ============================================================================

void vectorAddInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(val<vec<int32_t>>::Load(a) + val<vec<int32_t>>::Load(b)).Store(c);
}

void vectorMulInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(val<vec<int32_t>>::Load(a) * val<vec<int32_t>>::Load(b)).Store(c);
}

val<int32_t> vectorReduceAddInt(val<const int32_t*> a) {
	return val<vec<int32_t>>::Load(a).ReduceAdd();
}

val<int32_t> vectorReduceMinInt(val<const int32_t*> a) {
	return val<vec<int32_t>>::Load(a).ReduceMin();
}

val<int32_t> vectorReduceMaxInt(val<const int32_t*> a) {
	return val<vec<int32_t>>::Load(a).ReduceMax();
}

// ============================================================================
// Double
// ============================================================================

void vectorAddDouble(val<const double*> a, val<const double*> b, val<double*> c) {
	(val<vec<double>>::Load(a) + val<vec<double>>::Load(b)).Store(c);
}

// ============================================================================
// Vector<T> alias — Load / Store round-trip
// ============================================================================

void vectorAliasLoadStore(val<const float*> a, val<float*> c) {
	Vector<float>::Load(a).Store(c);
}

// ============================================================================
// vector_load<T> convenience function
// ============================================================================

void vectorConvenienceLoad(val<const float*> a, val<const float*> b, val<float*> c) {
	(vector_load<float>(a) + vector_load<float>(b)).Store(c);
}

// ============================================================================
// Realistic use cases — composite SIMD patterns
// ============================================================================

// --- Dot product: sum(a[i] * b[i]) ---
val<float> vectorDotProductFloat(val<const float*> a, val<const float*> b) {
	return (val<vec<float>>::Load(a) * val<vec<float>>::Load(b)).ReduceAdd();
}

val<int32_t> vectorDotProductInt(val<const int32_t*> a, val<const int32_t*> b) {
	return (val<vec<int32_t>>::Load(a) * val<vec<int32_t>>::Load(b)).ReduceAdd();
}

// --- Squared L2 norm: sum(a[i]^2) ---
val<float> vectorSquaredNormFloat(val<const float*> a) {
	auto va = val<vec<float>>::Load(a);
	return (va * va).ReduceAdd();
}

// --- SAXPY: y[i] = alpha * x[i] + y[i] (classic BLAS-1) ---
// Uses FMA for fused multiply-add
void vectorSaxpyFloat(val<const float*> x, val<float*> y, val<const float*> alpha_vec) {
	auto vx = val<vec<float>>::Load(x);
	auto vy = val<vec<float>>::Load(y);
	auto valpha = val<vec<float>>::Load(alpha_vec);
	Fma(valpha, vx, vy).Store(y);
}

// --- Clamp: clamp each element to [lo, hi] ---
void vectorClampFloat(val<const float*> a, val<const float*> lo_arr, val<const float*> hi_arr, val<float*> c) {
	auto va = val<vec<float>>::Load(a);
	auto vlo = val<vec<float>>::Load(lo_arr);
	auto vhi = val<vec<float>>::Load(hi_arr);
	Max(Min(va, vhi), vlo).Store(c);
}

// --- Squared Euclidean distance: sum((a[i] - b[i])^2) ---
val<float> vectorDistanceSquaredFloat(val<const float*> a, val<const float*> b) {
	auto diff = val<vec<float>>::Load(a) - val<vec<float>>::Load(b);
	return (diff * diff).ReduceAdd();
}

// --- Polynomial evaluation (Horner's method): c0 + x*(c1 + x*(c2 + x*c3)) ---
// Evaluates a degree-3 polynomial on each lane.
void vectorPolyEvalFloat(val<const float*> x_arr, val<const float*> c0_arr, val<const float*> c1_arr,
                         val<const float*> c2_arr, val<const float*> c3_arr, val<float*> out) {
	auto vx = val<vec<float>>::Load(x_arr);
	auto vc0 = val<vec<float>>::Load(c0_arr);
	auto vc1 = val<vec<float>>::Load(c1_arr);
	auto vc2 = val<vec<float>>::Load(c2_arr);
	auto vc3 = val<vec<float>>::Load(c3_arr);
	// Horner: ((c3 * x + c2) * x + c1) * x + c0
	auto r = Fma(vc3, vx, vc2);
	r = Fma(r, vx, vc1);
	r = Fma(r, vx, vc0);
	r.Store(out);
}

// --- Conditional negate: negate elements where mask is true ---
// Pattern: blend(-a, a, mask)
void vectorConditionalNegateFloat(val<const float*> a, val<const float*> mask_arr, val<float*> c) {
	auto va = val<vec<float>>::Load(a);
	auto vmask = val<vec<float>>::Load(mask_arr);
	Blend(vmask, -va, va).Store(c);
}

// --- Sum of absolute differences (SAD): sum(|a[i] - b[i]|) ---
val<float> vectorSadFloat(val<const float*> a, val<const float*> b) {
	return (val<vec<float>>::Load(a) - val<vec<float>>::Load(b)).Abs().ReduceAdd();
}

// --- Weighted average: sum(a[i] * w[i]) / sum(w[i]) ---
// Returns both numerator and denominator via output pointers.
void vectorWeightedSumParts(val<const float*> a, val<const float*> w, val<float*> numer_out, val<float*> denom_out) {
	auto va = val<vec<float>>::Load(a);
	auto vw = val<vec<float>>::Load(w);
	*numer_out = (va * vw).ReduceAdd();
	*denom_out = vw.ReduceAdd();
}

// --- Element-wise linear interpolation: lerp(a, b, t) = a + t * (b - a) ---
void vectorLerpFloat(val<const float*> a, val<const float*> b, val<const float*> t_arr, val<float*> c) {
	auto va = val<vec<float>>::Load(a);
	auto vb = val<vec<float>>::Load(b);
	auto vt = val<vec<float>>::Load(t_arr);
	// a + t * (b - a) = FMA(t, b - a, a)
	Fma(vt, vb - va, va).Store(c);
}

// --- Range: max(a) - min(a) ---
val<float> vectorRangeFloat(val<const float*> a) {
	auto va = val<vec<float>>::Load(a);
	return va.ReduceMax() - va.ReduceMin();
}

// --- ReLU: max(0, a[i]) using compare+blend ---
void vectorReluFloat(val<const float*> a, val<const float*> zero_arr, val<float*> c) {
	auto va = val<vec<float>>::Load(a);
	auto vzero = val<vec<float>>::Load(zero_arr);
	auto mask = va > vzero;
	Blend(mask, va, vzero).Store(c);
}

// --- Multi-accumulate: a*b + c*d (e.g., complex multiply real part) ---
void vectorMulAccFloat(val<const float*> a, val<const float*> b, val<const float*> c, val<const float*> d,
                       val<float*> out) {
	auto prod1 = val<vec<float>>::Load(a) * val<vec<float>>::Load(b);
	auto prod2 = val<vec<float>>::Load(c) * val<vec<float>>::Load(d);
	(prod1 + prod2).Store(out);
}

// --- Normalize: a[i] / sum(a) (e.g., probability normalization) ---
// Reduces to scalar sum, then divides each element. Since we can't broadcast
// a scalar back, we store the sum and let caller use it.
val<float> vectorSumFloat(val<const float*> a) {
	return val<vec<float>>::Load(a).ReduceAdd();
}

// --- Min-max clamp chain with blend: saturate negatives to zero, cap at max ---
void vectorSaturateFloat(val<const float*> a, val<const float*> max_arr, val<float*> c) {
	auto va = val<vec<float>>::Load(a);
	auto vmax = val<vec<float>>::Load(max_arr);
	auto vzero = va - va; // zero vector
	// clamp to [0, max]: first floor at zero, then cap at max
	auto mask_pos = va > vzero;
	auto clamped_low = Blend(mask_pos, va, vzero);
	Min(clamped_low, vmax).Store(c);
}

// ============================================================================
// Broadcast — scalar to all lanes
// ============================================================================

void vectorBroadcastFloat(val<float> scalar, val<float*> out) {
	val<vec<float>>::Broadcast(scalar).Store(out);
}

void vectorBroadcastInt(val<int32_t> scalar, val<int32_t*> out) {
	val<vec<int32_t>>::Broadcast(scalar).Store(out);
}

void vectorBroadcastDouble(val<double> scalar, val<double*> out) {
	val<vec<double>>::Broadcast(scalar).Store(out);
}

// ============================================================================
// Gather — indexed load from non-contiguous memory
// ============================================================================

void vectorGatherFloat(val<const float*> base, val<const int32_t*> indices, val<float*> out) {
	val<vec<float>>::Gather(base, indices).Store(out);
}

void vectorGatherInt(val<const int32_t*> base, val<const int32_t*> indices, val<int32_t*> out) {
	val<vec<int32_t>>::Gather(base, indices).Store(out);
}

// ============================================================================
// Scatter — indexed store to non-contiguous memory
// ============================================================================

void vectorScatterFloat(val<const float*> src, val<float*> base, val<const int32_t*> indices) {
	val<vec<float>>::Load(src).Scatter(base, indices);
}

void vectorScatterInt(val<const int32_t*> src, val<int32_t*> base, val<const int32_t*> indices) {
	val<vec<int32_t>>::Load(src).Scatter(base, indices);
}

// ============================================================================
// Extract — get a single lane
// ============================================================================

val<float> vectorExtractFloat(val<const float*> a, val<int32_t> idx) {
	return val<vec<float>>::Load(a).Get(idx);
}

val<int32_t> vectorExtractInt(val<const int32_t*> a, val<int32_t> idx) {
	return val<vec<int32_t>>::Load(a).Get(idx);
}

// ============================================================================
// Insert — replace a single lane
// ============================================================================

void vectorInsertFloat(val<const float*> a, val<float> value, val<int32_t> idx, val<float*> out) {
	val<vec<float>>::Load(a).Set(idx, value).Store(out);
}

void vectorInsertInt(val<const int32_t*> a, val<int32_t> value, val<int32_t> idx, val<int32_t*> out) {
	val<vec<int32_t>>::Load(a).Set(idx, value).Store(out);
}

// ============================================================================
// Shift — integer left/right shift
// ============================================================================

void vectorShlInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(val<vec<int32_t>>::Load(a) << val<vec<int32_t>>::Load(b)).Store(c);
}

void vectorShrInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(val<vec<int32_t>>::Load(a) >> val<vec<int32_t>>::Load(b)).Store(c);
}

void vectorShlInt64(val<const int64_t*> a, val<const int64_t*> b, val<int64_t*> c) {
	(val<vec<int64_t>>::Load(a) << val<vec<int64_t>>::Load(b)).Store(c);
}

// ============================================================================
// Composite patterns using new operations
// ============================================================================

// Broadcast + arithmetic: scale all elements by a scalar factor
void vectorBroadcastMulFloat(val<const float*> a, val<float> scalar, val<float*> out) {
	auto va = val<vec<float>>::Load(a);
	auto vscalar = val<vec<float>>::Broadcast(scalar);
	(va * vscalar).Store(out);
}

// Gather + reduce: sum elements at indexed positions
val<float> vectorGatherReduceFloat(val<const float*> base, val<const int32_t*> indices) {
	return val<vec<float>>::Gather(base, indices).ReduceAdd();
}

// Compound shift: shift left then shift right (extract bit field)
void vectorCompoundShiftInt(val<const int32_t*> a, val<int32_t*> out) {
	auto va = val<vec<int32_t>>::Load(a);
	auto shift_left = val<vec<int32_t>>::Broadcast(val<int32_t>(4));
	auto shift_right = val<vec<int32_t>>::Broadcast(val<int32_t>(4));
	((va << shift_left) >> shift_right).Store(out);
}

} // namespace nautilus::engine
