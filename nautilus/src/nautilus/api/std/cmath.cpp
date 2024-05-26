#include <cmath>
#include <nautilus/function.hpp>
#include <nautilus/std/cmath.h>
#include <numbers>
namespace nautilus {

val<float> abs(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::abs(x); }, x);
}
val<double> abs(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::abs(x); }, x);
}
val<float> fabs(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::fabs(x); }, x);
}
val<double> fabs(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::fabs(x); }, x);
}
val<float> fabsf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::fabsf(x); }, x);
}

val<float> sin(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::sin(x); }, x);
}

val<double> sin(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::sin(x); }, x);
}

val<float> sinf(val<float> x) {
	return sin(x);
}

val<float> tan(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::tan(x); }, x);
}
val<double> tan(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::tan(x); }, x);
}
val<float> tanf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::tanf(x); }, x);
}

val<float> asin(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::asin(x); }, x);
}
val<double> asin(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::asin(x); }, x);
}
val<float> asinf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::asinf(x); }, x);
}

val<float> cos(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::cos(x); }, x);
}
val<double> cos(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::cos(x); }, x);
}
val<float> cosf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::cosf(x); }, x);
}

val<float> acos(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::asin(x); }, x);
}
val<double> acos(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::asin(x); }, x);
}
val<float> acosf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::asin(x); }, x);
}

val<float> atan(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::atan(x); }, x);
}
val<double> atan(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::atan(x); }, x);
}
val<float> atanf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::atanf(x); }, x);
}

val<float> atan2(val<float> y, val<float> x) {
	return invoke<>(
	    +[](float y, float x) { return std::atan2(y, x); }, y, x);
}
val<double> atan2(val<double> y, val<double> x) {
	return invoke<>(
	    +[](double y, double x) { return std::atan2(y, x); }, y, x);
}
val<float> atan2f(val<float> y, val<float> x) {
	return invoke<>(
	    +[](float y, float x) { return std::atan2(y, x); }, y, x);
}

val<float> sinh(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::sinh(x); }, x);
}
val<double> sinh(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::sinh(x); }, x);
}
val<float> sinhf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::sinh(x); }, x);
}

val<float> cosh(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::cosh(x); }, x);
}
val<double> cosh(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::cosh(x); }, x);
}
val<float> coshf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::coshf(x); }, x);
}

val<float> tanh(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::tanh(x); }, x);
}
val<double> tanh(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::tanh(x); }, x);
}
val<float> tanhf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::tanhf(x); }, x);
}

val<float> asinh(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::asinh(x); }, x);
}
val<double> asinh(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::asinh(x); }, x);
}
val<float> asinhf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::asinhf(x); }, x);
}

val<float> acosh(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::acosh(x); }, x);
}
val<double> acosh(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::acosh(x); }, x);
}
val<float> acoshf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::acoshf(x); }, x);
}

val<float> atanh(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::atanh(x); }, x);
}
val<double> atanh(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::atanh(x); }, x);
}
val<float> atanhf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::atanhf(x); }, x);
}

val<float> erf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::erf(x); }, x);
}
val<double> erf(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::erf(x); }, x);
}
val<float> erff(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::erff(x); }, x);
}

val<float> erfc(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::erfc(x); }, x);
}
val<double> erfc(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::erfc(x); }, x);
}
val<float> erfcf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::erfcf(x); }, x);
}

val<float> tgamma(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::tgamma(x); }, x);
}
val<double> tgamma(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::tgamma(x); }, x);
}
val<float> tgammaf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::tgammaf(x); }, x);
}

val<float> lgamma(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::lgamma(x); }, x);
}
val<double> lgamma(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::lgamma(x); }, x);
}
val<float> lgammaf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::lgammaf(x); }, x);
}

val<float> ceil(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::ceil(x); }, x);
}
val<double> ceil(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::ceil(x); }, x);
}
val<float> ceilf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::ceilf(x); }, x);
}

val<float> floor(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::floor(x); }, x);
}
val<double> floor(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::floor(x); }, x);
}
val<float> floorf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::floorf(x); }, x);
}

val<float> trunc(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::trunc(x); }, x);
}
val<double> trunc(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::trunc(x); }, x);
}
val<float> truncf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::truncf(x); }, x);
}

val<float> round(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::round(x); }, x);
}
val<double> round(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::round(x); }, x);
}
val<float> roundf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::roundf(x); }, x);
}

val<long> lround(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::lround(x); }, x);
}
val<long> lround(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::lround(x); }, x);
}
val<long> lroundf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::lroundf(x); }, x);
}

val<long long> llround(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::llround(x); }, x);
}
val<long long> llround(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::llround(x); }, x);
}
val<long long> llroundf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::llroundf(x); }, x);
}

val<float> nearbyint(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::nearbyint(x); }, x);
}
val<double> nearbyint(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::nearbyint(x); }, x);
}
val<float> nearbyintf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::nearbyintf(x); }, x);
}

val<float> rint(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::rint(x); }, x);
}
val<float> rintf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::rintf(x); }, x);
}
val<double> rint(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::rint(x); }, x);
}

val<long> lrint(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::lrint(x); }, x);
}
val<long> lrint(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::lrint(x); }, x);
}
val<long> lrintf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::lrintf(x); }, x);
}

val<long long> llrint(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::llrint(x); }, x);
}
val<long long> llrint(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::llrint(x); }, x);
}
val<long long> llrintf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::llrint(x); }, x);
}

val<float> frexp(val<float> x, val<int*> exp) {
	return invoke<>(
	    +[](float x, int* exp) { return std::frexp(x, exp); }, x, exp);
}
val<float> frexpf(val<float> x, val<int*> exp) {
	return invoke<>(
	    +[](float x, int* exp) { return std::frexp(x, exp); }, x, exp);
}
val<double> frexp(val<double> x, val<int*> exp) {
	return invoke<>(
	    +[](double x, int* exp) { return std::frexp(x, exp); }, x, exp);
}

val<float> ldexp(val<float> x, val<int> exp) {
	return invoke<>(
	    +[](float x, int exp) { return std::ldexp(x, exp); }, x, exp);
}
val<float> ldexpf(val<float> x, val<int> exp) {
	return invoke<>(
	    +[](float x, int exp) { return std::ldexpf(x, exp); }, x, exp);
}
val<double> ldexp(val<double> x, val<int> exp) {
	return invoke<>(
	    +[](double x, int exp) { return std::ldexp(x, exp); }, x, exp);
}

val<float> modf(val<float> x, val<float*> intpart) {
	return invoke<>(
	    +[](float x, float* intpart) { return std::modf(x, intpart); }, x, intpart);
}
val<float> modff(val<float> x, val<float*> intpart) {
	return invoke<>(
	    +[](float x, float* intpart) { return std::modff(x, intpart); }, x, intpart);
}
val<double> modf(val<double> x, val<double*> intpart) {
	return invoke<>(
	    +[](double x, double* intpart) { return std::modf(x, intpart); }, x, intpart);
}

val<float> scalbn(val<float> x, val<int> exp) {
	return invoke<>(
	    +[](float x, int exp) { return std::scalbn(x, exp); }, x, exp);
}
val<float> scalbnf(val<float> x, val<int> exp) {
	return invoke<>(
	    +[](float x, int exp) { return std::scalbnf(x, exp); }, x, exp);
}
val<double> scalbn(val<double> x, val<int> exp) {
	return invoke<>(
	    +[](double x, int exp) { return std::scalbn(x, exp); }, x, exp);
}

val<double> scalbln(val<double> x, val<long> exp) {
	return invoke<>(
	    +[](double x, long exp) { return std::scalbln(x, exp); }, x, exp);
}
val<float> scalblnf(val<float> x, val<long> exp) {
	return invoke<>(
	    +[](float x, long exp) { return std::scalbln(x, exp); }, x, exp);
}
val<float> scalbln(val<float> x, val<long> exp) {
	return invoke<>(
	    +[](float x, long exp) { return std::scalbln(x, exp); }, x, exp);
}

val<int> ilogb(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::ilogb(x); }, x);
}
val<int> ilogbf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::ilogbf(x); }, x);
}
val<int> ilogb(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::ilogb(x); }, x);
}

val<double> logb(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::logb(x); }, x);
}
val<float> logbf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::logbf(x); }, x);
}
val<float> logb(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::logb(x); }, x);
}

val<float> nextafter(val<float> from, val<float> to) {
	return invoke<>(
	    +[](float from, float to) { return std::nextafter(from, to); }, from, to);
}
val<double> nextafter(val<double> from, val<double> to) {
	return invoke<>(
	    +[](double from, double to) { return std::nextafter(from, to); }, from, to);
}
val<float> nextafterf(val<float> from, val<float> to) {
	return invoke<>(
	    +[](float from, float to) { return std::nextafter(from, to); }, from, to);
}
val<float> nexttoward(val<float> from, val<float> to) {
	return invoke<>(
	    +[](float from, float to) { return std::nextafter(from, to); }, from, to);
}
val<double> nexttoward(val<double> from, val<double> to) {
	return invoke<>(
	    +[](double from, double to) { return std::nextafter(from, to); }, from, to);
}
val<float> nexttowardf(val<float> from, val<float> to) {
	return invoke<>(
	    +[](float from, float to) { return std::nextafter(from, to); }, from, to);
}

val<float> copysign(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::copysign(x, y); }, x, y);
}
val<double> copysign(val<double> x, val<double> y) {
	return invoke<>(
	    +[](double x, double y) { return std::copysign(x, y); }, x, y);
}
val<float> copysignf(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::copysign(x, y); }, x, y);
}

// Exponential and logarithmic functions
val<double> exp(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::exp(x); }, x);
}

val<float> log(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::log(x); }, x);
}
val<double> log(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::log(x); }, x);
}
val<float> logf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::logf(x); }, x);
}

val<float> log10(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::log10(x); }, x);
}
val<double> log10(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::log10(x); }, x);
}
val<float> log10f(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::log10f(x); }, x);
}

val<float> expm1(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::expm1(x); }, x);
}
val<double> expm1(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::expm1(x); }, x);
}
val<float> expm1f(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::expm1f(x); }, x);
}

val<float> log1p(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::log1p(x); }, x);
}
val<double> log1p(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::log1p(x); }, x);
}
val<float> log1pf(val<float> x) {
	return invoke<>(
	    +[](double x) { return std::log1pf(x); }, x);
}

// Power functions
val<float> pow(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::pow(x, y); }, x, y);
}
val<double> pow(val<double> x, val<double> y) {
	return invoke<>(
	    +[](double x, double y) { return std::pow(x, y); }, x, y);
}
val<float> powf(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::powf(x, y); }, x, y);
}

val<float> sqrt(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::sqrt(x); }, x);
}
val<double> sqrt(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::sqrt(x); }, x);
}
val<float> sqrtf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::sqrt(x); }, x);
}

val<float> cbrt(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::cbrt(x); }, x);
}
val<double> cbrt(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::cbrt(x); }, x);
}
val<float> cbrtf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::cbrtf(x); }, x);
}

val<float> hypot(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::hypot(x, y); }, x, y);
}
val<double> hypot(val<double> x, val<double> y) {
	return invoke<>(
	    +[](double x, double y) { return std::hypot(x, y); }, x, y);
}
val<float> hypotf(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::hypotf(x, y); }, x, y);
}

val<float> fmod(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::fmod(x, y); }, x, y);
}
val<float> fmodf(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::fmodf(x, y); }, x, y);
}

val<double> remainder(val<double> x, val<double> y) {
	return invoke<>(
	    +[](double x, double y) { return std::remainder(x, y); }, x, y);
}

val<float> remainder(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::remainder(x, y); }, x, y);
}
val<float> remainderf(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::remainderf(x, y); }, x, y);
}

val<double> remquo(val<double> x, val<double> y, val<int*> quo) {
	return invoke<>(
	    +[](double x, double y, int* quo) { return std::remquo(x, y, quo); }, x, y, quo);
}

val<float> remquo(val<float> x, val<float> y, val<int*> quo) {
	return invoke<>(
	    +[](float x, float y, int* quo) { return std::remquo(x, y, quo); }, x, y, quo);
}
val<float> remquof(val<float> x, val<float> y, val<int*> quo) {
	return invoke<>(
	    +[](float x, float y, int* quo) { return std::remquof(x, y, quo); }, x, y, quo);
}

val<double> fma(val<double> x, val<double> y, val<double> z) {
	return invoke<>(
	    +[](double x, double y, double z) { return std::fma(x, y, z); }, x, y, z);
}

val<float> fma(val<float> x, val<float> y, val<float> z) {
	return invoke<>(
	    +[](float x, float y, float z) { return std::fma(x, y, z); }, x, y, z);
}
val<float> fmaf(val<float> x, val<float> y, val<float> z) {
	return invoke<>(
	    +[](float x, float y, float z) { return std::fmaf(x, y, z); }, x, y, z);
}

val<double> fmod(val<double> x, val<double> y) {
	return invoke<>(
	    +[](double x, double y) { return std::fmod(x, y); }, x, y);
}

val<double> nan(val<const char*> tagp) {
	return invoke<>(
	    +[](const char* tagp) { return std::nan(tagp); }, tagp);
}
val<float> nanf(val<const char*> tagp) {
	return invoke<>(
	    +[](const char* tagp) { return std::nanf(tagp); }, tagp);
}

val<float> fmax(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::fmax(x, y); }, x, y);
}
val<float> fmaxf(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::fmaxf(x, y); }, x, y);
}

val<double> fmin(val<double> x, val<double> y) {
	return invoke<>(
	    +[](double x, double y) { return std::fmin(x, y); }, x, y);
}

val<float> fmin(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::fmin(x, y); }, x, y);
}
val<float> fminf(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::fminf(x, y); }, x, y);
}

val<double> fdim(val<double> x, val<double> y) {
	return invoke<>(
	    +[](double x, double y) { return std::fdim(x, y); }, x, y);
}

val<float> fdim(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::fdim(x, y); }, x, y);
}
val<float> fdimf(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::fdimf(x, y); }, x, y);
}

val<float> exp(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::exp(x); }, x);
}
val<float> expf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::expf(x); }, x);
}
val<double> exp2(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::exp2(x); }, x);
}
val<float> exp2(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::exp2(x); }, x);
}
val<float> exp2f(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::exp2f(x); }, x);
}

// Classification functions
val<int> fpclassify(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::fpclassify(x); }, x);
}

val<bool> isfinite(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::isfinite(x); }, x);
}

val<bool> isinf(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::isinf(x); }, x);
}

val<bool> isnan(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::isnan(x); }, x);
}

val<bool> isnormal(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::isnormal(x); }, x);
}

val<bool> signbit(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::signbit(x); }, x);
}

// Comparison functions
val<bool> isgreater(val<double> x, val<double> y) {
	return invoke<>(
	    +[](double x, double y) { return std::isgreater(x, y); }, x, y);
}

val<bool> isgreaterequal(val<double> x, val<double> y) {
	return invoke<>(
	    +[](double x, double y) { return std::isgreaterequal(x, y); }, x, y);
}

val<bool> isless(val<double> x, val<double> y) {
	return invoke<>(
	    +[](double x, double y) { return std::isless(x, y); }, x, y);
}

val<bool> islessequal(val<double> x, val<double> y) {
	return invoke<>(
	    +[](double x, double y) { return std::islessequal(x, y); }, x, y);
}

val<bool> islessgreater(val<double> x, val<double> y) {
	return invoke<>(
	    +[](double x, double y) { return std::islessgreater(x, y); }, x, y);
}

val<bool> isunordered(val<double> x, val<double> y) {
	return invoke<>(
	    +[](double x, double y) { return std::isunordered(x, y); }, x, y);
}

} // namespace nautilus
