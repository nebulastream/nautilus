#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/vector.hpp>

namespace nautilus::engine {

// ============================================================================
// Float arithmetic — using operators and member methods
// ============================================================================

void vectorAddFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = Vector<float>::Load(a);
	auto vb = Vector<float>::Load(b);
	(va + vb).Store(c);
}

void vectorSubFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	(Vector<float>::Load(a) - Vector<float>::Load(b)).Store(c);
}

void vectorMulFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	(Vector<float>::Load(a) * Vector<float>::Load(b)).Store(c);
}

void vectorDivFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	(Vector<float>::Load(a) / Vector<float>::Load(b)).Store(c);
}

void vectorNegFloat(val<const float*> a, val<float*> c) {
	(-Vector<float>::Load(a)).Store(c);
}

void vectorAbsFloat(val<const float*> a, val<float*> c) {
	Vector<float>::Load(a).Abs().Store(c);
}

// ============================================================================
// Float min/max/fma — using top-level convenience functions
// ============================================================================

void vectorMinFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	Min(Vector<float>::Load(a), Vector<float>::Load(b)).Store(c);
}

void vectorMaxFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	Max(Vector<float>::Load(a), Vector<float>::Load(b)).Store(c);
}

void vectorFmaFloat(val<const float*> a, val<const float*> b, val<const float*> c, val<float*> d) {
	Fma(Vector<float>::Load(a), Vector<float>::Load(b), Vector<float>::Load(c)).Store(d);
}

// ============================================================================
// Float reductions — using member methods
// ============================================================================

val<float> vectorReduceAddFloat(val<const float*> a) {
	return Vector<float>::Load(a).ReduceAdd();
}

val<float> vectorReduceMinFloat(val<const float*> a) {
	return Vector<float>::Load(a).ReduceMin();
}

val<float> vectorReduceMaxFloat(val<const float*> a) {
	return Vector<float>::Load(a).ReduceMax();
}

// ============================================================================
// Chained operations
// ============================================================================

void vectorMulSubFloat(val<const float*> a, val<const float*> b, val<const float*> c, val<float*> d) {
	auto va = Vector<float>::Load(a);
	auto vb = Vector<float>::Load(b);
	auto vc = Vector<float>::Load(c);
	(va * vb - vc).Store(d);
}

// ============================================================================
// Compound assignment operators
// ============================================================================

void vectorCompoundAddFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = Vector<float>::Load(a);
	va += Vector<float>::Load(b);
	va.Store(c);
}

void vectorCompoundMulFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = Vector<float>::Load(a);
	va *= Vector<float>::Load(b);
	va.Store(c);
}

// ============================================================================
// Comparison operators
// ============================================================================

void vectorLtFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	(Vector<float>::Load(a) < Vector<float>::Load(b)).Store(c);
}

void vectorGeFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	(Vector<float>::Load(a) >= Vector<float>::Load(b)).Store(c);
}

void vectorEqInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(Vector<int32_t>::Load(a) == Vector<int32_t>::Load(b)).Store(c);
}

void vectorNeInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(Vector<int32_t>::Load(a) != Vector<int32_t>::Load(b)).Store(c);
}

// ============================================================================
// Blend — using top-level Blend()
// ============================================================================

void vectorBlendFloat(val<const float*> a, val<const float*> b, val<const float*> mask_arr, val<float*> c) {
	auto va = Vector<float>::Load(a);
	auto vb = Vector<float>::Load(b);
	auto vmask = Vector<float>::Load(mask_arr);
	Blend(vmask, va, vb).Store(c);
}

// ============================================================================
// Bitwise operators
// ============================================================================

void vectorAndInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(Vector<int32_t>::Load(a) & Vector<int32_t>::Load(b)).Store(c);
}

void vectorOrInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(Vector<int32_t>::Load(a) | Vector<int32_t>::Load(b)).Store(c);
}

void vectorXorInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(Vector<int32_t>::Load(a) ^ Vector<int32_t>::Load(b)).Store(c);
}

// ============================================================================
// Integer arithmetic + reductions
// ============================================================================

void vectorAddInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(Vector<int32_t>::Load(a) + Vector<int32_t>::Load(b)).Store(c);
}

void vectorMulInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(Vector<int32_t>::Load(a) * Vector<int32_t>::Load(b)).Store(c);
}

val<int32_t> vectorReduceAddInt(val<const int32_t*> a) {
	return Vector<int32_t>::Load(a).ReduceAdd();
}

val<int32_t> vectorReduceMinInt(val<const int32_t*> a) {
	return Vector<int32_t>::Load(a).ReduceMin();
}

val<int32_t> vectorReduceMaxInt(val<const int32_t*> a) {
	return Vector<int32_t>::Load(a).ReduceMax();
}

// ============================================================================
// Double tests
// ============================================================================

void vectorAddDouble(val<const double*> a, val<const double*> b, val<double*> c) {
	(Vector<double>::Load(a) + Vector<double>::Load(b)).Store(c);
}

// ============================================================================
// Vector<T>::Load / Store round-trip
// ============================================================================

void vectorFactoryLoadStore(val<const float*> a, val<float*> c) {
	Vector<float>::Load(a).Store(c);
}

// ============================================================================
// VectorFactory<128> explicit width (always 4 float lanes)
// ============================================================================

void vectorFactory128AddFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = VectorFactory<128>::Load<float>(a);
	auto vb = VectorFactory<128>::Load<float>(b);
	VectorFactory<128>::Store<float>(c, va + vb);
}

void vectorFactory128ReduceFloat(val<const float*> a, val<float*> c) {
	auto va = VectorFactory<128>::Load<float>(a);
	c[0] = va.ReduceAdd();
}

// ============================================================================
// VectorFactory<256> explicit width (always 8 float lanes)
// ============================================================================

void vectorFactory256AddFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = VectorFactory<256>::Load<float>(a);
	auto vb = VectorFactory<256>::Load<float>(b);
	VectorFactory<256>::Store<float>(c, va + vb);
}

// ============================================================================
// SIMD<T, N> with explicit type
// ============================================================================

void vectorSIMD128AddFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = SIMD<float, 4>::Load(a);
	auto vb = SIMD<float, 4>::Load(b);
	(va + vb).Store(c);
}

} // namespace nautilus::engine
