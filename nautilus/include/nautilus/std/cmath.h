#pragma once
#include <nautilus/val.hpp>
namespace nautilus {

/**
 * @brief Computes the absolute value of the floating-point value x.
 *
 * @param x floating-point
 * @return Absolute of x.
 */
val<float> abs(val<float> x);
val<double> abs(val<double> x);
val<int8_t> abs(val<int8_t> x);
val<int16_t> abs(val<int16_t> x);
val<int32_t> abs(val<int32_t> x);
val<int64_t> abs(val<int64_t> x);
val<float> fabs(val<float> x);
val<double> fabs(val<double> x);
val<float> fabsf(val<float> x);

/**
 * @brief Computes the floating-point remainder of x/y.
 *
 * @param x Numerator.
 * @param y Denominator.
 * @return Remainder of x/y.
 */
val<float> fmod(val<float> x, val<float> y);
val<float> fmodf(val<float> x, val<float> y);
val<double> fmod(val<double> x, val<double> y);

/**
 * @brief Computes the floating-point remainder of x/y (same as fmod).
 *
 * @param x Numerator.
 * @param y Denominator.
 * @return Remainder of x/y.
 */
val<float> remainder(val<float> x, val<float> y);
val<float> remainderf(val<float> x, val<float> y);
val<double> remainder(val<double> x, val<double> y);

/**
 * @brief Computes the remainder of the division operation x/y.
 *
 * @param x Numerator.
 * @param y Denominator.
 * @param quo Pointer to store the quotient.
 * @return Remainder of x/y.
 */
val<float> remquo(val<float> x, val<float> y, val<int*> quo);
val<float> remquof(val<float> x, val<float> y, val<int*> quo);
val<double> remquo(val<double> x, val<double> y, val<int*> quo);

/**
 * @brief Computes x * y + z as if to infinite precision and rounded only once
 * to fit the result type.
 *
 * @param x floating-point
 * @param y floating-point
 * @param z floating-point
 * @return x * y + z
 */
val<float> fma(val<float> x, val<float> y, val<float> z);
val<double> fma(val<double> x, val<double> y, val<double> z);
val<float> fmaf(val<float> x, val<float> y, val<float> z);

/**
 * @brief Computes the larger of two floating point arguments, treating NaNs as
 * missing data (between a NaN and a numeric value, the numeric value is
 * chosen).
 *
 * @param x floating-point
 * @param y floating-point
 * @return larger of two floating-point values
 */
val<float> fmax(val<float> x, val<float> y);
val<double> fmax(val<double> x, val<double> y);
val<float> fmaxf(val<float> x, val<float> y);

/**
 * @brief Computes the smaller of two floating point arguments, treating NaNs as
 * missing data (between a NaN and a numeric value, the numeric value is
 * chosen).
 *
 * @param x floating-point
 * @param y floating-point
 * @return smaller of two floating-point values
 */
val<float> fmin(val<float> x, val<float> y);
val<double> fmin(val<double> x, val<double> y);
val<float> fminf(val<float> x, val<float> y);

/**
 * @brief Computes he positive difference between x and y, that is, if x > y,
 * returns x - y, otherwise (i.e. if x <= y) returns +0
 *
 * @param x floating-point
 * @param y floating-point
 * @return smaller of two floating-point values
 */
val<float> fdim(val<float> x, val<float> y);
val<double> fdim(val<double> x, val<double> y);
val<float> fdimf(val<float> x, val<float> y);

/**
 * @brief Generates a quiet NaN (Not-a-Number).
 *
 * @param tagp Implementation-specific C-string to distinguish different NaN
 * values.
 * @return Quiet NaN.
 */
val<float> nanf(val<const char*> tagp);
val<double> nan(val<const char*> tagp);

// Linear interpolation
val<float> lerp(val<float> a, val<float> b, val<float> c);
val<double> lerp(val<double> a, val<double> b, val<double> c);

// Exponential and logarithmic functions

/**
 * @brief Computes the exponential function of x.
 *
 * @param x Value.
 * @return Exponential of x.
 */
val<float> exp(val<float> x);
val<double> exp(val<double> x);
val<float> expf(val<float> x);

/**
 * @brief Computes the returns 2 raised to the given power.
 *
 * @param x Value.
 * @return returns 2 raised to the given power.
 */
val<float> exp2(val<float> x);
val<double> exp2(val<double> x);
val<float> exp2f(val<float> x);

/**
 * @brief Computes the e (Euler's number, 2.7182818...) raised to the given
 * power num, minus 1.0.
 *
 * @param num floating-point .
 * @return returns e raised to the given power, minus one.
 */
val<float> expm1(val<float> num);
val<double> expm1(val<double> num);
val<float> expm1f(val<float> num);

/**
 * @brief Computes the natural logarithm of x.
 *
 * @param x Value.
 * @return Natural logarithm of x.
 */
val<float> log(val<float> x);
val<double> log(val<double> x);
val<float> logf(val<float> x);

/**
 * @brief Computes the base-10 logarithm of x.
 *
 * @param x Value.
 * @return Base-10 logarithm of x.
 */
val<float> log10(val<float> x);
val<double> log10(val<double> x);
#if defined(_LIBCPP_VERSION)
val<float> log10f(val<float> x);
#endif
/**
 * @brief Computes the base-2 logarithm of x.
 *
 * @param x Value.
 * @return Base-2 logarithm of x.
 */
val<float> log2(val<float> x);
val<double> log2(val<double> x);


/**
 * @brief natural logarithm (to base e) of 1 plus the given number (ln(1+x))
 *
 * @param x Value.
 * @return natural logarithm
 */
val<float> log1p(val<float> x);
val<double> log1p(val<double> x);
val<float> log1pf(val<float> x);

// Power functions

/**
 * @brief Computes x raised to the power of y.
 *
 * @param x Base.
 * @param y Exponent.
 * @return x^y.
 */
val<float> pow(val<float> x, val<float> y);
val<double> pow(val<double> x, val<double> y);
val<float> powf(val<float> x, val<float> y);

/**
 * @brief Computes the square root of x.
 *
 * @param x Value.
 * @return Square root of x.
 */
val<float> sqrt(val<float> x);
val<double> sqrt(val<double> x);
val<float> sqrtf(val<float> x);

/**
 * @brief Computes the cube root of x.
 *
 * @param x Value.
 * @return Cube root of x.
 */
val<float> cbrt(val<float> x);
val<double> cbrt(val<double> x);
val<float> cbrtf(val<float> x);

/**
 * @brief Computes the Euclidean distance between the points (x, 0) and (0, y).
 *
 * @param x First value.
 * @param y Second value.
 * @return sqrt(x^2 + y^2).
 */
val<float> hypot(val<float> x, val<float> y);
val<double> hypot(val<double> x, val<double> y);
val<float> hypotf(val<float> x, val<float> y);

// Trigonometric functions

/**
 * @brief Computes the sine of x.
 *
 * @param x Angle in radians.
 * @return Sine of x.
 */
val<float> sin(val<float> x);
val<double> sin(val<double> x);
val<float> sinf(val<float> x);

/**
 * @brief Computes the cosine of x.
 *
 * @param x Angle in radians.
 * @return Cosine of x.
 */
val<float> cos(val<float> x);
val<double> cos(val<double> x);
val<float> cosf(val<float> x);

/**
 * @brief Computes the tangent of x.
 *
 * @param x Angle in radians.
 * @return Tangent of x.
 */
val<float> tan(val<float> x);
val<double> tan(val<double> x);
val<float> tanf(val<float> x);

/**
 * @brief Computes the arc sine of x.
 *
 * @param x Value.
 * @return Arc sine of x in radians.
 */
val<float> asin(val<float> x);
val<double> asin(val<double> x);
val<float> asinf(val<float> x);

/**
 * @brief Computes the arc cosine of x.
 *
 * @param x Value.
 * @return Arc cosine of x in radians.
 */
val<float> acos(val<float> x);
val<double> acos(val<double> x);
val<float> acosf(val<float> x);

/**
 * @brief Computes the arc tangent of x.
 *
 * @param x Value.
 * @return Arc tangent of x in radians.
 */
val<float> atan(val<float> x);
val<double> atan(val<double> x);
val<float> atanf(val<float> x);

/**
 * @brief Computes the arc tangent of y/x.
 *
 * @param y y-coordinate.
 * @param x x-coordinate.
 * @return Arc tangent of y/x in radians.
 */
val<float> atan2(val<float> y, val<float> x);
val<double> atan2(val<double> y, val<double> x);
val<float> atan2f(val<float> y, val<float> x);

// Hyperbolic functions

/**
 * @brief Computes the hyperbolic sine of x.
 *
 * @param x Value.
 * @return Hyperbolic sine of x.
 */
val<float> sinh(val<float> x);
val<double> sinh(val<double> x);
val<float> sinhf(val<float> x);

/**
 * @brief Computes the hyperbolic cosine of x.
 *
 * @param x Value.
 * @return Hyperbolic cosine of x.
 */
val<float> cosh(val<float> x);
val<double> cosh(val<double> x);
val<float> coshf(val<float> x);

/**
 * @brief Computes the hyperbolic tangent of x.
 *
 * @param x Value.
 * @return Hyperbolic tangent of x.
 */
val<float> tanh(val<float> x);
val<double> tanh(val<double> x);
val<float> tanhf(val<float> x);

/**
 * @brief Computes the hyperbolic arc sine of x.
 *
 * @param x Value.
 * @return Hyperbolic arc sine of x.
 */
val<float> asinh(val<float> x);
val<double> asinh(val<double> x);
val<float> asinhf(val<float> x);

/**
 * @brief Computes the hyperbolic arc cosine of x.
 *
 * @param x Value.
 * @return Hyperbolic arc cosine of x.
 */
val<float> acosh(val<float> x);
val<double> acosh(val<double> x);
val<float> acoshf(val<float> x);

/**
 * @brief Computes the hyperbolic arc tangent of x.
 *
 * @param x Value.
 * @return Hyperbolic arc tangent of x.
 */
val<float> atanh(val<float> x);
val<double> atanh(val<double> x);
val<float> atanhf(val<float> x);

// Error and gamma functions

/**
 * @brief Computes the error function of x.
 *
 * @param x Value.
 * @return Error function of x.
 */
val<float> erf(val<float> x);
val<double> erf(val<double> x);
val<float> erff(val<float> x);

/**
 * @brief Computes the complementary error function of x.
 *
 * @param x Value.
 * @return Complementary error function of x.
 */
val<float> erfc(val<float> x);
val<double> erfc(val<double> x);
val<float> erfcf(val<float> x);

/**
 * @brief Computes the gamma function of x.
 *
 * @param x Value.
 * @return Gamma function of x.
 */
val<float> tgamma(val<float> x);
val<double> tgamma(val<double> x);
val<float> tgammaf(val<float> x);

/**
 * @brief Computes the natural logarithm of the absolute value of the gamma
 * function of x.
 *
 * @param x Value.
 * @return Natural logarithm of the absolute value of the gamma function of x.
 */
val<float> lgamma(val<float> x);
val<double> lgamma(val<double> x);
val<float> lgammaf(val<float> x);

// Rounding and remainder functions

/**
 * @brief Computes the smallest integer value not less than x.
 *
 * @param x Value.
 * @return Smallest integer not less than x.
 */
val<float> ceil(val<float> x);
val<double> ceil(val<double> x);
val<float> ceilf(val<float> x);

/**
 * @brief Computes the largest integer value not greater than x.
 *
 * @param x Value.
 * @return Largest integer not greater than x.
 */
val<float> floor(val<float> x);
val<double> floor(val<double> x);
val<float> floorf(val<float> x);

/**
 * @brief Computes the nearest integer not greater in magnitude than x.
 *
 * @param x Value.
 * @return Nearest integer not greater in magnitude than x.
 */
val<float> trunc(val<float> x);
val<double> trunc(val<double> x);
val<float> truncf(val<float> x);

/**
 * @brief Rounds x to the nearest integer.
 *
 * @param x Value.
 * @return Nearest integer to x.
 */
val<float> round(val<float> x);
val<double> round(val<double> x);
val<float> roundf(val<float> x);

/**
 * @brief Rounds x to the nearest integer and returns it as a long integer.
 *
 * @param x Value.
 * @return Nearest integer to x as long.
 */
val<long> lround(val<float> x);
val<long> lround(val<double> x);
val<long> lroundf(val<float> x);

/**
 * @brief Rounds x to the nearest integer and returns it as a long long integer.
 *
 * @param x Value.
 * @return Nearest integer to x as long long.
 */
val<long long> llround(val<float> x);
val<long long> llround(val<double> x);
val<long long> llroundf(val<float> x);

val<float> nearbyint(val<float> x);
val<double> nearbyint(val<double> x);
val<float> nearbyintf(val<float> x);

/**
 * @brief Rounds x to the nearest integer value according to the current
 * rounding mode.
 *
 * @param x Value.
 * @return Rounded value.
 */
val<float> rint(val<float> x);
val<float> rintf(val<float> x);
val<double> rint(val<double> x);

/**
 * @brief Rounds x to the nearest integer value according to the current
 * rounding mode and returns it as a long integer.
 *
 * @param x Value.
 * @return Rounded value as long.
 */
val<long> lrint(val<float> x);
val<long> lrint(val<double> x);
val<long> lrintf(val<float> x);

/**
 * @brief Rounds x to the nearest integer value according to the current
 * rounding mode and returns it as a long long integer.
 *
 * @param x Value.
 * @return Rounded value as long long.
 */
val<long long> llrint(val<float> x);
val<long long> llrint(val<double> x);
val<long long> llrintf(val<float> x);

// Floating-point manipulation functions
/**
 * @brief Breaks the floating point number x into its binary significand and an
 * integral exponent for 2.
 *
 * @param x Value.
 * @param exp Pointer to an integer to store the exponent.
 * @return Fractional part of x.
 */
val<float> frexp(val<float> x, val<int*> exp);
val<float> frexpf(val<float> x, val<int*> exp);
val<double> frexp(val<double> x, val<int*> exp);

/**
 * @brief Multiplies a floating point number by an integral power of 2.
 *
 * @param x Value.
 * @param exp Exponent.
 * @return x * 2^exp.
 */
val<float> ldexp(val<float> x, val<int> exp);
val<float> ldexpf(val<float> x, val<int> exp);
val<double> ldexp(val<double> x, val<int> exp);

/**
 * @brief Breaks the floating point number x into an integral and a fractional
 * part.
 *
 * @param x Value.
 * @param intpart Pointer to store the integral part.
 * @return Fractional part of x.
 */
val<float> modf(val<float> x, val<float*> intpart);
val<float> modff(val<float> x, val<float*> intpart);
val<double> modf(val<double> x, val<double*> intpart);

/**
 * @brief Scales x by 2 raised to the power of exp.
 *
 * @param x Value.
 * @param exp Exponent.
 * @return x * 2^exp.
 */
val<float> scalbn(val<float> x, val<int> exp);
val<float> scalbnf(val<float> x, val<int> exp);
val<double> scalbn(val<double> x, val<int> exp);

/**
 * @brief Scales x by 2 raised to the power of exp (more efficient than scalbn).
 *
 * @param x Value.
 * @param exp Exponent.
 * @return x * 2^exp.
 */
val<double> scalbln(val<double> x, val<long> exp);
val<float> scalblnf(val<float> x, val<long> exp);
val<float> scalbln(val<float> x, val<long> exp);

/**
 * @brief extracts exponent of the number
 *
 * @param x Value.
 * @return  exponent of the number
 */
val<int> ilogb(val<double> x);
val<int> ilogbf(val<float> x);
val<int> ilogb(val<float> x);

/**
 * @brief extracts exponent of the number
 *
 * @param x Value.
 * @param exp Exponent.
 * @return  exponent of the number
 */
val<double> logb(val<double> x);
val<float> logbf(val<float>);
val<float> logb(val<float> x);

/**
 * @brief next representable floating-point value towards the given value
 *
 * @param x Value.
 * @return  next representable floating-point value towards the given value
 */
val<float> nextafter(val<float> from, val<float> to);
val<double> nextafter(val<double> from, val<double> to);
val<float> nextafterf(val<float> from, val<float> to);
val<float> nexttoward(val<float> from, val<float> to);
val<double> nexttoward(val<double> from, val<double> to);
val<float> nexttowardf(val<float> from, val<float> to);

/**
 * @brief Returns a value with the magnitude of x and the sign of y.
 *
 * @param x Magnitude value.
 * @param y Sign value.
 * @return Value with magnitude of x and sign of y.
 */
val<float> copysign(val<float> x, val<float> y);
val<double> copysign(val<double> x, val<double> y);
val<float> copysignf(val<float> x, val<float> y);

// Classification functions

/**
 * @brief Determines the floating-point classification of x.
 *
 * @param x Value.
 * @return Classification macro value.
 */
val<int> fpclassify(val<double> x);

/**
 * @brief Determines if x is a finite value.
 *
 * @param x Value.
 * @return Non-zero if x is finite, otherwise 0.
 */
val<bool> isfinite(val<double> x);

/**
 * @brief Determines if x is an infinity value.
 *
 * @param x Value.
 * @return Non-zero if x is infinity, otherwise 0.
 */
val<bool> isinf(val<double> x);

/**
 * @brief Determines if x is a NaN (Not-a-Number) value.
 *
 * @param x Value.
 * @return Non-zero if x is NaN, otherwise 0.
 */
val<bool> isnan(val<double> x);

/**
 * @brief Determines if x is a normal value.
 *
 * @param x Value.
 * @return Non-zero if x is normal, otherwise 0.
 */
val<bool> isnormal(val<double> x);

/**
 * @brief Determines if the sign of x is negative.
 *
 * @param x Value.
 * @return Non-zero if the sign of x is negative, otherwise 0.
 */
val<bool> signbit(val<double> x);

// Comparison functions

/**
 * @brief Determines if x is greater than y.
 *
 * @param x First value.
 * @param y Second value.
 * @return Non-zero if x > y, otherwise 0.
 */
val<bool> isgreater(val<double> x, val<double> y);

/**
 * @brief Determines if x is greater than or equal to y.
 *
 * @param x First value.
 * @param y Second value.
 * @return Non-zero if x >= y, otherwise 0.
 */
val<bool> isgreaterequal(val<double> x, val<double> y);

/**
 * @brief Determines if x is less than y.
 *
 * @param x First value.
 * @param y Second value.
 * @return Non-zero if x < y, otherwise 0.
 */
val<bool> isless(val<double> x, val<double> y);

/**
 * @brief Determines if x is less than or equal to y.
 *
 * @param x First value.
 * @param y Second value.
 * @return Non-zero if x <= y, otherwise 0.
 */
val<bool> islessequal(val<double> x, val<double> y);

/**
 * @brief Determines if x is less than y or y is less than x.
 *
 * @param x First value.
 * @param y Second value.
 * @return Non-zero if x < y or y < x, otherwise 0.
 */
val<bool> islessgreater(val<double> x, val<double> y);

/**
 * @brief Determines if either x or y is NaN.
 *
 * @param x First value.
 * @param y Second value.
 * @return Non-zero if either x or y is NaN, otherwise 0.
 */
val<bool> isunordered(val<double> x, val<double> y);

// Mathematical special functions
#ifdef __STDCPP_MATH_SPEC_FUNCS__
/**
 * @brief Computes the associated Laguerre polynomials
 *
 * @param x First side.
 * @param y Second side.
 * @return Length of the hypotenuse.
 */
val<float> assoc_laguerre(val<unsigned int> n, val<unsigned int> m, val<float> x);
val<float> assoc_laguerref(val<unsigned int> n, val<unsigned int> m, val<float> x);
val<double> assoc_laguerre(val<unsigned int> n, val<unsigned int> m, val<double> x);

/**
 * @brief Computes the associated Legendre functions.
 *
 * @param x First side.
 * @param y Second side.
 * @return Length of the hypotenuse.
 */
val<float> assoc_legendre(val<uint8_t> n, val<uint8_t> m, val<float> x);
val<float> assoc_legendref(val<uint8_t> n, val<uint8_t> m, val<float> x);
val<double> assoc_legendre(val<uint8_t> n, val<uint8_t> m, val<double> x);

/**
 * @brief Computes the beta function
 *
 * @param x First side.
 * @param y Second side.
 * @return beta function
 */
val<float> betaf(val<float> n, val<float> m);
val<double> beta(val<double> n, val<double> m);
val<float> beta(val<float> n, val<float> m);

/**
 * @brief complete elliptic integral of the first kind
 *
 * @param k
 * @return comp_ellint_1
 */
val<float> comp_ellint_1(val<float> k);
val<double> comp_ellint_1(val<double> k);
val<float> comp_ellint_1f(val<float> k);

/**
 * @brief complete elliptic integral of the second kind
 *
 * @param k
 * @return comp_ellint_2
 */
val<float> comp_ellint_2(val<float> k);
val<double> comp_ellint_2(val<double> k);
val<float> comp_ellint_2f(val<float> k);

/**
 * @brief complete elliptic integral of the third kind
 *
 * @param k
 * @return comp_ellint_3
 */
val<float> comp_ellint_3(val<float> k);
val<double> comp_ellint_3(val<double> k);
val<float> comp_ellint_3f(val<float> k);

/**
 * @brief regular modified cylindrical Bessel functions
 *
 * @param k
 * @return comp_ellint_3
 */
val<float> cyl_bessel_i(val<float> nu, val<float> x);
val<double> cyl_bessel_i(val<double> nu, val<double> x);
val<float> cyl_bessel_if(val<float> nu, val<float> x);

/**
 * @brief Computes the cylindrical Bessel function of the first kind of nu and x
 *
 * @param k
 * @return cyl_bessel_j
 */
val<float> cyl_bessel_j(val<float> nu, val<float> x);
val<double> cyl_bessel_j(val<double> nu, val<double> x);
val<float> cyl_bessel_jf(val<float> nu, val<float> x);

/**
 * @brief Computes the irregular modified cylindrical Bessel function (also
 * known as modified Bessel function of the second kind) of nu and x.
 *
 * @param k
 * @return cyl_bessel_k
 */
val<float> cyl_bessel_k(val<float> nu, val<float> x);
val<double> cyl_bessel_k(val<double> nu, val<double> x);
val<float> cyl_bessel_kf(val<float> nu, val<float> x);

/**
 * @brief Computes the cylindrical Neumann function (also known as Bessel
 * function of the second kind or Weber function) of nu and x
 *
 * @param k
 * @return cyl_neumann
 */
val<float> cyl_neumann(val<float> nu, val<float> x);
val<double> cyl_neumann(val<double> nu, val<double> x);
val<float> cyl_neumannf(val<float> nu, val<float> x);

/**
 * @brief Computes the incomplete elliptic integral of the first kind of k and
 * phi.
 *
 * @param k elliptic modulus or eccentricity (a floating-point or integer value)
 * @param phi Jacobi amplitude (a floating-point or integer value, measured in
 * radians)
 * @return ellint_1f
 */
val<float> ellint_1(val<float> k, val<float> phi);
val<double> ellint_1(val<double> k, val<double> phi);
val<float> ellint_1f(val<float> k, val<float> phi);

/**
 * @brief Computes the incomplete elliptic integral of the second kind of k and
 * phi.
 *
 * @param k elliptic modulus or eccentricity (a floating-point or integer value)
 * @param phi Jacobi amplitude (a floating-point or integer value, measured in
 * radians)
 * @return ellint_2f
 */
val<float> ellint_2(val<float> k, val<float> phi);
val<double> ellint_2(val<double> k, val<double> phi);
val<float> ellint_2f(val<float> k, val<float> phi);

/**
 * @brief Computes the incomplete elliptic integral of the third kind of k and
 * phi.
 *
 * @param k elliptic modulus or eccentricity (a floating-point or integer value)
 * @param phi Jacobi amplitude (a floating-point or integer value, measured in
 * radians)
 * @return ellint_3
 */
val<float> ellint_3(val<float> k, val<float> phi);
val<double> ellint_3(val<double> k, val<double> phi);
val<float> expintf(val<float> k, val<float> phi);

/**
 * @brief Computes the Exponential integral of num.
 *
 * @param num floating-point or integer value
 * @return expint
 */
val<float> expint(val<float> k);
val<double> expint(val<double> k);
val<float> ellint_f(val<float> k);

/**
 * @brief Computes the (physicist's) Hermite polynomials of the degree n and
 * argument x.
 *
 * @param num floating-point or integer value
 * @return hermite
 */
val<float> hermite(val<unsigned int> k, val<float> x);
val<double> hermite(val<unsigned int> k, val<double> x);
val<float> hermitef(val<unsigned int> k, val<float> x);

/**
 * @brief Computes the unassociated Legendre polynomials of the degree n and
 * argument x.
 *
 * @param n the degree of the polynomial
 * @param x	the argument, a floating-point or integer value
 * @return legendre
 */
val<float> legendre(val<unsigned int> n, val<float> x);
val<double> legendre(val<unsigned int> n, val<double> x);
val<float> legendref(val<unsigned int> n, val<float> x);

/**
 * @brief Computes the non-associated Laguerre polynomials of the degree n and
 * argument x
 *
 * @param n the degree of the polynomial
 * @param x	the argument, a floating-point or integer value
 * @return laguerre
 */
val<float> laguerre(val<unsigned int> n, val<float> x);
val<double> laguerre(val<unsigned int> n, val<double> x);
val<float> laguerref(val<unsigned int> n, val<float> x);

/**
 * @brief Computes the Riemann zeta function of num
 *
 * @param num floating-point or value
 * @return riemann_zeta
 */
val<float> riemann_zeta(val<float> num);
val<double> riemann_zeta(val<double> num);
val<float> riemann_zetaf(val<float> num);

/**
 * @brief Computes the Riemann zeta function of num
 *
 * @param n	the order of the function
 * @return x the argument of the function
 */
val<float> sph_bessel(val<unsigned int> n, val<float> x);
val<double> sph_bessel(val<unsigned int> n, val<double> x);
val<float> sph_besself(val<unsigned int> n, val<float> x);

/**
 * @brief Computes the spherical associated Legendre function of degree l, order
 * m, and polar angle theta
 *
 * @param l degree
 * @param m order
 * @param theta polar angle, measured in radians
 */
val<float> sph_legendre(val<unsigned int> l, val<unsigned int> m, val<float> theta);
val<double> sph_legendre(val<unsigned int> l, val<unsigned int> m, val<double> theta);
val<float> sph_legendref(val<unsigned int> l, val<unsigned int> m, val<float> theta);

/**
 * @brief Computes the spherical Bessel function of the second kind, also known
 * as the spherical Neumann function, of n and x.
 *
 * @param n the order of the function
 * @param x the argument of the function
 */
val<float> sph_neumann(val<unsigned int> n, val<float> x);
val<double> sph_neumann(val<unsigned int> n, val<double> x);
val<float> sph_neumannf(val<unsigned int> n, val<float> x);
#endif

} // namespace nautilus
