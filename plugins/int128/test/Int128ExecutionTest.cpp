#include "ExecutionTest.hpp"
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <nautilus/Engine.hpp>
#include <nautilus/int128.hpp>

namespace {
using I128 = nautilus::val<nautilus::detail::int128_t>;
using nautilus::val;

val<uint64_t> multiplyHigh(val<uint64_t> x) {
	I128 wide(x);
	return (wide * I128(val<uint64_t>(0), val<int64_t>(1))).high();
}
val<int64_t> signedArithmetic(val<int64_t> x) {
	I128 wide(x);
	return static_cast<val<int64_t>>((wide * I128(7)) / I128(3));
}
val<uint64_t> arithmetic(val<uint64_t> a, val<uint64_t> b) {
	I128 value = (((I128(a) + I128(b)) - I128(9)) * I128(3)) / I128(2);
	return value.low() ^ static_cast<val<uint64_t>>(value.high());
}
val<uint64_t> bitwise(val<uint64_t> a, val<uint32_t> shift) {
	I128 x(a);
	auto value = ~(-(((x << shift) | I128(0x55)) ^ (x >> shift)));
	return value.low() ^ static_cast<val<uint64_t>>(value.high());
}
val<bool> comparisons(val<int64_t> a, val<int64_t> b) {
	I128 x(a);
	I128 y(b);
	return (x < y) && (x != y) && (y > x) && (x <= y) && (y >= x);
}
val<int32_t> casts(val<int32_t> input) {
	return static_cast<val<int32_t>>(I128(input));
}
val<bool> boolCast(val<uint64_t> low, val<int64_t> high) {
	return static_cast<val<bool>>(I128(low, high));
}
val<uint64_t> memoryRoundTrip(val<void*> output, val<const void*> input) {
	auto value = I128::Load(input) + I128(1);
	value.Store(output);
	return value.high();
}
val<uint64_t> notOnly(val<uint64_t> a) {
	I128 v = ~I128(a);
	return v.low() ^ static_cast<val<uint64_t>>(v.high());
}
val<uint64_t> negOnly(val<uint64_t> a) {
	I128 v = -I128(a);
	return v.low() ^ static_cast<val<uint64_t>>(v.high());
}
val<uint64_t> constMath(val<uint64_t> a) {
	I128 c(UINT64_C(0xfedcba9876543210), INT64_C(0x123456789abcdef0));
	I128 v = (c + I128(a)) * I128(2);
	return v.low() ^ static_cast<val<uint64_t>>(v.high());
}
val<uint64_t> unsignedCtor(val<uint64_t> a) {
	nautilus::detail::uint128_t c =
	    (nautilus::detail::uint128_t) UINT64_C(0x8000000000000000) << 64 | UINT64_C(0x123456789abcdef0);
	I128 v(c);
	I128 sum = v + I128(a);
	return sum.low() ^ sum.high();
}

void exercise(const nautilus::engine::NautilusEngine& engine) {
	auto arithmeticFn = engine.registerFunction(arithmetic);
	auto multiplyHighFn = engine.registerFunction(multiplyHigh);
	auto signedArithmeticFn = engine.registerFunction(signedArithmetic);
	auto bitwiseFn = engine.registerFunction(bitwise);
	auto comparisonsFn = engine.registerFunction(comparisons);
	auto castsFn = engine.registerFunction(casts);
	auto boolCastFn = engine.registerFunction(boolCast);
	auto memoryFn = engine.registerFunction(memoryRoundTrip);
	auto notOnlyFn = engine.registerFunction(notOnly);
	auto negOnlyFn = engine.registerFunction(negOnly);
	auto constMathFn = engine.registerFunction(constMath);
	auto unsignedCtorFn = engine.registerFunction(unsignedCtor);

	__extension__ typedef unsigned __int128 uint128;
	const uint64_t a = UINT64_C(0xfedcba9876543210);
	const uint64_t b = UINT64_C(0x123456789abcdef0);

	REQUIRE(multiplyHighFn(0x123456789ULL) == 0x123456789ULL);
	REQUIRE(signedArithmeticFn(-30) == -70);

	uint128 expectedArithmetic = ((uint128(a) + b - 9) * 3) / 2;
	REQUIRE(arithmeticFn(a, b) == (uint64_t(expectedArithmetic) ^ uint64_t(expectedArithmetic >> 64)));

	uint128 shifted = uint128(a) << 73;
	uint128 expectedBits = ~(-((shifted | 0x55) ^ (uint128(a) >> 73)));
	REQUIRE(bitwiseFn(a, 73) == (uint64_t(expectedBits) ^ uint64_t(expectedBits >> 64)));

	REQUIRE(comparisonsFn(-100, 42));
	REQUIRE_FALSE(comparisonsFn(42, -100));

	__extension__ typedef unsigned __int128 uint128;
	uint128 negA = ~(uint128) a;
	REQUIRE(notOnlyFn((uint64_t) a) == (uint64_t(negA) ^ uint64_t(negA >> 64)));
	uint128 posA = (uint128) a;
	REQUIRE(negOnlyFn((uint64_t) a) == (uint64_t(-posA) ^ uint64_t((-posA) >> 64)));

	uint128 c = ((uint128) (int64_t) INT64_C(0x123456789abcdef0) << 64) | UINT64_C(0xfedcba9876543210);
	uint128 cm = (c + posA) * 2;
	REQUIRE(constMathFn((uint64_t) a) == (uint64_t(cm) ^ uint64_t(cm >> 64)));

	nautilus::detail::uint128_t uc =
	    (nautilus::detail::uint128_t) UINT64_C(0x8000000000000000) << 64 | UINT64_C(0x123456789abcdef0);
	uint128 um = uc + posA;
	REQUIRE(unsignedCtorFn((uint64_t) a) == (uint64_t(um) ^ uint64_t(um >> 64)));

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
} // namespace

TEST_CASE("int128: every backend", "[int128]") {
	nautilus::testing::forEachBackend([](auto& engine) { exercise(engine); });
}