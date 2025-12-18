#include <bit>
#include <cstdint>
#include <nautilus/function.hpp>
#include <nautilus/std/bit.h>

namespace nautilus {

// ============================================================================
// Wrapper functions with extern linkage for intrinsic registration
// ============================================================================

// These wrappers are needed because std::bit functions are templates
// and don't have stable runtime addresses for intrinsic matching
// Using extern "C" linkage ensures stable names for intrinsic matching

extern "C" {

uint8_t countl_zero_u8_impl(uint8_t x) {
	return std::countl_zero(x);
}

uint16_t countl_zero_u16_impl(uint16_t x) {
	return std::countl_zero(x);
}

uint32_t countl_zero_u32_impl(uint32_t x) {
	return std::countl_zero(x);
}

uint64_t countl_zero_u64_impl(uint64_t x) {
	return std::countl_zero(x);
}

uint8_t countr_zero_u8_impl(uint8_t x) {
	return std::countr_zero(x);
}

uint16_t countr_zero_u16_impl(uint16_t x) {
	return std::countr_zero(x);
}

uint32_t countr_zero_u32_impl(uint32_t x) {
	return std::countr_zero(x);
}

uint64_t countr_zero_u64_impl(uint64_t x) {
	return std::countr_zero(x);
}

uint8_t popcount_u8_impl(uint8_t x) {
	return std::popcount(x);
}

uint16_t popcount_u16_impl(uint16_t x) {
	return std::popcount(x);
}

uint32_t popcount_u32_impl(uint32_t x) {
	return std::popcount(x);
}

uint64_t popcount_u64_impl(uint64_t x) {
	return std::popcount(x);
}

uint8_t rotl_u8_impl(uint8_t x, uint8_t s) {
	return std::rotl(x, static_cast<int>(s));
}

uint16_t rotl_u16_impl(uint16_t x, uint16_t s) {
	return std::rotl(x, static_cast<int>(s));
}

uint32_t rotl_u32_impl(uint32_t x, uint32_t s) {
	return std::rotl(x, static_cast<int>(s));
}

uint64_t rotl_u64_impl(uint64_t x, uint64_t s) {
	return std::rotl(x, static_cast<int>(s));
}

uint8_t rotr_u8_impl(uint8_t x, uint8_t s) {
	return std::rotr(x, static_cast<int>(s));
}

uint16_t rotr_u16_impl(uint16_t x, uint16_t s) {
	return std::rotr(x, static_cast<int>(s));
}

uint32_t rotr_u32_impl(uint32_t x, uint32_t s) {
	return std::rotr(x, static_cast<int>(s));
}

uint64_t rotr_u64_impl(uint64_t x, uint64_t s) {
	return std::rotr(x, static_cast<int>(s));
}

#if __cpp_lib_byteswap >= 202110L
uint16_t byteswap_u16_impl(uint16_t x) {
	return std::byteswap(x);
}

uint32_t byteswap_u32_impl(uint32_t x) {
	return std::byteswap(x);
}

uint64_t byteswap_u64_impl(uint64_t x) {
	return std::byteswap(x);
}
#else
uint16_t byteswap_u16_impl(uint16_t x) {
	return (x >> 8) | (x << 8);
}

uint32_t byteswap_u32_impl(uint32_t x) {
	return ((x >> 24) & 0x000000FF) | ((x >> 8) & 0x0000FF00) | ((x << 8) & 0x00FF0000) | ((x << 24) & 0xFF000000);
}

uint64_t byteswap_u64_impl(uint64_t x) {
	return ((x >> 56) & 0x00000000000000FFULL) | ((x >> 40) & 0x000000000000FF00ULL) |
	       ((x >> 24) & 0x0000000000FF0000ULL) | ((x >> 8) & 0x00000000FF000000ULL) |
	       ((x << 8) & 0x000000FF00000000ULL) | ((x << 24) & 0x0000FF0000000000ULL) |
	       ((x << 40) & 0x00FF000000000000ULL) | ((x << 56) & 0xFF00000000000000ULL);
}
#endif

} // extern "C"

// ============================================================================
// countl_zero - Count leading zeros
// ============================================================================

val<uint8_t> countl_zero(val<uint8_t> x) {
	return invoke<uint8_t, uint8_t>(countl_zero_u8_impl, x);
}

val<uint16_t> countl_zero(val<uint16_t> x) {
	return invoke<uint16_t, uint16_t>(countl_zero_u16_impl, x);
}

val<uint32_t> countl_zero(val<uint32_t> x) {
	return invoke<uint32_t, uint32_t>(countl_zero_u32_impl, x);
}

val<uint64_t> countl_zero(val<uint64_t> x) {
	return invoke<uint64_t, uint64_t>(countl_zero_u64_impl, x);
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
	return invoke<uint8_t, uint8_t>(countr_zero_u8_impl, x);
}

val<uint16_t> countr_zero(val<uint16_t> x) {
	return invoke<uint16_t, uint16_t>(countr_zero_u16_impl, x);
}

val<uint32_t> countr_zero(val<uint32_t> x) {
	return invoke<uint32_t, uint32_t>(countr_zero_u32_impl, x);
}

val<uint64_t> countr_zero(val<uint64_t> x) {
	return invoke<uint64_t, uint64_t>(countr_zero_u64_impl, x);
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
	return invoke<uint8_t, uint8_t>(popcount_u8_impl, x);
}

val<uint16_t> popcount(val<uint16_t> x) {
	return invoke<uint16_t, uint16_t>(popcount_u16_impl, x);
}

val<uint32_t> popcount(val<uint32_t> x) {
	return invoke<uint32_t, uint32_t>(popcount_u32_impl, x);
}

val<uint64_t> popcount(val<uint64_t> x) {
	return invoke<uint64_t, uint64_t>(popcount_u64_impl, x);
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
	return invoke<uint8_t, uint8_t, uint8_t>(rotl_u8_impl, x, s);
}

val<uint16_t> rotl(val<uint16_t> x, val<uint16_t> s) {
	return invoke<uint16_t, uint16_t, uint16_t>(rotl_u16_impl, x, s);
}

val<uint32_t> rotl(val<uint32_t> x, val<uint32_t> s) {
	return invoke<uint32_t, uint32_t, uint32_t>(rotl_u32_impl, x, s);
}

val<uint64_t> rotl(val<uint64_t> x, val<uint64_t> s) {
	return invoke<uint64_t, uint64_t, uint64_t>(rotl_u64_impl, x, s);
}

// ============================================================================
// rotr - Rotate right
// ============================================================================

val<uint8_t> rotr(val<uint8_t> x, val<uint8_t> s) {
	return invoke<uint8_t, uint8_t, uint8_t>(rotr_u8_impl, x, s);
}

val<uint16_t> rotr(val<uint16_t> x, val<uint16_t> s) {
	return invoke<uint16_t, uint16_t, uint16_t>(rotr_u16_impl, x, s);
}

val<uint32_t> rotr(val<uint32_t> x, val<uint32_t> s) {
	return invoke<uint32_t, uint32_t, uint32_t>(rotr_u32_impl, x, s);
}

val<uint64_t> rotr(val<uint64_t> x, val<uint64_t> s) {
	return invoke<uint64_t, uint64_t, uint64_t>(rotr_u64_impl, x, s);
}

// ============================================================================
// byteswap - Reverse byte order
// ============================================================================

val<uint16_t> byteswap(val<uint16_t> x) {
	return invoke<uint16_t, uint16_t>(byteswap_u16_impl, x);
}

val<uint32_t> byteswap(val<uint32_t> x) {
	return invoke<uint32_t, uint32_t>(byteswap_u32_impl, x);
}

val<uint64_t> byteswap(val<uint64_t> x) {
	return invoke<uint64_t, uint64_t>(byteswap_u64_impl, x);
}

} // namespace nautilus
