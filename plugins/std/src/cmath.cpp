#include "nautilus/extensions/IntrinsicRegistration.hpp"
#include <cmath>
#include <cstdint>
#include <nautilus/function.hpp>
#include <nautilus/std/cmath.h>
#include <numbers>
namespace nautilus {

val<float> abs(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::fabs), x);
}
val<double> abs(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::fabs), x);
}
val<float> fabs(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::fabs), x);
}
val<double> fabs(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::fabs), x);
}
val<int32_t> abs(const val<int32_t>& x) noexcept {
	return invoke<int32_t, int32_t>(static_cast<int32_t (*)(int32_t)>(std::abs), x);
}
val<int64_t> abs(const val<int64_t>& x) noexcept {
	return invoke<int64_t, int64_t>(static_cast<int64_t (*)(int64_t)>(std::abs), x);
}
#if defined(_LIBCPP_VERSION)
val<float> fabsf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::fabsf), x);
}
#endif

val<float> sin(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::sin), x);
}

val<double> sin(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::sin), x);
}
#if defined(_LIBCPP_VERSION)
val<float> sinf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::sinf), x);
}
#endif
val<float> tan(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::tan), x);
}
val<double> tan(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::tan), x);
}
#if defined(_LIBCPP_VERSION)
val<float> tanf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::tanf), x);
}
#endif
val<float> asin(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::asin), x);
}
val<double> asin(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::asin), x);
}
#if defined(_LIBCPP_VERSION)
val<float> asinf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::asinf), x);
}
#endif
val<float> cos(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::cos), x);
}
val<double> cos(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::cos), x);
}

#if defined(_LIBCPP_VERSION)
val<float> cosf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::cosf), x);
}
#endif
val<float> acos(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::acos), x);
}
val<double> acos(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::acos), x);
}

#if defined(_LIBCPP_VERSION)
val<float> acosf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::acosf), x);
}
#endif
val<float> atan(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::atan), x);
}
val<double> atan(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::atan), x);
}
#if defined(_LIBCPP_VERSION)
val<float> atanf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::atanf), x);
}
#endif
val<float> atan2(const val<float>& y, const val<float>& x) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::atan2), y, x);
}
val<double> atan2(const val<double>& y, const val<double>& x) noexcept {
	return invoke<double, double, double>(static_cast<double (*)(double, double)>(std::atan2), y, x);
}

#if defined(_LIBCPP_VERSION)
val<float> atan2f(const val<float>& y, const val<float>& x) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::atan2f), y, x);
}
#endif
val<float> sinh(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::sinh), x);
}
val<double> sinh(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::sinh), x);
}

#if defined(_LIBCPP_VERSION)
val<float> sinhf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::sinhf), x);
}
#endif
val<float> cosh(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::cosh), x);
}
val<double> cosh(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::cosh), x);
}

#if defined(_LIBCPP_VERSION)
val<float> coshf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::coshf), x);
}
#endif

val<float> tanh(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::tanh), x);
}
val<double> tanh(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::tanh), x);
}

#if defined(_LIBCPP_VERSION)
val<float> tanhf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::tanhf), x);
}
#endif
val<float> asinh(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::asinh), x);
}
val<double> asinh(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::asinh), x);
}
#if defined(_LIBCPP_VERSION)
val<float> asinhf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::asinhf), x);
}
#endif
val<float> acosh(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::acosh), x);
}
val<double> acosh(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::acosh), x);
}

#if defined(_LIBCPP_VERSION)
val<float> acoshf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::acoshf), x);
}
#endif

val<float> atanh(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::atanh), x);
}
val<double> atanh(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::atanh), x);
}

#if defined(_LIBCPP_VERSION)
val<float> atanhf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::atanhf), x);
}
#endif

val<float> erf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::erf), x);
}
val<double> erf(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::erf), x);
}

#if defined(_LIBCPP_VERSION)
val<float> erff(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::erff), x);
}
#endif
val<float> erfc(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::erfc), x);
}
val<double> erfc(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::erfc), x);
}

#if defined(_LIBCPP_VERSION)
val<float> erfcf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::erfcf), x);
}
#endif
val<float> tgamma(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::tgamma), x);
}
val<double> tgamma(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::tgamma), x);
}

#if defined(_LIBCPP_VERSION)
val<float> tgammaf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::tgammaf), x);
}
#endif
val<float> lgamma(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::lgamma), x);
}
val<double> lgamma(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::lgamma), x);
}

#if defined(_LIBCPP_VERSION)
val<float> lgammaf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::lgammaf), x);
}
#endif
val<float> ceil(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::ceil), x);
}
val<double> ceil(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::ceil), x);
}

#if defined(_LIBCPP_VERSION)
val<float> ceilf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::ceilf), x);
}
#endif
val<float> floor(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::floor), x);
}
val<double> floor(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::floor), x);
}

#if defined(_LIBCPP_VERSION)
val<float> floorf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::floorf), x);
}
#endif
val<float> trunc(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::trunc), x);
}
val<double> trunc(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::trunc), x);
}

#if defined(_LIBCPP_VERSION)
val<float> truncf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::truncf), x);
}
#endif
val<float> round(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::round), x);
}
val<double> round(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::round), x);
}

#if defined(_LIBCPP_VERSION)
val<float> roundf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::roundf), x);
}
#endif
val<long> lround(const val<float>& x) noexcept {
	return invoke<long, float>(static_cast<long (*)(float)>(std::lround), x);
}
val<long> lround(const val<double>& x) noexcept {
	return invoke<long, double>(static_cast<long (*)(double)>(std::lround), x);
}

#if defined(_LIBCPP_VERSION)
val<long> lroundf(const val<float>& x) noexcept {
	return invoke<long, float>(static_cast<long (*)(float)>(std::lroundf), x);
}
#endif
val<long long> llround(const val<float>& x) noexcept {
	return invoke<long long, float>(static_cast<long long (*)(float)>(std::llround), x);
}
val<long long> llround(const val<double>& x) noexcept {
	return invoke<long long, double>(static_cast<long long (*)(double)>(std::llround), x);
}

#if defined(_LIBCPP_VERSION)
val<long long> llroundf(const val<float>& x) noexcept {
	return invoke<long long, float>(static_cast<long long (*)(float)>(std::llroundf), x);
}
#endif
val<float> nearbyint(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::nearbyint), x);
}
val<double> nearbyint(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::nearbyint), x);
}

#if defined(_LIBCPP_VERSION)
val<float> nearbyintf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::nearbyintf), x);
}
#endif
val<float> rint(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::rint), x);
}
val<float> rintf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::rintf), x);
}
val<double> rint(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::rint), x);
}

val<long> lrint(const val<float>& x) noexcept {
	return invoke<long, float>(static_cast<long (*)(float)>(std::lrint), x);
}
val<long> lrint(const val<double>& x) noexcept {
	return invoke<long, double>(static_cast<long (*)(double)>(std::lrint), x);
}

#if defined(_LIBCPP_VERSION)
val<long> lrintf(const val<float>& x) noexcept {
	return invoke<long, float>(static_cast<long (*)(float)>(std::lrintf), x);
}
#endif
val<long long> llrint(const val<float>& x) noexcept {
	return invoke<long long, float>(static_cast<long long (*)(float)>(std::llrint), x);
}
val<long long> llrint(const val<double>& x) noexcept {
	return invoke<long long, double>(static_cast<long long (*)(double)>(std::llrint), x);
}

#if defined(_LIBCPP_VERSION)
val<long long> llrintf(const val<float>& x) noexcept {
	return invoke<long long, float>(static_cast<long long (*)(float)>(std::llrintf), x);
}
#endif
val<float> frexp(const val<float>& x, const val<int*>& exp) noexcept {
	return invoke<float, float, int*>(static_cast<float (*)(float, int*)>(std::frexp), x, exp);
}

#if defined(_LIBCPP_VERSION)
val<float> frexpf(const val<float>& x, const val<int*>& exp) noexcept {
	return invoke<float, float, int*>(static_cast<float (*)(float, int*)>(std::frexpf), x, exp);
}
#endif
val<double> frexp(const val<double>& x, const val<int*>& exp) noexcept {
	return invoke<double, double, int*>(static_cast<double (*)(double, int*)>(std::frexp), x, exp);
}

val<float> ldexp(const val<float>& x, const val<int>& exp) noexcept {
	return invoke<float, float, int>(static_cast<float (*)(float, int)>(std::ldexp), x, exp);
}

#if defined(_LIBCPP_VERSION)
val<float> ldexpf(const val<float>& x, const val<int>& exp) noexcept {
	return invoke<float, float, int>(static_cast<float (*)(float, int)>(std::ldexpf), x, exp);
}
#endif
val<double> ldexp(const val<double>& x, const val<int>& exp) noexcept {
	return invoke<double, double, int>(static_cast<double (*)(double, int)>(std::ldexp), x, exp);
}

val<float> modf(const val<float>& x, const val<float*>& intpart) noexcept {
	return invoke<float, float, float*>(static_cast<float (*)(float, float*)>(std::modf), x, intpart);
}

#if defined(_LIBCPP_VERSION)
val<float> modff(const val<float>& x, const val<float*>& intpart) noexcept {
	return invoke<float, float, float*>(static_cast<float (*)(float, float*)>(std::modff), x, intpart);
}
#endif
val<double> modf(const val<double>& x, const val<double*>& intpart) noexcept {
	return invoke<double, double, double*>(static_cast<double (*)(double, double*)>(std::modf), x, intpart);
}

val<float> scalbn(const val<float>& x, const val<int>& exp) noexcept {
	return invoke<float, float, int>(static_cast<float (*)(float, int)>(std::scalbn), x, exp);
}
#if defined(_LIBCPP_VERSION)
val<float> scalbnf(const val<float>& x, const val<int>& exp) noexcept {
	return invoke<float, float, int>(static_cast<float (*)(float, int)>(std::scalbnf), x, exp);
}
#endif
val<double> scalbn(const val<double>& x, const val<int>& exp) noexcept {
	return invoke<double, double, int>(static_cast<double (*)(double, int)>(std::scalbn), x, exp);
}

val<double> scalbln(const val<double>& x, const val<long>& exp) noexcept {
	return invoke<double, double, long>(static_cast<double (*)(double, long)>(std::scalbln), x, exp);
}

#if defined(_LIBCPP_VERSION)
val<float> scalblnf(const val<float>& x, const val<long>& exp) noexcept {
	return invoke<float, float, long>(static_cast<float (*)(float, long)>(std::scalblnf), x, exp);
}
#endif
val<float> scalbln(const val<float>& x, const val<long>& exp) noexcept {
	return invoke<float, float, long>(static_cast<float (*)(float, long)>(std::scalbln), x, exp);
}

val<int> ilogb(const val<double>& x) noexcept {
	return invoke<int, double>(static_cast<int (*)(double)>(std::ilogb), x);
}
#if defined(_LIBCPP_VERSION)
val<int> ilogbf(const val<float>& x) noexcept {
	return invoke<int, float>(static_cast<int (*)(float)>(std::ilogbf), x);
}
#endif
val<int> ilogb(const val<float>& x) noexcept {
	return invoke<int, float>(static_cast<int (*)(float)>(std::ilogb), x);
}

val<double> logb(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::logb), x);
}
#if defined(_LIBCPP_VERSION)
val<float> logbf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::logbf), x);
}
#endif
val<float> logb(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::logb), x);
}

val<float> nextafter(const val<float>& from, const val<float>& to) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::nextafter), from, to);
}
val<double> nextafter(const val<double>& from, const val<double>& to) noexcept {
	return invoke<double, double, double>(static_cast<double (*)(double, double)>(std::nextafter), from, to);
}

#if defined(_LIBCPP_VERSION)
val<float> nextafterf(const val<float>& from, const val<float>& to) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::nextafterf), from, to);
}
#endif
val<float> nexttoward(const val<float>& from, const val<long double>& to) noexcept {
	return invoke<float, float, long double>(static_cast<float (*)(float, long double)>(std::nexttoward), from, to);
}
val<double> nexttoward(const val<double>& from, const val<long double>& to) noexcept {
	return invoke<double, double, long double>(static_cast<double (*)(double, long double)>(std::nexttoward), from, to);
}

#if defined(_LIBCPP_VERSION)
val<float> nexttowardf(const val<float>& from, const val<long double>& to) noexcept {
	return invoke<float, float, long double>(static_cast<float (*)(float, long double)>(std::nexttowardf), from, to);
}
#endif

val<float> copysign(const val<float>& x, const val<float>& y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::copysign), x, y);
}
val<double> copysign(const val<double>& x, const val<double>& y) noexcept {
	return invoke<double, double, double>(static_cast<double (*)(double, double)>(std::copysign), x, y);
}

#if defined(_LIBCPP_VERSION)
val<float> copysignf(const val<float>& x, const val<float>& y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::copysignf), x, y);
}
#endif

// Exponential and logarithmic functions
val<float> log(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::log), x);
}
val<double> log(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::log), x);
}

#if defined(_LIBCPP_VERSION)
val<float> logf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::logf), x);
}
#endif

val<float> log10(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::log10), x);
}
val<double> log10(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::log10), x);
}

val<float> log2(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::log2), x);
}
val<double> log2(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::log2), x);
}

#if defined(_LIBCPP_VERSION)
val<float> log10f(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::log10f), x);
}
#endif

val<float> expm1(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::expm1), x);
}
val<double> expm1(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::expm1), x);
}

#if defined(_LIBCPP_VERSION)
val<float> expm1f(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::expm1f), x);
}
#endif

val<float> log1p(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::log1p), x);
}
val<double> log1p(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::log1p), x);
}
val<float> log1pf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::log1pf), x);
}

// Power functions
val<float> pow(const val<float>& x, const val<float>& y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::pow), x, y);
}
val<double> pow(const val<double>& x, const val<double>& y) noexcept {
	return invoke<double, double, double>(static_cast<double (*)(double, double)>(std::pow), x, y);
}

#if defined(_LIBCPP_VERSION)
val<float> powf(const val<float>& x, const val<float>& y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::powf), x, y);
}
#endif

val<float> sqrt(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::sqrt), x);
}
val<double> sqrt(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::sqrt), x);
}

#if defined(_LIBCPP_VERSION)
val<float> sqrtf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::sqrtf), x);
}
#endif

val<float> cbrt(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::cbrt), x);
}
val<double> cbrt(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::cbrt), x);
}

#if defined(_LIBCPP_VERSION)
val<float> cbrtf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::cbrtf), x);
}
#endif

val<float> hypot(const val<float>& x, const val<float>& y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::hypot), x, y);
}
val<double> hypot(const val<double>& x, const val<double>& y) noexcept {
	return invoke<double, double, double>(static_cast<double (*)(double, double)>(std::hypot), x, y);
}
#if defined(_LIBCPP_VERSION)
val<float> hypotf(const val<float>& x, const val<float>& y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::hypotf), x, y);
}
#endif

val<float> fmod(const val<float>& x, const val<float>& y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::fmod), x, y);
}

#if defined(_LIBCPP_VERSION)
val<float> fmodf(const val<float>& x, const val<float>& y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::fmodf), x, y);
}
#endif

val<double> remainder(const val<double>& x, const val<double>& y) noexcept {
	return invoke<double, double, double>(static_cast<double (*)(double, double)>(std::remainder), x, y);
}

val<float> remainder(const val<float>& x, const val<float>& y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::remainder), x, y);
}
#if defined(_LIBCPP_VERSION)
val<float> remainderf(const val<float>& x, const val<float>& y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::remainderf), x, y);
}
#endif

val<double> remquo(const val<double>& x, const val<double>& y, const val<int*>& quo) noexcept {
	return invoke<double, double, double, int*>(static_cast<double (*)(double, double, int*)>(std::remquo), x, y, quo);
}

val<float> remquo(const val<float>& x, const val<float>& y, const val<int*>& quo) noexcept {
	return invoke<float, float, float, int*>(static_cast<float (*)(float, float, int*)>(std::remquof), x, y, quo);
}

#if defined(_LIBCPP_VERSION)
val<float> remquof(const val<float>& x, const val<float>& y, const val<int*>& quo) noexcept {
	return invoke<float, float, float, int*>(static_cast<float (*)(float, float, int*)>(std::remquof), x, y, quo);
}
#endif

val<double> fma(const val<double>& x, const val<double>& y, const val<double>& z) noexcept {
	return invoke<double, double, double, double>(static_cast<double (*)(double, double, double)>(std::fma), x, y, z);
}

val<float> fma(const val<float>& x, const val<float>& y, const val<float>& z) noexcept {
	return invoke<float, float, float, float>(static_cast<float (*)(float, float, float)>(std::fmaf), x, y, z);
}
#if defined(_LIBCPP_VERSION)
val<float> fmaf(const val<float>& x, const val<float>& y, const val<float>& z) noexcept {
	return invoke<float, float, float, float>(static_cast<float (*)(float, float, float)>(std::fmaf), x, y, z);
}
#endif

val<double> fmod(const val<double>& x, const val<double>& y) noexcept {
	return invoke<double, double, double>(static_cast<double (*)(double, double)>(std::fmod), x, y);
}

val<double> nan(const val<const char*>& tagp) noexcept {
	return invoke<double, const char*>(static_cast<double (*)(const char*)>(std::nan), tagp);
}
val<float> nanf(const val<const char*>& tagp) noexcept {
	return invoke<float, const char*>(static_cast<float (*)(const char*)>(std::nanf), tagp);
}

val<float> fmax(const val<float>& x, const val<float>& y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::fmax), x, y);
}
#if defined(_LIBCPP_VERSION)
val<float> fmaxf(const val<float>& x, const val<float>& y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::fmaxf), x, y);
}
#endif

val<double> fmin(const val<double>& x, const val<double>& y) noexcept {
	return invoke<double, double, double>(static_cast<double (*)(double, double)>(std::fmin), x, y);
}

val<float> fmin(const val<float>& x, const val<float>& y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::fmin), x, y);
}
#if defined(_LIBCPP_VERSION)
val<float> fminf(const val<float>& x, const val<float>& y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::fminf), x, y);
}
#endif

val<double> fdim(const val<double>& x, const val<double>& y) noexcept {
	return invoke<double, double, double>(static_cast<double (*)(double, double)>(std::fdim), x, y);
}

val<float> fdim(const val<float>& x, const val<float>& y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::fdim), x, y);
}
#if defined(_LIBCPP_VERSION)
val<float> fdimf(const val<float>& x, const val<float>& y) noexcept {
	return invoke<float, float, float>(static_cast<float (*)(float, float)>(std::fdimf), x, y);
}
#endif

val<double> exp(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::exp), x);
}

val<float> exp(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::exp), x);
}
#if defined(_LIBCPP_VERSION)
val<float> expf(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::expf), x);
}
#endif
val<double> exp2(const val<double>& x) noexcept {
	return invoke<double, double>(static_cast<double (*)(double)>(std::exp2), x);
}
val<float> exp2(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::exp2f), x);
}
#if defined(_LIBCPP_VERSION)
val<float> exp2f(const val<float>& x) noexcept {
	return invoke<float, float>(static_cast<float (*)(float)>(std::exp2f), x);
}
#endif
// Classification functions
val<int> fpclassify(const val<double>& x) noexcept {
	return invoke<int, double>(static_cast<int (*)(double)>(std::fpclassify), x);
}

val<bool> isfinite(const val<double>& x) noexcept {
	return invoke<bool, double>(static_cast<bool (*)(double)>(std::isfinite), x);
}

val<bool> isinf(const val<double>& x) noexcept {
	return invoke<bool, double>(static_cast<bool (*)(double)>(std::isinf), x);
}

val<bool> isnan(const val<double>& x) noexcept {
	return invoke<bool, double>(static_cast<bool (*)(double)>(std::isnan), x);
}

val<bool> isnormal(const val<double>& x) noexcept {
	return invoke<bool, double>(static_cast<bool (*)(double)>(std::isnormal), x);
}

val<bool> signbit(const val<double>& x) noexcept {
	return invoke<bool, double>(static_cast<bool (*)(double)>(std::signbit), x);
}

// Comparison functions
val<bool> isgreater(const val<double>& x, const val<double>& y) noexcept {
	return invoke<bool, double, double>(static_cast<bool (*)(double, double)>(std::isgreater), x, y);
}

val<bool> isgreaterequal(const val<double>& x, const val<double>& y) noexcept {
	return invoke<bool, double, double>(static_cast<bool (*)(double, double)>(std::isgreaterequal), x, y);
}

val<bool> isless(const val<double>& x, const val<double>& y) noexcept {
	return invoke<bool, double, double>(static_cast<bool (*)(double, double)>(std::isless), x, y);
}

val<bool> islessequal(const val<double>& x, const val<double>& y) noexcept {
	return invoke<bool, double, double>(static_cast<bool (*)(double, double)>(std::islessequal), x, y);
}

val<bool> islessgreater(const val<double>& x, const val<double>& y) noexcept {
	return invoke<bool, double, double>(static_cast<bool (*)(double, double)>(std::islessgreater), x, y);
}

val<bool> isunordered(const val<double>& x, const val<double>& y) noexcept {
	return invoke<bool, double, double>(static_cast<bool (*)(double, double)>(std::isunordered), x, y);
}

} // namespace nautilus

// The backend-neutral registration (04 §3.5): the floating-point functions are pure functions of their arguments
// (errno is not modelled), so the optimizer may fold and hoist them and a backend may lower them natively.
namespace {

struct MathIntrinsicRegistrar {
	MathIntrinsicRegistrar() {
		using namespace nautilus::ext;
		constexpr uint32_t pure =
		    AttrNoArgRead | AttrNoArgWrite | AttrNoOtherRead | AttrNoOtherWrite | AttrNoUnwind | AttrWillReturn;
#define NAUTILUS_REGISTER_MATH1(fn)                                                                                    \
	registerIntrinsicFor("std." #fn "_f32", static_cast<float (*)(float)>(&std::fn), pure);                            \
	registerIntrinsicFor("std." #fn "_f64", static_cast<double (*)(double)>(&std::fn), pure)
#define NAUTILUS_REGISTER_MATH2(fn)                                                                                    \
	registerIntrinsicFor("std." #fn "_f32", static_cast<float (*)(float, float)>(&std::fn), pure);                     \
	registerIntrinsicFor("std." #fn "_f64", static_cast<double (*)(double, double)>(&std::fn), pure)
#define NAUTILUS_REGISTER_MATH_INT(fn, R)                                                                              \
	registerIntrinsicFor("std." #fn "_f32", static_cast<R (*)(float)>(&std::fn), pure);                                \
	registerIntrinsicFor("std." #fn "_f64", static_cast<R (*)(double)>(&std::fn), pure)
		NAUTILUS_REGISTER_MATH1(fabs);
		NAUTILUS_REGISTER_MATH1(sin);
		NAUTILUS_REGISTER_MATH1(cos);
		NAUTILUS_REGISTER_MATH1(tan);
		NAUTILUS_REGISTER_MATH1(asin);
		NAUTILUS_REGISTER_MATH1(acos);
		NAUTILUS_REGISTER_MATH1(atan);
		NAUTILUS_REGISTER_MATH1(sinh);
		NAUTILUS_REGISTER_MATH1(cosh);
		NAUTILUS_REGISTER_MATH1(tanh);
		NAUTILUS_REGISTER_MATH1(exp);
		NAUTILUS_REGISTER_MATH1(exp2);
		NAUTILUS_REGISTER_MATH1(log);
		NAUTILUS_REGISTER_MATH1(log2);
		NAUTILUS_REGISTER_MATH1(log10);
		NAUTILUS_REGISTER_MATH1(sqrt);
		NAUTILUS_REGISTER_MATH1(ceil);
		NAUTILUS_REGISTER_MATH1(floor);
		NAUTILUS_REGISTER_MATH1(trunc);
		NAUTILUS_REGISTER_MATH1(round);
		NAUTILUS_REGISTER_MATH1(rint);
		NAUTILUS_REGISTER_MATH1(nearbyint);
		NAUTILUS_REGISTER_MATH2(atan2);
		NAUTILUS_REGISTER_MATH2(pow);
		NAUTILUS_REGISTER_MATH2(copysign);
		registerIntrinsicFor("std.fma_f32", static_cast<float (*)(float, float, float)>(&std::fma), pure);
		registerIntrinsicFor("std.fma_f64", static_cast<double (*)(double, double, double)>(&std::fma), pure);
		NAUTILUS_REGISTER_MATH_INT(lround, long);
		NAUTILUS_REGISTER_MATH_INT(llround, long long);
		NAUTILUS_REGISTER_MATH_INT(lrint, long);
		NAUTILUS_REGISTER_MATH_INT(llrint, long long);
		registerIntrinsicFor("std.abs_i32", static_cast<int32_t (*)(int32_t)>(&std::abs), pure);
		registerIntrinsicFor("std.abs_i64", static_cast<int64_t (*)(int64_t)>(&std::abs), pure);
#undef NAUTILUS_REGISTER_MATH_INT
#undef NAUTILUS_REGISTER_MATH2
#undef NAUTILUS_REGISTER_MATH1
	}
};
const MathIntrinsicRegistrar mathRegistrar_;

} // namespace
