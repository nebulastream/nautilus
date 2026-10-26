#include "nautilus/extensions/IntrinsicRegistration.hpp"
#include <cstring>
#include <nautilus/builtin/overflow/overflow.hpp>
#include <nautilus/function.hpp>

namespace nautilus::builtin {
namespace detail {
namespace {
__extension__ typedef __int128 native_int;

native_int load(const void* p) {
	native_int v;
	std::memcpy(&v, p, sizeof(v));
	return v;
}
void store(void* p, native_int v) {
	std::memcpy(p, &v, sizeof(v));
}
} // namespace

bool add_overflow_i32_impl(int32_t a, int32_t b, int32_t* out) noexcept {
	return __builtin_add_overflow(a, b, out);
}
bool sub_overflow_i32_impl(int32_t a, int32_t b, int32_t* out) noexcept {
	return __builtin_sub_overflow(a, b, out);
}
bool mul_overflow_i32_impl(int32_t a, int32_t b, int32_t* out) noexcept {
	return __builtin_mul_overflow(a, b, out);
}
bool add_overflow_i64_impl(int64_t a, int64_t b, int64_t* out) noexcept {
	return __builtin_add_overflow(a, b, out);
}
bool sub_overflow_i64_impl(int64_t a, int64_t b, int64_t* out) noexcept {
	return __builtin_sub_overflow(a, b, out);
}
bool mul_overflow_i64_impl(int64_t a, int64_t b, int64_t* out) noexcept {
	return __builtin_mul_overflow(a, b, out);
}
bool add_overflow_i128_impl(const void* a, const void* b, void* out) noexcept {
	native_int r;
	bool o = __builtin_add_overflow(load(a), load(b), &r);
	store(out, r);
	return o;
}
bool sub_overflow_i128_impl(const void* a, const void* b, void* out) noexcept {
	native_int r;
	bool o = __builtin_sub_overflow(load(a), load(b), &r);
	store(out, r);
	return o;
}
bool mul_overflow_i128_impl(const void* a, const void* b, void* out) noexcept {
	native_int r;
	bool o = __builtin_mul_overflow(load(a), load(b), &r);
	store(out, r);
	return o;
}
} // namespace detail

val<bool> add_overflow(val<int32_t> a, val<int32_t> b, val<int32_t*> out) {
	return invoke(detail::add_overflow_i32_impl, a, b, out);
}
val<bool> sub_overflow(val<int32_t> a, val<int32_t> b, val<int32_t*> out) {
	return invoke(detail::sub_overflow_i32_impl, a, b, out);
}
val<bool> mul_overflow(val<int32_t> a, val<int32_t> b, val<int32_t*> out) {
	return invoke(detail::mul_overflow_i32_impl, a, b, out);
}
val<bool> add_overflow(val<int64_t> a, val<int64_t> b, val<int64_t*> out) {
	return invoke(detail::add_overflow_i64_impl, a, b, out);
}
val<bool> sub_overflow(val<int64_t> a, val<int64_t> b, val<int64_t*> out) {
	return invoke(detail::sub_overflow_i64_impl, a, b, out);
}
val<bool> mul_overflow(val<int64_t> a, val<int64_t> b, val<int64_t*> out) {
	return invoke(detail::mul_overflow_i64_impl, a, b, out);
}
val<bool> add_overflow_i128(val<const void*> a, val<const void*> b, val<void*> out) {
	return invoke(detail::add_overflow_i128_impl, a, b, out);
}
val<bool> sub_overflow_i128(val<const void*> a, val<const void*> b, val<void*> out) {
	return invoke(detail::sub_overflow_i128_impl, a, b, out);
}
val<bool> mul_overflow_i128(val<const void*> a, val<const void*> b, val<void*> out) {
	return invoke(detail::mul_overflow_i128_impl, a, b, out);
}
} // namespace nautilus::builtin

// The backend-neutral registration (04 §3.5). The scalar forms only write through `out`; the 128-bit forms also read
// their operands through pointers. None throws, and each returns.
namespace {

struct OverflowIntrinsicRegistrar {
	OverflowIntrinsicRegistrar() {
		using namespace nautilus::ext;
		using namespace nautilus::builtin::detail;
		constexpr uint32_t scalar = AttrNoArgRead | AttrNoOtherRead | AttrNoOtherWrite | AttrNoUnwind | AttrWillReturn;
		constexpr uint32_t wide = AttrNoOtherRead | AttrNoOtherWrite | AttrNoUnwind | AttrWillReturn;
#define NAUTILUS_REGISTER_OVERFLOW(op, width, attrs)                                                                   \
	registerIntrinsicFor("builtin." #op "_overflow_" #width, &op##_overflow_##width##_impl, attrs)
		NAUTILUS_REGISTER_OVERFLOW(add, i32, scalar);
		NAUTILUS_REGISTER_OVERFLOW(sub, i32, scalar);
		NAUTILUS_REGISTER_OVERFLOW(mul, i32, scalar);
		NAUTILUS_REGISTER_OVERFLOW(add, i64, scalar);
		NAUTILUS_REGISTER_OVERFLOW(sub, i64, scalar);
		NAUTILUS_REGISTER_OVERFLOW(mul, i64, scalar);
		NAUTILUS_REGISTER_OVERFLOW(add, i128, wide);
		NAUTILUS_REGISTER_OVERFLOW(sub, i128, wide);
		NAUTILUS_REGISTER_OVERFLOW(mul, i128, wide);
#undef NAUTILUS_REGISTER_OVERFLOW
	}
};
const OverflowIntrinsicRegistrar overflowRegistrar_;

} // namespace
