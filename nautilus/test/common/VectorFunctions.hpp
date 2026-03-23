#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/vector.hpp>

namespace nautilus::engine {

// ============================================================================
// Default-width tests using Vector<T> and vector_load<T>
// ============================================================================

void vectorAddFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	vector_store(c, va + vb);
}

void vectorSubFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	vector_store(c, va - vb);
}

void vectorMulFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	vector_store(c, va * vb);
}

void vectorDivFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	vector_store(c, va / vb);
}

void vectorNegFloat(val<const float*> a, val<float*> c) {
	auto va = vector_load<float>(a);
	vector_store(c, -va);
}

void vectorMinFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	vector_store(c, vector_min(va, vb));
}

void vectorMaxFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	vector_store(c, vector_max(va, vb));
}

val<float> vectorReduceAddFloat(val<const float*> a) {
	return vector_reduce_add(vector_load<float>(a));
}

val<float> vectorReduceMinFloat(val<const float*> a) {
	return vector_reduce_min(vector_load<float>(a));
}

val<float> vectorReduceMaxFloat(val<const float*> a) {
	return vector_reduce_max(vector_load<float>(a));
}

void vectorFmaFloat(val<const float*> a, val<const float*> b, val<const float*> c, val<float*> d) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	auto vc = vector_load<float>(c);
	vector_store(d, vector_fma(va, vb, vc));
}

void vectorAbsFloat(val<const float*> a, val<float*> c) {
	vector_store(c, vector_abs(vector_load<float>(a)));
}

// ============================================================================
// Chained operator test: a * b - c
// ============================================================================

void vectorMulSubFloat(val<const float*> a, val<const float*> b, val<const float*> c, val<float*> d) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	auto vc = vector_load<float>(c);
	vector_store(d, va * vb - vc);
}

// ============================================================================
// Integer tests
// ============================================================================

void vectorAddInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	vector_store(c, vector_load<int32_t>(a) + vector_load<int32_t>(b));
}

void vectorMulInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	vector_store(c, vector_load<int32_t>(a) * vector_load<int32_t>(b));
}

val<int32_t> vectorReduceAddInt(val<const int32_t*> a) {
	return vector_reduce_add(vector_load<int32_t>(a));
}

val<int32_t> vectorReduceMinInt(val<const int32_t*> a) {
	return vector_reduce_min(vector_load<int32_t>(a));
}

val<int32_t> vectorReduceMaxInt(val<const int32_t*> a) {
	return vector_reduce_max(vector_load<int32_t>(a));
}

// ============================================================================
// Double tests
// ============================================================================

void vectorAddDouble(val<const double*> a, val<const double*> b, val<double*> c) {
	vector_store(c, vector_load<double>(a) + vector_load<double>(b));
}

// ============================================================================
// Comparison tests
// ============================================================================

void vectorLtFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	vector_store(c, vector_lt(vector_load<float>(a), vector_load<float>(b)));
}

void vectorEqInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	vector_store(c, vector_eq(vector_load<int32_t>(a), vector_load<int32_t>(b)));
}

// ============================================================================
// Blend test
// ============================================================================

void vectorBlendFloat(val<const float*> a, val<const float*> b, val<const float*> mask_arr, val<float*> c) {
	auto va = vector_load<float>(a);
	auto vb = vector_load<float>(b);
	auto vmask = vector_load<float>(mask_arr);
	vector_store(c, vector_blend(vmask, va, vb));
}

// ============================================================================
// Bitwise tests
// ============================================================================

void vectorAndInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	vector_store(c, vector_and(vector_load<int32_t>(a), vector_load<int32_t>(b)));
}

void vectorOrInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	vector_store(c, vector_or(vector_load<int32_t>(a), vector_load<int32_t>(b)));
}

void vectorXorInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	vector_store(c, vector_xor(vector_load<int32_t>(a), vector_load<int32_t>(b)));
}

// ============================================================================
// Vector<T>::Load / Store factory method tests
// ============================================================================

void vectorFactoryLoadStore(val<const float*> a, val<float*> c) {
	auto va = Vector<float>::Load(a);
	va.Store(c);
}

// ============================================================================
// VectorFactory<128> explicit width test (always 4 float lanes)
// ============================================================================

void vectorFactory128AddFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = VectorFactory<128>::Load<float>(a);
	auto vb = VectorFactory<128>::Load<float>(b);
	VectorFactory<128>::Store<float>(c, va + vb);
}

void vectorFactory128ReduceFloat(val<const float*> a, val<float*> c) {
	auto va = VectorFactory<128>::Load<float>(a);
	auto sum = vector_reduce_add(va);
	// Store scalar result to first element
	c[0] = sum;
}

// ============================================================================
// VectorFactory<256> explicit width test (always 8 float lanes)
// ============================================================================

void vectorFactory256AddFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = VectorFactory<256>::Load<float>(a);
	auto vb = VectorFactory<256>::Load<float>(b);
	VectorFactory<256>::Store<float>(c, va + vb);
}

} // namespace nautilus::engine
