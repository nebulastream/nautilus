#include <cstddef>
#include <cstdint>
#include <cstring>
#include <nautilus/Engine.hpp>
#include <nautilus/int128.hpp>

namespace {
using nautilus::int128;
__extension__ typedef unsigned __int128 uint128;
__extension__ typedef __int128 sint128;

nautilus::val<uint64_t> fuzzKernel(nautilus::val<uint64_t> a, nautilus::val<uint64_t> b,
                                   nautilus::val<uint32_t> shift) {
	int128 x(a);
	int128 y(b | 1);
	int128 mixed = (((x + y) * (x - y)) ^ ~(x | y));
	mixed = mixed & int128(UINT64_MAX, INT64_MAX);
	mixed = ((mixed << shift) >> shift) + (mixed / y) + (mixed % y);
	return mixed.low() ^ static_cast<nautilus::val<uint64_t>>(mixed.high());
}

uint64_t nativeKernel(uint64_t a, uint64_t b, uint32_t shift) {
	b |= 1;
	shift &= 127;
	uint128 x = a;
	uint128 y = b;
	uint128 mixed = (((x + y) * (x - y)) ^ ~(x | y));
	mixed &= (uint128(1) << 127) - 1;
	uint128 shifted = mixed << shift;
	uint128 shiftedBack = static_cast<uint128>(static_cast<sint128>(shifted) >> shift);
	mixed = shiftedBack + (mixed / y) + (mixed % y);
	return uint64_t(mixed) ^ uint64_t(mixed >> 64);
}

auto& compiledKernel() {
	static nautilus::engine::NautilusEngine engine(nautilus::engine::Options {});
	static auto kernel = engine.registerFunction(fuzzKernel);
	return kernel;
}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
	if (size < 20) {
		return 0;
	}
	uint64_t a;
	uint64_t b;
	uint32_t shift;
	std::memcpy(&a, data, sizeof(a));
	std::memcpy(&b, data + 8, sizeof(b));
	std::memcpy(&shift, data + 16, sizeof(shift));
	if (compiledKernel()(a, b, shift) != nativeKernel(a, b, shift)) {
		__builtin_trap();
	}
	return 0;
}
