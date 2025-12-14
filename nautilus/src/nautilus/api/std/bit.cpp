#include <bit>
#include <cstdint>
#include <nautilus/function.hpp>
#include <nautilus/std/bit.h>

namespace nautilus {

// ============================================================================
// countl_zero - Count leading zeros
// ============================================================================

val<uint8_t> countl_zero(val<uint8_t> x) {
	return invoke<uint8_t, uint8_t>(std::countl_zero<uint8_t>, x);
}

val<uint16_t> countl_zero(val<uint16_t> x) {
	return invoke<uint16_t, uint16_t>(std::countl_zero<uint16_t>, x);
}

val<uint32_t> countl_zero(val<uint32_t> x) {
	return invoke<uint32_t, uint32_t>(std::countl_zero<uint32_t>, x);
}

val<uint64_t> countl_zero(val<uint64_t> x) {
	return invoke<uint64_t, uint64_t>(std::countl_zero<uint64_t>, x);
}

// ============================================================================
// countl_one - Count leading ones
// ============================================================================

val<uint8_t> countl_one(val<uint8_t> x) {
	return invoke<uint8_t, uint8_t>(std::countl_one<uint8_t>, x);
}

val<uint16_t> countl_one(val<uint16_t> x) {
	return invoke<uint16_t, uint16_t>(std::countl_one<uint16_t>, x);
}

val<uint32_t> countl_one(val<uint32_t> x) {
	return invoke<uint32_t, uint32_t>(std::countl_one<uint32_t>, x);
}

val<uint64_t> countl_one(val<uint64_t> x) {
	return invoke<uint64_t, uint64_t>(std::countl_one<uint64_t>, x);
}

// ============================================================================
// countr_zero - Count trailing zeros
// ============================================================================

val<uint8_t> countr_zero(val<uint8_t> x) {
	return invoke<uint8_t, uint8_t>(std::countr_zero<uint8_t>, x);
}

val<uint16_t> countr_zero(val<uint16_t> x) {
	return invoke<uint16_t, uint16_t>(std::countr_zero<uint16_t>, x);
}

val<uint32_t> countr_zero(val<uint32_t> x) {
	return invoke<uint32_t, uint32_t>(std::countr_zero<uint32_t>, x);
}

val<uint64_t> countr_zero(val<uint64_t> x) {
	return invoke<uint64_t, uint64_t>(std::countr_zero<uint64_t>, x);
}

// ============================================================================
// countr_one - Count trailing ones
// ============================================================================

val<uint8_t> countr_one(val<uint8_t> x) {
	return invoke<uint8_t, uint8_t>(std::countr_one<uint8_t>, x);
}

val<uint16_t> countr_one(val<uint16_t> x) {
	return invoke<uint16_t, uint16_t>(std::countr_one<uint16_t>, x);
}

val<uint32_t> countr_one(val<uint32_t> x) {
	return invoke<uint32_t, uint32_t>(std::countr_one<uint32_t>, x);
}

val<uint64_t> countr_one(val<uint64_t> x) {
	return invoke<uint64_t, uint64_t>(std::countr_one<uint64_t>, x);
}

// ============================================================================
// popcount - Population count (count set bits)
// ============================================================================

val<uint8_t> popcount(val<uint8_t> x) {
	return invoke<uint8_t, uint8_t>(std::popcount<uint8_t>, x);
}

val<uint16_t> popcount(val<uint16_t> x) {
	return invoke<uint16_t, uint16_t>(std::popcount<uint16_t>, x);
}

val<uint32_t> popcount(val<uint32_t> x) {
	return invoke<uint32_t, uint32_t>(std::popcount<uint32_t>, x);
}

val<uint64_t> popcount(val<uint64_t> x) {
	return invoke<uint64_t, uint64_t>(std::popcount<uint64_t>, x);
}

// ============================================================================
// has_single_bit - Check if power of two
// ============================================================================

val<bool> has_single_bit(val<uint8_t> x) {
	return invoke<bool, uint8_t>(std::has_single_bit<uint8_t>, x);
}

val<bool> has_single_bit(val<uint16_t> x) {
	return invoke<bool, uint16_t>(std::has_single_bit<uint16_t>, x);
}

val<bool> has_single_bit(val<uint32_t> x) {
	return invoke<bool, uint32_t>(std::has_single_bit<uint32_t>, x);
}

val<bool> has_single_bit(val<uint64_t> x) {
	return invoke<bool, uint64_t>(std::has_single_bit<uint64_t>, x);
}

// ============================================================================
// bit_width - Bit width needed to represent value
// ============================================================================

val<uint8_t> bit_width(val<uint8_t> x) {
	return invoke<uint8_t, uint8_t>(std::bit_width<uint8_t>, x);
}

val<uint16_t> bit_width(val<uint16_t> x) {
	return invoke<uint16_t, uint16_t>(std::bit_width<uint16_t>, x);
}

val<uint32_t> bit_width(val<uint32_t> x) {
	return invoke<uint32_t, uint32_t>(std::bit_width<uint32_t>, x);
}

val<uint64_t> bit_width(val<uint64_t> x) {
	return invoke<uint64_t, uint64_t>(std::bit_width<uint64_t>, x);
}

// ============================================================================
// bit_ceil - Round up to next power of two
// ============================================================================

val<uint8_t> bit_ceil(val<uint8_t> x) {
	return invoke<uint8_t, uint8_t>(std::bit_ceil<uint8_t>, x);
}

val<uint16_t> bit_ceil(val<uint16_t> x) {
	return invoke<uint16_t, uint16_t>(std::bit_ceil<uint16_t>, x);
}

val<uint32_t> bit_ceil(val<uint32_t> x) {
	return invoke<uint32_t, uint32_t>(std::bit_ceil<uint32_t>, x);
}

val<uint64_t> bit_ceil(val<uint64_t> x) {
	return invoke<uint64_t, uint64_t>(std::bit_ceil<uint64_t>, x);
}

// ============================================================================
// bit_floor - Round down to previous power of two
// ============================================================================

val<uint8_t> bit_floor(val<uint8_t> x) {
	return invoke<uint8_t, uint8_t>(std::bit_floor<uint8_t>, x);
}

val<uint16_t> bit_floor(val<uint16_t> x) {
	return invoke<uint16_t, uint16_t>(std::bit_floor<uint16_t>, x);
}

val<uint32_t> bit_floor(val<uint32_t> x) {
	return invoke<uint32_t, uint32_t>(std::bit_floor<uint32_t>, x);
}

val<uint64_t> bit_floor(val<uint64_t> x) {
	return invoke<uint64_t, uint64_t>(std::bit_floor<uint64_t>, x);
}

// ============================================================================
// rotl - Rotate left
// ============================================================================

val<uint8_t> rotl(val<uint8_t> x, val<uint8_t> s) {
	return invoke<uint8_t, uint8_t, uint8_t>(std::rotl<uint8_t>, x, s);
}

val<uint16_t> rotl(val<uint16_t> x, val<uint16_t> s) {
	return invoke<uint16_t, uint16_t, uint16_t>(std::rotl<uint16_t>, x, s);
}

val<uint32_t> rotl(val<uint32_t> x, val<uint32_t> s) {
	return invoke<uint32_t, uint32_t, uint32_t>(std::rotl<uint32_t>, x, s);
}

val<uint64_t> rotl(val<uint64_t> x, val<uint64_t> s) {
	return invoke<uint64_t, uint64_t, uint64_t>(std::rotl<uint64_t>, x, s);
}

// ============================================================================
// rotr - Rotate right
// ============================================================================

val<uint8_t> rotr(val<uint8_t> x, val<uint8_t> s) {
	return invoke<uint8_t, uint8_t, uint8_t>(std::rotr<uint8_t>, x, s);
}

val<uint16_t> rotr(val<uint16_t> x, val<uint16_t> s) {
	return invoke<uint16_t, uint16_t, uint16_t>(std::rotr<uint16_t>, x, s);
}

val<uint32_t> rotr(val<uint32_t> x, val<uint32_t> s) {
	return invoke<uint32_t, uint32_t, uint32_t>(std::rotr<uint32_t>, x, s);
}

val<uint64_t> rotr(val<uint64_t> x, val<uint64_t> s) {
	return invoke<uint64_t, uint64_t, uint64_t>(std::rotr<uint64_t>, x, s);
}

// ============================================================================
// byteswap - Reverse byte order
// ============================================================================

#if __cpp_lib_byteswap >= 202110L
val<uint16_t> byteswap(val<uint16_t> x) {
	return invoke<uint16_t, uint16_t>(std::byteswap<uint16_t>, x);
}

val<uint32_t> byteswap(val<uint32_t> x) {
	return invoke<uint32_t, uint32_t>(std::byteswap<uint32_t>, x);
}

val<uint64_t> byteswap(val<uint64_t> x) {
	return invoke<uint64_t, uint64_t>(std::byteswap<uint64_t>, x);
}
#else
// Fallback implementation for older compilers
static uint16_t byteswap_impl_u16(uint16_t x) {
	return (x >> 8) | (x << 8);
}

static uint32_t byteswap_impl_u32(uint32_t x) {
	return ((x >> 24) & 0x000000FF) | ((x >> 8) & 0x0000FF00) | ((x << 8) & 0x00FF0000) | ((x << 24) & 0xFF000000);
}

static uint64_t byteswap_impl_u64(uint64_t x) {
	return ((x >> 56) & 0x00000000000000FFULL) | ((x >> 40) & 0x000000000000FF00ULL) |
	       ((x >> 24) & 0x0000000000FF0000ULL) | ((x >> 8) & 0x00000000FF000000ULL) |
	       ((x << 8) & 0x000000FF00000000ULL) | ((x << 24) & 0x0000FF0000000000ULL) |
	       ((x << 40) & 0x00FF000000000000ULL) | ((x << 56) & 0xFF00000000000000ULL);
}

val<uint16_t> byteswap(val<uint16_t> x) {
	return invoke<uint16_t, uint16_t>(byteswap_impl_u16, x);
}

val<uint32_t> byteswap(val<uint32_t> x) {
	return invoke<uint32_t, uint32_t>(byteswap_impl_u32, x);
}

val<uint64_t> byteswap(val<uint64_t> x) {
	return invoke<uint64_t, uint64_t>(byteswap_impl_u64, x);
}
#endif

} // namespace nautilus
