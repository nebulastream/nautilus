#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/std/cmath.h>

namespace nautilus::engine {

// ============================================================================
// Cmath Intrinsic Test Functions - Unary Operations
// ============================================================================

// Test function for sin intrinsic
val<float> sinFunction(val<float> x) {
	return sin(x);
}

// Test function for cos intrinsic
val<float> cosFunction(val<float> x) {
	return cos(x);
}

// Test function for sqrt intrinsic
val<float> sqrtFunction(val<float> x) {
	return sqrt(x);
}

// Test function for exp intrinsic
val<float> expFunction(val<float> x) {
	return exp(x);
}

// Test function for log intrinsic
val<float> logFunction(val<float> x) {
	return log(x);
}

// Test function for ceil intrinsic
val<float> ceilFunction(val<float> x) {
	return ceil(x);
}

// Test function for floor intrinsic
val<float> floorFunction(val<float> x) {
	return floor(x);
}

// Test function for fabs intrinsic
val<float> fabsFunction(val<float> x) {
	return fabs(x);
}

// Test function for tan intrinsic
val<float> tanFunction(val<float> x) {
	return tan(x);
}

// Test function for asin intrinsic
val<float> asinFunction(val<float> x) {
	return asin(x);
}

// Test function for acos intrinsic
val<float> acosFunction(val<float> x) {
	return acos(x);
}

// Test function for atan intrinsic
val<float> atanFunction(val<float> x) {
	return atan(x);
}

// ============================================================================
// Cmath Intrinsic Test Functions - Binary Operations
// ============================================================================

// Test function for pow intrinsic
val<float> powFunction(val<float> x, val<float> y) {
	return pow(x, y);
}

// Test function for atan2 intrinsic
val<float> atan2Function(val<float> y, val<float> x) {
	return atan2(y, x);
}

// Test function for fmin intrinsic
val<float> fminFunction(val<float> x, val<float> y) {
	return fmin(x, y);
}

// Test function for fmax intrinsic
val<float> fmaxFunction(val<float> x, val<float> y) {
	return fmax(x, y);
}

// Test function for fmod intrinsic
val<float> fmodFunction(val<float> x, val<float> y) {
	return fmod(x, y);
}

// Test function for copysign intrinsic
val<float> copysignFunction(val<float> x, val<float> y) {
	return copysign(x, y);
}

// ============================================================================
// Cmath Intrinsic Test Functions - Ternary Operations
// ============================================================================

// Test function for fma intrinsic
val<float> fmaFunction(val<float> x, val<float> y, val<float> z) {
	return fma(x, y, z);
}

// ============================================================================
// Cmath Intrinsic Test Functions - Combined Operations
// ============================================================================

// Test function that combines multiple cmath intrinsics
val<float> combinedMathFunction(val<float> x, val<float> y) {
	val<float> s = sin(x);
	val<float> c = cos(y);
	val<float> result = sqrt(s * s + c * c);
	return result;
}

} // namespace nautilus::engine
