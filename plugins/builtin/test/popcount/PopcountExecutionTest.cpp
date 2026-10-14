#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/builtin/plugin.hpp"
#include "nautilus/builtin/popcount/popcount.hpp"
#include <bit>
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <vector>

namespace nautilus::engine {

// ============================================================================
// Test Functions
// ============================================================================

template <typename T>
val<T> popcountOf(val<T> x) {
	return builtin::popcount(x);
}

/// popcount of a narrow sum that wraps around its width. Under lazy narrowing
/// the AsmJit backend may keep garbage above the low bits of the sum, which the
/// popcount lowering must not count.
template <typename T>
val<T> popcountOfSum(val<T> a, val<T> b) {
	return builtin::popcount(static_cast<val<T>>(a + b));
}

/// Total number of set bits in an array, feeding popcount results back into
/// arithmetic.
val<uint64_t> totalSetBits(val<uint64_t*> data, val<int64_t> count) {
	val<uint64_t> total = 0;
	for (val<int64_t> i = 0; i < count; i = i + 1) {
		total = total + builtin::popcount(static_cast<val<uint64_t>>(data[i]));
	}
	return total;
}

/// Uses the result in a branch, as e.g. a has_single_bit check would.
val<bool> hasSingleBit(val<uint32_t> x) {
	if (builtin::popcount(x) == 1) {
		return true;
	}
	return false;
}

// ============================================================================
// Test Body
// ============================================================================

template <typename T>
T expectedPopcount(T x) {
	return static_cast<T>(std::popcount(static_cast<std::make_unsigned_t<T>>(x)));
}

template <typename T>
std::vector<T> interestingValues() {
	using Limits = std::numeric_limits<T>;
	std::vector<T> values = {0,
	                         1,
	                         2,
	                         3,
	                         Limits::max(),
	                         Limits::min(),
	                         static_cast<T>(Limits::max() - 1),
	                         static_cast<T>(0x55),
	                         static_cast<T>(0xAA)};
	if constexpr (std::is_signed_v<T>) {
		values.push_back(-1);
		values.push_back(-2);
		values.push_back(static_cast<T>(Limits::min() + 1));
	}
	if constexpr (sizeof(T) >= 2) {
		values.push_back(static_cast<T>(0x8001));
		values.push_back(static_cast<T>(0x5555));
	}
	if constexpr (sizeof(T) >= 4) {
		values.push_back(static_cast<T>(0x80000001u));
		values.push_back(static_cast<T>(0x12345678u));
	}
	if constexpr (sizeof(T) >= 8) {
		values.push_back(static_cast<T>(0x8000000000000001ull));
		values.push_back(static_cast<T>(0x0123456789ABCDEFull));
	}
	return values;
}

template <typename T>
void checkPopcount(engine::NautilusEngine& engine) {
	auto f = engine.registerFunction(popcountOf<T>);
	for (auto value : interestingValues<T>()) {
		INFO("value = " << static_cast<int64_t>(value));
		REQUIRE(f(value) == expectedPopcount(value));
	}
}

template <typename T>
void checkPopcountOfSum(engine::NautilusEngine& engine) {
	using Limits = std::numeric_limits<T>;
	auto f = engine.registerFunction(popcountOfSum<T>);
	const T pairs[][2] = {{Limits::max(), 1}, {Limits::max(), Limits::max()},           {Limits::min(), Limits::min()},
	                      {Limits::max(), 2}, {static_cast<T>(-1), static_cast<T>(-1)}, {5, 6}};
	for (const auto& pair : pairs) {
		INFO("a = " << static_cast<int64_t>(pair[0]) << ", b = " << static_cast<int64_t>(pair[1]));
		REQUIRE(f(pair[0], pair[1]) == expectedPopcount(static_cast<T>(pair[0] + pair[1])));
	}
}

void popcountTests(engine::NautilusEngine& engine) {
	// bc passes narrow unsigned call arguments through dyncall's signed types,
	// and so does tbc on Apple arm64 (see NarrowCallArgumentTest.cpp), so the
	// fallback call receives a sign-extended uint8_t/uint16_t there.
	const auto backend = engine.getNameOfBackend();
	bool narrowUnsignedSupported = backend != "bc";
#if defined(__APPLE__) && defined(__aarch64__)
	narrowUnsignedSupported = narrowUnsignedSupported && backend != "tbc";
#endif

	SECTION("signed types") {
		checkPopcount<int8_t>(engine);
		checkPopcount<int16_t>(engine);
		checkPopcount<int32_t>(engine);
		checkPopcount<int64_t>(engine);
	}

	SECTION("unsigned types") {
		if (narrowUnsignedSupported) {
			checkPopcount<uint8_t>(engine);
			checkPopcount<uint16_t>(engine);
		}
		checkPopcount<uint32_t>(engine);
		checkPopcount<uint64_t>(engine);
	}

	SECTION("operand wrapped around its width") {
		checkPopcountOfSum<int8_t>(engine);
		checkPopcountOfSum<int16_t>(engine);
		checkPopcountOfSum<int32_t>(engine);
		if (narrowUnsignedSupported) {
			checkPopcountOfSum<uint8_t>(engine);
			checkPopcountOfSum<uint16_t>(engine);
		}
		checkPopcountOfSum<uint32_t>(engine);
	}

	SECTION("popcount in a loop") {
		uint64_t data[] = {0, 1, 0xFF, 0xFFFFFFFFFFFFFFFFull, 0x8000000000000000ull, 0x0123456789ABCDEFull};
		constexpr int64_t n = sizeof(data) / sizeof(data[0]);
		uint64_t expected = 0;
		for (auto value : data) {
			expected += std::popcount(value);
		}
		auto f = engine.registerFunction(totalSetBits);
		REQUIRE(f(data, n) == expected);
	}

	SECTION("popcount in a branch condition") {
		auto f = engine.registerFunction(hasSingleBit);
		REQUIRE(f(0) == false);
		REQUIRE(f(1) == true);
		REQUIRE(f(0x80000000u) == true);
		REQUIRE(f(3) == false);
		REQUIRE(f(0xFFFFFFFFu) == false);
	}
}

TEST_CASE("Popcount Interpreter Test") {
	auto engine = nautilus::testing::makeEngine("interpreter");
	popcountTests(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("Popcount Compiler Test") {
	nautilus::testing::forEachBackend([](engine::NautilusEngine& engine) { popcountTests(engine); },
	                                  /*include_interpreter=*/false);
}
#endif

} // namespace nautilus::engine
