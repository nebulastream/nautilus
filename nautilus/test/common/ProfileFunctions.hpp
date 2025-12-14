#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/profile/assume.hpp>
#include <nautilus/std/cmath.h>

namespace nautilus::engine {

// Test function that uses nautilus_assume
val<int32_t> assumeFunction(val<int32_t> x) {
	val<bool> condition = x > 0;
	nautilus_assume(condition);
	return x + 1;
}

// Test function that uses nautilus_assume with a more complex condition
val<int32_t> assumeComplexCondition(val<int32_t> x, val<int32_t> y) {
	val<bool> condition = (x > 0) && (y > 0);
	nautilus_assume(condition);
	return x + y;
}

// Test function that uses nautilus_assume_aligned
val<int32_t> assumeAlignedFunction(val<int32_t*> ptr) {
	nautilus_assume_aligned((val<void*>) ptr, 16);
	return *ptr;
}

// ============================================================================
// cmath Intrinsic Test Functions
// ============================================================================

// Trigonometric functions
val<double> cmathSinFunction(val<double> x) {
	return sin(x);
}

val<double> cmathCosFunction(val<double> x) {
	return cos(x);
}

val<double> cmathTanFunction(val<double> x) {
	return tan(x);
}

// Exponential and logarithmic functions
val<double> cmathExpFunction(val<double> x) {
	return exp(x);
}

val<double> cmathLogFunction(val<double> x) {
	return log(x);
}

val<double> cmathSqrtFunction(val<double> x) {
	return sqrt(x);
}

// Power functions
val<double> cmathPowFunction(val<double> x, val<double> y) {
	return pow(x, y);
}

// Rounding functions
val<double> cmathFloorFunction(val<double> x) {
	return floor(x);
}

val<double> cmathCeilFunction(val<double> x) {
	return ceil(x);
}

val<double> cmathRoundFunction(val<double> x) {
	return round(x);
}

// Absolute value and sign functions
val<double> cmathAbsFunction(val<double> x) {
	return abs(x);
}

val<double> cmathFabsFunction(val<double> x) {
	return fabs(x);
}

// Hyperbolic functions
val<double> cmathSinhFunction(val<double> x) {
	return sinh(x);
}

val<double> cmathCoshFunction(val<double> x) {
	return cosh(x);
}

// Min/Max functions
val<double> cmathFmaxFunction(val<double> x, val<double> y) {
	return fmax(x, y);
}

val<double> cmathFminFunction(val<double> x, val<double> y) {
	return fmin(x, y);
}

} // namespace nautilus::engine
