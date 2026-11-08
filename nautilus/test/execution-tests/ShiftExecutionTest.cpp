// `a << b` and `a >> b` have the promoted type of `a` alone, whatever the type of `b` is, and wrap at its width (NIR
// spec D10/D11). These tests shift a val by a val amount of the same, a narrower and a wider type, for every width and
// signedness of the shifted value, on every backend, and compare with plain C++. The amounts stay below the width of
// the promoted type: C++ does not define the others, and masking them is the business of the NIR tests.

#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"
#include <array>
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <typeinfo>
#include <vector>

namespace nautilus::engine {

namespace {

template <typename T, typename A>
void shiftKernel(val<T> x, val<A> amount, val<int64_t*> out) {
	out[0] = static_cast<val<int64_t>>(x << amount);
	out[1] = static_cast<val<int64_t>>(x >> amount);
}

template <typename T, typename A>
void checkShifts(NautilusEngine& engine) {
	using Promoted = decltype(+std::declval<T>());
	using UnsignedPromoted = std::make_unsigned_t<Promoted>;
	constexpr int bits = sizeof(Promoted) * 8;
	auto f = engine.registerFunction(&shiftKernel<T, A>);

	std::vector<T> values;
	for (long long v : {0LL, 1LL, -1LL, 2LL, 3LL, 100LL, -100LL, 127LL, -128LL, 255LL, 0x5555LL, 0x7fffLL, -0x8000LL,
	                    0xffffLL, 0x55555555LL, 0x7fffffffLL, -0x80000000LL, 0xffffffffLL, 0x7fffffffffffffffLL}) {
		values.push_back(static_cast<T>(v));
	}
	values.push_back(std::numeric_limits<T>::min());
	values.push_back(std::numeric_limits<T>::max());

	for (const T x : values) {
		for (int k = 0; k < bits; ++k) {
			const auto amount = static_cast<A>(k);
			std::array<int64_t, 2> got {};
			f(x, amount, got.data());
			// the left shift in the unsigned type: wrapping is what is defined of it
			const auto left = static_cast<Promoted>(static_cast<UnsignedPromoted>(static_cast<Promoted>(x)) << k);
			const auto right = static_cast<Promoted>(static_cast<Promoted>(x) >> k);
			INFO("x = " << +x << ", amount = " << k << ", x is " << typeid(T).name() << ", amount is "
			            << typeid(A).name());
			CHECK(got[0] == static_cast<int64_t>(left));
			CHECK(got[1] == static_cast<int64_t>(right));
		}
	}
}

template <typename T>
void checkAllAmounts(NautilusEngine& engine) {
	checkShifts<T, T>(engine);
	checkShifts<T, int32_t>(engine);
	checkShifts<T, uint8_t>(engine);
	checkShifts<T, int64_t>(engine);
	checkShifts<T, uint64_t>(engine);
}

} // namespace

TEST_CASE("Shifts: narrow shifted values") {
	nautilus::testing::forEachBackend([](NautilusEngine& engine) {
		checkAllAmounts<int8_t>(engine);
		checkAllAmounts<uint8_t>(engine);
		checkAllAmounts<int16_t>(engine);
		checkAllAmounts<uint16_t>(engine);
	});
}

// An int32_t shifted out of its 32 bits keeps 32 bits.
TEST_CASE("Shifts: 32-bit shifted values wrap") {
	nautilus::testing::forEachBackend([](NautilusEngine& engine) {
		checkAllAmounts<int32_t>(engine);
		checkAllAmounts<uint32_t>(engine);
	});
}

TEST_CASE("Shifts: 64-bit shifted values") {
	nautilus::testing::forEachBackend([](NautilusEngine& engine) {
		checkAllAmounts<int64_t>(engine);
		checkAllAmounts<uint64_t>(engine);
	});
}

} // namespace nautilus::engine
