#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace nautilus::serialization {

/// CRC-32C (Castagnoli), the checksum iSCSI, ext4 and SCTP use. A table-driven
/// software implementation: the checksum covers each buffer once, so it is not
/// worth the per-architecture intrinsics.
inline uint32_t crc32c(std::span<const std::byte> data) {
	static constexpr auto table = [] {
		std::array<uint32_t, 256> entries {};
		for (uint32_t i = 0; i < 256; ++i) {
			uint32_t crc = i;
			for (int bit = 0; bit < 8; ++bit) {
				crc = (crc & 1u) != 0 ? (crc >> 1) ^ 0x82F63B78u : crc >> 1;
			}
			entries[i] = crc;
		}
		return entries;
	}();
	uint32_t crc = 0xFFFFFFFFu;
	for (const auto byte : data) {
		crc = table[(crc ^ static_cast<uint8_t>(byte)) & 0xFFu] ^ (crc >> 8);
	}
	return crc ^ 0xFFFFFFFFu;
}

} // namespace nautilus::serialization
