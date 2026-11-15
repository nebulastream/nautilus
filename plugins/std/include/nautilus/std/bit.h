#pragma once
#include <nautilus/val.hpp>

namespace nautilus {

// ============================================================================
// Bit manipulation functions (C++20 <bit> header)
// ============================================================================

/**
 * @brief Counts the number of consecutive 0 bits in the value of x, starting
 * from the most significant bit ("left").
 *
 * @param x Unsigned integer value.
 * @return Number of consecutive 0 bits from the left.
 */
val<uint8_t> countl_zero(const val<uint8_t>& x) noexcept;
val<uint16_t> countl_zero(const val<uint16_t>& x) noexcept;
val<uint32_t> countl_zero(const val<uint32_t>& x) noexcept;
val<uint64_t> countl_zero(const val<uint64_t>& x) noexcept;

/**
 * @brief Counts the number of consecutive 1 bits in the value of x, starting
 * from the most significant bit ("left").
 *
 * @param x Unsigned integer value.
 * @return Number of consecutive 1 bits from the left.
 */
val<uint8_t> countl_one(const val<uint8_t>& x) noexcept;
val<uint16_t> countl_one(const val<uint16_t>& x) noexcept;
val<uint32_t> countl_one(const val<uint32_t>& x) noexcept;
val<uint64_t> countl_one(const val<uint64_t>& x) noexcept;

/**
 * @brief Counts the number of consecutive 0 bits in the value of x, starting
 * from the least significant bit ("right").
 *
 * @param x Unsigned integer value.
 * @return Number of consecutive 0 bits from the right.
 */
val<uint8_t> countr_zero(const val<uint8_t>& x) noexcept;
val<uint16_t> countr_zero(const val<uint16_t>& x) noexcept;
val<uint32_t> countr_zero(const val<uint32_t>& x) noexcept;
val<uint64_t> countr_zero(const val<uint64_t>& x) noexcept;

/**
 * @brief Counts the number of consecutive 1 bits in the value of x, starting
 * from the least significant bit ("right").
 *
 * @param x Unsigned integer value.
 * @return Number of consecutive 1 bits from the right.
 */
val<uint8_t> countr_one(const val<uint8_t>& x) noexcept;
val<uint16_t> countr_one(const val<uint16_t>& x) noexcept;
val<uint32_t> countr_one(const val<uint32_t>& x) noexcept;
val<uint64_t> countr_one(const val<uint64_t>& x) noexcept;

/**
 * @brief Counts the number of 1 bits in the value of x (population count).
 *
 * @param x Unsigned integer value.
 * @return Number of 1 bits in x.
 */
val<uint8_t> popcount(const val<uint8_t>& x) noexcept;
val<uint16_t> popcount(const val<uint16_t>& x) noexcept;
val<uint32_t> popcount(const val<uint32_t>& x) noexcept;
val<uint64_t> popcount(const val<uint64_t>& x) noexcept;

/**
 * @brief Checks if x is a power of two (has only a single 1 bit).
 *
 * @param x Unsigned integer value.
 * @return true if x is a power of two, false otherwise.
 */
val<bool> has_single_bit(const val<uint8_t>& x) noexcept;
val<bool> has_single_bit(const val<uint16_t>& x) noexcept;
val<bool> has_single_bit(const val<uint32_t>& x) noexcept;
val<bool> has_single_bit(const val<uint64_t>& x) noexcept;

/**
 * @brief Calculates the number of bits needed to represent the value x.
 *
 * @param x Unsigned integer value.
 * @return Number of bits needed to represent x.
 */
val<uint8_t> bit_width(const val<uint8_t>& x) noexcept;
val<uint16_t> bit_width(const val<uint16_t>& x) noexcept;
val<uint32_t> bit_width(const val<uint32_t>& x) noexcept;
val<uint64_t> bit_width(const val<uint64_t>& x) noexcept;

/**
 * @brief Finds the smallest power of two not less than x.
 *
 * @param x Unsigned integer value.
 * @return Smallest power of two >= x.
 */
val<uint8_t> bit_ceil(const val<uint8_t>& x) noexcept;
val<uint16_t> bit_ceil(const val<uint16_t>& x) noexcept;
val<uint32_t> bit_ceil(const val<uint32_t>& x) noexcept;
val<uint64_t> bit_ceil(const val<uint64_t>& x) noexcept;

/**
 * @brief Finds the largest power of two not greater than x.
 *
 * @param x Unsigned integer value.
 * @return Largest power of two <= x.
 */
val<uint8_t> bit_floor(const val<uint8_t>& x) noexcept;
val<uint16_t> bit_floor(const val<uint16_t>& x) noexcept;
val<uint32_t> bit_floor(const val<uint32_t>& x) noexcept;
val<uint64_t> bit_floor(const val<uint64_t>& x) noexcept;

/**
 * @brief Rotates the bits in x left by s positions.
 *
 * @param x Unsigned integer value.
 * @param s Number of positions to rotate.
 * @return x rotated left by s positions.
 */
val<uint8_t> rotl(const val<uint8_t>& x, const val<uint8_t>& s) noexcept;
val<uint16_t> rotl(const val<uint16_t>& x, const val<uint16_t>& s) noexcept;
val<uint32_t> rotl(const val<uint32_t>& x, const val<uint32_t>& s) noexcept;
val<uint64_t> rotl(const val<uint64_t>& x, const val<uint64_t>& s) noexcept;

/**
 * @brief Rotates the bits in x right by s positions.
 *
 * @param x Unsigned integer value.
 * @param s Number of positions to rotate.
 * @return x rotated right by s positions.
 */
val<uint8_t> rotr(const val<uint8_t>& x, const val<uint8_t>& s) noexcept;
val<uint16_t> rotr(const val<uint16_t>& x, const val<uint16_t>& s) noexcept;
val<uint32_t> rotr(const val<uint32_t>& x, const val<uint32_t>& s) noexcept;
val<uint64_t> rotr(const val<uint64_t>& x, const val<uint64_t>& s) noexcept;

/**
 * @brief Reverses the byte order of x (byteswap).
 *
 * @param x Unsigned integer value.
 * @return x with bytes in reversed order.
 */
val<uint16_t> byteswap(const val<uint16_t>& x) noexcept;
val<uint32_t> byteswap(const val<uint32_t>& x) noexcept;
val<uint64_t> byteswap(const val<uint64_t>& x) noexcept;

} // namespace nautilus
