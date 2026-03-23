#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/vector.hpp>

namespace nautilus::engine {

// ============================================================================
// Float arithmetic — using val<vec<float>> with operators
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
// Float reductions — using member methods
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
// Compound assignment operators
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

void vectorEqInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(val<vec<int32_t>>::Load(a) == val<vec<int32_t>>::Load(b)).Store(c);
}

void vectorNeInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(val<vec<int32_t>>::Load(a) != val<vec<int32_t>>::Load(b)).Store(c);
}

// ============================================================================
// Blend
// ============================================================================

void vectorBlendFloat(val<const float*> a, val<const float*> b, val<const float*> mask_arr, val<float*> c) {
	auto va = val<vec<float>>::Load(a);
	auto vb = val<vec<float>>::Load(b);
	auto vmask = val<vec<float>>::Load(mask_arr);
	Blend(vmask, va, vb).Store(c);
}

// ============================================================================
// Bitwise operators
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
// Double tests
// ============================================================================

void vectorAddDouble(val<const double*> a, val<const double*> b, val<double*> c) {
	(val<vec<double>>::Load(a) + val<vec<double>>::Load(b)).Store(c);
}

// ============================================================================
// Load / Store round-trip using Vector<T> alias
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
	c[0] = VectorFactory<128>::Load<float>(a).ReduceAdd();
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
// Explicit val<vec<float, 4>> (direct template parameters)
// ============================================================================

void vectorExplicit128AddFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	auto va = val<vec<float, 4>>::Load(a);
	auto vb = val<vec<float, 4>>::Load(b);
	(va + vb).Store(c);
}

// ============================================================================
// Vector<T, N> alias with explicit lane count
// ============================================================================

void vectorAlias256AddDouble(val<const double*> a, val<const double*> b, val<double*> c) {
	auto va = Vector<double, 4>::Load(a);
	auto vb = Vector<double, 4>::Load(b);
	(va + vb).Store(c);
}

} // namespace nautilus::engine
