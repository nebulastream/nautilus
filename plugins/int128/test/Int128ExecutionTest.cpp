#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <nautilus/Engine.hpp>
#include <nautilus/int128.hpp>

namespace {
using nautilus::int128;

nautilus::val<uint64_t> multiplyHigh(nautilus::val<uint64_t> x) {
	int128 wide(x);
	return (wide * int128(nautilus::val<uint64_t>(0), nautilus::val<int64_t>(1))).high();
}
nautilus::val<int64_t> signedArithmetic(nautilus::val<int64_t> x) {
	int128 wide(x);
	return static_cast<nautilus::val<int64_t>>((wide * int128(7)) / int128(3));
}
nautilus::val<uint64_t> arithmetic(nautilus::val<uint64_t> a, nautilus::val<uint64_t> b) {
	int128 value = (((int128(a) + int128(b)) - int128(9)) * int128(3)) / int128(2);
	return value.low() ^ static_cast<nautilus::val<uint64_t>>(value.high());
}
nautilus::val<uint64_t> bitwise(nautilus::val<uint64_t> a, nautilus::val<uint32_t> shift) {
	int128 x(a);
	auto value = ~(-(((x << shift) | int128(0x55)) ^ (x >> shift)));
	return value.low() ^ static_cast<nautilus::val<uint64_t>>(value.high());
}
nautilus::val<bool> comparisons(nautilus::val<int64_t> a, nautilus::val<int64_t> b) {
	int128 x(a);
	int128 y(b);
	return (x < y) && (x != y) && (y > x) && (x <= y) && (y >= x);
}
nautilus::val<int32_t> casts(nautilus::val<int32_t> input) {
	return static_cast<nautilus::val<int32_t>>(int128(input));
}
nautilus::val<bool> boolCast(nautilus::val<uint64_t> low, nautilus::val<int64_t> high) {
	return static_cast<nautilus::val<bool>>(int128(low, high));
}
nautilus::val<uint64_t> memoryRoundTrip(nautilus::val<void*> output, nautilus::val<const void*> input) {
	auto value = int128::Load(input) + int128(1);
	value.Store(output);
	return value.high();
}
} // namespace

TEST_CASE("int128 arithmetic executes through available backends") {
	nautilus::engine::NautilusEngine engine(nautilus::engine::Options {});
	auto mul = engine.registerFunction(multiplyHigh);
	auto arithmeticFn = engine.registerFunction(signedArithmetic);
	REQUIRE(mul(0x123456789ULL) == 0x123456789ULL);
	REQUIRE(arithmeticFn(-30) == -70);
}

TEST_CASE("int128 operators, casts, comparisons, and memory execute") {
	nautilus::engine::NautilusEngine engine(nautilus::engine::Options {});
	auto arithmeticFn = engine.registerFunction(arithmetic);
	auto multiplyHighFn = engine.registerFunction(multiplyHigh);
	auto bitwiseFn = engine.registerFunction(bitwise);
	auto comparisonsFn = engine.registerFunction(comparisons);
	auto castsFn = engine.registerFunction(casts);
	auto boolCastFn = engine.registerFunction(boolCast);
	auto memoryFn = engine.registerFunction(memoryRoundTrip);

	__extension__ typedef unsigned __int128 uint128;
	const uint64_t a = UINT64_C(0xfedcba9876543210);
	const uint64_t b = UINT64_C(0x123456789abcdef0);
	uint128 expectedArithmetic = ((uint128(a) + b - 9) * 3) / 2;
	REQUIRE(arithmeticFn(a, b) == (uint64_t(expectedArithmetic) ^ uint64_t(expectedArithmetic >> 64)));
	uint128 shifted = uint128(a) << 73;
	uint128 expectedBits = ~(-((shifted | 0x55) ^ (uint128(a) >> 73)));
	REQUIRE(bitwiseFn(a, 73) == (uint64_t(expectedBits) ^ uint64_t(expectedBits >> 64)));
	REQUIRE(comparisonsFn(-100, 42));
	REQUIRE_FALSE(comparisonsFn(42, -100));
	REQUIRE(castsFn(-1234567) == -1234567);
	REQUIRE_FALSE(boolCastFn(0, 0));
	REQUIRE(boolCastFn(0, 1));
	REQUIRE(boolCastFn(1, 0));
	for (int32_t value = -4096; value <= 4096; value += 127) {
		CAPTURE(value);
		REQUIRE(castsFn(value) == value);
		auto widened = static_cast<uint64_t>(static_cast<uint32_t>(value));
		REQUIRE(multiplyHighFn(widened) == widened);
	}

	alignas(16) nautilus::detail::int128_data input {UINT64_MAX, 4};
	alignas(16) nautilus::detail::int128_data output {};
	REQUIRE(memoryFn(&output, &input) == 5);
	REQUIRE(output.low == 0);
	REQUIRE(output.high == 5);

	std::array<std::byte, sizeof(input) + 2> unalignedInput {};
	std::array<std::byte, sizeof(output) + 2> unalignedOutput {};
	std::memcpy(unalignedInput.data() + 1, &input, sizeof(input));
	REQUIRE(memoryFn(unalignedOutput.data() + 1, unalignedInput.data() + 1) == 5);
	std::memcpy(&output, unalignedOutput.data() + 1, sizeof(output));
	REQUIRE(output.low == 0);
	REQUIRE(output.high == 5);
}
