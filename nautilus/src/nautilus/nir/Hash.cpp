#include "nautilus/nir/Hash.hpp"
#include "nautilus/nir/Image.hpp"
#define XXH_INLINE_ALL
#include "xxhash.h"
#include <algorithm>

namespace nautilus::nir {

std::array<uint8_t, 16> Hash128::bytes() const {
	std::array<uint8_t, 16> out {};
	for (int i = 0; i < 8; ++i) {
		out[i] = static_cast<uint8_t>(low >> (8 * i));
		out[8 + i] = static_cast<uint8_t>(high >> (8 * i));
	}
	return out;
}

std::string Hash128::hex() const {
	static const char* digits = "0123456789abcdef";
	std::string s;
	for (auto b : bytes()) {
		s += digits[b >> 4];
		s += digits[b & 15];
	}
	return s;
}

Hash128 hashBytes(std::span<const uint8_t> data) {
	auto h = XXH3_128bits_withSeed(data.data(), data.size(), 0);
	return {h.low64, h.high64};
}

Hash128 structuralHash(const Module& m, const EffectiveOptions& options, const CanonOptions& canon) {
	CanonOptions co = canon;
	co.anonymous = true;
	auto image = writeImage(canonicalize(m, co));
	auto block = encodeOptionsBlock(options);
	std::vector<uint8_t> input(image.begin() + sizeof(ImageHeader), image.end());
	input.insert(input.end(), block.begin(), block.end());
	return hashBytes(input);
}

Hash128 keyWithInlinedImports(const Hash128& moduleKey, std::vector<std::pair<std::string, Hash128>> exporters) {
	std::sort(exporters.begin(), exporters.end(), [](auto& a, auto& b) { return a.first < b.first; });
	std::vector<uint8_t> input;
	auto key = moduleKey.bytes();
	input.insert(input.end(), key.begin(), key.end());
	auto put32 = [&](uint32_t v) {
		for (int i = 0; i < 4; ++i) {
			input.push_back(static_cast<uint8_t>(v >> (8 * i)));
		}
	};
	put32(static_cast<uint32_t>(exporters.size()));
	for (auto& [name, hash] : exporters) {
		put32(static_cast<uint32_t>(name.size()));
		input.insert(input.end(), name.begin(), name.end());
		auto h = hash.bytes();
		input.insert(input.end(), h.begin(), h.end());
	}
	return hashBytes(input);
}

} // namespace nautilus::nir
