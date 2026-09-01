#include <cmath>
#include <cstdint>
#include <nautilus/function.hpp>
#include <nautilus/std/cmath.h>
#include <numbers>
namespace nautilus {

val<float> abs(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::fabs), x);
}
val<double> abs(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::fabs), x);
}
val<float> fabs(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::fabs), x);
}
val<double> fabs(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::fabs), x);
}
val<int32_t> abs(val<int32_t> x) noexcept {
	return invoke<int32_t, int32_t>(static_cast<int32_t (*)(int32_t)>(std::abs), x);
}
val<int64_t> abs(val<int64_t> x) noexcept {
	return invoke<int64_t, int64_t>(static_cast<int64_t (*)(int64_t)>(std::abs), x);
}
#if defined(_LIBCPP_VERSION)
val<float> fabsf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::fabsf), x);
}
#endif

val<float> sin(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::sin), x);
}

val<double> sin(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::sin), x);
}
#if defined(_LIBCPP_VERSION)
val<float> sinf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::sinf), x);
}
#endif
val<float> tan(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::tan), x);
}
val<double> tan(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::tan), x);
}
#if defined(_LIBCPP_VERSION)
val<float> tanf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::tanf), x);
}
#endif
val<float> asin(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::asin), x);
}
val<double> asin(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::asin), x);
}
#if defined(_LIBCPP_VERSION)
val<float> asinf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::asinf), x);
}
#endif
val<float> cos(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::cos), x);
}
val<double> cos(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::cos), x);
}

#if defined(_LIBCPP_VERSION)
val<float> cosf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::cosf), x);
}
#endif
val<float> acos(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::acos), x);
}
val<double> acos(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::acos), x);
}

#if defined(_LIBCPP_VERSION)
val<float> acosf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::acosf), x);
}
#endif
val<float> atan(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::atan), x);
}
val<double> atan(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::atan), x);
}
#if defined(_LIBCPP_VERSION)
val<float> atanf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::atanf), x);
}
#endif
val<float> atan2(val<float> y, val<float> x) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::atan2), y, x);
}
val<double> atan2(val<double> y, val<double> x) noexcept {
	return invoke<double, double, double>(static_cast<double (*)(double, double)>(std::atan2), y, x);
}

#if defined(_LIBCPP_VERSION)
val<float> atan2f(val<float> y, val<float> x) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::atan2f), y, x);
}
#endif
val<float> sinh(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::sinh), x);
}
val<double> sinh(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::sinh), x);
}

#if defined(_LIBCPP_VERSION)
val<float> sinhf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::sinhf), x);
}
#endif
val<float> cosh(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::cosh), x);
}
val<double> cosh(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::cosh), x);
}

#if defined(_LIBCPP_VERSION)
val<float> coshf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::coshf), x);
}
#endif

val<float> tanh(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::tanh), x);
}
val<double> tanh(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::tanh), x);
}

#if defined(_LIBCPP_VERSION)
val<float> tanhf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::tanhf), x);
}
#endif
val<float> asinh(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::asinh), x);
}
val<double> asinh(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::asinh), x);
}
#if defined(_LIBCPP_VERSION)
val<float> asinhf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::asinhf), x);
}
#endif
val<float> acosh(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::acosh), x);
}
val<double> acosh(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::acosh), x);
}

#if defined(_LIBCPP_VERSION)
val<float> acoshf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::acoshf), x);
}
#endif

val<float> atanh(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::atanh), x);
}
val<double> atanh(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::atanh), x);
}

#if defined(_LIBCPP_VERSION)
val<float> atanhf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::atanhf), x);
}
#endif

val<float> erf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::erf), x);
}
val<double> erf(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::erf), x);
}

#if defined(_LIBCPP_VERSION)
val<float> erff(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::erff), x);
}
#endif
val<float> erfc(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::erfc), x);
}
val<double> erfc(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::erfc), x);
}

#if defined(_LIBCPP_VERSION)
val<float> erfcf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::erfcf), x);
}
#endif
val<float> tgamma(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::tgamma), x);
}
val<double> tgamma(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::tgamma), x);
}

#if defined(_LIBCPP_VERSION)
val<float> tgammaf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::tgammaf), x);
}
#endif
val<float> lgamma(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::lgamma), x);
}
val<double> lgamma(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::lgamma), x);
}

#if defined(_LIBCPP_VERSION)
val<float> lgammaf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::lgammaf), x);
}
#endif
val<float> ceil(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::ceil), x);
}
val<double> ceil(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::ceil), x);
}

#if defined(_LIBCPP_VERSION)
val<float> ceilf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::ceilf), x);
}
#endif
val<float> floor(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::floor), x);
}
val<double> floor(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::floor), x);
}

#if defined(_LIBCPP_VERSION)
val<float> floorf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::floorf), x);
}
#endif
val<float> trunc(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::trunc), x);
}
val<double> trunc(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::trunc), x);
}

#if defined(_LIBCPP_VERSION)
val<float> truncf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::truncf), x);
}
#endif
val<float> round(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::round), x);
}
val<double> round(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::round), x);
}

#if defined(_LIBCPP_VERSION)
val<float> roundf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::roundf), x);
}
#endif
val<long> lround(val<float> x) noexcept {
	return invoke<long, float>(static_cast<long (*)(float)>(std::lround), x);
}
val<long> lround(val<double> x) noexcept {
	return invoke<long, double>(static_cast<long (*)(double)>(std::lround), x);
}

#if defined(_LIBCPP_VERSION)
val<long> lroundf(val<float> x) noexcept {
	return invoke<long, float>(static_cast<long (*)(float)>(std::lroundf), x);
}
#endif
val<long long> llround(val<float> x) noexcept {
	return invoke<long long, float>(static_cast<long long (*)(float)>(std::llround), x);
}
val<long long> llround(val<double> x) noexcept {
	return invoke<long long, double>(static_cast<long long (*)(double)>(std::llround), x);
}

#if defined(_LIBCPP_VERSION)
val<long long> llroundf(val<float> x) noexcept {
	return invoke<long long, float>(static_cast<long long (*)(float)>(std::llroundf), x);
}
#endif
val<float> nearbyint(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::nearbyint), x);
}
val<double> nearbyint(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::nearbyint), x);
}

#if defined(_LIBCPP_VERSION)
val<float> nearbyintf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::nearbyintf), x);
}
#endif
val<float> rint(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::rint), x);
}
val<float> rintf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::rintf), x);
}
val<double> rint(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::rint), x);
}

val<long> lrint(val<float> x) noexcept {
	return invoke<long, float>(static_cast<long (*)(float)>(std::lrint), x);
}
val<long> lrint(val<double> x) noexcept {
	return invoke<long, double>(static_cast<long (*)(double)>(std::lrint), x);
}

#if defined(_LIBCPP_VERSION)
val<long> lrintf(val<float> x) noexcept {
	return invoke<long, float>(static_cast<long (*)(float)>(std::lrintf), x);
}
#endif
val<long long> llrint(val<float> x) noexcept {
	return invoke<long long, float>(static_cast<long long (*)(float)>(std::llrint), x);
}
val<long long> llrint(val<double> x) noexcept {
	return invoke<long long, double>(static_cast<long long (*)(double)>(std::llrint), x);
}

#if defined(_LIBCPP_VERSION)
val<long long> llrintf(val<float> x) noexcept {
	return invoke<long long, float>(static_cast<long long (*)(float)>(std::llrintf), x);
}
#endif
val<float> frexp(val<float> x, val<int*> exp) noexcept {
	return invoke<float, float, int*>(static_cast<float (*)(float, int*)>(std::frexp), x, exp);
}

#if defined(_LIBCPP_VERSION)
val<float> frexpf(val<float> x, val<int*> exp) noexcept {
	return invoke<float, float, int*>(static_cast<float (*)(float, int*)>(std::frexpf), x, exp);
}
#endif
val<double> frexp(val<double> x, val<int*> exp) noexcept {
	return invoke<double, double, int*>(static_cast<double (*)(double, int*)>(std::frexp), x, exp);
}

val<float> ldexp(val<float> x, val<int> exp) noexcept {
	return invoke<float, float, int>(static_cast<float (*)(float, int)>(std::ldexp), x, exp);
}

#if defined(_LIBCPP_VERSION)
val<float> ldexpf(val<float> x, val<int> exp) noexcept {
	return invoke<float, float, int>(static_cast<float (*)(float, int)>(std::ldexpf), x, exp);
}
#endif
val<double> ldexp(val<double> x, val<int> exp) noexcept {
	return invoke<double, double, int>(static_cast<double (*)(double, int)>(std::ldexp), x, exp);
}

val<float> modf(val<float> x, val<float*> intpart) noexcept {
	return invoke<float, float, float*>(static_cast<float (*)(float, float*)>(std::modf), x, intpart);
}

#if defined(_LIBCPP_VERSION)
val<float> modff(val<float> x, val<float*> intpart) noexcept {
	return invoke<float, float, float*>(static_cast<float (*)(float, float*)>(std::modff), x, intpart);
}
#endif
val<double> modf(val<double> x, val<double*> intpart) noexcept {
	return invoke<double, double, double*>(static_cast<double (*)(double, double*)>(std::modf), x, intpart);
}

val<float> scalbn(val<float> x, val<int> exp) noexcept {
	return invoke<float, float, int>(static_cast<float (*)(float, int)>(std::scalbn), x, exp);
}
#if defined(_LIBCPP_VERSION)
val<float> scalbnf(val<float> x, val<int> exp) noexcept {
	return invoke<float, float, int>(static_cast<float (*)(float, int)>(std::scalbnf), x, exp);
}
#endif
val<double> scalbn(val<double> x, val<int> exp) noexcept {
	return invoke<double, double, int>(static_cast<double (*)(double, int)>(std::scalbn), x, exp);
}

val<double> scalbln(val<double> x, val<long> exp) noexcept {
	return invoke<double, double, long>(static_cast<double (*)(double, long)>(std::scalbln), x, exp);
}

#if defined(_LIBCPP_VERSION)
val<float> scalblnf(val<float> x, val<long> exp) noexcept {
	return invoke<float, float, long>(static_cast<float (*)(float, long)>(std::scalblnf), x, exp);
}
#endif
val<float> scalbln(val<float> x, val<long> exp) noexcept {
	return invoke<float, float, long>(static_cast<float (*)(float, long)>(std::scalbln), x, exp);
}

val<int> ilogb(val<double> x) noexcept {
	return invoke<int, double>(static_cast<int (*)(double)>(std::ilogb), x);
}
#if defined(_LIBCPP_VERSION)
val<int> ilogbf(val<float> x) noexcept {
	return invoke<int, float>(static_cast<int (*)(float)>(std::ilogbf), x);
}
#endif
val<int> ilogb(val<float> x) noexcept {
	return invoke<int, float>(static_cast<int (*)(float)>(std::ilogb), x);
}

val<double> logb(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::logb), x);
}
#if defined(_LIBCPP_VERSION)
val<float> logbf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::logbf), x);
}
#endif
val<float> logb(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::logb), x);
}

val<float> nextafter(val<float> from, val<float> to) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::nextafter), from, to);
}
val<double> nextafter(val<double> from, val<double> to) noexcept {
	return invoke<double, double, double>(static_cast<double (*)(double, double)>(std::nextafter), from, to);
}

#if defined(_LIBCPP_VERSION)
val<float> nextafterf(val<float> from, val<float> to) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::nextafterf), from, to);
}
#endif
val<float> nexttoward(val<float> from, val<long double> to) noexcept {
	return invoke<float, float, long double>(static_cast<float (*)(float, long double)>(std::nexttoward), from, to);
}
val<double> nexttoward(val<double> from, val<long double> to) noexcept {
	return invoke<double, double, long double>(static_cast<double (*)(double, long double)>(std::nexttoward), from, to);
}

#if defined(_LIBCPP_VERSION)
val<float> nexttowardf(val<float> from, val<long double> to) noexcept {
	return invoke<float, float, long double>(static_cast<float (*)(float, long double)>(std::nexttowardf), from, to);
}
#endif

val<float> copysign(val<float> x, val<float> y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::copysign), x, y);
}
val<double> copysign(val<double> x, val<double> y) noexcept {
	return invoke<double, double, double>(static_cast<double (*)(double, double)>(std::copysign), x, y);
}

#if defined(_LIBCPP_VERSION)
val<float> copysignf(val<float> x, val<float> y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::copysignf), x, y);
}
#endif

// Exponential and logarithmic functions
val<float> log(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::log), x);
}
val<double> log(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::log), x);
}

#if defined(_LIBCPP_VERSION)
val<float> logf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::logf), x);
}
#endif

val<float> log10(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::log10), x);
}
val<double> log10(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::log10), x);
}

val<float> log2(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::log2), x);
}
val<double> log2(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::log2), x);
}

#if defined(_LIBCPP_VERSION)
val<float> log10f(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::log10f), x);
}
#endif

val<float> expm1(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::expm1), x);
}
val<double> expm1(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::expm1), x);
}

#if defined(_LIBCPP_VERSION)
val<float> expm1f(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::expm1f), x);
}
#endif

val<float> log1p(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::log1p), x);
}
val<double> log1p(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::log1p), x);
}
val<float> log1pf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::log1pf), x);
}

// Power functions
val<float> pow(val<float> x, val<float> y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::pow), x, y);
}
val<double> pow(val<double> x, val<double> y) noexcept {
	return invoke<double, double, double>(static_cast<double (*)(double, double)>(std::pow), x, y);
}

#if defined(_LIBCPP_VERSION)
val<float> powf(val<float> x, val<float> y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::powf), x, y);
}
#endif

val<float> sqrt(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::sqrt), x);
}
val<double> sqrt(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::sqrt), x);
}

#if defined(_LIBCPP_VERSION)
val<float> sqrtf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::sqrtf), x);
}
#endif

val<float> cbrt(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::cbrt), x);
}
val<double> cbrt(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::cbrt), x);
}

#if defined(_LIBCPP_VERSION)
val<float> cbrtf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::cbrtf), x);
}
#endif

val<float> hypot(val<float> x, val<float> y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::hypot), x, y);
}
val<double> hypot(val<double> x, val<double> y) noexcept {
	return invoke<double, double, double>(static_cast<double (*)(double, double)>(std::hypot), x, y);
}
#if defined(_LIBCPP_VERSION)
val<float> hypotf(val<float> x, val<float> y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::hypotf), x, y);
}
#endif

val<float> fmod(val<float> x, val<float> y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::fmod), x, y);
}

#if defined(_LIBCPP_VERSION)
val<float> fmodf(val<float> x, val<float> y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::fmodf), x, y);
}
#endif

val<double> remainder(val<double> x, val<double> y) noexcept {
	return invoke<double, double, double>(static_cast<double (*)(double, double)>(std::remainder), x, y);
}

val<float> remainder(val<float> x, val<float> y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::remainder), x, y);
}
#if defined(_LIBCPP_VERSION)
val<float> remainderf(val<float> x, val<float> y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::remainderf), x, y);
}
#endif

val<double> remquo(val<double> x, val<double> y, val<int*> quo) noexcept {
	return invoke<double, double, double, int*>(static_cast<double (*)(double, double, int*)>(std::remquo), x, y, quo);
}

val<float> remquo(val<float> x, val<float> y, val<int*> quo) noexcept {
	return invoke<float, float, float, int*>(static_cast<float (*)(float, float, int*)>(std::remquof), x, y, quo);
}

#if defined(_LIBCPP_VERSION)
val<float> remquof(val<float> x, val<float> y, val<int*> quo) noexcept {
	return invoke<float, float, float, int*>(static_cast<float (*)(float, float, int*)>(std::remquof), x, y, quo);
}
#endif

val<double> fma(val<double> x, val<double> y, val<double> z) noexcept {
	return invoke<double, double, double, double>(static_cast<double (*)(double, double, double)>(std::fma), x, y, z);
}

val<float> fma(val<float> x, val<float> y, val<float> z) noexcept {
	return invoke<float, float, float, float>(static_cast<float (*)(float, float, float)>(std::fmaf), x, y, z);
}
#if defined(_LIBCPP_VERSION)
val<float> fmaf(val<float> x, val<float> y, val<float> z) noexcept {
	return invoke<float, float, float, float>(static_cast<float (*)(float, float, float)>(std::fmaf), x, y, z);
}
#endif

val<double> fmod(val<double> x, val<double> y) noexcept {
	return invoke<double, double, double>(static_cast<double (*)(double, double)>(std::fmod), x, y);
}

val<double> nan(val<const char*> tagp) noexcept {
	return invoke<double, const char*>(static_cast<double (*)(const char*)>(std::nan), tagp);
}
val<float> nanf(val<const char*> tagp) noexcept {
	return invoke<float, const char*>(static_cast<float (*)(const char*)>(std::nanf), tagp);
}

val<float> fmax(val<float> x, val<float> y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::fmax), x, y);
}
#if defined(_LIBCPP_VERSION)
val<float> fmaxf(val<float> x, val<float> y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::fmaxf), x, y);
}
#endif

val<double> fmin(val<double> x, val<double> y) noexcept {
	return invoke<double, double, double>(static_cast<double (*)(double, double)>(std::fmin), x, y);
}

val<float> fmin(val<float> x, val<float> y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::fmin), x, y);
}
#if defined(_LIBCPP_VERSION)
val<float> fminf(val<float> x, val<float> y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::fminf), x, y);
}
#endif

val<double> fdim(val<double> x, val<double> y) noexcept {
	return invoke<double, double, double>(static_cast<double (*)(double, double)>(std::fdim), x, y);
}

val<float> fdim(val<float> x, val<float> y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::fdim), x, y);
}
#if defined(_LIBCPP_VERSION)
val<float> fdimf(val<float> x, val<float> y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::fdimf), x, y);
}
#endif

val<double> exp(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::exp), x);
}

val<float> exp(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::exp), x);
}
#if defined(_LIBCPP_VERSION)
val<float> expf(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::expf), x);
}
#endif
val<double> exp2(val<double> x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::exp2), x);
}
val<float> exp2(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::exp2f), x);
}
#if defined(_LIBCPP_VERSION)
val<float> exp2f(val<float> x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::exp2f), x);
}
#endif
// Classification functions
val<int> fpclassify(val<double> x) noexcept {
	return invoke<int, double>(static_cast<int (*)(double)>(std::fpclassify), x);
}

val<bool> isfinite(val<double> x) noexcept {
	return invoke<bool, double>(static_cast<bool (*)(double)>(std::isfinite), x);
}

val<bool> isinf(val<double> x) noexcept {
	return invoke<bool, double>(static_cast<bool (*)(double)>(std::isinf), x);
}

val<bool> isnan(val<double> x) noexcept {
	return invoke<bool, double>(static_cast<bool (*)(double)>(std::isnan), x);
}

val<bool> isnormal(val<double> x) noexcept {
	return invoke<bool, double>(static_cast<bool (*)(double)>(std::isnormal), x);
}

val<bool> signbit(val<double> x) noexcept {
	return invoke<bool, double>(static_cast<bool (*)(double)>(std::signbit), x);
}

// Comparison functions
val<bool> isgreater(val<double> x, val<double> y) noexcept {
	return invoke<bool, double, double>(static_cast<bool (*)(double, double)>(std::isgreater), x, y);
}

val<bool> isgreaterequal(val<double> x, val<double> y) noexcept {
	return invoke<bool, double, double>(static_cast<bool (*)(double, double)>(std::isgreaterequal), x, y);
}

val<bool> isless(val<double> x, val<double> y) noexcept {
	return invoke<bool, double, double>(static_cast<bool (*)(double, double)>(std::isless), x, y);
}

val<bool> islessequal(val<double> x, val<double> y) noexcept {
	return invoke<bool, double, double>(static_cast<bool (*)(double, double)>(std::islessequal), x, y);
}

val<bool> islessgreater(val<double> x, val<double> y) noexcept {
	return invoke<bool, double, double>(static_cast<bool (*)(double, double)>(std::islessgreater), x, y);
}

val<bool> isunordered(val<double> x, val<double> y) noexcept {
	return invoke<bool, double, double>(static_cast<bool (*)(double, double)>(std::isunordered), x, y);
}

} // namespace nautilus
