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
val<int8_t> abs(val<int8_t> x) {
	return invoke<>(
	    +[](int8_t x) -> int8_t { return std::abs(x); }, x);
}
val<int16_t> abs(val<int16_t> x) {
	return invoke<>(
	    +[](int16_t x) -> int16_t { return std::abs(x); }, x);
}
val<int32_t> abs(val<int32_t> x) {
	return invoke<>(
	    +[](int32_t x) -> int32_t { return std::abs(x); }, x);
}
val<int64_t> abs(val<int64_t> x) {
	return invoke<>(
	    +[](int64_t x) -> int64_t { return std::abs(x); }, x);
}
#if defined(_LIBCPP_VERSION)
val<float> fabsf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::fabsf(x); }, x);
}
#endif

val<float> sin(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::sin(x); }, x);
}

val<double> sin(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::sin(x); }, x);
}
#if defined(_LIBCPP_VERSION)
val<float> sinf(val<float> x) {
	return sin(x);
}
#endif
val<float> tan(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::tan(x); }, x);
}
val<double> tan(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::tan(x); }, x);
}
#if defined(_LIBCPP_VERSION)
val<float> tanf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::tanf(x); }, x);
}
#endif
val<float> asin(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::asin(x); }, x);
}
val<double> asin(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::asin(x); }, x);
}
#if defined(_LIBCPP_VERSION)
val<float> asinf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::asinf(x); }, x);
}
#endif
val<float> cos(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::cos(x); }, x);
}
val<double> cos(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::cos(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> cosf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::cosf(x); }, x);
}
#endif
val<float> acos(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::acos(x); }, x);
}
val<double> acos(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::acos(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> acosf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::asin(x); }, x);
}
#endif
val<float> atan(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::atan(x); }, x);
}
val<double> atan(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::atan(x); }, x);
}
#if defined(_LIBCPP_VERSION)
val<float> atanf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::atanf(x); }, x);
}
#endif
val<float> atan2(val<float> y, val<float> x) {
	return invoke<>(
	    +[](float y, float x) { return std::atan2(y, x); }, y, x);
}
val<double> atan2(val<double> y, val<double> x) {
	return invoke<>(
	    +[](double y, double x) { return std::atan2(y, x); }, y, x);
}

#if defined(_LIBCPP_VERSION)
val<float> atan2f(val<float> y, val<float> x) {
	return invoke<>(
	    +[](float y, float x) { return std::atan2(y, x); }, y, x);
}
#endif
val<float> sinh(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::sinh(x); }, x);
}
val<double> sinh(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::sinh(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> sinhf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::sinh(x); }, x);
}
#endif
val<float> cosh(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::cosh(x); }, x);
}
val<double> cosh(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::cosh(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> coshf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::coshf(x); }, x);
}
#endif

val<float> tanh(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::tanh(x); }, x);
}
val<double> tanh(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::tanh(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> tanhf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::tanhf(x); }, x);
}
#endif
val<float> asinh(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::asinh(x); }, x);
}
val<double> asinh(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::asinh(x); }, x);
}
#if defined(_LIBCPP_VERSION)
val<float> asinhf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::asinhf(x); }, x);
}
#endif
val<float> acosh(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::acosh(x); }, x);
}
val<double> acosh(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::acosh(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> acoshf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::acoshf(x); }, x);
}
#endif

val<float> atanh(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::atanh(x); }, x);
}
val<double> atanh(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::atanh(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> atanhf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::atanhf(x); }, x);
}
#endif

val<float> erf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::erf(x); }, x);
}
val<double> erf(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::erf(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> erff(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::erff(x); }, x);
}
#endif
val<float> erfc(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::erfc(x); }, x);
}
val<double> erfc(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::erfc(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> erfcf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::erfcf(x); }, x);
}
#endif
val<float> tgamma(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::tgamma(x); }, x);
}
val<double> tgamma(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::tgamma(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> tgammaf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::tgammaf(x); }, x);
}
#endif
val<float> lgamma(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::lgamma(x); }, x);
}
val<double> lgamma(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::lgamma(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> lgammaf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::lgammaf(x); }, x);
}
#endif
val<float> ceil(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::ceil(x); }, x);
}
val<double> ceil(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::ceil(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> ceilf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::ceilf(x); }, x);
}
#endif
val<float> floor(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::floor(x); }, x);
}
val<double> floor(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::floor(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> floorf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::floorf(x); }, x);
}
#endif
val<float> trunc(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::trunc(x); }, x);
}
val<double> trunc(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::trunc(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> truncf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::truncf(x); }, x);
}
#endif
val<float> round(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::round(x); }, x);
}
val<double> round(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::round(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> roundf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::roundf(x); }, x);
}
#endif
val<long> lround(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::lround(x); }, x);
}
val<long> lround(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::lround(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<long> lroundf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::lroundf(x); }, x);
}
#endif
val<long long> llround(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::llround(x); }, x);
}
val<long long> llround(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::llround(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<long long> llroundf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::llroundf(x); }, x);
}
#endif
val<float> nearbyint(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::nearbyint(x); }, x);
}
val<double> nearbyint(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::nearbyint(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> nearbyintf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::nearbyintf(x); }, x);
}
#endif
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

#if defined(_LIBCPP_VERSION)
val<long> lrintf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::lrintf(x); }, x);
}
#endif
val<long long> llrint(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::llrint(x); }, x);
}
val<long long> llrint(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::llrint(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<long long> llrintf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::llrint(x); }, x);
}
#endif
val<float> frexp(val<float> x, val<int*> exp) {
	return invoke<>(
	    +[](float x, int* exp) { return std::frexp(x, exp); }, x, exp);
}

#if defined(_LIBCPP_VERSION)
val<float> frexpf(val<float> x, val<int*> exp) {
	return invoke<>(
	    +[](float x, int* exp) { return std::frexp(x, exp); }, x, exp);
}
#endif
val<double> frexp(val<double> x, val<int*> exp) {
	return invoke<>(
	    +[](double x, int* exp) { return std::frexp(x, exp); }, x, exp);
}

val<float> ldexp(val<float> x, val<int> exp) {
	return invoke<>(
	    +[](float x, int exp) { return std::ldexp(x, exp); }, x, exp);
}

#if defined(_LIBCPP_VERSION)
val<float> ldexpf(val<float> x, val<int> exp) {
	return invoke<>(
	    +[](float x, int exp) { return std::ldexpf(x, exp); }, x, exp);
}
#endif
val<double> ldexp(val<double> x, val<int> exp) {
	return invoke<>(
	    +[](double x, int exp) { return std::ldexp(x, exp); }, x, exp);
}

val<float> modf(val<float> x, val<float*> intpart) {
	return invoke<>(
	    +[](float x, float* intpart) { return std::modf(x, intpart); }, x, intpart);
}

#if defined(_LIBCPP_VERSION)
val<float> modff(val<float> x, val<float*> intpart) {
	return invoke<>(
	    +[](float x, float* intpart) { return std::modff(x, intpart); }, x, intpart);
}
#endif
val<double> modf(val<double> x, val<double*> intpart) {
	return invoke<>(
	    +[](double x, double* intpart) { return std::modf(x, intpart); }, x, intpart);
}

val<float> scalbn(val<float> x, val<int> exp) {
	return invoke<>(
	    +[](float x, int exp) { return std::scalbn(x, exp); }, x, exp);
}
#if defined(_LIBCPP_VERSION)
val<float> scalbnf(val<float> x, val<int> exp) {
	return invoke<>(
	    +[](float x, int exp) { return std::scalbnf(x, exp); }, x, exp);
}
#endif
val<double> scalbn(val<double> x, val<int> exp) {
	return invoke<>(
	    +[](double x, int exp) { return std::scalbn(x, exp); }, x, exp);
}

val<double> scalbln(val<double> x, val<long> exp) {
	return invoke<>(
	    +[](double x, long exp) { return std::scalbln(x, exp); }, x, exp);
}

#if defined(_LIBCPP_VERSION)
val<float> scalblnf(val<float> x, val<long> exp) {
	return invoke<>(
	    +[](float x, long exp) { return std::scalbln(x, exp); }, x, exp);
}
#endif
val<float> scalbln(val<float> x, val<long> exp) {
	return invoke<>(
	    +[](float x, long exp) { return std::scalbln(x, exp); }, x, exp);
}

val<int> ilogb(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::ilogb(x); }, x);
}
#if defined(_LIBCPP_VERSION)
val<int> ilogbf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::ilogbf(x); }, x);
}
#endif
val<int> ilogb(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::ilogb(x); }, x);
}

val<double> logb(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::logb(x); }, x);
}
#if defined(_LIBCPP_VERSION)
val<float> logbf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::logbf(x); }, x);
}
#endif
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

#if defined(_LIBCPP_VERSION)
val<float> nextafterf(val<float> from, val<float> to) {
	return invoke<>(
	    +[](float from, float to) { return std::nextafter(from, to); }, from, to);
}
#endif
val<float> nexttoward(val<float> from, val<float> to) {
	return invoke<>(
	    +[](float from, float to) { return std::nextafter(from, to); }, from, to);
}
val<double> nexttoward(val<double> from, val<double> to) {
	return invoke<>(
	    +[](double from, double to) { return std::nextafter(from, to); }, from, to);
}

#if defined(_LIBCPP_VERSION)
val<float> nexttowardf(val<float> from, val<float> to) {
	return invoke<>(
	    +[](float from, float to) { return std::nextafter(from, to); }, from, to);
}
#endif

val<float> copysign(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::copysign(x, y); }, x, y);
}
val<double> copysign(val<double> x, val<double> y) {
	return invoke<>(
	    +[](double x, double y) { return std::copysign(x, y); }, x, y);
}

#if defined(_LIBCPP_VERSION)
val<float> copysignf(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::copysign(x, y); }, x, y);
}
#endif

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

#if defined(_LIBCPP_VERSION)
val<float> logf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::logf(x); }, x);
}
#endif

val<float> log10(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::log10(x); }, x);
}
val<double> log10(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::log10(x); }, x);
}

val<float> log2(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::log2(x); }, x);
}
val<double> log2(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::log2(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> log10f(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::log10f(x); }, x);
}
#endif

val<float> expm1(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::expm1(x); }, x);
}
val<double> expm1(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::expm1(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> expm1f(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::expm1f(x); }, x);
}
#endif

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

#if defined(_LIBCPP_VERSION)
val<float> powf(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::powf(x, y); }, x, y);
}
#endif

val<float> sqrt(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::sqrt(x); }, x);
}
val<double> sqrt(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::sqrt(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> sqrtf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::sqrt(x); }, x);
}
#endif

val<float> cbrt(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::cbrt(x); }, x);
}
val<double> cbrt(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::cbrt(x); }, x);
}

#if defined(_LIBCPP_VERSION)
val<float> cbrtf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::cbrtf(x); }, x);
}
#endif

val<float> hypot(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::hypot(x, y); }, x, y);
}
val<double> hypot(val<double> x, val<double> y) {
	return invoke<>(
	    +[](double x, double y) { return std::hypot(x, y); }, x, y);
}
#if defined(_LIBCPP_VERSION)
val<float> hypotf(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::hypotf(x, y); }, x, y);
}
#endif

val<float> fmod(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::fmod(x, y); }, x, y);
}

#if defined(_LIBCPP_VERSION)
val<float> fmodf(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::fmodf(x, y); }, x, y);
}
#endif

val<double> remainder(val<double> x, val<double> y) {
	return invoke<>(
	    +[](double x, double y) { return std::remainder(x, y); }, x, y);
}

val<float> remainder(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::remainder(x, y); }, x, y);
}
#if defined(_LIBCPP_VERSION)
val<float> remainderf(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::remainderf(x, y); }, x, y);
}
#endif

val<double> remquo(val<double> x, val<double> y, val<int*> quo) {
	return invoke<>(
	    +[](double x, double y, int* quo) { return std::remquo(x, y, quo); }, x, y, quo);
}

val<float> remquo(val<float> x, val<float> y, val<int*> quo) {
	return invoke<>(
	    +[](float x, float y, int* quo) { return std::remquo(x, y, quo); }, x, y, quo);
}

#if defined(_LIBCPP_VERSION)
val<float> remquof(val<float> x, val<float> y, val<int*> quo) {
	return invoke<>(
	    +[](float x, float y, int* quo) { return std::remquof(x, y, quo); }, x, y, quo);
}
#endif

val<double> fma(val<double> x, val<double> y, val<double> z) {
	return invoke<>(
	    +[](double x, double y, double z) { return std::fma(x, y, z); }, x, y, z);
}

val<float> fma(val<float> x, val<float> y, val<float> z) {
	return invoke<>(
	    +[](float x, float y, float z) { return std::fma(x, y, z); }, x, y, z);
}
#if defined(_LIBCPP_VERSION)
val<float> fmaf(val<float> x, val<float> y, val<float> z) {
	return invoke<>(
	    +[](float x, float y, float z) { return std::fmaf(x, y, z); }, x, y, z);
}
#endif

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
#if defined(_LIBCPP_VERSION)
val<float> fmaxf(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::fmaxf(x, y); }, x, y);
}
#endif

val<double> fmin(val<double> x, val<double> y) {
	return invoke<>(
	    +[](double x, double y) { return std::fmin(x, y); }, x, y);
}

val<float> fmin(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::fmin(x, y); }, x, y);
}
#if defined(_LIBCPP_VERSION)
val<float> fminf(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::fminf(x, y); }, x, y);
}
#endif

val<double> fdim(val<double> x, val<double> y) {
	return invoke<>(
	    +[](double x, double y) { return std::fdim(x, y); }, x, y);
}

val<float> fdim(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::fdim(x, y); }, x, y);
}
#if defined(_LIBCPP_VERSION)
val<float> fdimf(val<float> x, val<float> y) {
	return invoke<>(
	    +[](float x, float y) { return std::fdimf(x, y); }, x, y);
}
#endif

val<float> exp(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::exp(x); }, x);
}
#if defined(_LIBCPP_VERSION)
val<float> expf(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::expf(x); }, x);
}
#endif
val<double> exp2(val<double> x) {
	return invoke<>(
	    +[](double x) { return std::exp2(x); }, x);
}
val<float> exp2(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::exp2(x); }, x);
}
#if defined(_LIBCPP_VERSION)
val<float> exp2f(val<float> x) {
	return invoke<>(
	    +[](float x) { return std::exp2f(x); }, x);
}
#endif
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
