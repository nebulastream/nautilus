#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/vector.hpp>

namespace nautilus::engine {

// ============================================================================
// Vector Load/Store + Add (float)
// ============================================================================

void vectorAddFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	auto vc = vector_add(va, vb);
	vector_store(c, vc);
}

// ============================================================================
// Vector Sub (float)
// ============================================================================

void vectorSubFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	auto vc = vector_sub(va, vb);
	vector_store(c, vc);
}

// ============================================================================
// Vector Mul (float)
// ============================================================================

void vectorMulFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	auto vc = vector_mul(va, vb);
	vector_store(c, vc);
}

// ============================================================================
// Vector Div (float)
// ============================================================================

void vectorDivFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	auto vc = vector_div(va, vb);
	vector_store(c, vc);
}

// ============================================================================
// Vector Neg (float)
// ============================================================================

void vectorNegFloat(val<const float*> a, val<float*> c) {
	auto va = vector_load<float>(a);
	auto vc = vector_neg(va);
	vector_store(c, vc);
}

// ============================================================================
// Vector Min/Max (float)
// ============================================================================

void vectorMinFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	auto vc = vector_min(va, vb);
	vector_store(c, vc);
}

void vectorMaxFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	auto vc = vector_max(va, vb);
	vector_store(c, vc);
}

// ============================================================================
// Vector Reduce Add (float)
// ============================================================================

val<float> vectorReduceAddFloat(val<const float*> a) {
	auto va = vector_load<float>(a);
	return vector_reduce_add(va);
}

// ============================================================================
// Vector FMA (float): a * b + c
// ============================================================================

void vectorFmaFloat(val<const float*> a, val<const float*> b, val<const float*> c, val<float*> d) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	auto vc = vector_load<float>(c);
	auto vd = vector_fma(va, vb, vc);
	vector_store(d, vd);
}

// ============================================================================
// Vector Add (int32_t)
// ============================================================================

void vectorAddInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	auto va = vector_load<int32_t>(a);
	auto vb = vector_load<int32_t>(b);
	auto vc = vector_add(va, vb);
	vector_store(c, vc);
}

// ============================================================================
// Vector Mul (int32_t)
// ============================================================================

void vectorMulInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	auto va = vector_load<int32_t>(a);
	auto vb = vector_load<int32_t>(b);
	auto vc = vector_mul(va, vb);
	vector_store(c, vc);
}

// ============================================================================
// Vector Reduce Add (int32_t)
// ============================================================================

val<int32_t> vectorReduceAddInt(val<const int32_t*> a) {
	auto va = vector_load<int32_t>(a);
	return vector_reduce_add(va);
}

// ============================================================================
// Vector Add (double)
// ============================================================================

void vectorAddDouble(val<const double*> a, val<const double*> b, val<double*> c) {
	auto va = vector_load<double>(a);
	auto vb = vector_load<double>(b);
	auto vc = vector_add(va, vb);
	vector_store(c, vc);
}

// ============================================================================
// Operator overload tests (float)
// ============================================================================

void vectorOperatorAddFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	vector_store(c, va + vb);
}

void vectorOperatorMulSubFloat(val<const float*> a, val<const float*> b, val<const float*> c, val<float*> d) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	auto vc = vector_load<float>(c);
	vector_store(d, va * vb - vc);
}

void vectorOperatorNegFloat(val<const float*> a, val<float*> c) {
	auto va = vector_load<float>(a);
	vector_store(c, -va);
}

// ============================================================================
// Reduce min/max (float)
// ============================================================================

val<float> vectorReduceMinFloat(val<const float*> a) {
	auto va = vector_load<float>(a);
	return vector_reduce_min(va);
}

val<float> vectorReduceMaxFloat(val<const float*> a) {
	auto va = vector_load<float>(a);
	return vector_reduce_max(va);
}

// ============================================================================
// Reduce min/max (int32_t)
// ============================================================================

val<int32_t> vectorReduceMinInt(val<const int32_t*> a) {
	auto va = vector_load<int32_t>(a);
	return vector_reduce_min(va);
}

val<int32_t> vectorReduceMaxInt(val<const int32_t*> a) {
	auto va = vector_load<int32_t>(a);
	return vector_reduce_max(va);
}

// ============================================================================
// Comparison (float)
// ============================================================================

void vectorLtFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	auto vc = vector_lt(va, vb);
	vector_store(c, vc);
}

// ============================================================================
// Blend (float)
// ============================================================================

void vectorBlendFloat(val<const float*> a, val<const float*> b, val<const float*> mask_arr, val<float*> c) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	auto vmask = vector_load<float>(mask_arr);
	auto vc = vector_blend(vmask, va, vb);
	vector_store(c, vc);
}

// ============================================================================
// Bitwise AND (int32_t)
// ============================================================================

void vectorAndInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	auto va = vector_load<int32_t>(a);
	auto vb = vector_load<int32_t>(b);
	auto vc = vector_and(va, vb);
	vector_store(c, vc);
}

} // namespace nautilus::engine
