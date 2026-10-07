#include "nautilus/static.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace nautilus {
namespace {

template <typename T>
concept InvariantScalar = requires(T&& value) { cacheInvariant(std::forward<T>(value)); };

} // namespace

TEST_CASE("Invariant scalar factories preserve nontracing values and types", "[value][constant-origin]") {
	enum class ScalarEnum : int32_t { Value = 7 };
	static_assert(InvariantScalar<int32_t>);
	static_assert(InvariantScalar<const int64_t&>);
	static_assert(InvariantScalar<bool&>);
	static_assert(InvariantScalar<double>);
	static_assert(!InvariantScalar<int64_t*>);
	static_assert(!InvariantScalar<std::nullptr_t>);
	static_assert(!InvariantScalar<ScalarEnum>);
	static_assert(!InvariantScalar<val<int64_t>>);
	static_assert(!InvariantScalar<static_val<int64_t>>);
	static_assert(std::is_same_v<decltype(cacheLiteral<true>()), val<bool>>);
	static_assert(std::is_same_v<decltype(cacheLiteral<int32_t {7}>()), val<int32_t>>);
	static_assert(std::is_same_v<decltype(cacheInvariant(std::declval<const int64_t&>())), val<int64_t>>);
	const auto check = []<typename T>(T input) {
		const T constant = input;
		auto copied = cacheInvariant(constant);
		auto moved = cacheInvariant(std::move(input));
		static_assert(std::is_same_v<decltype(copied), val<T>>);
		REQUIRE(details::RawValueResolver<T>::getRawValue(copied) == constant);
		REQUIRE(details::RawValueResolver<T>::getRawValue(moved) == constant);
	};
	check(int8_t {-7});
	check(uint8_t {251});
	check(int16_t {-319});
	check(uint16_t {65000});
	check(int32_t {-123456});
	check(uint32_t {3456789012U});
	check(int64_t {-1234567890123});
	check(uint64_t {123456789012345});
	check(float {1.25});
	check(double {-2.5});
	check(false);
	check(true);
	REQUIRE(details::RawValueResolver<int64_t>::getRawValue(cacheLiteral<int64_t {-17}>()) == -17);
	REQUIRE(details::RawValueResolver<bool>::getRawValue(cacheLiteral<true>()));
	REQUIRE_FALSE(details::RawValueResolver<bool>::getRawValue(cacheLiteral<false>()));
	REQUIRE(details::RawValueResolver<int64_t>::getRawValue(val<int64_t> {}) == 0);
	REQUIRE_FALSE(details::RawValueResolver<bool>::getRawValue(val<bool> {}));
	auto number = cacheLiteral<int64_t {7}>();
	REQUIRE(details::RawValueResolver<int64_t>::getRawValue(++number) == 8);
	REQUIRE(details::RawValueResolver<int64_t>::getRawValue(number--) == 8);
	REQUIRE(details::RawValueResolver<int64_t>::getRawValue(-number) == -7);
}

} // namespace nautilus
