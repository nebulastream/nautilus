#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace nautilus::fuzz {

/**
 * @brief Deterministic, FuzzedDataProvider-style consumer of a raw fuzzer buffer.
 *
 * Every decision the AST generator makes (which node, which operand, which
 * immediate, which argument value) is drawn from this reader, so a given input
 * buffer always reproduces the exact same program + inputs. When the buffer is
 * exhausted the reader keeps returning zeros, which lets generation terminate
 * gracefully by forcing leaf nodes.
 */
class ByteReader {
public:
	ByteReader(const uint8_t* data, size_t size) : data_(data), size_(size) {
	}

	/// True once every byte of the buffer has been consumed.
	[[nodiscard]] bool exhausted() const {
		return pos_ >= size_;
	}

	/// Next byte, or 0 if the buffer is exhausted.
	uint8_t byte() {
		if (pos_ >= size_) {
			return 0;
		}
		return data_[pos_++];
	}

	/// Consume sizeof(T) bytes little-endian into a trivially-copyable T,
	/// zero-padding any bytes past the end of the buffer.
	template <typename T>
	T consume() {
		T value {};
		uint8_t* out = reinterpret_cast<uint8_t*>(&value);
		for (size_t i = 0; i < sizeof(T); ++i) {
			out[i] = byte();
		}
		return value;
	}

	/// Uniform-ish index in [0, n) for small n (n must be > 0).
	uint32_t pick(uint32_t n) {
		if (n <= 1) {
			return 0;
		}
		// Two bytes give enough spread for the small fan-outs used here.
		uint32_t raw = static_cast<uint32_t>(byte()) | (static_cast<uint32_t>(byte()) << 8);
		return raw % n;
	}

private:
	const uint8_t* data_;
	size_t size_;
	size_t pos_ = 0;
};

/// bool-specific consume(): the generic memcpy-based consume<T>() would
/// reinterpret a raw byte as bool, and a byte other than 0/1 is not a valid
/// bool object representation. Masking to the low bit keeps every consumed
/// bool well-defined.
template <>
inline bool ByteReader::consume<bool>() {
	return (byte() & 0x1) != 0;
}

} // namespace nautilus::fuzz
