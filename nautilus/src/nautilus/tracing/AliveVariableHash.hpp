#pragma once

#include <array>
#include <cstdint>

namespace nautilus::tracing {

class AliveVariableHash {
	static constexpr size_t N = 1u << 16;

	std::array<uint32_t, N> counts{};
	std::array<uint64_t, N> hashes{};
	uint64_t alive_hash = 0;

	static constexpr uint64_t splitmix64(uint64_t x) noexcept {
		x += 0x9e3779b97f4a7c15;
		x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
		x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
		return x ^ (x >> 31);
	}

public:
	AliveVariableHash() {
		for (uint32_t i = 0; i < N; ++i)
			hashes[i] = splitmix64(i);
	}

	inline void increment(uint16_t id) noexcept {
		uint32_t& c = counts[id];
		if (c == 0)
			alive_hash ^= hashes[id];
		++c;
	}

	inline void decrement(uint16_t id) noexcept {
		uint32_t& c = counts[id];
		--c;
		if (c == 0)
			alive_hash ^= hashes[id];
	}

	inline uint64_t hash() const noexcept {
		return alive_hash;
	}

	inline void clear() noexcept {
		counts.fill(0);
		alive_hash = 0;
	}
};

} // namespace nautilus::tracing
