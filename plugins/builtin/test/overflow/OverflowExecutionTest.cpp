#include "ExecutionTest.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <cstring>
#include <limits>
#include <nautilus/Engine.hpp>
#include <nautilus/builtin/overflow/overflow.hpp>
#include <nautilus/builtin/plugin.hpp>
#include <nautilus/val_std.hpp>

namespace {
using nautilus::val;
namespace builtin = nautilus::builtin;

// Each kernel returns the wrapped result and writes the overflow flag to `flag`.
template <class T, val<bool> (*OP)(val<T>, val<T>, val<T*>)>
val<T> checked(val<T> a, val<T> b, val<bool*> flag) {
	auto slot = nautilus::details::nautilus_alloca<T>();
	val<bool> o = OP(a, b, slot);
	*flag = o;
	return *slot;
}

template <val<bool> (*OP)(val<const void*>, val<const void*>, val<void*>)>
val<bool> checkedWide(val<const void*> a, val<const void*> b, val<void*> out) {
	return OP(a, b, out);
}

// The flag feeding control flow (what kernels do: count overflowing rows).
val<int64_t> countOverflows(val<int64_t*> xs, val<int64_t> n, val<int64_t> factor) {
	val<int64_t> overflows(0);
	auto slot = nautilus::details::nautilus_alloca<int64_t>();
	for (val<int64_t> i(0); i < n; i = i + val<int64_t>(1)) {
		if (builtin::mul_overflow(*(xs + i), factor, slot)) {
			overflows = overflows + val<int64_t>(1);
		}
	}
	return overflows;
}

template <class T, class F, class R>
void expectScalar(F& fn, T a, T b, R reference) {
	T expected;
	bool expectedOverflow = reference(a, b, &expected);
	bool flag = false;
	T got = fn(a, b, &flag);
	REQUIRE(flag == expectedOverflow);
	REQUIRE(got == expected);
}

template <class T, class Engine>
void exerciseScalar(Engine& engine) {
	auto add = engine.registerFunction(checked<T, builtin::add_overflow>);
	auto sub = engine.registerFunction(checked<T, builtin::sub_overflow>);
	auto mul = engine.registerFunction(checked<T, builtin::mul_overflow>);
	constexpr T MAX = std::numeric_limits<T>::max();
	constexpr T MIN = std::numeric_limits<T>::min();
	// around the square roots of the limits, where products start to overflow
	constexpr T ROOT = sizeof(T) == 8 ? T(3037000500LL) : T(46341);
	const T values[] = {0, 1, -1, 2, 7, -9, MAX, MIN, MAX / 2, MIN / 2, ROOT, T(-ROOT), T(ROOT - 1)};
	for (T a : values) {
		for (T b : values) {
			expectScalar(add, a, b, [](T x, T y, T* r) { return __builtin_add_overflow(x, y, r); });
			expectScalar(sub, a, b, [](T x, T y, T* r) { return __builtin_sub_overflow(x, y, r); });
			expectScalar(mul, a, b, [](T x, T y, T* r) { return __builtin_mul_overflow(x, y, r); });
		}
	}
}

__extension__ typedef __int128 i128;
__extension__ typedef unsigned __int128 u128;

template <class Engine>
void exerciseWide(Engine& engine) {
	auto add = engine.registerFunction(checkedWide<builtin::add_overflow_i128>);
	auto sub = engine.registerFunction(checkedWide<builtin::sub_overflow_i128>);
	auto mul = engine.registerFunction(checkedWide<builtin::mul_overflow_i128>);
	const i128 MAX = static_cast<i128>((static_cast<u128>(1) << 127) - 1);
	const i128 MIN = -MAX - 1;
	const i128 values[] = {
	    0, 1, -1, 10, -77, MAX, MIN, MAX / 3, static_cast<i128>(1) << 64, -(static_cast<i128>(1) << 63)};
	for (i128 a : values) {
		for (i128 b : values) {
			// unaligned operands and result
			alignas(16) char buf[64];
			std::memcpy(buf + 1, &a, 16);
			std::memcpy(buf + 19, &b, 16);
			auto check = [&](auto& fn, auto reference) {
				i128 expected;
				bool expectedOverflow = reference(a, b, &expected);
				bool flag = fn((const void*) (buf + 1), (const void*) (buf + 19), (void*) (buf + 37));
				i128 got;
				std::memcpy(&got, buf + 37, 16);
				REQUIRE(flag == expectedOverflow);
				REQUIRE(got == expected);
			};
			check(add, [](i128 x, i128 y, i128* r) { return __builtin_add_overflow(x, y, r); });
			check(sub, [](i128 x, i128 y, i128* r) { return __builtin_sub_overflow(x, y, r); });
			check(mul, [](i128 x, i128 y, i128* r) { return __builtin_mul_overflow(x, y, r); });
		}
	}
}

template <class Engine>
void exercise(Engine& engine) {
	exerciseScalar<int32_t>(engine);
	exerciseScalar<int64_t>(engine);
	exerciseWide(engine);
	auto count = engine.registerFunction(countOverflows);
	int64_t xs[] = {1, 1LL << 40, -(1LL << 40), 3, INT64_MAX, 0};
	REQUIRE(count(xs, (int64_t) 6, (int64_t) (1LL << 30)) == 3);
}
} // namespace

TEST_CASE("overflow: every backend", "[overflow]") {
	nautilus::testing::forEachBackend([](auto& engine) { exercise(engine); });
}
