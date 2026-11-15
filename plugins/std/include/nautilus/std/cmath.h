#pragma once
#include <nautilus/val.hpp>
namespace nautilus {

/**
 * @brief Computes the absolute value of the floating-point value x.
 *
 * @param x floating-point
 * @return Absolute of x.
 */
val<float> abs(const val<float>& x) noexcept;
val<double> abs(const val<double>& x) noexcept;
val<int8_t> abs(const val<int8_t>& x) noexcept;
val<int16_t> abs(const val<int16_t>& x) noexcept;
val<int32_t> abs(const val<int32_t>& x) noexcept;
val<int64_t> abs(const val<int64_t>& x) noexcept;
val<float> fabs(const val<float>& x) noexcept;
val<double> fabs(const val<double>& x) noexcept;
val<float> fabsf(const val<float>& x) noexcept;

/**
 * @brief Computes the floating-point remainder of x/y.
 *
 * @param x Numerator.
 * @param y Denominator.
 * @return Remainder of x/y.
 */
val<float> fmod(const val<float>& x, const val<float>& y) noexcept;
val<float> fmodf(const val<float>& x, const val<float>& y) noexcept;
val<double> fmod(const val<double>& x, const val<double>& y) noexcept;

/**
 * @brief Computes the floating-point remainder of x/y (same as fmod).
 *
 * @param x Numerator.
 * @param y Denominator.
 * @return Remainder of x/y.
 */
val<float> remainder(const val<float>& x, const val<float>& y) noexcept;
val<float> remainderf(const val<float>& x, const val<float>& y) noexcept;
val<double> remainder(const val<double>& x, const val<double>& y) noexcept;

/**
 * @brief Computes the remainder of the division operation x/y.
 *
 * @param x Numerator.
 * @param y Denominator.
 * @param quo Pointer to store the quotient.
 * @return Remainder of x/y.
 */
val<float> remquo(const val<float>& x, const val<float>& y, const val<int*>& quo) noexcept;
val<float> remquof(const val<float>& x, const val<float>& y, const val<int*>& quo) noexcept;
val<double> remquo(const val<double>& x, const val<double>& y, const val<int*>& quo) noexcept;

/**
 * @brief Computes x * y + z as if to infinite precision and rounded only once
 * to fit the result type.
 *
 * @param x floating-point
 * @param y floating-point
 * @param z floating-point
 * @return x * y + z
 */
val<float> fma(const val<float>& x, const val<float>& y, const val<float>& z) noexcept;
val<double> fma(const val<double>& x, const val<double>& y, const val<double>& z) noexcept;
val<float> fmaf(const val<float>& x, const val<float>& y, const val<float>& z) noexcept;

/**
 * @brief Computes the larger of two floating point arguments, treating NaNs as
 * missing data (between a NaN and a numeric value, the numeric value is
 * chosen).
 *
 * @param x floating-point
 * @param y floating-point
 * @return larger of two floating-point values
 */
val<float> fmax(const val<float>& x, const val<float>& y) noexcept;
val<double> fmax(const val<double>& x, const val<double>& y) noexcept;
val<float> fmaxf(const val<float>& x, const val<float>& y) noexcept;

/**
 * @brief Computes the smaller of two floating point arguments, treating NaNs as
 * missing data (between a NaN and a numeric value, the numeric value is
 * chosen).
 *
 * @param x floating-point
 * @param y floating-point
 * @return smaller of two floating-point values
 */
val<float> fmin(const val<float>& x, const val<float>& y) noexcept;
val<double> fmin(const val<double>& x, const val<double>& y) noexcept;
val<float> fminf(const val<float>& x, const val<float>& y) noexcept;

/**
 * @brief Computes he positive difference between x and y, that is, if x > y,
 * returns x - y, otherwise (i.e. if x <= y) returns +0
 *
 * @param x floating-point
 * @param y floating-point
 * @return smaller of two floating-point values
 */
val<float> fdim(const val<float>& x, const val<float>& y) noexcept;
val<double> fdim(const val<double>& x, const val<double>& y) noexcept;
val<float> fdimf(const val<float>& x, const val<float>& y) noexcept;

/**
 * @brief Generates a quiet NaN (Not-a-Number).
 *
 * @param tagp Implementation-specific C-string to distinguish different NaN
 * values.
 * @return Quiet NaN.
 */
val<float> nanf(const val<const char*>& tagp) noexcept;
val<double> nan(const val<const char*>& tagp) noexcept;

// Linear interpolation
val<float> lerp(const val<float>& a, const val<float>& b, const val<float>& c) noexcept;
val<double> lerp(const val<double>& a, const val<double>& b, const val<double>& c) noexcept;

// Exponential and logarithmic functions

/**
 * @brief Computes the exponential function of x.
 *
 * @param x Value.
 * @return Exponential of x.
 */
val<float> exp(const val<float>& x) noexcept;
val<double> exp(const val<double>& x) noexcept;
val<float> expf(const val<float>& x) noexcept;

/**
 * @brief Computes the returns 2 raised to the given power.
 *
 * @param x Value.
 * @return returns 2 raised to the given power.
 */
val<float> exp2(const val<float>& x) noexcept;
val<double> exp2(const val<double>& x) noexcept;
val<float> exp2f(const val<float>& x) noexcept;

/**
 * @brief Computes the e (Euler's number, 2.7182818...) raised to the given
 * power num, minus 1.0.
 *
 * @param num floating-point .
 * @return returns e raised to the given power, minus one.
 */
val<float> expm1(const val<float>& num) noexcept;
val<double> expm1(const val<double>& num) noexcept;
val<float> expm1f(const val<float>& num) noexcept;

/**
 * @brief Computes the natural logarithm of x.
 *
 * @param x Value.
 * @return Natural logarithm of x.
 */
val<float> log(const val<float>& x) noexcept;
val<double> log(const val<double>& x) noexcept;
val<float> logf(const val<float>& x) noexcept;

/**
 * @brief Computes the base-10 logarithm of x.
 *
 * @param x Value.
 * @return Base-10 logarithm of x.
 */
val<float> log10(const val<float>& x) noexcept;
val<double> log10(const val<double>& x) noexcept;
#if defined(_LIBCPP_VERSION)
val<float> log10f(const val<float>& x) noexcept;
#endif
/**
 * @brief Computes the base-2 logarithm of x.
 *
 * @param x Value.
 * @return Base-2 logarithm of x.
 */
val<float> log2(const val<float>& x) noexcept;
val<double> log2(const val<double>& x) noexcept;

/**
 * @brief natural logarithm (to base e) of 1 plus the given number (ln(1+x))
 *
 * @param x Value.
 * @return natural logarithm
 */
val<float> log1p(const val<float>& x) noexcept;
val<double> log1p(const val<double>& x) noexcept;
val<float> log1pf(const val<float>& x) noexcept;

// Power functions

/**
 * @brief Computes x raised to the power of y.
 *
 * @param x Base.
 * @param y Exponent.
 * @return x^y.
 */
val<float> pow(const val<float>& x, const val<float>& y) noexcept;
val<double> pow(const val<double>& x, const val<double>& y) noexcept;
val<float> powf(const val<float>& x, const val<float>& y) noexcept;

/**
 * @brief Computes the square root of x.
 *
 * @param x Value.
 * @return Square root of x.
 */
val<float> sqrt(const val<float>& x) noexcept;
val<double> sqrt(const val<double>& x) noexcept;
val<float> sqrtf(const val<float>& x) noexcept;

/**
 * @brief Computes the cube root of x.
 *
 * @param x Value.
 * @return Cube root of x.
 */
val<float> cbrt(const val<float>& x) noexcept;
val<double> cbrt(const val<double>& x) noexcept;
val<float> cbrtf(const val<float>& x) noexcept;

/**
 * @brief Computes the Euclidean distance between the points (x, 0) and (0, y).
 *
 * @param x First value.
 * @param y Second value.
 * @return sqrt(x^2 + y^2).
 */
val<float> hypot(const val<float>& x, const val<float>& y) noexcept;
val<double> hypot(const val<double>& x, const val<double>& y) noexcept;
val<float> hypotf(const val<float>& x, const val<float>& y) noexcept;

// Trigonometric functions

/**
 * @brief Computes the sine of x.
 *
 * @param x Angle in radians.
 * @return Sine of x.
 */
val<float> sin(const val<float>& x) noexcept;
val<double> sin(const val<double>& x) noexcept;
val<float> sinf(const val<float>& x) noexcept;

/**
 * @brief Computes the cosine of x.
 *
 * @param x Angle in radians.
 * @return Cosine of x.
 */
val<float> cos(const val<float>& x) noexcept;
val<double> cos(const val<double>& x) noexcept;
val<float> cosf(const val<float>& x) noexcept;

/**
 * @brief Computes the tangent of x.
 *
 * @param x Angle in radians.
 * @return Tangent of x.
 */
val<float> tan(const val<float>& x) noexcept;
val<double> tan(const val<double>& x) noexcept;
val<float> tanf(const val<float>& x) noexcept;

/**
 * @brief Computes the arc sine of x.
 *
 * @param x Value.
 * @return Arc sine of x in radians.
 */
val<float> asin(const val<float>& x) noexcept;
val<double> asin(const val<double>& x) noexcept;
val<float> asinf(const val<float>& x) noexcept;

/**
 * @brief Computes the arc cosine of x.
 *
 * @param x Value.
 * @return Arc cosine of x in radians.
 */
val<float> acos(const val<float>& x) noexcept;
val<double> acos(const val<double>& x) noexcept;
val<float> acosf(const val<float>& x) noexcept;

/**
 * @brief Computes the arc tangent of x.
 *
 * @param x Value.
 * @return Arc tangent of x in radians.
 */
val<float> atan(const val<float>& x) noexcept;
val<double> atan(const val<double>& x) noexcept;
val<float> atanf(const val<float>& x) noexcept;

/**
 * @brief Computes the arc tangent of y/x.
 *
 * @param y y-coordinate.
 * @param x x-coordinate.
 * @return Arc tangent of y/x in radians.
 */
val<float> atan2(const val<float>& y, const val<float>& x) noexcept;
val<double> atan2(const val<double>& y, const val<double>& x) noexcept;
val<float> atan2f(const val<float>& y, const val<float>& x) noexcept;

// Hyperbolic functions

/**
 * @brief Computes the hyperbolic sine of x.
 *
 * @param x Value.
 * @return Hyperbolic sine of x.
 */
val<float> sinh(const val<float>& x) noexcept;
val<double> sinh(const val<double>& x) noexcept;
val<float> sinhf(const val<float>& x) noexcept;

/**
 * @brief Computes the hyperbolic cosine of x.
 *
 * @param x Value.
 * @return Hyperbolic cosine of x.
 */
val<float> cosh(const val<float>& x) noexcept;
val<double> cosh(const val<double>& x) noexcept;
val<float> coshf(const val<float>& x) noexcept;

/**
 * @brief Computes the hyperbolic tangent of x.
 *
 * @param x Value.
 * @return Hyperbolic tangent of x.
 */
val<float> tanh(const val<float>& x) noexcept;
val<double> tanh(const val<double>& x) noexcept;
val<float> tanhf(const val<float>& x) noexcept;

/**
 * @brief Computes the hyperbolic arc sine of x.
 *
 * @param x Value.
 * @return Hyperbolic arc sine of x.
 */
val<float> asinh(const val<float>& x) noexcept;
val<double> asinh(const val<double>& x) noexcept;
val<float> asinhf(const val<float>& x) noexcept;

/**
 * @brief Computes the hyperbolic arc cosine of x.
 *
 * @param x Value.
 * @return Hyperbolic arc cosine of x.
 */
val<float> acosh(const val<float>& x) noexcept;
val<double> acosh(const val<double>& x) noexcept;
val<float> acoshf(const val<float>& x) noexcept;

/**
 * @brief Computes the hyperbolic arc tangent of x.
 *
 * @param x Value.
 * @return Hyperbolic arc tangent of x.
 */
val<float> atanh(const val<float>& x) noexcept;
val<double> atanh(const val<double>& x) noexcept;
val<float> atanhf(const val<float>& x) noexcept;

// Error and gamma functions

/**
 * @brief Computes the error function of x.
 *
 * @param x Value.
 * @return Error function of x.
 */
val<float> erf(const val<float>& x) noexcept;
val<double> erf(const val<double>& x) noexcept;
val<float> erff(const val<float>& x) noexcept;

/**
 * @brief Computes the complementary error function of x.
 *
 * @param x Value.
 * @return Complementary error function of x.
 */
val<float> erfc(const val<float>& x) noexcept;
val<double> erfc(const val<double>& x) noexcept;
val<float> erfcf(const val<float>& x) noexcept;

/**
 * @brief Computes the gamma function of x.
 *
 * @param x Value.
 * @return Gamma function of x.
 */
val<float> tgamma(const val<float>& x) noexcept;
val<double> tgamma(const val<double>& x) noexcept;
val<float> tgammaf(const val<float>& x) noexcept;

/**
 * @brief Computes the natural logarithm of the absolute value of the gamma
 * function of x.
 *
 * @param x Value.
 * @return Natural logarithm of the absolute value of the gamma function of x.
 */
val<float> lgamma(const val<float>& x) noexcept;
val<double> lgamma(const val<double>& x) noexcept;
val<float> lgammaf(const val<float>& x) noexcept;

// Rounding and remainder functions

/**
 * @brief Computes the smallest integer value not less than x.
 *
 * @param x Value.
 * @return Smallest integer not less than x.
 */
val<float> ceil(const val<float>& x) noexcept;
val<double> ceil(const val<double>& x) noexcept;
val<float> ceilf(const val<float>& x) noexcept;

/**
 * @brief Computes the largest integer value not greater than x.
 *
 * @param x Value.
 * @return Largest integer not greater than x.
 */
val<float> floor(const val<float>& x) noexcept;
val<double> floor(const val<double>& x) noexcept;
val<float> floorf(const val<float>& x) noexcept;

/**
 * @brief Computes the nearest integer not greater in magnitude than x.
 *
 * @param x Value.
 * @return Nearest integer not greater in magnitude than x.
 */
val<float> trunc(const val<float>& x) noexcept;
val<double> trunc(const val<double>& x) noexcept;
val<float> truncf(const val<float>& x) noexcept;

/**
 * @brief Rounds x to the nearest integer.
 *
 * @param x Value.
 * @return Nearest integer to x.
 */
val<float> round(const val<float>& x) noexcept;
val<double> round(const val<double>& x) noexcept;
val<float> roundf(const val<float>& x) noexcept;

/**
 * @brief Rounds x to the nearest integer and returns it as a long integer.
 *
 * @param x Value.
 * @return Nearest integer to x as long.
 */
val<long> lround(const val<float>& x) noexcept;
val<long> lround(const val<double>& x) noexcept;
val<long> lroundf(const val<float>& x) noexcept;

/**
 * @brief Rounds x to the nearest integer and returns it as a long long integer.
 *
 * @param x Value.
 * @return Nearest integer to x as long long.
 */
val<long long> llround(const val<float>& x) noexcept;
val<long long> llround(const val<double>& x) noexcept;
val<long long> llroundf(const val<float>& x) noexcept;

val<float> nearbyint(const val<float>& x) noexcept;
val<double> nearbyint(const val<double>& x) noexcept;
val<float> nearbyintf(const val<float>& x) noexcept;

/**
 * @brief Rounds x to the nearest integer value according to the current
 * rounding mode.
 *
 * @param x Value.
 * @return Rounded value.
 */
val<float> rint(const val<float>& x) noexcept;
val<float> rintf(const val<float>& x) noexcept;
val<double> rint(const val<double>& x) noexcept;

/**
 * @brief Rounds x to the nearest integer value according to the current
 * rounding mode and returns it as a long integer.
 *
 * @param x Value.
 * @return Rounded value as long.
 */
val<long> lrint(const val<float>& x) noexcept;
val<long> lrint(const val<double>& x) noexcept;
val<long> lrintf(const val<float>& x) noexcept;

/**
 * @brief Rounds x to the nearest integer value according to the current
 * rounding mode and returns it as a long long integer.
 *
 * @param x Value.
 * @return Rounded value as long long.
 */
val<long long> llrint(const val<float>& x) noexcept;
val<long long> llrint(const val<double>& x) noexcept;
val<long long> llrintf(const val<float>& x) noexcept;

// Floating-point manipulation functions
/**
 * @brief Breaks the floating point number x into its binary significand and an
 * integral exponent for 2.
 *
 * @param x Value.
 * @param exp Pointer to an integer to store the exponent.
 * @return Fractional part of x.
 */
val<float> frexp(const val<float>& x, const val<int*>& exp) noexcept;
val<float> frexpf(const val<float>& x, const val<int*>& exp) noexcept;
val<double> frexp(const val<double>& x, const val<int*>& exp) noexcept;

/**
 * @brief Multiplies a floating point number by an integral power of 2.
 *
 * @param x Value.
 * @param exp Exponent.
 * @return x * 2^exp.
 */
val<float> ldexp(const val<float>& x, const val<int>& exp) noexcept;
val<float> ldexpf(const val<float>& x, const val<int>& exp) noexcept;
val<double> ldexp(const val<double>& x, const val<int>& exp) noexcept;

/**
 * @brief Breaks the floating point number x into an integral and a fractional
 * part.
 *
 * @param x Value.
 * @param intpart Pointer to store the integral part.
 * @return Fractional part of x.
 */
val<float> modf(const val<float>& x, const val<float*>& intpart) noexcept;
val<float> modff(const val<float>& x, const val<float*>& intpart) noexcept;
val<double> modf(const val<double>& x, const val<double*>& intpart) noexcept;

/**
 * @brief Scales x by 2 raised to the power of exp.
 *
 * @param x Value.
 * @param exp Exponent.
 * @return x * 2^exp.
 */
val<float> scalbn(const val<float>& x, const val<int>& exp) noexcept;
val<float> scalbnf(const val<float>& x, const val<int>& exp) noexcept;
val<double> scalbn(const val<double>& x, const val<int>& exp) noexcept;

/**
 * @brief Scales x by 2 raised to the power of exp (more efficient than scalbn).
 *
 * @param x Value.
 * @param exp Exponent.
 * @return x * 2^exp.
 */
val<double> scalbln(const val<double>& x, const val<long>& exp) noexcept;
val<float> scalblnf(const val<float>& x, const val<long>& exp) noexcept;
val<float> scalbln(const val<float>& x, const val<long>& exp) noexcept;

/**
 * @brief extracts exponent of the number
 *
 * @param x Value.
 * @return  exponent of the number
 */
val<int> ilogb(const val<double>& x) noexcept;
val<int> ilogbf(const val<float>& x) noexcept;
val<int> ilogb(const val<float>& x) noexcept;

/**
 * @brief extracts exponent of the number
 *
 * @param x Value.
 * @param exp Exponent.
 * @return  exponent of the number
 */
val<double> logb(const val<double>& x) noexcept;
val<float> logbf(val<float>) noexcept;
val<float> logb(const val<float>& x) noexcept;

/**
 * @brief next representable floating-point value towards the given value
 *
 * @param x Value.
 * @return  next representable floating-point value towards the given value
 */
val<float> nextafter(const val<float>& from, const val<float>& to) noexcept;
val<double> nextafter(const val<double>& from, const val<double>& to) noexcept;
val<float> nextafterf(const val<float>& from, const val<float>& to) noexcept;
val<float> nexttoward(const val<float>& from, const val<float>& to) noexcept;
val<double> nexttoward(const val<double>& from, const val<double>& to) noexcept;
val<float> nexttowardf(const val<float>& from, const val<float>& to) noexcept;

/**
 * @brief Returns a value with the magnitude of x and the sign of y.
 *
 * @param x Magnitude value.
 * @param y Sign value.
 * @return Value with magnitude of x and sign of y.
 */
val<float> copysign(const val<float>& x, const val<float>& y) noexcept;
val<double> copysign(const val<double>& x, const val<double>& y) noexcept;
val<float> copysignf(const val<float>& x, const val<float>& y) noexcept;

// Classification functions

/**
 * @brief Determines the floating-point classification of x.
 *
 * @param x Value.
 * @return Classification macro value.
 */
val<int> fpclassify(const val<double>& x) noexcept;

/**
 * @brief Determines if x is a finite value.
 *
 * @param x Value.
 * @return Non-zero if x is finite, otherwise 0.
 */
val<bool> isfinite(const val<double>& x) noexcept;

/**
 * @brief Determines if x is an infinity value.
 *
 * @param x Value.
 * @return Non-zero if x is infinity, otherwise 0.
 */
val<bool> isinf(const val<double>& x) noexcept;

/**
 * @brief Determines if x is a NaN (Not-a-Number) value.
 *
 * @param x Value.
 * @return Non-zero if x is NaN, otherwise 0.
 */
val<bool> isnan(const val<double>& x) noexcept;

/**
 * @brief Determines if x is a normal value.
 *
 * @param x Value.
 * @return Non-zero if x is normal, otherwise 0.
 */
val<bool> isnormal(const val<double>& x) noexcept;

/**
 * @brief Determines if the sign of x is negative.
 *
 * @param x Value.
 * @return Non-zero if the sign of x is negative, otherwise 0.
 */
val<bool> signbit(const val<double>& x) noexcept;

// Comparison functions

/**
 * @brief Determines if x is greater than y.
 *
 * @param x First value.
 * @param y Second value.
 * @return Non-zero if x > y, otherwise 0.
 */
val<bool> isgreater(const val<double>& x, const val<double>& y) noexcept;

/**
 * @brief Determines if x is greater than or equal to y.
 *
 * @param x First value.
 * @param y Second value.
 * @return Non-zero if x >= y, otherwise 0.
 */
val<bool> isgreaterequal(const val<double>& x, const val<double>& y) noexcept;

/**
 * @brief Determines if x is less than y.
 *
 * @param x First value.
 * @param y Second value.
 * @return Non-zero if x < y, otherwise 0.
 */
val<bool> isless(const val<double>& x, const val<double>& y) noexcept;

/**
 * @brief Determines if x is less than or equal to y.
 *
 * @param x First value.
 * @param y Second value.
 * @return Non-zero if x <= y, otherwise 0.
 */
val<bool> islessequal(const val<double>& x, const val<double>& y) noexcept;

/**
 * @brief Determines if x is less than y or y is less than x.
 *
 * @param x First value.
 * @param y Second value.
 * @return Non-zero if x < y or y < x, otherwise 0.
 */
val<bool> islessgreater(const val<double>& x, const val<double>& y) noexcept;

/**
 * @brief Determines if either x or y is NaN.
 *
 * @param x First value.
 * @param y Second value.
 * @return Non-zero if either x or y is NaN, otherwise 0.
 */
val<bool> isunordered(const val<double>& x, const val<double>& y) noexcept;

// Mathematical special functions
#ifdef __STDCPP_MATH_SPEC_FUNCS__
/**
 * @brief Computes the associated Laguerre polynomials
 *
 * @param x First side.
 * @param y Second side.
 * @return Length of the hypotenuse.
 */
val<float> assoc_laguerre(const val<unsigned int>& n, const val<unsigned int>& m, const val<float>& x) noexcept;
val<float> assoc_laguerref(const val<unsigned int>& n, const val<unsigned int>& m, const val<float>& x) noexcept;
val<double> assoc_laguerre(const val<unsigned int>& n, const val<unsigned int>& m, const val<double>& x) noexcept;

/**
 * @brief Computes the associated Legendre functions.
 *
 * @param x First side.
 * @param y Second side.
 * @return Length of the hypotenuse.
 */
val<float> assoc_legendre(const val<uint8_t>& n, const val<uint8_t>& m, const val<float>& x) noexcept;
val<float> assoc_legendref(const val<uint8_t>& n, const val<uint8_t>& m, const val<float>& x) noexcept;
val<double> assoc_legendre(const val<uint8_t>& n, const val<uint8_t>& m, const val<double>& x) noexcept;

/**
 * @brief Computes the beta function
 *
 * @param x First side.
 * @param y Second side.
 * @return beta function
 */
val<float> betaf(const val<float>& n, const val<float>& m) noexcept;
val<double> beta(const val<double>& n, const val<double>& m) noexcept;
val<float> beta(const val<float>& n, const val<float>& m) noexcept;

/**
 * @brief complete elliptic integral of the first kind
 *
 * @param k
 * @return comp_ellint_1
 */
val<float> comp_ellint_1(const val<float>& k) noexcept;
val<double> comp_ellint_1(const val<double>& k) noexcept;
val<float> comp_ellint_1f(const val<float>& k) noexcept;

/**
 * @brief complete elliptic integral of the second kind
 *
 * @param k
 * @return comp_ellint_2
 */
val<float> comp_ellint_2(const val<float>& k) noexcept;
val<double> comp_ellint_2(const val<double>& k) noexcept;
val<float> comp_ellint_2f(const val<float>& k) noexcept;

/**
 * @brief complete elliptic integral of the third kind
 *
 * @param k
 * @return comp_ellint_3
 */
val<float> comp_ellint_3(const val<float>& k) noexcept;
val<double> comp_ellint_3(const val<double>& k) noexcept;
val<float> comp_ellint_3f(const val<float>& k) noexcept;

/**
 * @brief regular modified cylindrical Bessel functions
 *
 * @param k
 * @return comp_ellint_3
 */
val<float> cyl_bessel_i(const val<float>& nu, const val<float>& x) noexcept;
val<double> cyl_bessel_i(const val<double>& nu, const val<double>& x) noexcept;
val<float> cyl_bessel_if(const val<float>& nu, const val<float>& x) noexcept;

/**
 * @brief Computes the cylindrical Bessel function of the first kind of nu and x
 *
 * @param k
 * @return cyl_bessel_j
 */
val<float> cyl_bessel_j(const val<float>& nu, const val<float>& x) noexcept;
val<double> cyl_bessel_j(const val<double>& nu, const val<double>& x) noexcept;
val<float> cyl_bessel_jf(const val<float>& nu, const val<float>& x) noexcept;

/**
 * @brief Computes the irregular modified cylindrical Bessel function (also
 * known as modified Bessel function of the second kind) of nu and x.
 *
 * @param k
 * @return cyl_bessel_k
 */
val<float> cyl_bessel_k(const val<float>& nu, const val<float>& x) noexcept;
val<double> cyl_bessel_k(const val<double>& nu, const val<double>& x) noexcept;
val<float> cyl_bessel_kf(const val<float>& nu, const val<float>& x) noexcept;

/**
 * @brief Computes the cylindrical Neumann function (also known as Bessel
 * function of the second kind or Weber function) of nu and x
 *
 * @param k
 * @return cyl_neumann
 */
val<float> cyl_neumann(const val<float>& nu, const val<float>& x) noexcept;
val<double> cyl_neumann(const val<double>& nu, const val<double>& x) noexcept;
val<float> cyl_neumannf(const val<float>& nu, const val<float>& x) noexcept;

/**
 * @brief Computes the incomplete elliptic integral of the first kind of k and
 * phi.
 *
 * @param k elliptic modulus or eccentricity (a floating-point or integer value)
 * @param phi Jacobi amplitude (a floating-point or integer value, measured in
 * radians)
 * @return ellint_1f
 */
val<float> ellint_1(const val<float>& k, const val<float>& phi) noexcept;
val<double> ellint_1(const val<double>& k, const val<double>& phi) noexcept;
val<float> ellint_1f(const val<float>& k, const val<float>& phi) noexcept;

/**
 * @brief Computes the incomplete elliptic integral of the second kind of k and
 * phi.
 *
 * @param k elliptic modulus or eccentricity (a floating-point or integer value)
 * @param phi Jacobi amplitude (a floating-point or integer value, measured in
 * radians)
 * @return ellint_2f
 */
val<float> ellint_2(const val<float>& k, const val<float>& phi) noexcept;
val<double> ellint_2(const val<double>& k, const val<double>& phi) noexcept;
val<float> ellint_2f(const val<float>& k, const val<float>& phi) noexcept;

/**
 * @brief Computes the incomplete elliptic integral of the third kind of k and
 * phi.
 *
 * @param k elliptic modulus or eccentricity (a floating-point or integer value)
 * @param phi Jacobi amplitude (a floating-point or integer value, measured in
 * radians)
 * @return ellint_3
 */
val<float> ellint_3(const val<float>& k, const val<float>& phi) noexcept;
val<double> ellint_3(const val<double>& k, const val<double>& phi) noexcept;
val<float> expintf(const val<float>& k, const val<float>& phi) noexcept;

/**
 * @brief Computes the Exponential integral of num.
 *
 * @param num floating-point or integer value
 * @return expint
 */
val<float> expint(const val<float>& k) noexcept;
val<double> expint(const val<double>& k) noexcept;
val<float> ellint_f(const val<float>& k) noexcept;

/**
 * @brief Computes the (physicist's) Hermite polynomials of the degree n and
 * argument x.
 *
 * @param num floating-point or integer value
 * @return hermite
 */
val<float> hermite(const val<unsigned int>& k, const val<float>& x) noexcept;
val<double> hermite(const val<unsigned int>& k, const val<double>& x) noexcept;
val<float> hermitef(const val<unsigned int>& k, const val<float>& x) noexcept;

/**
 * @brief Computes the unassociated Legendre polynomials of the degree n and
 * argument x.
 *
 * @param n the degree of the polynomial
 * @param x	the argument, a floating-point or integer value
 * @return legendre
 */
val<float> legendre(const val<unsigned int>& n, const val<float>& x) noexcept;
val<double> legendre(const val<unsigned int>& n, const val<double>& x) noexcept;
val<float> legendref(const val<unsigned int>& n, const val<float>& x) noexcept;

/**
 * @brief Computes the non-associated Laguerre polynomials of the degree n and
 * argument x
 *
 * @param n the degree of the polynomial
 * @param x	the argument, a floating-point or integer value
 * @return laguerre
 */
val<float> laguerre(const val<unsigned int>& n, const val<float>& x) noexcept;
val<double> laguerre(const val<unsigned int>& n, const val<double>& x) noexcept;
val<float> laguerref(const val<unsigned int>& n, const val<float>& x) noexcept;

/**
 * @brief Computes the Riemann zeta function of num
 *
 * @param num floating-point or value
 * @return riemann_zeta
 */
val<float> riemann_zeta(const val<float>& num) noexcept;
val<double> riemann_zeta(const val<double>& num) noexcept;
val<float> riemann_zetaf(const val<float>& num) noexcept;

/**
 * @brief Computes the Riemann zeta function of num
 *
 * @param n	the order of the function
 * @return x the argument of the function
 */
val<float> sph_bessel(const val<unsigned int>& n, const val<float>& x) noexcept;
val<double> sph_bessel(const val<unsigned int>& n, const val<double>& x) noexcept;
val<float> sph_besself(const val<unsigned int>& n, const val<float>& x) noexcept;

/**
 * @brief Computes the spherical associated Legendre function of degree l, order
 * m, and polar angle theta
 *
 * @param l degree
 * @param m order
 * @param theta polar angle, measured in radians
 */
val<float> sph_legendre(const val<unsigned int>& l, const val<unsigned int>& m, const val<float>& theta) noexcept;
val<double> sph_legendre(const val<unsigned int>& l, const val<unsigned int>& m, const val<double>& theta) noexcept;
val<float> sph_legendref(const val<unsigned int>& l, const val<unsigned int>& m, const val<float>& theta) noexcept;

/**
 * @brief Computes the spherical Bessel function of the second kind, also known
 * as the spherical Neumann function, of n and x.
 *
 * @param n the order of the function
 * @param x the argument of the function
 */
val<float> sph_neumann(const val<unsigned int>& n, const val<float>& x) noexcept;
val<double> sph_neumann(const val<unsigned int>& n, const val<double>& x) noexcept;
val<float> sph_neumannf(const val<unsigned int>& n, const val<float>& x) noexcept;
#endif

} // namespace nautilus
